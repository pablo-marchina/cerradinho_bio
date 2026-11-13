// Comandos do dashboard (topico cmd): o JSON recebido faz o que os botoes fazem na placa
// (ambiente bancada, SWITCH_BUTTONS) e toda resposta traz ok ou erro.
#include <cassert>
#include <cstdio>
#include <cstdint>
#include <string>
#define GUARA_HOST_INTEGRATION 1
#define DEMO_MODE 1
#define SWITCH_BUTTONS 1
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
void attachInterrupt(int, void (*)(), int) {}
int digitalPinToInterrupt(int p) { return p; }
static void run(uint32_t ms) { for (uint32_t i = 0; i < ms; i += 5) { fakeMs += 5; loop(); } }
static bool contains(const std::string &text, const char *part) { return text.find(part) != std::string::npos; }
// Envia o JSON ao tratador e devolve o que o dispositivo imprimiu como resposta.
static std::string cmd(const char *json) {
  Serial.output.clear();
  handleDashboardCommand(json, std::strlen(json));
  return Serial.output;
}
static void expectOk(const char *json, const char *id, const char *action) {
  const std::string out = cmd(json);
  const std::string want = std::string("\"id\":\"") + id + "\",\"acao\":\"" + action + "\",\"ok\":true}";
  assert(contains(out, want.c_str()));
}
static void expectError(const char *json, const char *error) {
  const std::string out = cmd(json);
  const std::string want = std::string("\"ok\":false,\"erro\":\"") + error + "\"}";
  assert(contains(out, want.c_str()));
}
int main() {
  // ---- leitor de JSON ----
  char text[32];
  long number = 0;
  const char *sample = "{ \"id\" : \"a1\", \"acao\":\"rede\",\"segundos\": 30 , \"ok\":true}";
  assert(DashboardCmd::textValue(sample, std::strlen(sample), "id", text, sizeof(text)) && std::strcmp(text, "a1") == 0);
  assert(DashboardCmd::textValue(sample, std::strlen(sample), "acao", text, sizeof(text)) && std::strcmp(text, "rede") == 0);
  assert(DashboardCmd::intValue(sample, std::strlen(sample), "segundos", 1, 3600, &number) && number == 30);
  assert(!DashboardCmd::intValue(sample, std::strlen(sample), "segundos", 60, 3600, &number));  // fora da faixa
  assert(!DashboardCmd::textValue(sample, std::strlen(sample), "valor", text, sizeof(text)));    // ausente
  assert(!DashboardCmd::textValue("{\"acao\":\"a\\\"b\"}", 14, "acao", text, sizeof(text)));       // barra invertida
  assert(!DashboardCmd::textValue("{\"acao\":\"nao fecha", 18, "acao", text, sizeof(text)));       // aspas abertas
  assert(!DashboardCmd::textValue("{\"acao\":\"muito_muito_muito_longo_demais\"}", 40, "acao", text, 8));

  FakeNVS::reset();
  for (int &pin : fakePins) pin = HIGH;
  setup();
  run(1000);
  assert(poweredOn && simulatedNetworkAvailable() && !stableSeal && !cycleActive);

  // ---- erros de formato ----
  expectError("", "json_vazio");
  expectError("{\"id\":\"x\"}", "acao_ausente");
  expectError("{\"id\":\"x\",\"acao\":\"dancar\"}", "acao_desconhecida");
  const std::string big(300, 'a');
  expectError(("{\"acao\":\"" + big + "\"}").c_str(), "payload_grande");

  // ---- telas do LCD ----
  const Screen screen0 = screens.current();
  expectOk("{\"id\":\"t1\",\"acao\":\"avancar\"}", "t1", "avancar");
  assert(screens.current() != screen0);
  expectOk("{\"id\":\"t2\",\"acao\":\"voltar\"}", "t2", "voltar");
  assert(screens.current() == screen0);

  // ---- ciclo ----
  expectOk("{\"id\":\"c1\",\"acao\":\"ciclo_iniciar\"}", "c1", "ciclo_iniciar");
  assert(cycleActive);
  assert(contains(Serial.output, "\"type\":\"CICLO_INICIADO\""));
  expectError("{\"id\":\"c2\",\"acao\":\"ciclo_iniciar\"}", "ciclo_ativo");
  expectError("{\"id\":\"c3\",\"acao\":\"ciclo_encerrar\"}", "encerramento_recusado");  // sem NF de retorno
  assert(cycleActive);

  // ---- lacre simulado ----
  expectOk("{\"id\":\"l1\",\"acao\":\"lacre\",\"valor\":\"aberto\"}", "l1", "lacre");
  assert(stableSeal && seal.violated);
  assert(contains(Serial.output, "\"type\":\"VIOLACAO_LACRE\""));
  expectError("{\"acao\":\"lacre\",\"valor\":\"aberto\"}", "lacre_ja_aberto");
  expectError("{\"acao\":\"lacre\",\"valor\":\"meio\"}", "valor_invalido");
  expectOk("{\"id\":\"l2\",\"acao\":\"lacre\",\"valor\":\"fechado\"}", "l2", "lacre");
  assert(!stableSeal);
  expectError("{\"acao\":\"lacre\",\"valor\":\"fechado\"}", "lacre_ja_fechado");

  // ---- autorizacao e retirada autorizada ----
  expectOk("{\"id\":\"a1\",\"acao\":\"autorizar\"}", "a1", "autorizar");
  assert(contains(Serial.output, "\"type\":\"AUTORIZACAO_REGISTRADA\""));
  expectOk("{\"acao\":\"lacre\",\"valor\":\"aberto\"}", "", "lacre");
  assert(contains(Serial.output, "\"type\":\"RETIRADA_AUTORIZADA\""));
  expectOk("{\"acao\":\"lacre\",\"valor\":\"fechado\"}", "", "lacre");

  // ---- rede simulada: o prazo religa sozinho ----
  expectError("{\"acao\":\"rede\",\"valor\":\"desligada\"}", "segundos_invalido");
  expectError("{\"acao\":\"rede\",\"valor\":\"desligada\",\"segundos\":99999}", "segundos_invalido");
  expectError("{\"acao\":\"rede\",\"valor\":\"talvez\"}", "valor_invalido");
  expectOk("{\"id\":\"r1\",\"acao\":\"rede\",\"valor\":\"desligada\",\"segundos\":2}", "r1", "rede");
  run(100);
  assert(!simulatedNetworkAvailable());
  run(2100);
  assert(simulatedNetworkAvailable() && networkRestoreAt == 0);
  expectOk("{\"acao\":\"rede\",\"valor\":\"desligada\",\"segundos\":30}", "", "rede");
  expectOk("{\"acao\":\"rede\",\"valor\":\"ligada\"}", "", "rede");  // cancela o prazo
  assert(simulatedNetworkAvailable() && networkRestoreAt == 0);

  // ---- ligar e desligar ----
  expectOk("{\"id\":\"p1\",\"acao\":\"desligar\"}", "p1", "desligar");
  run(50);
  assert(!poweredOn);
  expectError("{\"acao\":\"avancar\"}", "dispositivo_desligado");
  expectError("{\"acao\":\"lacre\",\"valor\":\"aberto\"}", "dispositivo_desligado");
  expectOk("{\"id\":\"p2\",\"acao\":\"ligar\"}", "p2", "ligar");
  run(50);
  assert(poweredOn);

  // ---- encerramento com NF de retorno ----
  Serial.inject("NF\n");
  run(50);
  expectOk("{\"id\":\"c4\",\"acao\":\"ciclo_encerrar\"}", "c4", "ciclo_encerrar");
  assert(!cycleActive);
  expectError("{\"acao\":\"ciclo_encerrar\"}", "sem_ciclo_ativo");

  // ---- "vincular" e alias de ciclo_iniciar ----
  expectOk("{\"id\":\"v1\",\"acao\":\"vincular\"}", "v1", "vincular");
  assert(cycleActive);
  std::puts("PASS: comandos do dashboard (cmd) e leitor de JSON");
  return 0;
}
