// Falha de cmd/resposta: a acao executada deve conservar o mesmo id para reenvio.
#include <cassert>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <string>
#define GUARA_HOST_INTEGRATION 1
#define DEMO_MODE 1
#define SWITCH_BUTTONS 1
#include "../src/firmware/main.cpp"

FakeSerial Serial, Serial2;
FakeWiFi WiFi;
int gpio_get_level(gpio_num_t p) { return digitalRead(static_cast<int>(p)); }
int esp_reset_reason() { return 1; }
static uint32_t fakeMs = 1000;
static int fakePins[64] = {};
unsigned long millis() { return fakeMs; }
void delay(unsigned long value) { fakeMs += static_cast<uint32_t>(value); }
void pinMode(int, int) {}
void digitalWrite(int, int) {}
int digitalRead(int pin) { return pin >= 0 && pin < 64 ? fakePins[pin] : 0; }
int analogRead(int) { return 2048; }
void analogReadResolution(int) {}
void tone(int, unsigned int, unsigned long) {}
void noTone(int) {}
void noInterrupts() {}
void interrupts() {}
void attachInterrupt(int, void (*)(), int) {}
int digitalPinToInterrupt(int pin) { return pin; }

static bool contains(const std::string &text, const char *part) {
  return text.find(part) != std::string::npos;
}

int main() {
  FakeNVS::reset();
  for (int &pin : fakePins) pin = HIGH;
  setup();
  mqtt.connectedState = true;

  mqtt.failNextPublish = true;
  const char *authorize = "{\"id\":\"resp-1\",\"acao\":\"autorizar\"}";
  assert(handleDashboardCommand(authorize, std::strlen(authorize)));
  assert(seal.authorized);
  assert(contains(pendingCmdResponse.c_str(), "\"id\":\"resp-1\""));
  assert(FakeNVS::bytes.count(PendingCmdResponseKey) == 1);
  assert(contains(Serial.output, "resposta nao publicada"));

  // Enquanto o reenvio falha, um novo comando recebido nao pode alterar o ciclo.
  mqtt.failNextPublish = true;
  char topic[] = "guaratracking/prototipo-001/cmd";
  char next[] = "{\"id\":\"resp-2\",\"acao\":\"ciclo_iniciar\"}";
  onMqttMessage(topic, reinterpret_cast<uint8_t *>(next), std::strlen(next));
  assert(!cycleActive);
  assert(contains(pendingCmdResponse.c_str(), "\"id\":\"resp-1\""));

  // Mesmo quando o reenvio passa, o comando recebido nessa chamada nao e executado.
  onMqttMessage(topic, reinterpret_cast<uint8_t *>(next), std::strlen(next));
  assert(pendingCmdResponse.length() == 0);
  assert(FakeNVS::bytes.count(PendingCmdResponseKey) == 0);
  assert(!mqtt.published.empty() && contains(mqtt.published.back(), "\"id\":\"resp-1\""));
  assert(!cycleActive);

  // O dashboard repete o novo id somente depois de receber a resposta anterior.
  onMqttMessage(topic, reinterpret_cast<uint8_t *>(next), std::strlen(next));
  assert(cycleActive);
  assert(!mqtt.published.empty() && contains(mqtt.published.back(), "\"id\":\"resp-2\""));

  std::puts("PASS: cmd/resposta falha, persiste o id, bloqueia novo comando e reenvia");
  return 0;
}

