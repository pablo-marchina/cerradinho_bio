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
#include "indicator_controller.h"
#include "queue_snapshot.h"
#include "seal_debounce.h"
#include "transport_ack.h"
#include <driver/gpio.h>
#include <esp_system.h>

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
IndicatorController indicators;  // classe do grupo utilizada por setup()/loop()
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
bool discardingSerialLine = false;
bool contextDirty = false;  // ciclo/lacre mudou e precisa ir para a NVS
bool pendingDirty = false;  // mantido ate putBytes verificado
bool degradedMode = false;  // falha irrecuperavel: nao reabrir ciclo nem sobrescrever evidencias
uint32_t lastPersistenceRetry = 0;
// ACK exclusivamente de bancada via Serial. Nao e MQTT, HTTP, nem servidor real.
bool awaitingSimAck = false;
bool simAckIsSummary = false;
uint32_t simAckToken = 0;
uint32_t simAckLastSentAt = 0;
constexpr uint32_t SimAckRetryMs = 3000;
uint32_t pendingGeneration = 0;
int8_t pendingSlot = -1;
QueuePersistence::Snapshot queueIo{};
QueuePersistence::Snapshot queueBest{};

// ISR apenas captura bordas; nunca bloqueia, escreve NVS ou usa Serial.
// O loop reconstrói transições estáveis a partir dos timestamps de cada borda.
constexpr uint8_t SealEdgeCapacity = 64;
struct SealEdge { uint32_t at; bool opened; };
volatile SealEdge sealEdges[SealEdgeCapacity] = {};
volatile uint8_t sealEdgeHead = 0, sealEdgeTail = 0;
volatile uint32_t sealEdgeOverruns = 0;

void IRAM_ATTR onSealEdge() {
  const uint8_t head = sealEdgeHead;
  const uint8_t next = (uint8_t)((head + 1U) % SealEdgeCapacity);
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
  sealEdgeTail = (uint8_t)((tail + 1U) % SealEdgeCapacity);
  interrupts();
  return true;
}


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
  indicators.display(poweredOn, seal.violated, seal.removal_in_progress,
                     cycleActive && cycle_yellow_led(&cycle,
                         (now / Config::BlinkHalfPeriodMs) % 2 == 0),
                     simulatedNetworkAvailable(), now);
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
constexpr uint32_t ContextMagic = 0x47544331UL;  // 'GTC1'
struct ContextSnapshot {
  uint32_t magic;
  uint32_t generation;
  StoredContext context;
  uint32_t checksum;
};
uint32_t contextGeneration = 0;
int8_t contextSlot = -1;
ContextSnapshot contextIo{};

bool contextValid(const ContextSnapshot &snapshot) {
  return snapshot.magic == ContextMagic &&
      snapshot.context.version == ContextVersion &&
      snapshot.context.dayMs == Config::DayMs &&
      snapshot.checksum == QueuePersistence::crc32(
          reinterpret_cast<const uint8_t *>(&snapshot), offsetof(ContextSnapshot, checksum));
}

void saveContext() {
  contextDirty = true;
  if (!storageReady || degradedMode) return;
  const StoredContext context{ContextVersion, Config::DayMs, clockMs(), cycle, cycleStartedAt,
                              cycleActive, returnInvoiceIssued, closedCycles, seal, stableSeal};
  memset(&contextIo, 0, sizeof(contextIo));
  contextIo.magic = ContextMagic;
  contextIo.generation = contextGeneration + 1;
  contextIo.context = context;
  contextIo.checksum = QueuePersistence::crc32(
      reinterpret_cast<const uint8_t *>(&contextIo), offsetof(ContextSnapshot, checksum));
  const int8_t target = contextSlot == 0 ? 1 : 0;
  const char *key = target == 0 ? (DEMO_MODE ? "ctxd0" : "ctxw0")
                                  : (DEMO_MODE ? "ctxd1" : "ctxw1");
  if (storage.putBytes(key, &contextIo, sizeof(contextIo)) != sizeof(contextIo) ||
      storage.getBytesLength(key) != sizeof(contextIo) ||
      storage.getBytes(key, &contextIo, sizeof(contextIo)) != sizeof(contextIo) ||
      !contextValid(contextIo)) {
    Serial.printf("[memoria] falha ao salvar contexto %s; anterior preservado\r\n", key);
    return;
  }
  contextGeneration = contextIo.generation;
  contextSlot = target;
  contextDirty = false;
}

