#include <Arduino.h>
#include <TinyGPS++.h>
#include <WiFi.h>
#include <Preferences.h>
#include "config.h"
#include "tracking_math.h"
#include "cycle_rules.h"
#include "seal_rules.h"
#include "event_queue.h"
#include "battery_rules.h"
#include "sim_clock.h"

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
bool poweredOn = true;
bool networkWasAvailable = true;
String serialLine;
bool contextDirty = false;  // ciclo/lacre mudou e precisa ir para a NVS

// Botao com debounce e deteccao por borda: segurar nao repete o evento.
struct Button {
  int pin;
  bool raw, pressed;
  uint32_t changedAt, pressedAt;
  bool longFired;
};
Button authorizeButton{Config::AuthorizeButtonPin, false, false, 0, 0, false};
Button cycleButton{Config::CycleButtonPin, false, false, 0, 0, false};

uint64_t clockMs() {
  return simClock.elapsed_ms;
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

void updateLeds(uint32_t now) {
  // Verde: operando, lacre integro (piscando = retirada autorizada em curso).
  // Amarelo: piscando = atraso, fixo = permanencia. Vermelho: violacao do lacre.
  // Azul: rede simulada indisponivel. Desligado: tudo apagado.
  const bool blinkOn = (now / Config::BlinkHalfPeriodMs) % 2 == 0;
  const bool green = !seal.violated && (!seal.removal_in_progress || blinkOn);
  digitalWrite(Config::GreenLedPin, poweredOn && green);
  digitalWrite(Config::YellowLedPin,
               poweredOn && cycleActive && cycle_yellow_led(&cycle, blinkOn));
  digitalWrite(Config::RedLedPin, poweredOn && seal.violated && blinkOn);
  digitalWrite(Config::BlueLedPin, poweredOn && !simulatedNetworkAvailable() && blinkOn);
}

bool freshFix() {
  return gps.location.isValid() && gps.location.age() <= Config::FixMaxAgeMs &&
         gps.satellites.isValid() && gps.satellites.value() > 0 &&
         valid_coordinates(gps.location.lat(), gps.location.lng());
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
    record += havePrevious ? String(distance_m(previousLat, previousLon, lat, lon), 2) : "null";
    record += ",\"moved\":";
    record += havePrevious ? (distance_m(previousLat, previousLon, lat, lon) >
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
};
constexpr uint32_t ContextVersion = 1;

void saveContext() {
  contextDirty = false;
  if (!storageReady) return;
  const StoredContext context{ContextVersion, Config::DayMs, clockMs(), cycle, cycleStartedAt,
                              cycleActive, returnInvoiceIssued, closedCycles, seal, stableSeal};
  if (storage.putBytes("ctx", &context, sizeof(context)) != sizeof(context)) {
    Serial.println("[memoria] falha ao salvar o contexto do ciclo");
  }
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

void savePending() {
  if (!storageReady) return;
  StoredQueueMeta meta{pending.count, {}, pending.lost, pending.overflow_count,
                       pending.overflow_first_ms, pending.overflow_last_ms};
  for (uint8_t i = 0; i < EVENT_QUEUE_CAPACITY; ++i) {
    const String key = "p" + String(i);
    if (i < pending.count) {
      meta.critical[i] = pending.critical[i];
      storage.putString(key.c_str(), pending.text[i]);
    } else if (storage.isKey(key.c_str())) {
      storage.remove(key.c_str());
    }
  }
  if (storage.putBytes("pmeta", &meta, sizeof(meta)) != sizeof(meta)) {
    Serial.println("[memoria] falha ao salvar a fila de eventos");
  }
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
    if (text.length() == 0) break;  // entrada ausente: preserva so as anteriores
    queue_push(&pending, text.c_str(), meta.critical[i], 0);
  }
  pending.lost = meta.lost;
  pending.overflow_count = meta.overflowCount;
  pending.overflow_first_ms = meta.overflowFirstMs;
  pending.overflow_last_ms = meta.overflowLastMs;
}

bool hasPending() {
  return pending.count > 0 || pending.overflow_count > 0;
}

void printPending() {
  Serial.printf("[FILA] %u %s", pending.count, pending.count == 1 ? "pendente" : "pendentes");
  if (pending.overflow_count > 0) {
    Serial.printf(" + resumo de %lu violacao(oes)", (unsigned long)pending.overflow_count);
  }
  Serial.print("\r\n");
}

// Com rede, o JSON sai no Serial (representa a mensagem MQTT). Sem rede, vai para a
// fila e sai na reconexao, na ordem e com o dia em que ocorreu (RF010; RN09).
void publishEvent(const char *type, bool critical = false, const String &extra = "") {
  String record = baseRecord(type);
  record += ",\"ciclo_id\":"; record += cycle.cycle_id;
  record += ",\"ciclo_ativo\":"; record += cycleActive ? "true" : "false";
  record += ",\"dias_fora\":"; record += cycle.days_out;
  record += ",\"dia_ocorrencia\":"; record += simulatedDay(clockMs());
  record += ",\"atraso\":"; record += cycle_is_late(&cycle) ? "true" : "false";
  record += ",\"permanencia\":"; record += cycle_in_stay(&cycle) ? "true" : "false";
  record += ",\"critico\":"; record += critical ? "true" : "false";
  record += extra;
  record += "}";
  if (simulatedNetworkAvailable()) {
    Serial.println(record);
    return;
  }
  const int result = queue_push(&pending, record.c_str(), critical, clockMs());
  if (result == QUEUE_SUMMARIZED) {
    Serial.printf("[FILA] FILA_CHEIA: so ha violacoes; %s somado ao resumo (%lu no resumo)\r\n",
                  type, (unsigned long)pending.overflow_count);
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
    queue_remove(&pending, 0);
  }
  // As violacoes do resumo ocorreram depois das guardadas na fila: saem por ultimo.
  if (pending.overflow_count > 0) {
    String extra = ",\"quantidade\":";
    extra += pending.overflow_count;
    extra += ",\"dia_primeira\":"; extra += simulatedDay(pending.overflow_first_ms);
    extra += ",\"dia_ultima\":"; extra += simulatedDay(pending.overflow_last_ms);
    extra += ",\"primeira_ms\":"; extra += String((unsigned long long)pending.overflow_first_ms);
    extra += ",\"ultima_ms\":"; extra += String((unsigned long long)pending.overflow_last_ms);
    publishEvent("VIOLACAO_LACRE_RESUMO", true, extra);
    queue_clear_summary(&pending);
  }
  savePending();
  printPending();
}

void updateNetwork() {
  const bool available = simulatedNetworkAvailable();
  if (available && !networkWasAvailable) flushPending();
  networkWasAvailable = available;
}

void startCycle(uint64_t clock, uint32_t id) {
  cycle_start(&cycle, id, Config::ExpectedReturnDays, Config::StayThresholdDays,
              Config::StatusIntervalDays);
  cycleStartedAt = clock;
  cycleActive = true;
  returnInvoiceIssued = false;
  contextDirty = true;
}

// Avanca um dia por vez para nao pular status nem alerta se o loop atrasar.
void updateCycle(uint64_t clock) {
  if (!cycleActive) return;
  const uint64_t days = (clock - cycleStartedAt) / Config::DayMs;
  while (cycle.days_out < days) {
    contextDirty = true;
    const unsigned events = cycle_advance_day(&cycle);
    if (events & CYCLE_EVENT_STATUS) publishEvent("STATUS_SEMANAL");
    if (events & CYCLE_EVENT_LATE) publishEvent("ALERTA_ATRASO");
    if (events & CYCLE_EVENT_STAY) publishEvent("ALERTA_PERMANENCIA");
  }
}

// Encerrar exige lacre fechado e NF de retorno; o historico e preservado (RF016; RN11).
void requestClose() {
  if (!cycleActive) {
    publishEvent("ENCERRAMENTO_RECUSADO", false, ",\"motivo\":\"sem_ciclo_ativo\"");
  } else if (stableSeal) {
    publishEvent("ENCERRAMENTO_RECUSADO", false, ",\"motivo\":\"lacre_aberto\"");
  } else if (!returnInvoiceIssued) {
    publishEvent("ENCERRAMENTO_RECUSADO", false, ",\"motivo\":\"nf_retorno_ausente\"");
  } else {
    ++closedCycles;
    String extra = ",\"ciclos_encerrados\":";
    extra += closedCycles;
    publishEvent("CICLO_ENCERRADO", false, extra);
    cycleActive = false;
    cycle.days_out = 0;
    contextDirty = true;
  }
}

// Um dispositivo so pode estar em um ciclo ativo por vez (RF003, RF004; RN01).
void requestLink(uint64_t clock) {
  if (cycleActive) {
    publishEvent("VINCULO_RECUSADO", false, ",\"motivo\":\"ciclo_ativo\"");
    return;
  }
  startCycle(clock, cycle.cycle_id + 1);
  publishEvent("CICLO_INICIADO");
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
    button.pressedAt = now;
    button.longFired = false;
  }
  return button.pressed;
}

void readButtons(uint32_t now, uint64_t clock) {
  if (updateButton(authorizeButton, now)) {
    seal_authorize(&seal, clock);
    contextDirty = true;
    String extra = ",\"janela_dias\":";
    extra += Config::AuthorizationWindowDays;
    publishEvent("AUTORIZACAO_REGISTRADA", false, extra);
  }
  const bool wasPressed = cycleButton.pressed;
  updateButton(cycleButton, now);
  // Segurar 2 s encerra; toque curto pede novo vinculo.
  if (cycleButton.pressed && !cycleButton.longFired &&
      now - cycleButton.pressedAt >= Config::LongPressMs) {
    cycleButton.longFired = true;
    requestClose();
  }
  if (wasPressed && !cycleButton.pressed && !cycleButton.longFired) requestLink(clock);
}

void readBattery(uint32_t now) {
  if (now - lastBatteryCheck < Config::BatteryCheckMs) return;
  lastBatteryCheck = now;
  const int raw = analogRead(Config::BatteryPin);
  const unsigned events = battery_update(&battery, raw, simulatedBatteryPercent());
  if (events & BATTERY_EVENT_SENSOR_FAILURE) publishEvent("SENSOR_BATERIA_FALHA");
  if (events & BATTERY_EVENT_LOW) publishEvent("BATERIA_BAIXA");
}

// Comandos no Monitor Serial: NF (lanca a NF de retorno) e LIMIAR n (RN03).
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
    } else if (serialLine.startsWith("LIMIAR ")) {
      const long days = serialLine.substring(7).toInt();
      if (days > 0) {
        // Nao rearma ALERTA_PERMANENCIA ja emitido neste ciclo (RN04).
        cycle_set_stay_threshold(&cycle, days);
        contextDirty = true;
        String extra = ",\"limiar_dias\":";
        extra += days;
        publishEvent("LIMIAR_ALTERADO", false, extra);
      }
    } else if (serialLine.length() > 0) {
      Serial.println("[comando] use NF ou LIMIAR n");
    }
    serialLine = "";
  }
}

