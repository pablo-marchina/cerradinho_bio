// Executa as rotinas reais do firmware com NVS e GPIO de bancada, sem Wokwi.
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
int analogRead(int){return 2048;}
void analogReadResolution(int){}
void tone(int,unsigned int,unsigned long){}
void noTone(int){}
void noInterrupts(){}
void interrupts(){}
void attachInterrupt(int,void(*)(),int){}
int digitalPinToInterrupt(int p){return p;}
int main(){
  FakeNVS::reset();
  for(int &pin:fakePins) pin=HIGH;
  fakePins[Config::SealPin]=LOW;
  fakePins[Config::NetworkSwitchPin]=HIGH; // offline
  fakePins[Config::PowerSwitchPin]=LOW;
  setup();
  assert(storageReady && !contextDirty);
  const auto oldContext = FakeNVS::bytes.at("ctx");
  cycle.daysOut++; contextDirty=true;
  FakeNVS::failNextWrite=true;
  saveContext();
  assert(contextDirty && contextRetryPending && FakeNVS::bytes.at("ctx")==oldContext);
  const uint32_t failureTime = lastContextFailureAt;
  // Enquanto nao passaram 1000ms, uma falha nao dispara nova tentativa automatica.
  assert(fakeMs - failureTime < 1000U);
  saveContext();
  assert(!contextDirty && !contextRetryPending && FakeNVS::bytes.at("ctx")!=oldContext);
  auto oldMeta=FakeNVS::bytes.at("pmeta");
  queuePush(&pending, "{\"type\":\"INJETADO\"}", true, clockMs());
  FakeNVS::failNextWrite=true;
  assert(!savePending() && pendingDirty && pendingRetryPending);
  assert(FakeNVS::bytes.at("pmeta")==oldMeta);
  assert(savePending() && !pendingDirty && !pendingRetryPending);
  assert(FakeNVS::bytes.at("pmeta")!=oldMeta);
  std::puts("PASS: firmware real, gravacao NVS falha, dirty preservado e retentativa (host)");
}
