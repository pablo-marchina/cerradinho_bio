#include <Arduino.h>
#include <cstring>
#include <driver/gpio.h>
#include <esp_system.h>
#include <TinyGPS++.h>
#include <WiFi.h>
#include <time.h>
#include <Preferences.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "config.h"
#include "tracking_math.h"
#include "cycle_rules.h"
#include "seal_rules.h"
#include "event_queue.h"
#include "battery_rules.h"
#include "gnss_rules.h"
#include "sim_clock.h"
#include "screen_controller.h"
#include "seal_debounce.h"
#include "dashboard_cmd.h"
#if MQTT_ENABLED
#include <PubSubClient.h>
#if MQTT_TLS
#include <WiFiClientSecure.h>
// Certificados raiz embutidos no core do ESP32 (inclui o ISRG Root X1 do HiveMQ Cloud).
extern const uint8_t rootca_crt_bundle_start[] asm("_binary_x509_crt_bundle_start");
#endif
#endif

namespace {
TinyGPSPlus gps;
Preferences storage;
bool storageReady = false;
bool havePrevious = false;
double previousLat = 0, previousLon = 0;
double latestLat = 0, latestLon = 0;
bool haveLatest = false;
uint32_t latestAt = 0;
uint32_t lastHeartbeat = 0, lastWifiRetry = 0;
uint32_t lastSample = 0, requestAt = 0;
bool sampleRequested = true;
bool rawSeal = false, stableSeal = false;
uint32_t sealChangedAt = 0;
bool wasConnected = false;
SimClock simClock;  // ms acumulados, sem a volta de ~49,7 dias do millis()
CycleState cycle;
uint64_t cycleStartedAt = 0;
bool cycleActive = false;
bool returnInvoiceIssued = false;  // NF de retorno lancada (comando NF no Serial)
uint32_t closedCycles = 0;
SealState seal;
EventQueue pending;
BatteryState battery;
uint32_t lastBatteryCheck = 0;
int batteryPct = 0;  // ultima leitura, usada pelo LCD sem ler o ADC a cada volta
int wakeReferencePct = -100;  // bateria no ultimo despertar do LCD; -100 = sem referencia
bool poweredOn = true;
bool networkWasAvailable = true;
String serialLine;
bool contextDirty = false;  // ciclo/lacre mudou e precisa ir para a NVS
bool pendingDirty = false;
bool contextRetryPending = false, pendingRetryPending = false;
uint32_t lastContextFailureAt = 0, lastPendingFailureAt = 0;

// Bordas capturadas fora do loop; sem gravacoes nem serial na ISR.
constexpr uint8_t SealEdgeCapacity = 64;
struct SealEdge { uint32_t at; bool opened; };
volatile SealEdge sealEdges[SealEdgeCapacity] = {};
volatile uint8_t sealEdgeHead = 0, sealEdgeTail = 0;
volatile uint32_t sealEdgeOverruns = 0;
// ISR do lacre: guarda o instante e o nivel de cada borda no buffer circular.
void IRAM_ATTR onSealEdge() {
  const uint8_t head = sealEdgeHead;
  const uint8_t next = static_cast<uint8_t>((head + 1U) % SealEdgeCapacity);
  if (next == sealEdgeTail) { ++sealEdgeOverruns; return; }
  sealEdges[head].at = millis();
  sealEdges[head].opened = gpio_get_level(static_cast<gpio_num_t>(Config::SealPin)) != 0;
  sealEdgeHead = next;
}
// Retira a borda mais antiga do buffer da ISR; false quando nao ha nenhuma.
bool popSealEdge(SealEdge &edge) {
  noInterrupts();
  if (sealEdgeTail == sealEdgeHead) { interrupts(); return false; }
  const uint8_t tail = sealEdgeTail;
  edge.at = sealEdges[tail].at;
  edge.opened = sealEdges[tail].opened;
  sealEdgeTail = static_cast<uint8_t>((tail + 1U) % SealEdgeCapacity);
  interrupts();
  return true;
}

char shipmentInvoice[Config::InvoiceMaxLength + 1] = "";  // NF da remessa (comando REMESSA)
bool alarmActive = false;   // buzzer da violacao, limitado a BuzzerAlarmMs
uint32_t alarmStartedAt = 0;
bool buzzerOn = false;
GnssState gnssHealth;
uint32_t lastGpsCharsAt = 0;  // ultimo byte NMEA recebido, para separar sem dados de sem fix

// Botao com debounce e deteccao por borda: segurar nao repete o evento.
struct Button {
  int pin;
  bool raw, pressed;
  uint32_t changedAt, pressedAt;
  bool longFired;
};
Button authorizeButton{Config::AuthorizeButtonPin, false, false, 0, 0, false};
Button cycleButton{Config::CycleButtonPin, false, false, 0, 0, false};
Button nextButton{Config::NextButtonPin, false, false, 0, 0, false};
Button previousButton{Config::PreviousButtonPin, false, false, 0, 0, false};
// SWITCH_BUTTONS: botoes no lugar das chaves de rede e ON/OFF, com o estado guardado aqui.
Button networkButton{Config::NetworkSwitchPin, false, false, 0, 0, false};
Button powerButton{Config::PowerSwitchPin, false, false, 0, 0, false};
bool networkOn = true, powerOn = true;
uint32_t networkRestoreAt = 0;  // comando `rede` desligada por tempo limitado: religa neste millis(); 0 = sem prazo
bool sealButtonRaw = true, sealButtonStable = true;  // nivel do botao do lacre; HIGH = solto
LiquidCrystal_I2C lcd(Config::LcdAddress, ScreenController::Columns, 2);
ScreenController screens;  // telas do LCD (classe propria, usada no setup e no loop)
ScreenController::Line shownTop = "", shownBottom = "";
bool lcdLit = true;
uint32_t lastInteractionAt = 0;  // ultimo botao, mensagem ou variacao da bateria; o LCD apaga apos LcdTimeoutMs

// Configura o LCD: fios longos em protoboard corrompem o I2C na velocidade padrao.
void initLcd() {
  // Os pinos do LCD primeiro: o init() da biblioteca chama Wire.begin() sem argumentos, e o
  // core do ESP32 ignora um segundo begin() com o barramento ja iniciado.
  Wire.begin(Config::LcdSdaPin, Config::LcdSclPin);
  lcd.init();
  Wire.setClock(Config::LcdI2cHz);
  lcd.backlight();
}

// Tempo do ciclo em ms, sem a volta do millis().
uint64_t clockMs() {
  return simClock.elapsedMs;
}

// Dia do ciclo correspondente a um instante (no demo, 1 s = 1 dia).
uint32_t simulatedDay(uint64_t clock) {
  return (uint32_t)(clock / Config::DayMs);
}

// Chave de rede (on/off do SIM): esquerda = com sinal. Com botao, cada toque alterna.
bool simulatedNetworkAvailable() {
  return Config::SwitchButtons ? networkOn : digitalRead(Config::NetworkSwitchPin) == LOW;
}

// Chave ON/OFF: esquerda = ligado. Com botao, segurar LongPressMs alterna.
bool powerSwitchOn() {
  return Config::SwitchButtons ? powerOn : digitalRead(Config::PowerSwitchPin) == LOW;
}

// Fix recente, com satelites e coordenadas validas.
bool freshFix() {
  return gps.location.isValid() && gps.location.age() <= Config::FixMaxAgeMs &&
         gps.satellites.isValid() && gps.satellites.value() > 0 &&
         validCoordinates(gps.location.lat(), gps.location.lng());
}

// Nivel de bateria de 0 a 100% lido no potenciometro (ADC1, GPIO34).
int simulatedBatteryPercent() {
  // Entrada sintetica 0..3,3 V. Nao ligar uma celula diretamente ao GPIO34.
  // Media de 16 leituras contra o ruido do ADC; a histerese do LCD fica em readBattery().
  long sum = 0;
  for (int i = 0; i < 16; ++i) sum += analogRead(Config::BatteryPin);
  return constrain((sum / 16 * 100L) / 4095L, 0L, 100L);
}

// Registro de diagnostico na NVS (q0..q15); cheio, preserva os antigos e so imprime o novo.
void enqueue(const String &record) {
  Serial.println(record);
  if (!storageReady) {
    Serial.println("[registro] armazenamento indisponivel; registro apenas na serial");
    return;
  }
  for (uint8_t index = 0; index < Config::QueueCapacity; ++index) {
    const String key = "q" + String(index);
    if (storage.isKey(key.c_str())) continue;
    if (storage.putString(key.c_str(), record) == record.length()) {
      Serial.println("[registro] diagnostico gravado na NVS; servidor ainda nao configurado");
    } else {
      Serial.println("[registro] falha ao gravar diagnostico; registro disponivel na serial");
    }
    return;
  }
  Serial.printf("[registro] limite de %u registros de diagnostico; antigos preservados, novo apenas na serial\r\n",
                Config::QueueCapacity);
}

// Inicio comum do JSON de todo registro e evento: dispositivo, tipo, tempo, lacre, bateria e rede.
String baseRecord(const char *type) {
  String result;
  result.reserve(420);
  result = "{\"device_id\":\"";
  result += Config::DeviceId;
  result += "\",\"type\":\"";
  result += type;
  result += "\",\"uptime_ms\":";
  result += millis();
  result += ",\"clock\":\"uptime_only\",\"demo\":";
  result += DEMO_MODE ? "true" : "false";
  result += ",\"seal\":\"";
  result += stableSeal ? "violated" : "intact";
  result += "\",\"battery_sim_pct\":";
  result += simulatedBatteryPercent();
  result += ",\"wifi_connected\":";
  result += WiFi.status() == WL_CONNECTED ? "true" : "false";
  result += ",\"network_sim_available\":";
  result += simulatedNetworkAvailable() ? "true" : "false";
  return result;
}

// Guarda a ultima coleta valida na NVS como referencia para o proximo reinicio.
void savePrevious(double lat, double lon) {
  if (!storageReady) return;
  // Uma unica chave evita um par latitude/longitude parcialmente atualizado.
  const double point[2] = {lat, lon};
  if (storage.putBytes("previous", point, sizeof(point)) != sizeof(point)) {
    Serial.println("[memoria] falha ao salvar referencia entre reinicios");
  }
}

// Fecha uma coleta de posicao: com fix calcula distancia e movimento; sem fix mantem a referencia.
void samplePosition(bool valid) {
  String record = baseRecord(valid ? "location_sample" : "location_unavailable");
  record += ",\"fix_valid\":";
  record += valid ? "true" : "false";
  if (valid) {
    const double lat = gps.location.lat(), lon = gps.location.lng();
    record += ",\"latitude\":"; record += String(lat, 6);
    record += ",\"longitude\":"; record += String(lon, 6);
    record += ",\"distance_m\":";
    record += havePrevious ? String(distanceM(previousLat, previousLon, lat, lon), 2) : "null";
    record += ",\"moved\":";
    record += havePrevious ? (distanceM(previousLat, previousLon, lat, lon) >
                             Config::MovementThresholdM ? "true" : "false") : "null";
    record += ",\"baseline_created\":";
    record += havePrevious ? "false" : "true";
    previousLat = lat; previousLon = lon; havePrevious = true;
    savePrevious(lat, lon);
  } else {
    record += ",\"latitude\":null,\"longitude\":null,\"moved\":null";
    if (haveLatest) {
      record += ",\"last_valid_latitude\":"; record += String(latestLat, 6);
      record += ",\"last_valid_longitude\":"; record += String(latestLon, 6);
      record += ",\"last_valid_age_ms\":"; record += millis() - latestAt;
    } else if (havePrevious) {
      record += ",\"last_valid_latitude\":"; record += String(previousLat, 6);
      record += ",\"last_valid_longitude\":"; record += String(previousLon, 6);
      record += ",\"last_valid_age_ms\":null";
    }
    Serial.println("[gps] sem fix recente; referencia anterior preservada");
  }
  record += "}";
  enqueue(record);
}

// Le o NMEA do GNSS e atualiza a ultima posicao valida.
void readGps() {
  if (Serial2.available()) lastGpsCharsAt = millis();
  while (Serial2.available()) {
    gps.encode(Serial2.read());
    if (gps.location.isUpdated() && freshFix()) {
      latestLat = gps.location.lat(); latestLon = gps.location.lng();
      haveLatest = true; latestAt = millis();
    }
  }
}

// Contexto do ciclo e do lacre na NVS: reinicio nao reabre o ciclo 1 nem apaga
// o historico (RF016, RF017; RN09, RN11). A versao muda se o formato mudar; a
// duracao do dia separa demo de weekly, para um nao restaurar o contexto do outro.
struct StoredContext {
  uint32_t version;
  uint32_t dayMs;
  uint64_t clockMs;
  CycleState cycle;
  uint64_t cycleStartedAt;
  bool cycleActive;
  bool returnInvoiceIssued;
  uint32_t closedCycles;
  SealState seal;
  bool sealOpen;
  char invoice[Config::InvoiceMaxLength + 1];
};
constexpr uint32_t ContextVersion = 2;  // V2 acrescentou a NF da remessa

// Grava ciclo, lacre e NF na NVS; em falha, marca nova tentativa.
bool saveContext() {
  contextDirty = true;
  if (!storageReady) {
    contextRetryPending = true;
    lastContextFailureAt = millis();
    return false;
  }
  StoredContext context{ContextVersion, Config::DayMs, clockMs(), cycle, cycleStartedAt,
                        cycleActive, returnInvoiceIssued, closedCycles, seal, stableSeal, {}};
  memcpy(context.invoice, shipmentInvoice, sizeof(context.invoice));
  if (storage.putBytes("ctx", &context, sizeof(context)) != sizeof(context)) {
    contextRetryPending = true;
    lastContextFailureAt = millis();
    Serial.println("[memoria] falha ao gravar contexto; retentativa pendente");
    return false;
  }
  contextDirty = false;
  contextRetryPending = false;
  return true;
}

// Le o contexto salvo; false se nao existe, esta incompleto ou e de outra versao ou escala de dia.
bool restoreContext(StoredContext &context) {
  return storageReady && storage.isKey("ctx") &&
         storage.getBytesLength("ctx") == sizeof(context) &&
         storage.getBytes("ctx", &context, sizeof(context)) == sizeof(context) &&
         context.version == ContextVersion && context.dayMs == Config::DayMs;
}

// Fila de eventos de negocio na NVS: chaves p0..p7 com o texto e "pmeta" com
// quantidade, criticos e perdas. Regravada a cada mudanca da fila.
struct StoredQueueMeta {
  uint8_t count;
  bool critical[EVENT_QUEUE_CAPACITY];
  uint32_t lost;
  uint32_t overflowCount;
  uint64_t overflowFirstMs, overflowLastMs;
};

// Grava a fila de eventos sem rede na NVS (p0..p7 e metadados).
bool savePending() {
  pendingDirty = true;
  if (!storageReady) {
    pendingRetryPending = true;
    lastPendingFailureAt = millis();
    return false;
  }
  StoredQueueMeta meta{pending.count, {}, pending.lost, pending.overflowCount,
                       pending.overflowFirstMs, pending.overflowLastMs};
  bool complete = true;
  for (uint8_t i = 0; i < EVENT_QUEUE_CAPACITY; ++i) {
    const String key = "p" + String(i);
    if (i < pending.count) {
      meta.critical[i] = pending.critical[i];
      if (storage.putString(key.c_str(), pending.text[i]) != strlen(pending.text[i])) complete = false;
    } else if (storage.isKey(key.c_str())) {
      if (!storage.remove(key.c_str())) complete = false;
    }
  }
  if (complete) complete = storage.putBytes("pmeta", &meta, sizeof(meta)) == sizeof(meta);
  if (!complete) {
    pendingRetryPending = true;
    lastPendingFailureAt = millis();
    Serial.println("[memoria] falha ao gravar fila; retentativa pendente, NVS nao transacional");
    return false;
  }
  pendingDirty = false;
  pendingRetryPending = false;
  return true;
}

// Restaura a fila de eventos salva antes do reinicio.
void restorePending() {
  StoredQueueMeta meta{};
  if (!storageReady || !storage.isKey("pmeta") ||
      storage.getBytesLength("pmeta") != sizeof(meta) ||
      storage.getBytes("pmeta", &meta, sizeof(meta)) != sizeof(meta) ||
      meta.count > EVENT_QUEUE_CAPACITY) {
    return;
  }
  for (uint8_t i = 0; i < meta.count; ++i) {
    const String text = storage.getString(("p" + String(i)).c_str(), "");
    if (text.length() == 0) {
      Serial.printf("[memoria] ALERTA: item p%u ausente; restauracao parcial da fila\r\n", i);
      break;  // metadados antigos nao contem checksum; nao afirmar entrega integra
    }
    queuePush(&pending, text.c_str(), meta.critical[i], 0);
  }
  pending.lost = meta.lost;
  pending.overflowCount = meta.overflowCount;
  pending.overflowFirstMs = meta.overflowFirstMs;
  pending.overflowLastMs = meta.overflowLastMs;
}

// Ha eventos ou resumo de violacoes esperando a rede.
bool hasPending() {
  return pending.count > 0 || pending.overflowCount > 0;
}

// Mostra na Serial quantos eventos estao na fila.
void printPending() {
  Serial.printf("[FILA] %u %s", pending.count, pending.count == 1 ? "pendente" : "pendentes");
  if (pending.overflowCount > 0) {
    Serial.printf(" + resumo de %lu violacao(oes)", (unsigned long)pending.overflowCount);
  }
  Serial.print("\r\n");
}

#if MQTT_ENABLED
// Todo evento muda o estado: marca o `status` retido para republicar em updateMqtt().
bool mqttStatusDirty = false;
// Definidas junto do cliente MQTT, mais abaixo.
bool mqttConnected();
bool publishMqttEvent(const char *json);
#endif

#if MQTT_ENABLED
// Eventos periodicos nao mudam o estado do dispositivo e nao republicam o `status` retido: no
// demo o STATUS_SEMANAL sai a cada ~7 s e gastaria a cota do broker. Os demais eventos (violacao
// do lacre, bateria, ciclo, GNSS, autorizacao, limiar etc.) mudam o estado e o republicam.
bool eventChangesStatus(const char *type) {
  static const char *const periodicTypes[] = {"STATUS", "STATUS_SEMANAL"};
  for (const char *periodic : periodicTypes) {
    if (strcmp(type, periodic) == 0) return false;
  }
  return true;
}
#endif

// Com MQTT, so ha rede com o broker conectado; sem MQTT, vale a chave de sinal.
bool networkOnline() {
#if MQTT_ENABLED
  return simulatedNetworkAvailable() && mqttConnected();
#else
  return simulatedNetworkAvailable();
#endif
}

// Envia um JSON: Serial (log) e, com MQTT, o topico `evento`. false = nao saiu, fica na fila.
bool sendRecord(const char *json) {
  Serial.println(json);
#if MQTT_ENABLED
  return publishMqttEvent(json);
#else
  return true;
#endif
}

// Com rede, o JSON sai no Serial e no MQTT. Sem rede (ou se a publicacao falhar), vai para a
// fila e sai na reconexao, na ordem e com o dia em que ocorreu (RF010; RN09).
void publishEvent(const char *type, bool critical = false, const String &extra = "") {
#if MQTT_ENABLED
  if (eventChangesStatus(type)) mqttStatusDirty = true;
#endif
  String record = baseRecord(type);
  record += ",\"ciclo_id\":"; record += cycle.cycleId;
  record += ",\"ciclo_ativo\":"; record += cycleActive ? "true" : "false";
  record += ",\"dias_fora\":"; record += cycle.daysOut;
  record += ",\"dia_ocorrencia\":"; record += simulatedDay(clockMs());
  record += ",\"atraso\":"; record += cycleIsLate(&cycle) ? "true" : "false";
  record += ",\"permanencia\":"; record += cycleInStay(&cycle) ? "true" : "false";
  record += ",\"critico\":"; record += critical ? "true" : "false";
  record += extra;
  record += "}";
  if (networkOnline() && sendRecord(record.c_str())) return;
  const int result = queuePush(&pending, record.c_str(), critical, clockMs());
  if (result == QUEUE_SUMMARIZED) {
    Serial.printf("[FILA] FILA_CHEIA: so ha violacoes; %s somado ao resumo (%lu no resumo)\r\n",
                  type, (unsigned long)pending.overflowCount);
    savePending();
    printPending();
    return;
  }
  if (result == QUEUE_REJECTED) {
    Serial.printf("[FILA] FILA_CHEIA: so ha eventos criticos; %s nao coube (perdidos=%lu)\r\n",
                  type, (unsigned long)pending.lost);
    savePending();
    return;
  }
  if (result == QUEUE_STORED_DROPPED_OLDEST) {
    Serial.println("[FILA] FILA_CHEIA: descartado o evento nao critico mais antigo");
  }
  Serial.printf("[FILA] guardado: %s\r\n", type);
  savePending();
  printPending();
}

// Rede de volta: reenvia a fila na ordem e depois o resumo das violacoes excedentes.
// Com MQTT, so esvazia com o broker conectado (updateMqtt chama de novo ao conectar).
void flushPending() {
  if (!hasPending() || !networkOnline()) return;
  Serial.printf("[FILA] rede disponivel; reenviando %u evento(s) na ordem\r\n", pending.count);
  while (pending.count > 0) {
    if (!sendRecord(pending.text[0])) break;  // so sai da fila o que foi publicado
    queueRemove(&pending, 0);
  }
  // As violacoes do resumo ocorreram depois das guardadas na fila: saem por ultimo.
  if (pending.count == 0 && pending.overflowCount > 0) {
    String extra = ",\"quantidade\":";
    extra += pending.overflowCount;
    extra += ",\"dia_primeira\":"; extra += simulatedDay(pending.overflowFirstMs);
    extra += ",\"dia_ultima\":"; extra += simulatedDay(pending.overflowLastMs);
    extra += ",\"primeira_ms\":"; extra += String((unsigned long long)pending.overflowFirstMs);
    extra += ",\"ultima_ms\":"; extra += String((unsigned long long)pending.overflowLastMs);
    publishEvent("VIOLACAO_LACRE_RESUMO", true, extra);
    queueClearSummary(&pending);
  }
  savePending();
  printPending();
}


// Abre um ciclo novo (vinculo): zera dias, NF de retorno e NF da remessa.
void startCycle(uint64_t clock, uint32_t id) {
  cycleStart(&cycle, id, Config::ExpectedReturnDays, Config::StayThresholdDays,
              Config::StatusIntervalDays);
  cycleStartedAt = clock;
  cycleActive = true;
  returnInvoiceIssued = false;
  shipmentInvoice[0] = '\0';  // cada remessa recebe a sua NF pelo comando REMESSA
  contextDirty = true;
}

// Avanca um dia por vez para nao pular status nem alerta se o loop atrasar.
void updateCycle(uint64_t clock) {
  if (!cycleActive) return;
  const uint64_t days = (clock - cycleStartedAt) / Config::DayMs;
  while (cycle.daysOut < days) {
    contextDirty = true;
    const unsigned events = cycleAdvanceDay(&cycle);
    if (events & CYCLE_EVENT_STATUS) publishEvent("STATUS_SEMANAL");
    if (events & CYCLE_EVENT_LATE) publishEvent("ALERTA_ATRASO");
    if (events & CYCLE_EVENT_STAY) publishEvent("ALERTA_PERMANENCIA");
  }
}

// LCD aceso enquanto houve uso nos ultimos LcdTimeoutMs.
bool displayAwake(uint32_t now) {
  // Diferenca com sinal: um comando MQTT ou uma mensagem dentro do mesmo ciclo do loop marca
  // lastInteractionAt depois de `now`, e a conta sem sinal daria um valor enorme e apagaria o LCD.
  return static_cast<int32_t>(now - lastInteractionAt) < static_cast<int32_t>(Config::LcdTimeoutMs);
}

// Mostra uma mensagem curta no LCD e acende o visor.
void showMessage(const char *top, const char *bottom) {
  lastInteractionAt = millis();
  screens.flash(top, bottom, lastInteractionAt, Config::MessageMs);
}

// Encerrar exige lacre fechado e NF de retorno; o historico e preservado (RF016; RN11).
void requestClose() {
  if (!cycleActive) {
    publishEvent("ENCERRAMENTO_RECUSADO", false, ",\"motivo\":\"sem_ciclo_ativo\"");
    showMessage("Nao encerrado", "sem ciclo ativo");
  } else if (stableSeal) {
    publishEvent("ENCERRAMENTO_RECUSADO", false, ",\"motivo\":\"lacre_aberto\"");
    showMessage("Nao encerrado", "lacre aberto");
  } else if (!returnInvoiceIssued) {
    publishEvent("ENCERRAMENTO_RECUSADO", false, ",\"motivo\":\"nf_retorno_ausente\"");
    showMessage("Nao encerrado", "falta NF retorno");
  } else {
    ++closedCycles;
    cycleActive = false;
    cycle.daysOut = 0;
    contextDirty = true;
    String extra = ",\"ciclos_encerrados\":";
    extra += closedCycles;
    // O evento deve retratar o estado ja encerrado, inclusive quando fica na fila.
    publishEvent("CICLO_ENCERRADO", false, extra);
    showMessage("Ciclo encerrado", "");
  }
}

// Um dispositivo so pode estar em um ciclo ativo por vez (RF003, RF004; RN01).
void requestLink(uint64_t clock) {
  if (cycleActive) {
    publishEvent("VINCULO_RECUSADO", false, ",\"motivo\":\"ciclo_ativo\"");
    showMessage("Nao iniciado", "ciclo ja ativo");
    return;
  }
  startCycle(clock, cycle.cycleId + 1);
  publishEvent("CICLO_INICIADO");
  showMessage("Ciclo iniciado", "aguardando NF");
}

// Chave de sinal (on/off do SIM): reenvia a fila na volta e mostra o resultado no LCD.
void updateNetwork() {
  // Com MQTT, a rede so existe com o Wi-Fi e o broker conectados (networkOnline); a queda real
  // mostra "Sem sinal" no LCD como a chave simulada (#124).
  const bool available = networkOnline();
  if (available == networkWasAvailable) return;
  networkWasAvailable = available;
  static bool announced = false;  // a 1a conexao depois do boot e silenciosa: o "MQTT ok" ja a anuncia
  char bottom[ScreenController::Columns + 1];
  if (available) {
    // Com MQTT o broker ainda reconecta aqui: a fila sai depois, em updateMqtt.
    const unsigned before = pending.count + (pending.overflowCount > 0 ? 1u : 0u);
    flushPending();
    if (!announced) return;
    const unsigned sent = before - pending.count - (pending.overflowCount > 0 ? 1u : 0u);
    snprintf(bottom, sizeof(bottom), "enviados: %u", sent);
    showMessage("Sinal ok", bottom);
  } else {
    announced = true;
    snprintf(bottom, sizeof(bottom), "na fila: %u", pending.count);
    showMessage("Sem sinal", bottom);
  }
}

// Retorna true na borda de pressao (apos debounce).
bool updateButton(Button &button, uint32_t now) {
  const bool value = digitalRead(button.pin) == LOW;
  if (value != button.raw) { button.raw = value; button.changedAt = now; }
  if (button.raw == button.pressed || now - button.changedAt < Config::ButtonDebounceMs) {
    return false;
  }
  button.pressed = button.raw;
  if (button.pressed) {
    lastInteractionAt = now;  // qualquer botao acende o LCD
    button.pressedAt = now;
    button.longFired = false;
  }
  return button.pressed;
}

// Retorna true uma vez, quando o botao completa LongPressMs pressionado (V2: 3 s).
// Soltar antes disso nao registra nada.
bool holdCompleted(Button &button, uint32_t now, const char *name) {
  const bool wasPressed = button.pressed;
  updateButton(button, now);
  if (wasPressed && !button.pressed && !button.longFired) {
    Serial.printf("[botao] %s solto antes de %lu s; nada registrado\r\n", name,
                  (unsigned long)(Config::LongPressMs / 1000UL));
  }
  if (!button.pressed || button.longFired || now - button.pressedAt < Config::LongPressMs) {
    return false;
  }
  button.longFired = true;
  return true;
}

// Registra a autorizacao de retirada (botao AUTORIZAR por 3 s ou comando `autorizar` do dashboard).
void registerAuthorization(uint64_t clock) {
  sealAuthorize(&seal, clock);
  contextDirty = true;
  String extra = ",\"janela_dias\":";
  extra += Config::AuthorizationWindowDays;
  publishEvent("AUTORIZACAO_REGISTRADA", false, extra);
  char bottom[ScreenController::Columns + 1];
  snprintf(bottom, sizeof(bottom), "vale %lu dias", (unsigned long)Config::AuthorizationWindowDays);
  showMessage("Autorizacao ok", bottom);
}

// Navegacao do LCD e acoes de 3 s (autorizacao e ciclo).
void readButtons(uint32_t now, uint64_t clock) {
  // Com o LCD apagado, o primeiro toque em avancar/voltar so acende o visor.
  const bool awake = displayAwake(now);
  if (Config::SwitchButtons && updateButton(networkButton, now)) networkOn = !networkOn;
  if (updateButton(nextButton, now) && awake) screens.next();
  if (updateButton(previousButton, now) && awake) screens.previous();
  if (holdCompleted(authorizeButton, now, "BTN_AUTORIZAR")) registerAuthorization(clock);
  // Segurar 3 s inicia o ciclo quando nao ha ciclo ativo e encerra quando ha.
  if (holdCompleted(cycleButton, now, "BTN_CICLO")) {
    if (cycleActive) requestClose();
    else requestLink(clock);
  }
}

// Le a bateria a cada BatteryCheckMs e emite alerta baixo ou falha do sensor (RN10).
void readBattery(uint32_t now) {
  if (now - lastBatteryCheck < Config::BatteryCheckMs) return;
  lastBatteryCheck = now;
  const int raw = analogRead(Config::BatteryPin);
  const int percent = simulatedBatteryPercent();
  // Histerese de 1 ponto so no visor: a regra usa a media, para o alerta nao atrasar (RN10).
  if (abs(percent - batteryPct) >= 2 || percent == 0 || percent == 100) batteryPct = percent;
  // Variacao da bateria acende o LCD na tela de bateria. Compara com o ultimo valor que contou
  // como uso (nao com a leitura anterior), com folga de 3 pontos contra o ruido do ADC.
  if (abs(batteryPct - wakeReferencePct) >= 3) {
    if (wakeReferencePct != -100) { lastInteractionAt = now; screens.show(Screen::Battery); }
    wakeReferencePct = batteryPct;
  }
  const unsigned events = batteryUpdate(&battery, raw, percent);
  if (events & BATTERY_EVENT_SENSOR_FAILURE) publishEvent("SENSOR_BATERIA_FALHA");
  if (events & BATTERY_EVENT_LOW) publishEvent("BATERIA_BAIXA");
}

// NF da remessa: letras, digitos e separadores comuns, ate InvoiceMaxLength.
bool validInvoice(const String &text) {
  if (text.length() == 0 || text.length() > Config::InvoiceMaxLength) return false;
  for (unsigned i = 0; i < text.length(); ++i) {
    const char c = text.c_str()[i];
    if (!isalnum((unsigned char)c) && c != '-' && c != '.' && c != '/') return false;
  }
  return true;
}

// Comandos locais do Monitor Serial: VINCULAR tenta um novo vinculo (RF003/RF004; RN01),
// NF lanca a NF de retorno, REMESSA x representa a NF da remessa e LIMIAR n ajusta RN03.
// VINCULAR e entrada de bancada; nao representa autenticacao nem concorrencia na plataforma.
// NF de retorno (RN11): so com ciclo ativo e uma vez por ciclo. nullptr = aceita; senao, o motivo.
const char *registerReturnInvoice() {
  if (!cycleActive) return "sem_ciclo_ativo";
  if (returnInvoiceIssued) return "nf_ja_lancada";
  returnInvoiceIssued = true;
  contextDirty = true;
  publishEvent("NF_RETORNO_LANCADA");
  return nullptr;
}

// NF de remessa (RN02): so com ciclo ativo e no formato aceito. nullptr = aceita; senao, o motivo.
const char *registerShipmentInvoice(const String &invoice) {
  if (!cycleActive) return "sem_ciclo_ativo";
  if (!validInvoice(invoice)) return "nf_invalida";
  snprintf(shipmentInvoice, sizeof(shipmentInvoice), "%s", invoice.c_str());
  contextDirty = true;
  Serial.printf("[comando] NF da remessa registrada: %s\r\n", shipmentInvoice);
  showMessage("NF registrada", shipmentInvoice);
  return nullptr;
}

void readCommands() {
  while (Serial.available()) {
    const char c = Serial.read();
    if (c != '\n' && c != '\r') { serialLine += c; continue; }
    serialLine.trim();
    serialLine.toUpperCase();
    if (serialLine == "VINCULAR") {
      // Entrada explicita, mesmo com ciclo ativo. BTN_CICLO permanece reservado ao encerramento.
      requestLink(clockMs());
    } else if (serialLine == "NF") {
      if (const char *error = registerReturnInvoice()) Serial.printf("[comando] NF de retorno recusada: %s\r\n", error);
    } else if (serialLine.startsWith("REMESSA ")) {
      String invoice = serialLine.substring(8);
      invoice.trim();
      const char *error = registerShipmentInvoice(invoice);
      if (error && !cycleActive) {
        Serial.println("[comando] sem ciclo ativo; inicie o ciclo antes da REMESSA");
      } else if (error) {
        Serial.printf("[comando] NF invalida; use ate %u letras, digitos, '-', '.' ou '/'\r\n",
                      Config::InvoiceMaxLength);
      }
    } else if (serialLine.startsWith("LIMIAR ")) {
      const long days = serialLine.substring(7).toInt();
      if (days > 0) {
        // Nao rearma ALERTA_PERMANENCIA ja emitido neste ciclo (RN04).
        cycleSetStayThreshold(&cycle, days);
        contextDirty = true;
        String extra = ",\"limiar_dias\":";
        extra += days;
        publishEvent("LIMIAR_ALTERADO", false, extra);
      }
    } else if (serialLine.length() > 0) {
      Serial.println("[comando] use VINCULAR, NF, REMESSA x ou LIMIAR n");
    }
    serialLine = "";
  }
}

// O relogio do ciclo ja foi calculado uma unica vez no loop(). O horario de
// confirmacao do debounce serve ao diagnostico; nao avanca o relogio de novo.
void sealTransition(bool opened, uint32_t confirmedAt, uint32_t now, uint64_t clock) {
  stableSeal = opened;
  contextDirty = true;
  const int event = stableSeal ? sealOpened(&seal, clock) : sealClosed(&seal);
  // Persistir imediatamente o estado critico; somente falhas sao atrasadas.
  if (!contextRetryPending) saveContext();
  String record = baseRecord("seal_changed");
  record += ",\"authorized_removal\":";
  record += seal.removalInProgress ? "true" : "false";
  record += ",\"debounce_confirmed_at_ms\":"; record += confirmedAt;
  record += "}";
  enqueue(record);
  if (event == SEAL_EVENT_VIOLATION) {
    publishEvent("VIOLACAO_LACRE", true);
    alarmActive = true;
    alarmStartedAt = now;
    showMessage("VIOLACAO", "registrada");
  } else if (event == SEAL_EVENT_REMOVAL) {
    publishEvent("RETIRADA_AUTORIZADA");
    showMessage("Retirada", "autorizada");
  } else if (event == SEAL_EVENT_RESEALED) {
    publishEvent("LACRE_REAPLICADO");
    showMessage("Lacre fechado", "reaplicado");
  } else if (event == SEAL_EVENT_CLOSED) {
    publishEvent("LACRE_FECHADO");
    showMessage("Lacre fechado", "apos violacao");
  }
}

// Captura historico de bordas do lacre, inclusive quando Serial/NVS ocupou o loop.
// Com SWITCH_BUTTONS o debounce acompanha o nivel do botao, e cada toque alterna o lacre.
void readSeal(uint32_t now, uint64_t clock) {
  bool &raw = Config::SwitchButtons ? sealButtonRaw : rawSeal;
  bool &stable = Config::SwitchButtons ? sealButtonStable : stableSeal;
  const auto apply = [&](const SealDebounceEvent &e) {
    if (!Config::SwitchButtons) sealTransition(e.opened, e.confirmedAt, now, clock);
    else if (!e.opened) sealTransition(!stableSeal, e.confirmedAt, now, clock);  // pressionou
  };
  SealEdge edge{};
  SealDebounceEvent event{};
  while (popSealEdge(edge)) {
    if (seal_on_edge(raw, stable, sealChangedAt,
                     edge.opened, edge.at, Config::SealDebounceMs, event)) {
      apply(event);
    }
  }
  // Reconcilia o nivel corrente se uma borda ocorreu antes da ativacao da ISR.
  const bool actual = digitalRead(Config::SealPin) == HIGH;
  if (actual != raw && seal_on_edge(raw, stable, sealChangedAt,
                                    actual, now, Config::SealDebounceMs, event)) {
    apply(event);
  }
  if (seal_settle(raw, stable, sealChangedAt, now, Config::SealDebounceMs, event)) apply(event);
  static uint32_t lastOverruns = 0;
  noInterrupts(); const uint32_t overrunCount = sealEdgeOverruns; interrupts();
  if (overrunCount != lastOverruns) {
    Serial.printf("[lacre] ALERTA: buffer ISR cheio; total=%lu, verificar eventos ausentes\r\n",
                  static_cast<unsigned long>(overrunCount));
    lastOverruns = overrunCount;
  }
}

// Bipe intermitente enquanto o lacre segue violado, por ate BuzzerAlarmMs.
void updateBuzzer(uint32_t now) {
  if (alarmActive && (!seal.violated || now - alarmStartedAt >= Config::BuzzerAlarmMs)) {
    alarmActive = false;
  }
  const bool on = poweredOn && alarmActive &&
                  ((now - alarmStartedAt) / Config::BuzzerBeepMs) % 2 == 0;
  if (on == buzzerOn) return;
  buzzerOn = on;
  ledcWrite(Config::BuzzerChannel, on ? 128 : 0);  // 50% de 8 bits: onda quadrada
}

// Ao fim de cada coleta: registra o inicio e o fim de cada falha do GNSS, uma vez cada.
void updateGnssHealth(bool valid, uint32_t now, uint64_t clock) {
  const bool receiving = now - lastGpsCharsAt < Config::GpsNoDataMs;
  const uint8_t reason = gnssClassify(receiving, gps.satellites.isValid(), gps.satellites.value(),
                                      Config::GpsWeakSatellites);
  const int event = gnssSample(&gnssHealth, valid, reason, clock);
  if (event == GNSS_EVENT_LOST) {
    String extra = ",\"motivo\":\"";
    extra += gnssReasonName(gnssHealth.reason);
    extra += "\",\"satelites\":";
    extra += gps.satellites.isValid() ? String(gps.satellites.value()) : String("null");
    publishEvent("GNSS_SEM_FIX", false, extra);
  } else if (event == GNSS_EVENT_RECOVERED) {
    String extra = ",\"tempo_sem_posicao_s\":";
    extra += (unsigned long)(gnssHealth.lastOutageMs / 1000ULL);
    extra += ",\"coletas_sem_fix\":";
    extra += gnssHealth.lastFailedSamples;
    publishEvent("GNSS_RECUPERADO", false, extra);
  }
}

// Estado do lacre para a tela do LCD.
SealView sealView(uint64_t clock) {
  if (seal.violated) return SealView::Violated;
  if (seal.removalInProgress || sealAuthorizationValid(&seal, clock)) {
    return SealView::Authorized;
  }
  return SealView::Intact;
}

// Tempo restante enquanto um botao de 3 s esta pressionado.
bool holdRemaining(const Button &button, uint32_t now, uint32_t &remaining) {
  if (!button.pressed || button.longFired) return false;
  const uint32_t held = now - button.pressedAt;
  remaining = held < Config::LongPressMs ? Config::LongPressMs - held : 0;
  return true;
}

// Escreve no LCD so quando o texto muda. Desligado ou sem uso por LcdTimeoutMs,
// apaga o visor e a luz de fundo, que dominam o consumo do LCD.
void updateDisplay(uint32_t now, uint64_t clock) {
  if (!poweredOn || !displayAwake(now)) {
    if (lcdLit) {
      lcd.clear();
      lcd.noBacklight();
      lcdLit = false;
      shownTop[0] = shownBottom[0] = '\0';
    }
    return;
  }
  if (!lcdLit) {
    // So a bancada reinicia o LCD: fios longos embaralham o visor, e lcd.init() trava o loop por 1 s.
    if (Config::SwitchButtons) initLcd();
    else lcd.backlight();
    lcdLit = true;
  }
  ScreenController::Line top, bottom;
  uint32_t remaining = 0;
  if (holdRemaining(powerButton, now, remaining)) {
    ScreenController::renderHold("Desligar", remaining, top, bottom);
  } else if (holdRemaining(cycleButton, now, remaining)) {
    ScreenController::renderHold(cycleActive ? "Encerrar ciclo" : "Iniciar ciclo", remaining,
                                 top, bottom);
  } else if (holdRemaining(authorizeButton, now, remaining)) {
    ScreenController::renderHold("Autorizar lacre", remaining, top, bottom);
  } else {
    const StatusView view{shipmentInvoice, cycleActive, cycle.daysOut,
                          cycle.expectedReturnDays, haveLatest || havePrevious, haveLatest,
                          (uint32_t)((now - latestAt) / 1000UL), batteryPct, battery.lowSent,
                          battery.failureSent, sealView(clock), stableSeal,
                          networkOnline(), static_cast<uint32_t>(pending.count)};
    screens.render(view, now, top, bottom);
  }
  if (strcmp(top, shownTop) != 0) {
    lcd.setCursor(0, 0);
    lcd.printf("%-16s", top);
    memcpy(shownTop, top, sizeof(shownTop));
  }
  if (strcmp(bottom, shownBottom) != 0) {
    lcd.setCursor(0, 1);
    lcd.printf("%-16s", bottom);
    memcpy(shownBottom, bottom, sizeof(shownBottom));
  }
}

// ---- Comandos do dashboard (topico cmd) ----
// Contrato: document/outros/3-desenvolvimento-resultados/3.4.3-prototipo-mqtt-i2c/protocolo-mqtt-dashboard.md.
// Cada comando e um JSON {"id":"...","acao":"...","valor":"...","segundos":N} e recebe uma resposta
// {"id","acao","ok"[,"erro"]} em cmd/resposta. As acoes repetem o que os botoes fazem na placa.
void publishCmdResponse(const char *id, const char *action, bool ok, const char *error);

constexpr long NetworkOffMaxSeconds = 3600;

// Executa um comando do dashboard e responde sempre. true = executado.
[[maybe_unused]] bool handleDashboardCommand(const char *payload, size_t length) {
  lastInteractionAt = millis();  // qualquer comando, mesmo recusado, acende o LCD como um botao
  char id[DashboardCmd::MaxId + 1] = "";
  char action[DashboardCmd::MaxText + 1] = "";
  char value[DashboardCmd::MaxText + 1] = "";
  const auto fail = [&](const char *error) {
    publishCmdResponse(id, action, false, error);
    return false;
  };
  if (length == 0) return fail("json_vazio");
  if (length > DashboardCmd::MaxPayload) return fail("payload_grande");
  DashboardCmd::textValue(payload, length, "id", id, sizeof(id));
  if (!DashboardCmd::textValue(payload, length, "acao", action, sizeof(action))) return fail("acao_ausente");
  DashboardCmd::textValue(payload, length, "valor", value, sizeof(value));
  const uint32_t now = millis();
  const uint64_t clock = clockMs();
  const auto is = [&](const char *name) { return std::strcmp(action, name) == 0; };
  const bool known = is("avancar") || is("voltar") || is("ligar") || is("desligar") || is("autorizar") ||
                     is("ciclo_iniciar") || is("vincular") || is("ciclo_encerrar") || is("lacre") || is("rede") ||
                     is("remessa") || is("nf_retorno");
  if (!known) return fail("acao_desconhecida");
  if (!poweredOn && !is("ligar")) return fail("dispositivo_desligado");

  if (is("avancar") || is("voltar")) {
    lastInteractionAt = now;  // acende o LCD, como qualquer botao
    if (is("avancar")) screens.next();
    else screens.previous();
  } else if (is("ligar") || is("desligar")) {
    if (!Config::SwitchButtons) return fail("somente_modo_botoes");
    powerOn = is("ligar");  // loop() registra DISPOSITIVO_DESLIGADO e o retorno
  } else if (is("autorizar")) {
    registerAuthorization(clock);
  } else if (is("ciclo_iniciar") || is("vincular")) {
    if (cycleActive) return fail("ciclo_ativo");
    requestLink(clock);
  } else if (is("ciclo_encerrar")) {
    const bool wasActive = cycleActive;
    requestClose();  // recusa sozinho: sem ciclo, lacre aberto ou sem NF de retorno
    if (!wasActive) return fail("sem_ciclo_ativo");
    if (cycleActive) return fail("encerramento_recusado");
  } else if (is("lacre")) {
    if (!Config::SwitchButtons) return fail("somente_modo_botoes");
    const bool toOpen = std::strcmp(value, "aberto") == 0;
    if (!toOpen && std::strcmp(value, "fechado") != 0) return fail("valor_invalido");
    if (toOpen == stableSeal) return fail(toOpen ? "lacre_ja_aberto" : "lacre_ja_fechado");
    sealTransition(toOpen, now, now, clock);
  } else if (is("remessa")) {
    String invoice = value;
    invoice.trim();
    invoice.toUpperCase();  // como na Serial, que converte a linha toda
    if (const char *error = registerShipmentInvoice(invoice)) return fail(error);
  } else if (is("nf_retorno")) {
    if (const char *error = registerReturnInvoice()) return fail(error);
  } else if (is("rede")) {
    if (!Config::SwitchButtons) return fail("somente_modo_botoes");
    if (std::strcmp(value, "ligada") == 0) {
      networkOn = true;
      networkRestoreAt = 0;
    } else if (std::strcmp(value, "desligada") == 0) {
      // Sem a rede o broker nao entrega mais comandos: o prazo religa sozinho.
      long seconds = 0;
      if (!DashboardCmd::intValue(payload, length, "segundos", 1, NetworkOffMaxSeconds, &seconds)) {
        return fail("segundos_invalido");
      }
      networkOn = false;
      networkRestoreAt = now + static_cast<uint32_t>(seconds) * 1000UL;
      if (networkRestoreAt == 0) networkRestoreAt = 1;
    } else {
      return fail("valor_invalido");
    }
  }
  publishCmdResponse(id, action, true, nullptr);
  return true;
}

// Resposta no Serial e, com MQTT, no topico cmd/resposta (QoS 0, nao retida).
String cmdResponseJson(const char *id, const char *action, bool ok, const char *error) {
  String json = "{\"id\":\"";
  json += id;
  json += "\",\"acao\":\"";
  json += action;
  json += "\",\"ok\":";
  json += ok ? "true" : "false";
  if (error) {
    json += ",\"erro\":\"";
    json += error;
    json += "\"";
  }
  json += "}";
  return json;
}

#if !MQTT_ENABLED
void publishCmdResponse(const char *id, const char *action, bool ok, const char *error) {
  Serial.println(String("[cmd] resposta ") + cmdResponseJson(id, action, ok, error));
}
#endif

#if MQTT_ENABLED
// Walking skeleton MQTT (card #122): conecta ao broker (padrao: HiveMQ no Docker, porta 1883,
// sem TLS e sem login; TLS com usuario/senha para HiveMQ Cloud por MQTT_TLS 1), publica `status`
// retido, assina `cmd` e mantem `online` retido com LWT. Contrato: secao 3.4.3, Artefato 8.
constexpr const char *TopicStatus = MQTT_TOPIC_PREFIX "status";
constexpr const char *TopicOnline = MQTT_TOPIC_PREFIX "online";
constexpr const char *TopicCmd = MQTT_TOPIC_PREFIX "cmd";
constexpr const char *TopicCmdResposta = MQTT_TOPIC_PREFIX "cmd/resposta";
constexpr const char *TopicEvento = MQTT_TOPIC_PREFIX "evento";
constexpr uint32_t MqttRetryMs = 10000;
constexpr uint32_t MqttStatusMs = 60000;
constexpr uint32_t MqttStatusMinMs = 1000;  // intervalo minimo entre dois `status` por evento
#if MQTT_TLS
WiFiClientSecure mqttNet;
#else
WiFiClient mqttNet;
#endif
PubSubClient mqtt(mqttNet);
uint32_t lastMqttTry = 0, lastMqttStatus = 0;
bool mqttWasConnected = false, mqttConfigured = false;

// Comando do dashboard: registra no Serial e executa (handleDashboardCommand responde em cmd/resposta).
void onMqttMessage(char *topic, uint8_t *payload, unsigned int length) {
  // O payload do PubSubClient nao termina em nulo: o texto e copiado com o tamanho exato.
  if (length == 0) {
    Serial.printf("[mqtt] cmd vazio em %s (0 bytes)\r\n", topic);
    handleDashboardCommand("", 0);
    return;
  }
  String text;
  text.concat(reinterpret_cast<const char *>(payload), length);
  Serial.printf("[mqtt] cmd recebido em %s (%u bytes): %s\r\n", topic, length, text.c_str());
  handleDashboardCommand(text.c_str(), text.length());
}

bool mqttConnected() { return mqtt.connected(); }

// Resposta ao comando: Serial e, com broker conectado, cmd/resposta (QoS 0, nao retida).
void publishCmdResponse(const char *id, const char *action, bool ok, const char *error) {
  const String json = cmdResponseJson(id, action, ok, error);
  Serial.println(String("[cmd] resposta ") + json);
  if (mqtt.connected()) mqtt.publish(TopicCmdResposta, json.c_str(), false);
}

// Publica o evento (QoS 0, nao retido); false = desconectado ou falha, e quem chama enfileira.
// beginPublish/write/endPublish enviam em partes, entao o JSON pode passar do buffer de 512 bytes.
bool publishMqttEvent(const char *json) {
  if (!mqtt.connected()) return false;
  const size_t length = strlen(json);
  bool sent = mqtt.beginPublish(TopicEvento, length, false);
  if (sent) sent = mqtt.write(reinterpret_cast<const uint8_t *>(json), length) == length;
  if (sent) sent = mqtt.endPublish();
  if (!sent) Serial.println("[mqtt] evento nao publicado");
  return sent;
}

// Estado resumido do dispositivo (retido): lacre, bateria, ciclo, NF, dias fora, GNSS e fila.
void publishMqttStatus() {
  String json;
  json.reserve(256);
  json = "{\"device_id\":\"";
  json += Config::DeviceId;
  json += "\",\"lacre\":\"";
  json += stableSeal ? "violado" : "integro";
  json += "\",\"bateria_pct\":";
  json += batteryPct;
  json += ",\"ciclo_ativo\":";
  json += cycleActive ? "true" : "false";
  json += ",\"nf\":\"";
  json += shipmentInvoice;
  json += "\",\"dias_fora\":";
  json += cycle.daysOut;
  json += ",\"gnss_fix\":";
  json += freshFix() ? "true" : "false";
  json += ",\"fila\":";
  json += pending.count;
  json += "}";
  const bool sent = mqtt.publish(TopicStatus, json.c_str(), true);
  Serial.printf("[mqtt] status %s (%u bytes)\r\n", sent ? "publicado" : "falhou", (unsigned)json.length());
}

// Liga o cliente MQTT ao broker; tentativa a cada MqttRetryMs, so com Wi-Fi e rede simulada.
void updateMqtt(uint32_t now) {
  if (!mqttConfigured) {
    mqttConfigured = true;
#if MQTT_TLS
#if defined(MQTT_TLS_INSECURE) && MQTT_TLS_INSECURE
    mqttNet.setInsecure();
#elif defined(MQTT_CA_CERT)
    mqttNet.setCACert(MQTT_CA_CERT);  // CA embutida (secrets.h), ex.: mqtt_ca_emqx.h
#else
    mqttNet.setCACertBundle(rootca_crt_bundle_start);
#endif
    mqttNet.setTimeout(5);  // segundos; o handshake TLS bloqueia o loop por pouco tempo
    configTime(0, 0, "pool.ntp.org", "time.google.com");  // TLS valida a data do certificado
#endif
    mqtt.setServer(MQTT_HOST, MQTT_PORT);
    mqtt.setBufferSize(512);  // o padrao de 256 bytes corta o JSON dos eventos
    mqtt.setKeepAlive(30);
    mqtt.setCallback(onMqttMessage);
  }
  if (WiFi.status() != WL_CONNECTED || !simulatedNetworkAvailable()) {
    if (mqtt.connected()) {
      // disconnect() e uma desconexao limpa e nao dispara o Last Will; sem isto o `online`
      // retido fica `online` enquanto a rede simulada esta desligada. Publica `offline` antes.
      mqtt.publish(TopicOnline, "offline", true);
      mqtt.disconnect();
    }
    mqttWasConnected = false;
    return;
  }
  if (mqtt.connected()) {
    mqtt.loop();
    // Refresco a cada MqttStatusMs, ou antes se um evento mudou o estado (com intervalo minimo).
    const uint32_t sinceStatus = now - lastMqttStatus;
    if (sinceStatus >= MqttStatusMs || (mqttStatusDirty && sinceStatus >= MqttStatusMinMs)) {
      lastMqttStatus = now;
      mqttStatusDirty = false;
      publishMqttStatus();
    }
    return;
  }
  if (mqttWasConnected) {
    Serial.println("[mqtt] desconectado");
    mqttWasConnected = false;
  }
  if (now - lastMqttTry < MqttRetryMs && lastMqttTry != 0) return;
  lastMqttTry = now;
  // Dois clientes com o mesmo clientId se derrubam no broker: MQTT_CLIENT_ID vem do secrets.h.
  const String clientId = MQTT_CLIENT_ID[0] ? String(MQTT_CLIENT_ID)
                                            : String("guaratracking-") + Config::DeviceId;
  // Usuario vazio = broker local sem autenticacao.
  const char *user = MQTT_USER[0] ? MQTT_USER : nullptr;
  const char *password = MQTT_USER[0] ? MQTT_PASSWORD : nullptr;
  if (!mqtt.connect(clientId.c_str(), user, password, TopicOnline, 1, true, "offline")) {
    Serial.printf("[mqtt] falha na conexao, estado=%d\r\n", mqtt.state());
    return;
  }
  mqttWasConnected = true;
  Serial.printf("[mqtt] conectado a %s:%d\r\n", MQTT_HOST, (int)MQTT_PORT);
  mqtt.publish(TopicOnline, "online", true);
  // O PubSubClient nao le o SUBACK: false = nao saiu; true = foi enviada, sem saber se a ACL aceitou.
  const bool cmdSubscribed = mqtt.subscribe(TopicCmd);
  Serial.printf("[mqtt] assinatura de cmd %s\r\n", cmdSubscribed ? "enviada ao broker" : "nao enviada");
  flushPending();  // eventos guardados sem broker saem antes do `status`, que entao mostra a fila
  lastMqttStatus = now;
  mqttStatusDirty = false;
  publishMqttStatus();
  showMessage("MQTT ok", MQTT_TLS ? "TLS" : "broker local");
}
#endif
}