bool restoreContext(StoredContext &context) {
  if (!storageReady) return false;
  bool foundSlot = false, restored = false;
  ContextSnapshot selected{};
  for (int8_t slot = 0; slot < 2; ++slot) {
    const char *key = slot == 0 ? (DEMO_MODE ? "ctxd0" : "ctxw0")
                                  : (DEMO_MODE ? "ctxd1" : "ctxw1");
    if (!storage.isKey(key)) continue;
    foundSlot = true;
    if (storage.getBytesLength(key) != sizeof(contextIo) ||
        storage.getBytes(key, &contextIo, sizeof(contextIo)) != sizeof(contextIo) ||
        !contextValid(contextIo)) {
      Serial.printf("[memoria] ALERTA: contexto %s invalido; ignorado\r\n", key);
      continue;
    }
    if (!restored || QueuePersistence::newer(contextIo.generation, contextGeneration)) {
      selected = contextIo;
      contextGeneration = contextIo.generation;
      contextSlot = slot;
      restored = true;
    }
  }
  if (restored) {
    context = selected.context;
    return true;
  }
  if (foundSlot) {
    Serial.println("[memoria] ERRO: nao ha contexto integro; operacao bloqueada para preservar evidencias");
    degradedMode = true;
    return false;
  }
  // Migracao somente de NVS anterior ao patch; nao sobrepor snapshots invalidos.
  if (storage.isKey("ctx") && storage.getBytesLength("ctx") == sizeof(context) &&
      storage.getBytes("ctx", &context, sizeof(context)) == sizeof(context) &&
      context.version == ContextVersion && context.dayMs == Config::DayMs) {
    Serial.println("[memoria] contexto legado recuperado; migra na proxima gravacao");
    contextDirty = true;
    return true;
  }
  return false;
}

// Persistencia copy-on-write: um snapshot versionado e validado com CRC32 e
// escrito no slot inativo. Nunca sobrescrevemos o ultimo slot integro antes
// de confirmar integralmente o seguinte. Slots "d" e "w" isolam demo/weekly.
// Os dados antigos p0..p7 sao migrados apenas na ausencia dos novos snapshots.
// Nao ha transacao atomica entre evento local e transporte externo.
struct LegacyQueueMeta {
  uint8_t count;
  bool critical[EVENT_QUEUE_CAPACITY];
  uint32_t lost;
  uint32_t overflowCount;
  uint64_t overflowFirstMs, overflowLastMs;
};

bool savePending() {
  pendingDirty = true;
  if (!storageReady || degradedMode) return false;
  const int8_t target = pendingSlot == 0 ? 1 : 0;
  const char *key = target == 0 ? (DEMO_MODE ? "pqd0" : "pqw0")
                                  : (DEMO_MODE ? "pqd1" : "pqw1");
  QueuePersistence::prepare(queueIo, pending, pendingGeneration + 1);
  if (storage.putBytes(key, &queueIo, sizeof(queueIo)) != sizeof(queueIo) ||
      storage.getBytesLength(key) != sizeof(queueIo) ||
      storage.getBytes(key, &queueIo, sizeof(queueIo)) != sizeof(queueIo) ||
      !QueuePersistence::valid(queueIo)) {
    Serial.printf("[memoria] falha no snapshot %s; anterior preservado; nova tentativa pendente\r\n", key);
    return false;
  }
  pendingGeneration = queueIo.generation;
  pendingSlot = target;
  pendingDirty = false;
  return true;
}

