#include <Arduino.h>
#include <cstring>
#include <driver/gpio.h>
#include <esp_system.h>
#include <TinyGPS++.h>
#include <WiFi.h>
#include <Preferences.h>
#include <LiquidCrystal_I2C.h>
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
void IRAM_ATTR onSealEdge() {
  const uint8_t head = sealEdgeHead;
  const uint8_t next = static_cast<uint8_t>((head + 1U) % SealEdgeCapacity);
  if (next == sealEdgeTail) { ++sealEdgeOverruns; return; }
  sealEdges[head].at = millis();
  sealEdges[head].opened = gpio_get_level(static_cast<gpio_num_t>(Config::SealPin)) != 0;
  sealEdgeHead = next;
}
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
bool ledOn = false;
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
LiquidCrystal_I2C lcd(Config::LcdAddress, ScreenController::Columns, 2);
ScreenController screens;  // telas do LCD (classe propria, usada no setup e no loop)
ScreenController::Line shownTop = "", shownBottom = "";
bool lcdLit = true;
uint32_t lastInteractionAt = 0;  // ultimo botao ou mensagem; o LCD apaga apos LcdTimeoutMs

uint64_t clockMs() {
  return simClock.elapsedMs;
}

uint32_t simulatedDay(uint64_t clock) {
  return (uint32_t)(clock / Config::DayMs);
}

bool simulatedNetworkAvailable() {
  return digitalRead(Config::NetworkSwitchPin) == LOW;
}

bool powerSwitchOn() {
  return digitalRead(Config::PowerSwitchPin) == LOW;
}

bool freshFix() {
  return gps.location.isValid() && gps.location.age() <= Config::FixMaxAgeMs &&
         gps.satellites.isValid() && gps.satellites.value() > 0 &&
         validCoordinates(gps.location.lat(), gps.location.lng());
}

int simulatedBatteryPercent() {
  // Entrada sintetica 0..3,3 V. Nao ligar uma celula diretamente ao GPIO34.
  return constrain((analogRead(Config::BatteryPin) * 100L) / 4095L, 0L, 100L);
}

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

void savePrevious(double lat, double lon) {
  if (!storageReady) return;
  // Uma unica chave evita um par latitude/longitude parcialmente atualizado.
  const double point[2] = {lat, lon};
  if (storage.putBytes("previous", point, sizeof(point)) != sizeof(point)) {
    Serial.println("[memoria] falha ao salvar referencia entre reinicios");
  }
}

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

bool hasPending() {
  return pending.count > 0 || pending.overflowCount > 0;
}

void printPending() {
  Serial.printf("[FILA] %u %s", pending.count, pending.count == 1 ? "pendente" : "pendentes");
  if (pending.overflowCount > 0) {
    Serial.printf(" + resumo de %lu violacao(oes)", (unsigned long)pending.overflowCount);
  }
  Serial.print("\r\n");
}

