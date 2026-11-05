// Ambiente bancada (SWITCH_BUTTONS): lacre e rede alternam no toque; ON/OFF exige 3 s.
#include <cassert>
#include <cstdio>
#include <cstdint>
#define GUARA_HOST_INTEGRATION 1
#define DEMO_MODE 1
#define SWITCH_BUTTONS 1
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
int analogRead(int){return 2048;}
void analogReadResolution(int){}
void tone(int,unsigned int,unsigned long){}
void noTone(int){}
void noInterrupts(){}
void interrupts(){}
void attachInterrupt(int,void(*)(),int){}
int digitalPinToInterrupt(int p){return p;}
static void run(uint32_t ms){ for(uint32_t i=0;i<ms;i+=5){ fakeMs+=5; loop(); } }
static void hold(int pin, uint32_t ms){ fakePins[pin]=LOW; run(ms); fakePins[pin]=HIGH; run(150); }
int main(){
  FakeNVS::reset();
  for(int &pin:fakePins) pin=HIGH;  // todos os botoes soltos
  setup();
  run(1000);
  assert(poweredOn && simulatedNetworkAvailable() && !stableSeal && !seal.violated);
  // Lacre: um toque abre (violacao sem autorizacao), outro fecha.
  hold(Config::SealPin, 120);
  assert(stableSeal && seal.violated);
  run(2000);
  assert(stableSeal);  // soltar o botao nao fecha o lacre
  hold(Config::SealPin, 120);
  assert(!stableSeal && !seal.violated);
  // Rede: cada toque alterna.
  hold(Config::NetworkSwitchPin, 120);
  assert(!simulatedNetworkAvailable());
  hold(Config::NetworkSwitchPin, 120);
  assert(simulatedNetworkAvailable());
  // ON/OFF: o LCD conta 3, 2, 1 enquanto o botao esta apertado.
  fakePins[Config::PowerSwitchPin]=LOW; run(200);
  assert(strcmp(shownTop, "Desligar") == 0 && strcmp(shownBottom, "segure mais 3 s") == 0);
  run(1000);
  assert(strcmp(shownBottom, "segure mais 2 s") == 0);
  run(1000);
  assert(strcmp(shownBottom, "segure mais 1 s") == 0);
  fakePins[Config::PowerSwitchPin]=HIGH; run(150);
  assert(poweredOn);
  // ON/OFF: soltar antes de 3 s nao muda; 3 s desliga; outros 3 s ligam.
  hold(Config::PowerSwitchPin, 1000);
  assert(poweredOn);
  hold(Config::PowerSwitchPin, 3200);
  assert(!poweredOn);
  hold(Config::PowerSwitchPin, 3200);
  assert(poweredOn);
  std::puts("PASS: bancada com botoes: lacre e rede no toque, ON/OFF em 3 s (host)");
}