void restoreLegacyPending() {
  LegacyQueueMeta meta{};
  if (!storage.isKey("pmeta") || storage.getBytesLength("pmeta") != sizeof(meta) ||
      storage.getBytes("pmeta", &meta, sizeof(meta)) != sizeof(meta) ||
      meta.count > EVENT_QUEUE_CAPACITY) return;
  bool incomplete = false;
  for (uint8_t i = 0; i < meta.count; ++i) {
    const String item = storage.getString(("p" + String(i)).c_str(), "");
    if (item.length() == 0) { incomplete = true; continue; }
    queue_push(&pending, item.c_str(), meta.critical[i], 0);
  }
  pending.lost = meta.lost;
  pending.overflow_count = meta.overflowCount;
  pending.overflow_first_ms = meta.overflowFirstMs;
  pending.overflow_last_ms = meta.overflowLastMs;
  if (incomplete) Serial.println("[memoria] ALERTA: fila legada tem lacunas; integridade historica indeterminada");
  Serial.printf("[memoria] migrando fila legada: %u registro(s) recuperado(s)\r\n", pending.count);
  savePending();
}

void restorePending() {
  if (!storageReady) {
    Serial.println("[memoria] ALERTA: persistencia indisponivel");
    return;
  }
  bool hasSnapshot = false;
  bool validSnapshot = false;
  for (int8_t slot = 0; slot < 2; ++slot) {
    const char *key = slot == 0 ? (DEMO_MODE ? "pqd0" : "pqw0")
                                  : (DEMO_MODE ? "pqd1" : "pqw1");
    if (!storage.isKey(key)) continue;
    hasSnapshot = true;
    if (storage.getBytesLength(key) != sizeof(queueIo) ||
        storage.getBytes(key, &queueIo, sizeof(queueIo)) != sizeof(queueIo) ||
        !QueuePersistence::valid(queueIo)) {
      Serial.printf("[memoria] ALERTA: snapshot %s invalido/incompleto; ignorado\r\n", key);
      continue;
    }
    if (!validSnapshot || QueuePersistence::newer(queueIo.generation, pendingGeneration)) {
      queueBest = queueIo;
      pendingGeneration = queueIo.generation;
      pendingSlot = slot;
      validSnapshot = true;
    }
  }
  if (validSnapshot) {
    pending = queueBest.queue;
    Serial.printf("[memoria] fila restaurada: snapshot %d geracao %lu, %u evento(s)\r\n",
                  pendingSlot, (unsigned long)pendingGeneration, pending.count);
  } else if (hasSnapshot) {
    Serial.println("[memoria] ERRO: todos os snapshots invalidos; operacao bloqueada para preservar evidencias");
    degradedMode = true;
  } else {
    restoreLegacyPending();
  }
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

// Fila persistida ANTES de qualquer transmissao simulada. Nesta Sprint o
// "receptor" e o operador que digita ACK <token> na Serial; essa resposta NAO
// valida RF009/MQTT nem prova recebimento por um servidor.
void flushPending();

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
  const int result = queue_push(&pending, record.c_str(), critical, clockMs());
  if (result == QUEUE_SUMMARIZED) {
    Serial.printf("[FILA] resumo de violacoes +1 (%lu agregadas), payloads individuais excedentes nao guardados\r\n",
                  (unsigned long)pending.overflow_count);
  } else if (result == QUEUE_REJECTED) {
    Serial.printf("[FILA] DESCARTADO nao critico por falta de capacidade (%lu)\r\n",
                  (unsigned long)pending.lost);
  } else if (result == QUEUE_STORED_DROPPED_OLDEST) {
    Serial.println("[FILA] DESCARTADO nao critico mais antigo; eventos criticos priorizados");
  } else {
    Serial.printf("[FILA] enfileirado: %s\r\n", type);
  }
  // Pode haver troca do registro na cabeca por descarte; token antigo nao deve
  // jamais confirmar novo conteudo.
  if (awaitingSimAck && pending.count > 0 &&
      !simAckIsSummary && TransportAck::token(pending.text[0]) != simAckToken) {
    awaitingSimAck = false;
  }
  if (!savePending()) {
    Serial.println("[FILA] falha NVS: nao transmitido; RAM pode se perder com reset");
    return;
  }
  printPending();
  if (simulatedNetworkAvailable() && poweredOn) flushPending();
}