// A referencia e o ultimo estado estavel salvo (fechado na primeira inicializacao):
// ligar com o lacre aberto passa pelo mesmo caminho e gera a violacao.
void readSeal(uint32_t now, uint64_t clock) {
  const bool value = digitalRead(Config::SealPin) == HIGH;
  if (value != rawSeal) { rawSeal = value; sealChangedAt = now; }
  if (rawSeal != stableSeal && now - sealChangedAt >= Config::SealDebounceMs) {
    stableSeal = rawSeal;
    contextDirty = true;
    const int event = stableSeal ? seal_opened(&seal, clock) : seal_closed(&seal);
    String record = baseRecord("seal_changed");
    record += ",\"authorized_removal\":";
    record += seal.removal_in_progress ? "true" : "false";
    record += "}";
    enqueue(record);
    // Violacao e critica e sai na hora, fora da rotina periodica (RF006; RN05).
    if (event == SEAL_EVENT_VIOLATION) publishEvent("VIOLACAO_LACRE", true);
    if (event == SEAL_EVENT_REMOVAL) publishEvent("RETIRADA_AUTORIZADA");
    if (event == SEAL_EVENT_RESEALED) publishEvent("LACRE_REAPLICADO");
    if (event == SEAL_EVENT_CLOSED) publishEvent("LACRE_FECHADO");
  }
}
}

