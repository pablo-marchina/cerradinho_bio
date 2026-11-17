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

  // A resposta de sucesso so entra na outbox depois do estado duravel.
  mqtt.failNextPublish = true;
  const char *authorize = "{\"id\":\"resp-1\",\"acao\":\"autorizar\"}";
  assert(handleDashboardCommand(authorize, std::strlen(authorize)));
  assert(seal.authorized);
  assert(pendingCmdResponses.count == 1);
  assert(contains(pendingCmdResponses.text[0], "\"id\":\"resp-1\""));
  StoredContext restored{};
  assert(restoreContext(restored) && restored.seal.authorized);
  assert(contains(Serial.output, "resposta nao publicada"));

  // Simula a perda da RAM: o reboot restaura o estado novo e o snapshot valido da outbox.
  seal = {};
  cycleActive = false;
  pendingCmdResponses = {};
  cmdResponseGeneration = 0;
  cmdResponseSlot = -1;
  assert(restoreContext(restored));
  seal = restored.seal;
  cycle = restored.cycle;
  cycleActive = restored.cycleActive;
  returnInvoiceIssued = restored.returnInvoiceIssued;
  stableSeal = restored.sealOpen;
  restorePendingCmdResponses();
  assert(seal.authorized && pendingCmdResponses.count == 1);
  assert(contains(pendingCmdResponses.text[0], "\"id\":\"resp-1\""));

  // O publish do reenvio sobrescreve o buffer recebido no fake como no PubSubClient 2.8. Como o
  // callback copia antes de publicar, o comando novo ainda e lido e executado corretamente.
  char topic[] = "guaratracking/prototipo-001/cmd";
  char next[] = "{\"id\":\"resp-2\",\"acao\":\"ciclo_iniciar\"}";
  mqtt.overwriteSharedBufferOnPublish = true;
  mqtt.sharedBufferPayload = reinterpret_cast<uint8_t *>(next);
  mqtt.sharedBufferLength = std::strlen(next);
  onMqttMessage(topic, reinterpret_cast<uint8_t *>(next), std::strlen(next));
  mqtt.overwriteSharedBufferOnPublish = false;
  mqtt.sharedBufferPayload = nullptr;
  mqtt.sharedBufferLength = 0;
  assert(cycleActive);
  assert(pendingCmdResponses.count == 0);
  assert(contains(mqtt.published.back(), "\"id\":\"resp-2\""));

  // Copy-on-write: falha parcial ao remover nao pode apagar a resposta seguinte. O reboot escolhe
  // o outro snapshot, ainda integro; no maximo repete a resposta ja publicada.
  assert(enqueueCmdResponse("{\"id\":\"fila-a\",\"acao\":\"autorizar\",\"ok\":true}"));
  assert(enqueueCmdResponse("{\"id\":\"fila-b\",\"acao\":\"autorizar\",\"ok\":true}"));
  FakeNVS::failNextWrite = true;
  FakeNVS::partialNextWrite = sizeof(StoredCmdResponseSnapshot) / 2;
  assert(!removeFirstPendingCmdResponse());
  assert(pendingCmdResponses.count == 2);
  pendingCmdResponses = {};
  cmdResponseGeneration = 0;
  cmdResponseSlot = -1;
  restorePendingCmdResponses();
  assert(pendingCmdResponses.count == 2);
  assert(contains(pendingCmdResponses.text[0], "\"id\":\"fila-a\""));
  assert(contains(pendingCmdResponses.text[1], "\"id\":\"fila-b\""));

  assert(removeFirstPendingCmdResponse());
  assert(pendingCmdResponses.count == 1 && contains(pendingCmdResponses.text[0], "fila-b"));
  FakeNVS::corruptNextWrite = true;  // escrita completa, mas checksum invalido na verificacao
  assert(!removeFirstPendingCmdResponse());
  pendingCmdResponses = {};
  cmdResponseGeneration = 0;
  cmdResponseSlot = -1;
  restorePendingCmdResponses();
  assert(pendingCmdResponses.count == 1 && contains(pendingCmdResponses.text[0], "fila-b"));
  assert(removeFirstPendingCmdResponse());
  assert(pendingCmdResponses.count == 0);

  // Enquanto o reenvio falha, a acao nova nao roda e recebe uma recusa recuperavel com seu id.
  assert(enqueueCmdResponse("{\"id\":\"antiga\",\"acao\":\"autorizar\",\"ok\":true}"));
  mqtt.failNextPublish = true;
  char blocked[] = "{\"id\":\"bloqueada\",\"acao\":\"nf_retorno\"}";
  onMqttMessage(topic, reinterpret_cast<uint8_t *>(blocked), std::strlen(blocked));
  assert(!returnInvoiceIssued && pendingCmdResponses.count == 2);
  assert(contains(pendingCmdResponses.text[1], "\"erro\":\"resposta_anterior_pendente\""));
  assert(resendPendingCmdResponses() && pendingCmdResponses.count == 0);

  // Quatro vagas regulares cheias: a quinta, reservada, registra a recusa de saturacao.
  for (int i = 0; i < CmdResponseRegularCapacity; ++i) {
    const String queued = String("{\"id\":\"cheia-") + String(i) +
                          "\",\"acao\":\"autorizar\",\"ok\":true}";
    assert(enqueueCmdResponse(queued));
  }
  mqtt.failNextPublish = true;
  char saturated[] = "{\"id\":\"saturada\",\"acao\":\"nf_retorno\"}";
  onMqttMessage(topic, reinterpret_cast<uint8_t *>(saturated), std::strlen(saturated));
  assert(!returnInvoiceIssued && pendingCmdResponses.count == CmdResponseCapacity);
  assert(contains(pendingCmdResponses.text[CmdResponseCapacity - 1], "\"id\":\"saturada\""));
  assert(contains(pendingCmdResponses.text[CmdResponseCapacity - 1],
                  "\"erro\":\"fila_respostas_cheia\""));
  const int loopsBeforeBackpressure = mqtt.loopCalls;
  mqtt.failNextPublish = true;
  updateMqtt(fakeMs);
  assert(mqtt.loopCalls == loopsBeforeBackpressure);  // nao recebe outro comando enquanto cheia
  assert(resendPendingCmdResponses() && pendingCmdResponses.count == 0);

  // A falha de contexto representa resultado pendente, nao recusa. O loop pode concluir a
  // persistencia depois, exatamente como o contrato informa ao dashboard.
  FakeNVS::failNextWrite = true;
  const char *invoice = "{\"id\":\"resp-3\",\"acao\":\"nf_retorno\"}";
  assert(!handleDashboardCommand(invoice, std::strlen(invoice)));
  assert(returnInvoiceIssued);
  assert(restoreContext(restored) && !restored.returnInvoiceIssued);
  assert(!mqtt.published.empty() && contains(mqtt.published.back(), "\"id\":\"resp-3\""));
  assert(contains(mqtt.published.back(), "\"erro\":\"persistencia_pendente\""));
  assert(contains(mqtt.published.back(), "\"pendente\":true"));
  fakeMs += 1100;
  loop();
  assert(restoreContext(restored) && restored.returnInvoiceIssued);

  std::puts("PASS: cmd/resposta copia buffer, recupera snapshots, sinaliza pendencia e saturacao");
  return 0;
}

