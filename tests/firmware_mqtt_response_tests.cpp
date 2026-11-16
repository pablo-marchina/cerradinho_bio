// Falha de cmd/resposta: estado duravel, fila ordenada e resposta explicita ao comando novo.
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
  assert(pendingCmdResponses.count == 1);
  assert(contains(pendingCmdResponses.text[0], "\"id\":\"resp-1\""));
  StoredContext restored{};
  assert(restoreContext(restored) && restored.seal.authorized);
  assert(contains(Serial.output, "resposta nao publicada"));

  // Simula a perda da RAM: o reboot restaura primeiro o estado novo e a resposta de sucesso.
  seal = {};
  cycleActive = false;
  pendingCmdResponses = {};
  assert(restoreContext(restored));
  seal = restored.seal;
  cycle = restored.cycle;
  cycleActive = restored.cycleActive;
  returnInvoiceIssued = restored.returnInvoiceIssued;
  stableSeal = restored.sealOpen;
  restorePendingCmdResponses();
  assert(seal.authorized && pendingCmdResponses.count == 1);
  assert(contains(pendingCmdResponses.text[0], "\"id\":\"resp-1\""));

  // Enquanto o reenvio falha, a acao nova nao roda e recebe uma recusa recuperavel com seu id.
  mqtt.failNextPublish = true;
  char topic[] = "guaratracking/prototipo-001/cmd";
  char next[] = "{\"id\":\"resp-2\",\"acao\":\"ciclo_iniciar\"}";
  onMqttMessage(topic, reinterpret_cast<uint8_t *>(next), std::strlen(next));
  assert(!cycleActive);
  assert(pendingCmdResponses.count == 2);
  assert(contains(pendingCmdResponses.text[0], "\"id\":\"resp-1\""));
  assert(contains(pendingCmdResponses.text[1], "\"id\":\"resp-2\""));
  assert(contains(pendingCmdResponses.text[1], "\"erro\":\"resposta_anterior_pendente\""));

  assert(resendPendingCmdResponses());
  assert(pendingCmdResponses.count == 0);
  assert(mqtt.published.size() >= 2);
  assert(contains(mqtt.published[mqtt.published.size() - 2], "\"id\":\"resp-1\""));
  assert(contains(mqtt.published.back(), "\"id\":\"resp-2\""));

  // Se o reenvio anterior passa no callback, o comando que chegou na mesma chamada e processado.
  assert(enqueueCmdResponse("{\"id\":\"antiga\",\"acao\":\"autorizar\",\"ok\":true}"));
  onMqttMessage(topic, reinterpret_cast<uint8_t *>(next), std::strlen(next));
  assert(cycleActive);
  assert(pendingCmdResponses.count == 0);
  assert(!mqtt.published.empty() && contains(mqtt.published.back(), "\"id\":\"resp-2\""));

  // Falha ao salvar a mudanca nunca produz sucesso recuperavel nem atualiza o contexto duravel.
  FakeNVS::failNextWrite = true;
  const char *invoice = "{\"id\":\"resp-3\",\"acao\":\"nf_retorno\"}";
  assert(!handleDashboardCommand(invoice, std::strlen(invoice)));
  assert(returnInvoiceIssued);
  assert(restoreContext(restored) && !restored.returnInvoiceIssued);
  assert(!mqtt.published.empty() && contains(mqtt.published.back(), "\"id\":\"resp-3\""));
  assert(contains(mqtt.published.back(), "\"erro\":\"persistencia_falhou\""));

  std::puts("PASS: cmd/resposta preserva estado, fila ids e trata comando concorrente");
  return 0;
}