void setup() {
  Serial.begin(Config::ConsoleBaud);
  delay(200);
  Serial.println("\n[boot] ESP32 executando - prototipo Cerradinho");
  Serial.printf("[config] modo=%s intervalo=%lu s tolerancia=%.0f m\r\n",
                DEMO_MODE ? "demo" : "semanal", Config::SampleIntervalMs / 1000UL,
                Config::MovementThresholdM);
  pinMode(Config::SealPin, INPUT_PULLUP);
  pinMode(Config::AuthorizeButtonPin, INPUT_PULLUP);
  pinMode(Config::CycleButtonPin, INPUT_PULLUP);
  pinMode(Config::NetworkSwitchPin, INPUT_PULLUP);
  pinMode(Config::PowerSwitchPin, INPUT_PULLUP);
  for (int pin : {Config::GreenLedPin, Config::YellowLedPin,
                  Config::RedLedPin, Config::BlueLedPin}) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  }
  pinMode(Config::BatteryPin, INPUT);
  analogReadResolution(12);
  Serial2.begin(Config::GpsBaud, SERIAL_8N1, Config::GpsRx, Config::GpsTx);
  storageReady = storage.begin("tracker", false);
  double point[2] = {};
  if (storageReady && storage.isKey("previous") &&
      storage.getBytesLength("previous") == sizeof(point) &&
      storage.getBytes("previous", point, sizeof(point)) == sizeof(point) &&
      valid_coordinates(point[0], point[1])) {
    previousLat = latestLat = point[0]; previousLon = latestLon = point[1];
    havePrevious = true;
    // O ponto salvo e referencia historica, nao fix novo nem idade conhecida.
  }
  Serial.printf("[memoria] NVS=%s; diagnostico limitado a %u registros; fila de eventos de %u\r\n",
                storageReady ? "ok" : "falhou", Config::QueueCapacity, EVENT_QUEUE_CAPACITY);
  WiFi.mode(WIFI_STA);
  WiFi.begin(TRACKER_WIFI_SSID, TRACKER_WIFI_PASSWORD);
  requestAt = lastSample = lastWifiRetry = millis();
  seal_init(&seal, (uint64_t)Config::AuthorizationWindowDays * Config::DayMs);
  queue_init(&pending);
  restorePending();
  battery_init(&battery, Config::BatteryLowPct, Config::BatteryRecoverPct);
  networkWasAvailable = simulatedNetworkAvailable();
  poweredOn = powerSwitchOn();
  StoredContext context{};
  const bool restored = restoreContext(context);
  if (restored) {
    sim_clock_init(&simClock, millis(), context.clockMs);
    cycle = context.cycle;
    cycleStartedAt = context.cycleStartedAt;
    cycleActive = context.cycleActive;
    returnInvoiceIssued = context.returnInvoiceIssued;
    closedCycles = context.closedCycles;
    seal = context.seal;
    rawSeal = stableSeal = context.sealOpen;
  } else {
    sim_clock_init(&simClock, millis(), 0);
    rawSeal = stableSeal = false;
    startCycle(clockMs(), 1);
  }
  sealChangedAt = millis();
  saveContext();
  if (hasPending()) {
    printPending();
    // Fila restaurada com rede ja disponivel: reenvia sem esperar a chave mudar.
    if (poweredOn && networkWasAvailable) flushPending();
  }
  if (poweredOn) {
    publishEvent("STATUS", false, restored ? ",\"contexto_restaurado\":true" : ",\"contexto_restaurado\":false");
  }
  updateLeds(millis());
  Serial.println("[boot] aguardando GPS; heartbeat aparece a cada 2 s");
  Serial.println("[transporte] JSON na serial + fila local; envio HTTP/MQTT pendente");
  Serial.printf("[ciclo] 1 dia = %lu s; status a cada %lu dias; prazo %lu; limiar %lu\r\n",
                Config::DayMs / 1000UL, (unsigned long)Config::StatusIntervalDays,
                (unsigned long)Config::ExpectedReturnDays,
                (unsigned long)Config::StayThresholdDays);
  Serial.printf("[regras] janela de autorizacao %lu dias; fila sem rede de %u eventos; bateria %u%%/%u%%\r\n",
                (unsigned long)Config::AuthorizationWindowDays, EVENT_QUEUE_CAPACITY,
                Config::BatteryLowPct, Config::BatteryRecoverPct);
  Serial.println("[comandos] NF = lanca NF de retorno; LIMIAR n = muda o limiar de permanencia");
  Serial.println("[bancada] SW_REDE simula disponibilidade sem desligar o Wi-Fi; SW_ENERGIA liga/desliga");
}

