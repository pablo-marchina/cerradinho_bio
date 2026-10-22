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
CycleState cycle;
uint32_t cycleStartedAt = 0;
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

// Botao com debounce e deteccao por borda: segurar nao repete o evento.
struct Button {
  int pin;
  bool raw, pressed;
  uint32_t changedAt, pressedAt;
  bool longFired;
};
Button authorizeButton{Config::AuthorizeButtonPin, false, false, 0, 0, false};
Button cycleButton{Config::CycleButtonPin, false, false, 0, 0, false};

uint32_t simulatedDay(uint32_t now) {
  return now / Config::DayMs;
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
    Serial.println("[fila] armazenamento indisponivel; registro apenas na serial");
    return;
  }
  for (uint8_t index = 0; index < Config::QueueCapacity; ++index) {
    const String key = "q" + String(index);
    if (storage.isKey(key.c_str())) continue;
    if (storage.putString(key.c_str(), record) == record.length()) {
      Serial.println("[fila] registro local pendente; servidor ainda nao configurado");
    } else {
      Serial.println("[fila] falha ao gravar registro; registro disponivel na serial");
    }
    return;
  }
  Serial.println("[fila] limite de 16 registros; antigos preservados, novo apenas na serial");
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

void printPending() {
  Serial.printf("[FILA] %u %s\r\n", pending.count, pending.count == 1 ? "pendente" : "pendentes");
}

// Com rede, o JSON sai no Serial (representa a mensagem MQTT). Sem rede, vai para a
// fila e sai na reconexao, na ordem e com o dia em que ocorreu (RF010; RN09).
void publishEvent(const char *type, bool critical = false, const String &extra = "") {
  String record = baseRecord(type);
  record += ",\"ciclo_id\":"; record += cycle.cycle_id;
  record += ",\"ciclo_ativo\":"; record += cycleActive ? "true" : "false";
  record += ",\"dias_fora\":"; record += cycle.days_out;
  record += ",\"dia_ocorrencia\":"; record += simulatedDay(millis());
  record += ",\"atraso\":"; record += cycle_is_late(&cycle) ? "true" : "false";
  record += ",\"permanencia\":"; record += cycle_in_stay(&cycle) ? "true" : "false";
  record += ",\"critico\":"; record += critical ? "true" : "false";
  record += extra;
  record += "}";
  if (simulatedNetworkAvailable()) {
    Serial.println(record);
    return;
  }
  const int result = queue_push(&pending, record.c_str(), critical);
  if (result == QUEUE_REJECTED) {
    Serial.printf("[FILA] FILA_CHEIA: so ha eventos criticos; %s nao coube (perdidos=%lu)\r\n",
                  type, (unsigned long)pending.lost);
    return;
  }
  if (result == QUEUE_STORED_DROPPED_OLDEST) {
    Serial.println("[FILA] FILA_CHEIA: descartado o evento nao critico mais antigo");
  }
  Serial.printf("[FILA] guardado: %s\r\n", type);
  printPending();
}

void flushPending() {
  if (pending.count == 0) return;
  Serial.printf("[FILA] rede disponivel; reenviando %u evento(s) na ordem\r\n", pending.count);
  while (pending.count > 0) {
    Serial.println(pending.text[0]);
    queue_remove(&pending, 0);
  }
  printPending();
}

void updateNetwork() {
  const bool available = simulatedNetworkAvailable();
  if (available && !networkWasAvailable) flushPending();
  networkWasAvailable = available;
}

void startCycle(uint32_t now, uint32_t id) {
  cycle_start(&cycle, id, Config::ExpectedReturnDays, Config::StayThresholdDays,
              Config::StatusIntervalDays);
  cycleStartedAt = now;
  cycleActive = true;
  returnInvoiceIssued = false;
}

// Avanca um dia por vez para nao pular status nem alerta se o loop atrasar.
void updateCycle(uint32_t now) {
  if (!cycleActive) return;
  const uint32_t days = (now - cycleStartedAt) / Config::DayMs;
  while (cycle.days_out < days) {
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
  }
}

// Um dispositivo so pode estar em um ciclo ativo por vez (RF003, RF004; RN01).
void requestLink(uint32_t now) {
  if (cycleActive) {
    publishEvent("VINCULO_RECUSADO", false, ",\"motivo\":\"ciclo_ativo\"");
    return;
  }
  startCycle(now, cycle.cycle_id + 1);
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

void readButtons(uint32_t now) {
  if (updateButton(authorizeButton, now)) {
    seal_authorize(&seal, simulatedDay(now));
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
  if (wasPressed && !cycleButton.pressed && !cycleButton.longFired) requestLink(now);
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
      publishEvent("NF_RETORNO_LANCADA");
    } else if (serialLine.startsWith("LIMIAR ")) {
      const long days = serialLine.substring(7).toInt();
      if (days > 0) {
        cycle.stay_threshold_days = days;
        cycle.stay_sent = false;
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

void readSeal(uint32_t now) {
  const bool value = digitalRead(Config::SealPin) == HIGH;
  if (value != rawSeal) { rawSeal = value; sealChangedAt = now; }
  if (rawSeal != stableSeal && now - sealChangedAt >= Config::SealDebounceMs) {
    stableSeal = rawSeal;
    const int event = stableSeal ? seal_opened(&seal, simulatedDay(now)) : seal_closed(&seal);
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
  rawSeal = stableSeal = digitalRead(Config::SealPin) == HIGH;
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
  Serial.printf("[memoria] NVS=%s; fila limitada a %u registros\r\n",
                storageReady ? "ok" : "falhou", Config::QueueCapacity);
  WiFi.mode(WIFI_STA);
  WiFi.begin(TRACKER_WIFI_SSID, TRACKER_WIFI_PASSWORD);
  requestAt = lastSample = lastWifiRetry = millis();
  seal_init(&seal, Config::AuthorizationWindowDays);
  queue_init(&pending);
  battery_init(&battery, Config::BatteryLowPct, Config::BatteryRecoverPct);
  networkWasAvailable = simulatedNetworkAvailable();
  poweredOn = powerSwitchOn();
  startCycle(millis(), 1);
  if (poweredOn) publishEvent("STATUS");
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
  // Chave ON/OFF: desligar registra o evento e suspende o dispositivo.
  const bool on = powerSwitchOn();
  if (on != poweredOn) {
    if (!on) publishEvent("DISPOSITIVO_DESLIGADO");
    poweredOn = on;
    if (on) publishEvent("STATUS");
  }
  updateLeds(now);
  if (!poweredOn) return;
  readGps();
  readSeal(now);
  readButtons(now);
  readBattery(now);
  readCommands();
  updateNetwork();
  updateCycle(now);
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