String currentSummaryRecord() {
  String extra = ",\"quantidade\":";
  extra += pending.overflow_count;
  extra += ",\"dia_primeira\":"; extra += simulatedDay(pending.overflow_first_ms);
  extra += ",\"dia_ultima\":"; extra += simulatedDay(pending.overflow_last_ms);
  // snprintf com %llu evita depender de construtor String(uint64_t), que
  // pode mudar entre versoes do core Arduino ESP32.
  char timestampText[24];
  snprintf(timestampText, sizeof(timestampText), "%llu",
           static_cast<unsigned long long>(pending.overflow_first_ms));
  extra += ",\"primeira_ms\":"; extra += timestampText;
  snprintf(timestampText, sizeof(timestampText), "%llu",
           static_cast<unsigned long long>(pending.overflow_last_ms));
  extra += ",\"ultima_ms\":"; extra += timestampText;
  // Envelope deterministico: campos de telemetria dependentes do momento
  // fariam o token mudar entre retransmissoes.
  String record = "{\"device_id\":\"";
  record += Config::DeviceId;
  record += "\",\"type\":\"VIOLACAO_LACRE_RESUMO\"";
  record += ",\"critico\":true";
  record += extra;
  record += "}";
  return record;
}

void flushPending() {
  if (!hasPending() || !poweredOn || !simulatedNetworkAvailable() || degradedMode) return;
  if (pendingDirty && !savePending()) return;
  const bool summary = pending.count == 0;
  // Resumo e construido com o tempo apenas dos eventos agregados; o token
  // precisa permanecer estavel entre retentativas e nao pode depender do uptime.
  const String record = summary ? currentSummaryRecord() : String(pending.text[0]);
  const uint32_t token = TransportAck::token(record.c_str());
  const uint32_t now = millis();
  if (awaitingSimAck && token == simAckToken && summary == simAckIsSummary &&
      (uint32_t)(now - simAckLastSentAt) < SimAckRetryMs) return;
  // Sem ACK a mensagem e repetida; aplicacoes reais precisam desduplicar por ID.
  awaitingSimAck = true;
  simAckIsSummary = summary;
  simAckToken = token;
  simAckLastSentAt = now;
  Serial.printf("[TX_SIM] token=%08lX tipo=%s (sem confirmacao remota)\r\n",
                (unsigned long)token, summary ? "resumo" : "evento");
  Serial.println(record);
  Serial.printf("[TX_SIM] para simular recebimento: ACK %08lX\r\n", (unsigned long)token);
}

void acknowledgeSim(const String &arg) {
  if (!awaitingSimAck || !simulatedNetworkAvailable()) {
    Serial.println("[ACK_SIM] RECUSADO: nao ha envio aguardando com rede simulada");
    return;
  }
  uint32_t token = 0;
  if (!TransportAck::parseHex8(arg.c_str(), token)) {
    Serial.println("[ACK_SIM] RECUSADO: token deve conter oito hexadecimais");
    return;
  }
  if (token != simAckToken) {
    Serial.println("[ACK_SIM] RECUSADO: token incorreto; fila mantida");
    return;
  }
  const String current = simAckIsSummary ? currentSummaryRecord() :
                         (pending.count > 0 ? String(pending.text[0]) : String());
  if (current.length() == 0 || TransportAck::token(current.c_str()) != token) {
    Serial.println("[ACK_SIM] RECUSADO: fila alterada desde emissao");
    awaitingSimAck = false;
    return;
  }
  if (simAckIsSummary) queue_clear_summary(&pending);
  else queue_remove(&pending, 0);
  awaitingSimAck = false;
  const bool stored = savePending();
  Serial.printf("[ACK_SIM] confirmado manualmente; snapshot=%s (NAO e ACK MQTT)\r\n",
                stored ? "ok" : "falhou: possivel duplicata ao reiniciar");
  printPending();
  if (stored) flushPending();
}