// Configura pinos, LCD, GNSS, NVS e Wi-Fi, restaura o contexto e publica o STATUS inicial.
void setup() {
  Serial.begin(Config::ConsoleBaud);
  delay(200);
  Serial.println("\n[boot] ESP32 executando - prototipo Cerradinho");
  Serial.printf("[boot] uptime_inicio_ms=%lu reset_reason=%d\r\n",
                static_cast<unsigned long>(millis()), static_cast<int>(esp_reset_reason()));
  Serial.printf("[config] modo=%s intervalo=%lu s tolerancia=%.0f m\r\n",
                DEMO_MODE ? "demo" : "semanal", Config::SampleIntervalMs / 1000UL,
                Config::MovementThresholdM);
  pinMode(Config::SealPin, INPUT_PULLUP);
  pinMode(Config::AuthorizeButtonPin, INPUT_PULLUP);
  pinMode(Config::CycleButtonPin, INPUT_PULLUP);
  pinMode(Config::NetworkSwitchPin, INPUT_PULLUP);
  pinMode(Config::PowerSwitchPin, INPUT_PULLUP);
  pinMode(Config::NextButtonPin, INPUT_PULLUP);
  pinMode(Config::PreviousButtonPin, INPUT_PULLUP);
  ledcSetup(Config::BuzzerChannel, Config::BuzzerFrequencyHz, 8);
  ledcAttachPin(Config::BuzzerPin, Config::BuzzerChannel);
  ledcWrite(Config::BuzzerChannel, 0);
  initLcd();
  pinMode(Config::BatteryPin, INPUT);
  analogReadResolution(12);
  batteryPct = simulatedBatteryPercent();
  Serial2.begin(Config::GpsBaud, SERIAL_8N1, Config::GpsRx, Config::GpsTx);
  storageReady = storage.begin("tracker", false);
  double point[2] = {};
  if (storageReady && storage.isKey("previous") &&
      storage.getBytesLength("previous") == sizeof(point) &&
      storage.getBytes("previous", point, sizeof(point)) == sizeof(point) &&
      validCoordinates(point[0], point[1])) {
    previousLat = latestLat = point[0]; previousLon = latestLon = point[1];
    havePrevious = true;
    // O ponto salvo e referencia historica, nao fix novo nem idade conhecida.
  }
  Serial.printf("[memoria] NVS=%s; diagnostico limitado a %u registros; fila de eventos de %u\r\n",
                storageReady ? "ok" : "falhou", Config::QueueCapacity, EVENT_QUEUE_CAPACITY);
  WiFi.mode(WIFI_STA);
  WiFi.begin(TRACKER_WIFI_SSID, TRACKER_WIFI_PASSWORD);
  requestAt = lastSample = lastWifiRetry = millis();
  sealInit(&seal, (uint64_t)Config::AuthorizationWindowDays * Config::DayMs);
  queueInit(&pending);
  gnssInit(&gnssHealth);
  restorePending();
  batteryInit(&battery, Config::BatteryLowPct, Config::BatteryRecoverPct);
  networkWasAvailable = networkOnline();  // com MQTT, falso ate o broker conectar
  poweredOn = powerSwitchOn();
  StoredContext context{};
  const bool restored = restoreContext(context);
  if (restored) {
    simClockInit(&simClock, millis(), context.clockMs);
    cycle = context.cycle;
    cycleStartedAt = context.cycleStartedAt;
    cycleActive = context.cycleActive;
    returnInvoiceIssued = context.returnInvoiceIssued;
    closedCycles = context.closedCycles;
    seal = context.seal;
    rawSeal = stableSeal = context.sealOpen;
    memcpy(shipmentInvoice, context.invoice, sizeof(shipmentInvoice));
    shipmentInvoice[Config::InvoiceMaxLength] = '\0';
  } else {
    simClockInit(&simClock, millis(), 0);
    rawSeal = stableSeal = false;
    // Primeiro boot sem ciclo: o ciclo nasce com a remessa (RF002, RN02), entao so
    // comeca ao segurar CICLO por 3 s (vinculo, RF003); o proximo ciclo_id e o 1.
    cycleStart(&cycle, 0, Config::ExpectedReturnDays, Config::StayThresholdDays,
               Config::StatusIntervalDays);
    cycleActive = false;
    contextDirty = true;
  }
  sealChangedAt = millis();
  attachInterrupt(digitalPinToInterrupt(Config::SealPin), onSealEdge, CHANGE);
  saveContext();
  if (hasPending()) {
    printPending();
    // Fila restaurada com rede ja disponivel: reenvia sem esperar a chave mudar.
    if (poweredOn && networkWasAvailable) flushPending();
  }
  if (poweredOn) {
    publishEvent("STATUS", false, restored ? ",\"contexto_restaurado\":true" : ",\"contexto_restaurado\":false");
  }
  lastInteractionAt = millis();
  updateDisplay(millis(), clockMs());
  Serial.println("[boot] aguardando GPS; heartbeat aparece a cada 2 s");
  if (!cycleActive) Serial.println("[ciclo] sem ciclo ativo; segure CICLO por 3 s para iniciar e use REMESSA x para a NF");
#if MQTT_ENABLED
  Serial.println("[transporte] JSON na serial + fila local; envio ao broker MQTT");
#else
  Serial.println("[transporte] JSON na serial + fila local; MQTT desligado");
#endif
  Serial.printf("[ciclo] 1 dia = %lu s; status a cada %lu dias; prazo %lu; limiar %lu\r\n",
                Config::DayMs / 1000UL, (unsigned long)Config::StatusIntervalDays,
                (unsigned long)Config::ExpectedReturnDays,
                (unsigned long)Config::StayThresholdDays);
  Serial.printf("[regras] janela de autorizacao %lu dias; fila sem rede de %u eventos; bateria %u%%/%u%%\r\n",
                (unsigned long)Config::AuthorizationWindowDays, EVENT_QUEUE_CAPACITY,
                Config::BatteryLowPct, Config::BatteryRecoverPct);
  Serial.println("[comandos] VINCULAR = tenta vinculo local (RF004); NF = NF retorno; REMESSA x = NF remessa; LIMIAR n = limiar");
  Serial.printf("[interface] LCD 16x2 I2C 0x%02X; AVANCAR/VOLTAR trocam a tela; CICLO e AUTORIZAR exigem %lu s; LCD apaga apos %lu s sem uso\r\n",
                Config::LcdAddress, (unsigned long)(Config::LongPressMs / 1000UL),
                (unsigned long)(Config::LcdTimeoutMs / 1000UL));
  Serial.println("[bancada] SW_REDE simula disponibilidade sem desligar o Wi-Fi; SW_ENERGIA liga/desliga");
  if (Config::SwitchButtons) {
    Serial.println("[bancada] botoes: LACRE e REDE alternam a cada toque; ON/OFF segurar 3 s");
  }
}

