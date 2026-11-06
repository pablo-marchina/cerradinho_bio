// Issue #113 / RF004-RN01: a entrada real de comando VINCULAR deve alcançar a recusa,
// sem chamar o helper diretamente ou alterar o ciclo ja ativo.
#include <cassert>
#include <cstdio>
#include <cstdint>
#include <string>
#define GUARA_HOST_INTEGRATION 1
#ifndef DEMO_MODE
#define DEMO_MODE 1
#endif
#include "../src/firmware/main.cpp"
FakeSerial Serial, Serial2;
int gpio_get_level(gpio_num_t p) { return digitalRead(static_cast<int>(p)); }
FakeWiFi WiFi;
int esp_reset_reason() { return 1; }
static uint32_t fakeMs = 1000;
static int fakePins[64] = {};
unsigned long millis() { return fakeMs; }
void delay(unsigned long v) { fakeMs += static_cast<uint32_t>(v); }
void pinMode(int, int) {}
void digitalWrite(int, int) {}
int digitalRead(int p) { return p >= 0 && p < 64 ? fakePins[p] : 0; }
int analogRead(int) { return 2048; }
void analogReadResolution(int) {}
void tone(int, unsigned int, unsigned long) {}
void noTone(int) {}
void noInterrupts() {}
void interrupts() {}
void attachInterrupt(int, void(*)(), int) {}
int digitalPinToInterrupt(int p) { return p; }
static void run(uint32_t ms) {
  for (uint32_t i = 0; i < ms; i += 5) { fakeMs += 5; loop(); }
}
static void command(const char *text) {
  Serial.inject(text);
  run(10);  // passa pelo loop() -> readCommands() -> requestLink()
}
static void hold(int pin, uint32_t ms) {
  fakePins[pin] = LOW; run(ms);
  fakePins[pin] = HIGH; run(150);
}
static bool contains(const std::string &text, const char *part) {
  return text.find(part) != std::string::npos;
}
static size_t count(const std::string &text, const char *part) {
  size_t n = 0, pos = 0;
  while ((pos = text.find(part, pos)) != std::string::npos) { ++n; pos += std::strlen(part); }
  return n;
}
int main() {
  FakeNVS::reset();
  for (int &pin : fakePins) pin = HIGH;
  fakePins[Config::SealPin] = LOW;
  fakePins[Config::NetworkSwitchPin] = LOW;
  fakePins[Config::PowerSwitchPin] = LOW;
  setup();
  assert(poweredOn && !cycleActive && cycle.cycleId == 0);

  // Inativo: o comando explicito cria um ciclo, tal como o BTN_CICLO (RF003).
  Serial.output.clear();
  command("vincular\n");  // case insensitive
  assert(cycleActive && cycle.cycleId == 1 && closedCycles == 0);
  assert(count(Serial.output, "\"type\":\"CICLO_INICIADO\"") == 1);

  command("REMESSA NF-113\n");
  command("NF\n");  // NF de retorno: o ciclo poderia ser encerrado se o botao fosse usado.
  run(1000);
  const CycleState cycleBefore = cycle;  // inclusive limites e flags de alertas
  const uint32_t id = cycle.cycleId;
  const uint32_t days = cycle.daysOut;
  const uint64_t startedAt = cycleStartedAt;
  const uint32_t closed = closedCycles;
  const bool invoice = returnInvoiceIssued;
  const String nf = shipmentInvoice;
  const bool sealWasViolated = seal.violated;
  const bool hadReference = havePrevious;
  const double referenceLat = previousLat, referenceLon = previousLon;
  const std::string deviceId = Config::DeviceId;

  // CT15: tentar novo vinculo pela Serial enquanto ha ciclo ativo.
  Serial.output.clear();
  command("  VINCULAR  \r\n");
  assert(cycleActive && cycle.cycleId == id && cycle.daysOut == days);
  assert(cycleStartedAt == startedAt && closedCycles == closed);
  assert(returnInvoiceIssued == invoice);
  assert(std::strcmp(shipmentInvoice, nf.c_str()) == 0);
  assert(seal.violated == sealWasViolated);
  assert(cycle.expectedReturnDays == cycleBefore.expectedReturnDays);
  assert(cycle.stayThresholdDays == cycleBefore.stayThresholdDays);
  assert(cycle.statusIntervalDays == cycleBefore.statusIntervalDays);
  assert(cycle.lateSent == cycleBefore.lateSent && cycle.staySent == cycleBefore.staySent);
  assert(havePrevious == hadReference && previousLat == referenceLat && previousLon == referenceLon);
  assert(count(Serial.output, "\"type\":\"VINCULO_RECUSADO\"") == 1);
  assert(contains(Serial.output, "\"motivo\":\"ciclo_ativo\""));
  assert(contains(Serial.output, (std::string("\"device_id\":\"") + deviceId + "\"").c_str()));
  assert(contains(Serial.output, "\"ciclo_id\":1,"));
  assert(contains(Serial.output, "\"ciclo_ativo\":true"));
  assert(!contains(Serial.output, "\"type\":\"CICLO_INICIADO\""));
  assert(!contains(Serial.output, "\"type\":\"CICLO_ENCERRADO\""));

  // Com ciclo ativo, BTN_CICLO continua encerrando; VINCULAR nao reconfigura o gesto.
  Serial.output.clear();
  hold(Config::CycleButtonPin, 3200);
  assert(!cycleActive && closedCycles == closed + 1);
  assert(count(Serial.output, "\"type\":\"CICLO_ENCERRADO\"") == 1);
  assert(!contains(Serial.output, "\"type\":\"VINCULO_RECUSADO\""));

  // Inativo outra vez, comando continua iniciando ciclo com novo ID; historico preservado.
  Serial.output.clear();
  command("VINCULAR\n");
  assert(cycleActive && cycle.cycleId == id + 1 && closedCycles == closed + 1);
  assert(!returnInvoiceIssued && shipmentInvoice[0] == '\0');
  assert(count(Serial.output, "\"type\":\"CICLO_INICIADO\"") == 1);

  // Sem NF de retorno, o botao nao encerra ciclo e nem tenta novo vinculo (RN11).
  Serial.output.clear();
  hold(Config::CycleButtonPin, 3200);
  assert(cycleActive && cycle.cycleId == id + 1);
  assert(contains(Serial.output, "\"type\":\"ENCERRAMENTO_RECUSADO\""));
  assert(contains(Serial.output, "\"motivo\":\"nf_retorno_ausente\""));
  assert(!contains(Serial.output, "\"type\":\"VINCULO_RECUSADO\""));
  std::printf("PASS: #113 CT15 HOST (DEMO_MODE=%d): VINCULAR recusa com ciclo ativo; "
              "estado e NF preservados; botao encerra/rejeita; reutilizacao funciona\n", DEMO_MODE);
}