void updateNetwork() {
  const bool available = simulatedNetworkAvailable();
  if (available && !networkWasAvailable) flushPending();
  if (!available) awaitingSimAck = false;  // proximo envio sera uma retransmissao
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
    cycleActive = false;
    cycle.days_out = 0;
    contextDirty = true;
    String extra = ",\"ciclos_encerrados\":";
    extra += closedCycles;
    // O evento deve retratar o estado ja encerrado, inclusive quando fica na fila.
    publishEvent("CICLO_ENCERRADO", false, extra);
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
    if (c != '\n' && c != '\r') {
      if (discardingSerialLine) continue;
      if (serialLine.length() < 96) serialLine += c;
      else { serialLine = ""; discardingSerialLine = true;
             Serial.println("[comando] linha excedeu 96 caracteres; ignorada"); }
      continue;
    }
    if (discardingSerialLine) { discardingSerialLine = false; serialLine = ""; continue; }
    serialLine.trim();
    serialLine.toUpperCase();
    if (serialLine == "NF") {
      returnInvoiceIssued = true;
      contextDirty = true;
      publishEvent("NF_RETORNO_LANCADA");
    } else if (serialLine.startsWith("ACK ")) {
      acknowledgeSim(serialLine.substring(4));
    } else if (serialLine == "PENDENTES") {
      printPending();
      if (awaitingSimAck) Serial.printf("[ACK_SIM] aguardando %08lX\r\n", (unsigned long)simAckToken);
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
      Serial.println("[comando] use NF, LIMIAR n, PENDENTES ou ACK XXXXXXXX");
    }
    serialLine = "";
  }
}

// A referencia e o ultimo estado estavel salvo (fechado na primeira inicializacao):
// ligar com o lacre aberto passa pelo mesmo caminho e gera a violacao.
void sealTransition(bool opened, uint32_t at) {
  stableSeal = opened;
  contextDirty = true;
  // Bordas podem continuar chegando enquanto eventos anteriores sao persistidos.
  const uint32_t observedAt = millis();
  const uint64_t observedClock = sim_clock_update(&simClock, observedAt);
  const uint32_t elapsed = (uint32_t)(observedAt - at);
  const uint64_t transitionClock = observedClock >= elapsed ? observedClock - elapsed : 0;
  const int event = opened ? seal_opened(&seal, transitionClock) : seal_closed(&seal);
  String record = baseRecord("seal_changed");
  record += ",\"edge_at_ms\":";
  record += at;
  record += ",\"authorized_removal\":";
  record += seal.removal_in_progress ? "true" : "false";
  record += "}";
  enqueue(record);
  if (event == SEAL_EVENT_VIOLATION) publishEvent("VIOLACAO_LACRE", true);
  if (event == SEAL_EVENT_REMOVAL) publishEvent("RETIRADA_AUTORIZADA");
  if (event == SEAL_EVENT_RESEALED) publishEvent("LACRE_REAPLICADO");
  if (event == SEAL_EVENT_CLOSED) publishEvent("LACRE_FECHADO");
}