// Uma volta: relogio, chave ON/OFF, LCD, buzzer, entradas, ciclo, NVS, Wi-Fi e coleta de posicao.
void loop() {
  const uint32_t now = millis();
  // O relogio do ciclo segue no desligamento logico da chave ON/OFF.
  const uint64_t clock = simClockUpdate(&simClock, now);
  // Comando `rede` desligada: a rede simulada volta sozinha no fim do prazo.
  if (networkRestoreAt != 0 && static_cast<int32_t>(now - networkRestoreAt) >= 0) {
    networkOn = true;
    networkRestoreAt = 0;
  }
  // Chave ON/OFF: desligar registra o evento e suspende o dispositivo.
  if (Config::SwitchButtons && holdCompleted(powerButton, now, "BTN_ENERGIA")) powerOn = !powerOn;
  const bool on = powerSwitchOn();
  if (on != poweredOn) {
    if (!on) publishEvent("DISPOSITIVO_DESLIGADO");
    poweredOn = on;
    if (on) {
      lastInteractionAt = now;
      wakeReferencePct = -100;  // variacao com a chave desligada nao troca a tela ao religar
      if (simulatedNetworkAvailable()) flushPending();  // fila restaurada durante o desligamento
      publishEvent("STATUS");
    }
  }
  updateDisplay(now, clock);
  updateBuzzer(now);
  if (!poweredOn) {
#if MQTT_ENABLED
    updateMqtt(now);  // desligado ainda escuta o `cmd`, para o dashboard poder ligar de novo
#endif
    return;
  }
  readSeal(now, clock);  // antes de I/O potencialmente bloqueante
  readGps();
  readButtons(now, clock);
  readBattery(now);
  readCommands();
  updateNetwork();
#if MQTT_ENABLED
  updateMqtt(now);
#endif
  updateCycle(clock);
  if (contextDirty && (!contextRetryPending ||
      static_cast<uint32_t>(now - lastContextFailureAt) >= 1000U)) saveContext();
  if (pendingDirty && (!pendingRetryPending ||
      static_cast<uint32_t>(now - lastPendingFailureAt) >= 1000U)) savePending();
  const bool connected = WiFi.status() == WL_CONNECTED;
  if (connected != wasConnected) {
    Serial.printf("[wifi] %s\r\n", connected ? "conectado" : "desconectado");
    wasConnected = connected;
  }
  if (!connected && now - lastWifiRetry >= Config::WifiRetryMs) {
    WiFi.reconnect(); lastWifiRetry = now;
  }
  if (!sampleRequested && now - lastSample >= Config::SampleIntervalMs) {
    sampleRequested = true; requestAt = now;
  }
  if (sampleRequested && (freshFix() || now - requestAt >= Config::GpsWaitMs)) {
    const bool valid = freshFix();
    samplePosition(valid);
    updateGnssHealth(valid, now, clock);
    sampleRequested = false; lastSample = now;
  }
  if (now - lastHeartbeat >= Config::HeartbeatMs) {
    lastHeartbeat = now;
    Serial.printf("[vivo] tempo=%lu s gps_chars=%lu fix=%s lacre=%s bateria_sim=%d%% wifi=%s rede_sim=%s autorizar=%d ciclo=%d avancar=%d voltar=%d tela=%u dias_fora=%lu fila=%u\r\n",
                  now / 1000UL, (unsigned long)gps.charsProcessed(),
                  freshFix() ? "valido" : "ausente",
                  stableSeal ? "violado" : "integro", batteryPct,
                  connected ? "on" : "off", simulatedNetworkAvailable() ? "on" : "off",
                  digitalRead(Config::AuthorizeButtonPin) == LOW,
                  digitalRead(Config::CycleButtonPin) == LOW,
                  digitalRead(Config::NextButtonPin) == LOW,
                  digitalRead(Config::PreviousButtonPin) == LOW,
                  (unsigned)screens.current() + 1, (unsigned long)cycle.daysOut, pending.count);
  }
}
