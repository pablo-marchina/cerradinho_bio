// Botoes e ciclo com o firmware real em host: navegacao do LCD e boot sem ciclo.
#include <cassert>
#include <cstdio>
#include <cstdint>
#define GUARA_HOST_INTEGRATION 1
#define DEMO_MODE 1
#include "../src/firmware/main.cpp"
FakeSerial Serial, Serial2;
int gpio_get_level(gpio_num_t p){ return digitalRead(static_cast<int>(p)); }
FakeWiFi WiFi;
int esp_reset_reason(){return 1;}
static uint32_t fakeMs = 1000;
static int fakePins[64] = {};
unsigned long millis(){return fakeMs;}
void delay(unsigned long v){fakeMs += static_cast<uint32_t>(v);}
void pinMode(int,int){}
void digitalWrite(int,int){}
int digitalRead(int p){return p>=0&&p<64?fakePins[p]:0;}
static int fakeAdc = 2048;
int analogRead(int){return fakeAdc;}
void analogReadResolution(int){}
void tone(int,unsigned int,unsigned long){}
void noTone(int){}
void noInterrupts(){}
void interrupts(){}
void attachInterrupt(int,void(*)(),int){}
int digitalPinToInterrupt(int p){return p;}
static void run(uint32_t ms){ for(uint32_t i=0;i<ms;i+=5){ fakeMs+=5; loop(); } }
static void hold(int pin, uint32_t ms){ fakePins[pin]=LOW; run(ms); fakePins[pin]=HIGH; run(150); }
static unsigned screen(){ return static_cast<unsigned>(screens.current()); }
int main(){
  FakeNVS::reset();
  for(int &pin:fakePins) pin=HIGH;
  fakePins[Config::SealPin]=LOW;
  fakePins[Config::NetworkSwitchPin]=LOW;
  fakePins[Config::PowerSwitchPin]=LOW;
  setup();
  // Primeiro boot: sem ciclo e sem contar dias ate o vinculo (RF002, RF003, RN02).
  assert(!cycleActive);
  run(10000);
  assert(!cycleActive && cycle.daysOut == 0);
  hold(Config::CycleButtonPin, 1000);  // soltou antes de 3 s
  assert(!cycleActive);
  hold(Config::CycleButtonPin, 3200);
  assert(cycleActive && cycle.cycleId == 1 && cycle.daysOut == 0);
  run(6500);
  assert(cycle.daysOut == 6 && cycleIsLate(&cycle));
  // Avancar e voltar em ordem circular, inclusive com toques rapidos.
  const unsigned start = screen();
  hold(Config::NextButtonPin, 120);
  assert(screen() == (start + 1) % ScreenCount);
  hold(Config::PreviousButtonPin, 120);
  hold(Config::PreviousButtonPin, 120);
  assert(screen() == (start + ScreenCount - 1) % ScreenCount);
  for (int i = 0; i < 6; ++i) hold(Config::NextButtonPin, 60);
  assert(screen() == (start + 5) % ScreenCount);
  // LCD apagado: o primeiro toque so acende, o segundo troca a tela.
  run(Config::LcdTimeoutMs + 1000);
  assert(!lcdLit);
  const unsigned before = screen();
  hold(Config::NextButtonPin, 120);
  assert(lcdLit && screen() == before);
  hold(Config::NextButtonPin, 120);
  assert(screen() == (before + 1) % ScreenCount);
  // Descida lenta: 19% real dispara o alerta (RN10), mesmo com a histerese do LCD.
  for (int pct = 40; pct >= 19; --pct) {
    fakeAdc = (pct * 4095 + 99) / 100;
    run(1000);
  }
  assert(battery.lowSent);
  std::puts("PASS: boot sem ciclo, vinculo por 3 s, contagem a partir do vinculo, navegacao do LCD e alerta de bateria (host)");
}