void readSeal(uint32_t now) {
  // Processa timestamps capturados pela ISR, inclusive periodos estaveis que
  // ocorreram enquanto o loop fazia I/O bloqueante.
  SealEdge edge{};
  SealDebounceEvent confirmed{};
  while (popSealEdge(edge)) {
    if (seal_on_edge(rawSeal, stableSeal, sealChangedAt, edge.opened,
                     edge.at, Config::SealDebounceMs, confirmed)) {
      sealTransition(confirmed.opened, confirmed.confirmedAt);
    }
  }
  // Captura estado inicial ou borda extraviada pelo estouro do buffer.
  now = millis();
  const bool current = digitalRead(Config::SealPin) == HIGH;
  if (current != rawSeal &&
      seal_on_edge(rawSeal, stableSeal, sealChangedAt, current, now,
                   Config::SealDebounceMs, confirmed)) {
    sealTransition(confirmed.opened, confirmed.confirmedAt);
  }
  if (seal_settle(rawSeal, stableSeal, sealChangedAt, now,
                  Config::SealDebounceMs, confirmed)) {
    sealTransition(confirmed.opened, confirmed.confirmedAt);
  }
  static uint32_t reported = 0;
  noInterrupts();
  const uint32_t overrun = sealEdgeOverruns;
  interrupts();
  if (overrun != reported) {
    Serial.printf("[lacre] ALERTA: %lu borda(s) perdidas por overflow da ISR; reteste necessario\r\n",
                  (unsigned long)(overrun - reported));
    reported = overrun;
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
  indicators.begin();
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
  Serial.printf("[boot] reset_reason=%d uptime_ms=%lu (diagnostico de CT22)\r\n",
                (int)esp_reset_reason(), (unsigned long)millis());
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
    if (!degradedMode) startCycle(clockMs(), 1);
  }
  sealChangedAt = millis();
  // Ativar interrupcoes somente depois que a NVS e o estado logico existem.
  attachInterrupt(digitalPinToInterrupt(Config::SealPin), onSealEdge, CHANGE);
  saveContext();
  if (degradedMode) {
    Serial.println("[boot] MODO SEGURO: NVS inconsistente; sem novos vinculos ou remocoes. Inspecione slots antes de limpar flash.");
  }
  if (hasPending() && !degradedMode) {
    printPending();
    // Fila restaurada com rede ja disponivel: reenvia sem esperar a chave mudar.
    if (poweredOn && networkWasAvailable) flushPending();
  }
  if (poweredOn && !degradedMode) {
    publishEvent("STATUS", false, restored ? ",\"contexto_restaurado\":true" : ",\"contexto_restaurado\":false");
  }
  updateLeds(millis());
  Serial.println("[boot] aguardando GPS; heartbeat aparece a cada 2 s");
  Serial.println("[transporte] FIFO persistida + ACK manual serial; MQTT/HTTP e servidor pendentes");
  Serial.printf("[ciclo] 1 dia = %lu s; status a cada %lu dias; prazo %lu; limiar %lu\r\n",
                Config::DayMs / 1000UL, (unsigned long)Config::StatusIntervalDays,
                (unsigned long)Config::ExpectedReturnDays,
                (unsigned long)Config::StayThresholdDays);
  Serial.printf("[regras] janela de autorizacao %lu dias; fila sem rede de %u eventos; bateria %u%%/%u%%\r\n",
                (unsigned long)Config::AuthorizationWindowDays, EVENT_QUEUE_CAPACITY,
                Config::BatteryLowPct, Config::BatteryRecoverPct);
  Serial.println("[comandos] NF; LIMIAR n; PENDENTES; ACK XXXXXXXX (confirmacao manual, nao MQTT)");
  Serial.println("[bancada] SW_REDE simula disponibilidade sem desligar o Wi-Fi; SW_ENERGIA liga/desliga");
}

void loop() {
  const uint32_t now = millis();
  if (degradedMode) { updateLeds(now); return; }
  // O relogio do ciclo segue no desligamento logico da chave ON/OFF.
  // Antes de bloqueios de Serial/NVS, capturar periodos estaveis do lacre.

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
  // Captura timestamp atual para eventos ocorridos durante eventual I/O de boot.
  const uint32_t sealNow = millis();
  readSeal(sealNow);
  readGps();
  readButtons(now, clock);
  readBattery(now);
  readCommands();
  updateNetwork();
  updateCycle(clock);
  flushPending();  // retentativa limitada por tempo; sem descartar antes de ACK manual
  if ((contextDirty || pendingDirty) && now - lastPersistenceRetry >= 1000) {
    lastPersistenceRetry = now;
    if (contextDirty) saveContext();
    if (pendingDirty) savePending();
  }
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