// Com rede, o JSON sai no Serial (representa a mensagem MQTT). Sem rede, vai para a
// fila e sai na reconexao, na ordem e com o dia em que ocorreu (RF010; RN09).
void publishEvent(const char *type, bool critical = false, const String &extra = "") {
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
  if (simulatedNetworkAvailable()) {
    Serial.println(record);
    return;
  }
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

void flushPending() {
  if (!hasPending()) return;
  Serial.printf("[FILA] rede disponivel; reenviando %u evento(s) na ordem\r\n", pending.count);
  while (pending.count > 0) {
    Serial.println(pending.text[0]);
    queueRemove(&pending, 0);
  }
  // As violacoes do resumo ocorreram depois das guardadas na fila: saem por ultimo.
  if (pending.overflowCount > 0) {
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

bool displayAwake(uint32_t now) {
  return now - lastInteractionAt < Config::LcdTimeoutMs;
}

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
  const bool available = simulatedNetworkAvailable();
  if (available == networkWasAvailable) return;
  networkWasAvailable = available;
  char bottom[ScreenController::Columns + 1];
  if (available) {
    const unsigned sent = pending.count + (pending.overflowCount > 0 ? 1u : 0u);
    flushPending();
    snprintf(bottom, sizeof(bottom), "enviados: %u", sent);
    showMessage("Sinal ok", bottom);
  } else {
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

void readButtons(uint32_t now, uint64_t clock) {
  // Com o LCD apagado, o primeiro toque em avancar/voltar so acende o visor.
  const bool awake = displayAwake(now);
  if (updateButton(nextButton, now) && awake) screens.next();
  if (updateButton(previousButton, now) && awake) screens.previous();
  if (holdCompleted(authorizeButton, now, "BTN_AUTORIZAR")) {
    sealAuthorize(&seal, clock);
    contextDirty = true;
    String extra = ",\"janela_dias\":";
    extra += Config::AuthorizationWindowDays;
    publishEvent("AUTORIZACAO_REGISTRADA", false, extra);
    char bottom[ScreenController::Columns + 1];
    snprintf(bottom, sizeof(bottom), "vale %lu dias", (unsigned long)Config::AuthorizationWindowDays);
    showMessage("Autorizacao ok", bottom);
  }
  // Segurar 3 s inicia o ciclo quando nao ha ciclo ativo e encerra quando ha.
  if (holdCompleted(cycleButton, now, "BTN_CICLO")) {
    if (cycleActive) requestClose();
    else requestLink(clock);
  }
}

void readBattery(uint32_t now) {
  if (now - lastBatteryCheck < Config::BatteryCheckMs) return;
  lastBatteryCheck = now;
  const int raw = analogRead(Config::BatteryPin);
  batteryPct = simulatedBatteryPercent();
  const unsigned events = batteryUpdate(&battery, raw, batteryPct);
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

// Comandos no Monitor Serial: NF (lanca a NF de retorno), REMESSA x (NF da remessa,
// simulando a plataforma) e LIMIAR n (RN03).
void readCommands() {
  while (Serial.available()) {
    const char c = Serial.read();
    if (c != '\n' && c != '\r') { serialLine += c; continue; }
    serialLine.trim();
    serialLine.toUpperCase();
    if (serialLine == "NF") {
      returnInvoiceIssued = true;
      contextDirty = true;
      publishEvent("NF_RETORNO_LANCADA");
    } else if (serialLine.startsWith("REMESSA ")) {
      String invoice = serialLine.substring(8);
      invoice.trim();
      if (!cycleActive) {
        Serial.println("[comando] sem ciclo ativo; inicie o ciclo antes da REMESSA");
      } else if (!validInvoice(invoice)) {
        Serial.printf("[comando] NF invalida; use ate %u letras, digitos, '-', '.' ou '/'\r\n",
                      Config::InvoiceMaxLength);
      } else {
        snprintf(shipmentInvoice, sizeof(shipmentInvoice), "%s", invoice.c_str());
        contextDirty = true;
        Serial.printf("[comando] NF da remessa registrada: %s\r\n", shipmentInvoice);
        showMessage("NF registrada", shipmentInvoice);
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
      Serial.println("[comando] use NF, REMESSA x ou LIMIAR n");
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
void readSeal(uint32_t now, uint64_t clock) {
  SealEdge edge{};
  SealDebounceEvent event{};
  while (popSealEdge(edge)) {
    if (seal_on_edge(rawSeal, stableSeal, sealChangedAt,
                     edge.opened, edge.at, Config::SealDebounceMs, event)) {
      sealTransition(event.opened, event.confirmedAt, now, clock);
    }
  }
  // Reconcilia o nivel corrente se uma borda ocorreu antes da ativacao da ISR.
  const bool actual = digitalRead(Config::SealPin) == HIGH;
  if (actual != rawSeal && seal_on_edge(rawSeal, stableSeal, sealChangedAt,
                                        actual, now, Config::SealDebounceMs, event)) {
    sealTransition(event.opened, event.confirmedAt, now, clock);
  }
  if (seal_settle(rawSeal, stableSeal, sealChangedAt, now, Config::SealDebounceMs, event)) {
    sealTransition(event.opened, event.confirmedAt, now, clock);
  }
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
  if (on) tone(Config::BuzzerPin, Config::BuzzerFrequencyHz);
  else noTone(Config::BuzzerPin);
}

// LED vermelho: pulso curto a cada segundo enquanto o lacre segue violado.
void updateLed(uint32_t now) {
  const bool on = poweredOn && seal.violated && now % Config::LedBlinkPeriodMs < Config::LedBlinkOnMs;
  if (on == ledOn) return;
  ledOn = on;
  digitalWrite(Config::ViolationLedPin, on ? HIGH : LOW);
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
    lcd.backlight();
    lcdLit = true;
  }
  ScreenController::Line top, bottom;
  uint32_t remaining = 0;
  if (holdRemaining(cycleButton, now, remaining)) {
    ScreenController::renderHold(cycleActive ? "Encerrar ciclo" : "Iniciar ciclo", remaining,
                                 top, bottom);
  } else if (holdRemaining(authorizeButton, now, remaining)) {
    ScreenController::renderHold("Autorizar lacre", remaining, top, bottom);
  } else {
    const StatusView view{shipmentInvoice, cycleActive, cycle.daysOut,
                          cycle.expectedReturnDays, haveLatest || havePrevious, haveLatest,
                          (uint32_t)((now - latestAt) / 1000UL), batteryPct, battery.lowSent,
                          battery.failureSent, sealView(clock), stableSeal};
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
}

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
  pinMode(Config::BuzzerPin, OUTPUT);
  digitalWrite(Config::BuzzerPin, LOW);
  pinMode(Config::ViolationLedPin, OUTPUT);
  digitalWrite(Config::ViolationLedPin, LOW);
  lcd.init();
  lcd.backlight();
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
  networkWasAvailable = simulatedNetworkAvailable();
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
    startCycle(clockMs(), 1);
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
  Serial.println("[transporte] JSON na serial + fila local; envio HTTP/MQTT pendente");
  Serial.printf("[ciclo] 1 dia = %lu s; status a cada %lu dias; prazo %lu; limiar %lu\r\n",
                Config::DayMs / 1000UL, (unsigned long)Config::StatusIntervalDays,
                (unsigned long)Config::ExpectedReturnDays,
                (unsigned long)Config::StayThresholdDays);
  Serial.printf("[regras] janela de autorizacao %lu dias; fila sem rede de %u eventos; bateria %u%%/%u%%\r\n",
                (unsigned long)Config::AuthorizationWindowDays, EVENT_QUEUE_CAPACITY,
                Config::BatteryLowPct, Config::BatteryRecoverPct);
  Serial.println("[comandos] NF = lanca NF de retorno; REMESSA x = NF da remessa; LIMIAR n = muda o limiar de permanencia");
  Serial.printf("[interface] LCD 16x2 I2C 0x%02X; AVANCAR/VOLTAR trocam a tela; CICLO e AUTORIZAR exigem %lu s; LCD apaga apos %lu s sem uso\r\n",
                Config::LcdAddress, (unsigned long)(Config::LongPressMs / 1000UL),
                (unsigned long)(Config::LcdTimeoutMs / 1000UL));
  Serial.println("[bancada] SW_REDE simula disponibilidade sem desligar o Wi-Fi; SW_ENERGIA liga/desliga");
}

void loop() {
  const uint32_t now = millis();
  // O relogio do ciclo segue no desligamento logico da chave ON/OFF.
  const uint64_t clock = simClockUpdate(&simClock, now);
  // Chave ON/OFF: desligar registra o evento e suspende o dispositivo.
  const bool on = powerSwitchOn();
  if (on != poweredOn) {
    if (!on) publishEvent("DISPOSITIVO_DESLIGADO");
    poweredOn = on;
    if (on) {
      lastInteractionAt = now;
      if (simulatedNetworkAvailable()) flushPending();  // fila restaurada durante o desligamento
      publishEvent("STATUS");
    }
  }
  updateDisplay(now, clock);
  updateBuzzer(now);
  updateLed(now);
  if (!poweredOn) return;
  readSeal(now, clock);  // antes de I/O potencialmente bloqueante
  readGps();
  readButtons(now, clock);
  readBattery(now);
  readCommands();
  updateNetwork();
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
