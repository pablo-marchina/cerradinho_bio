// Exercita o firmware real e ISR com um loop artificialmente ocupado.
// Nao substitui um reteste de CT18 no simulador Wokwi.
#include <cassert>
#include <cstdint>
#include <cstdio>
#define GUARA_HOST_INTEGRATION 1
#define DEMO_MODE 1
#include "../src/firmware/main.cpp"
FakeSerial Serial, Serial2;
FakeWiFi WiFi;
int esp_reset_reason(){return 1;}
static uint32_t fakeMs = 2000;
static int fakePins[64] = {};
static void(*fakeIsr)() = nullptr;
int gpio_get_level(gpio_num_t p) { return digitalRead(static_cast<int>(p)); }
unsigned long millis(){return fakeMs;}
void delay(unsigned long v){fakeMs+=static_cast<uint32_t>(v);}
void pinMode(int,int){}
void digitalWrite(int,int){}
int digitalRead(int p){return p>=0&&p<64?fakePins[p]:0;}
int analogRead(int){return 2048;}
void analogReadResolution(int){}
void tone(int,unsigned int,unsigned long){}
void noTone(int){}
void noInterrupts(){}
void interrupts(){}
void attachInterrupt(int,void(*cb)(),int){fakeIsr=cb;}
int digitalPinToInterrupt(int p){return p;}
int main(){
 FakeNVS::reset();
 for (int &pin:fakePins) pin=HIGH;
 fakePins[Config::SealPin]=LOW;
 fakePins[Config::PowerSwitchPin]=LOW;
 fakePins[Config::NetworkSwitchPin]=HIGH;
 setup();
 assert(fakeIsr!=nullptr && sealEdgeOverruns==0);
 // vinte transicoes de 200ms, coletadas numa unica chamada do loop.
 for (int i=0; i<20; ++i) {
   fakePins[Config::SealPin]=(i%2==0)?HIGH:LOW;
   fakeIsr();
   fakeMs+=200;
 }
 const uint64_t initialClock = clockMs();
 readSeal(millis(), initialClock + 4000);
 assert(!stableSeal && sealEdgeOverruns==0 && sealEdgeHead==sealEdgeTail);
 // O processamento das bordas nao deve atualizar o relogio simulado novamente.
 assert(clockMs() == initialClock);
 // As 20 bordas foram confirmadas no host; o estado final deve estar em NVS.
 StoredContext persisted{};
 assert(restoreContext(persisted) && !persisted.sealOpen && !contextDirty);
 // 10 aberturas violadoras; todas representadas como individual ou resumo.
 unsigned individual=0;
 for (unsigned i=0;i<pending.count;i++){
   if (std::string(pending.text[i]).find("VIOLACAO_LACRE")!=std::string::npos) individual++;
 }
 assert(individual+pending.overflowCount==10);
 assert(pending.overflowCount>0);
 std::puts("PASS: 20 bordas ISR, relogio unico, persistencia imediata da transicao em host");
}