void loop() {
  const uint32_t now = millis();
  // O relogio do ciclo segue no desligamento logico da chave ON/OFF.
  const uint64_t clock = sim_clock_update(&simClock, now);
  // Chave ON/OFF: desligar registra o evento e suspende o dispositivo.
  const bool on = powerSwitchOn();
  if (on != poweredOn) {
    if (!on) publishEvent("DISPOSITIVO_DESLIGADO");
    poweredOn = on;
    if (on) {
      if (simulatedNetworkAvailable()) flushPending();  // fila restaurada durante o desligamento
      publishEvent("STATUS");
    }
  }
  updateLeds(now);
  if (!poweredOn) return;
  readGps();
  readSeal(now, clock);
  readButtons(now, clock);
  readBattery(now);
  readCommands();
  updateNetwork();
  updateCycle(clock);
  if (contextDirty) saveContext();
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
    samplePosition(freshFix());
    sampleRequested = false; lastSample = now;
  }
  if (now - lastHeartbeat >= Config::HeartbeatMs) {
    lastHeartbeat = now;
    Serial.printf("[vivo] tempo=%lu s gps_chars=%lu fix=%s lacre=%s bateria_sim=%d%% wifi=%s rede_sim=%s autorizar=%d ciclo=%d dias_fora=%lu fila=%u\r\n",
                  now / 1000UL, (unsigned long)gps.charsProcessed(),
                  freshFix() ? "valido" : "ausente",
                  stableSeal ? "violado" : "integro", simulatedBatteryPercent(),
                  connected ? "on" : "off", simulatedNetworkAvailable() ? "on" : "off",
                  digitalRead(Config::AuthorizeButtonPin) == LOW,
                  digitalRead(Config::CycleButtonPin) == LOW,
                  (unsigned long)cycle.days_out, pending.count);
  }
}
