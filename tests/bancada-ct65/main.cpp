// CT65 na bancada: pulsos curtos no lacre, com o firmware de producao incluido sem mudancas.
// Ligacao: um jumper do GPIO32 ao GPIO27. Em dreno aberto, nivel baixo equivale a apertar o
// botao do lacre, sem disputar o sinal com ele. Nao toque no botao do lacre durante o ensaio.
// Sequencia: lote com rede aos 10 s, aviso para desligar a REDE e lote sem rede.
#include <TinyGPS++.h>
#include <WiFi.h>
#include <Preferences.h>
#include <LiquidCrystal_I2C.h>
#define setup trackerSetup
#define loop trackerLoop
#include "../../src/firmware/main.cpp"
#undef setup
#undef loop

namespace {
constexpr int PulsePin = 32;

void press(uint32_t widthMs) {
  digitalWrite(PulsePin, LOW);
  delay(widthMs);
  digitalWrite(PulsePin, HIGH);
}

// Mesmos estimulos do CT18: pulsos de 20 e 40 ms, ignorados, e 20 toques de 200 ms, que
// alternam o lacre 20 vezes e terminam com ele fechado.
void runBatch(const char *name) {
  Serial.printf("[ensaio] CT65 lote %s pulsos uptime_ms=%lu\r\n", name, millis());
  press(20);
  delay(180);
  press(40);
  delay(1000);
  Serial.printf("[ensaio] CT65 lote %s estaveis uptime_ms=%lu\r\n", name, millis());
  for (int i = 0; i < 20; ++i) {
    press(200);
    delay(200);
  }
}

// Tarefa no outro nucleo: a largura do pulso nao depende do loop do firmware.
void stimulusTask(void *) {
  delay(10000);
  runBatch("com_rede");
  delay(3000);
  Serial.println("[ensaio] CT65 toque REDE para desligar");
  delay(15000);
  runBatch("sem_rede");
  delay(3000);
  Serial.println("[ensaio] FIM CT65");
  vTaskDelete(nullptr);
}
}

void setup() {
  pinMode(PulsePin, OUTPUT_OPEN_DRAIN);
  digitalWrite(PulsePin, HIGH);  // solto desde o boot
  trackerSetup();
  xTaskCreatePinnedToCore(stimulusTask, "ct64", 4096, nullptr, 1, nullptr, 0);
}

void loop() {
  trackerLoop();
}
