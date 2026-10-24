// Execucao do firmware real com GPIO, tempo, Serial e NVS falsos.
// Ensaio deterministico de host: NAO e simulacao Wokwi nem teste fisico.
#include <cassert>
#include <cstdio>
#include <map>
#include <cstdint>
#include <vector>
#define GUARA_HOST_INTEGRATION 1
#define DEMO_MODE 1
#include "../src/firmware/main.cpp"

FakeSerial Serial, Serial2;
FakeWiFi WiFi;
static uint32_t fakeMs = 1000;
static int fakePins[64] = {};
static uint32_t fakeIoWrites=0;
static void (*fakeIsr)() = nullptr;
int gpio_get_level(gpio_num_t pin){return digitalRead(static_cast<int>(pin));}
int esp_reset_reason(){return 1;}
unsigned long millis(){ return fakeMs; }
void delay(unsigned long ms){ fakeMs += static_cast<uint32_t>(ms); }
void pinMode(int,int){}
void digitalWrite(int,int){ ++fakeIoWrites; }
int digitalRead(int p){ return p>=0&&p<64?fakePins[p]:0; }
int analogRead(int){ return 2048; }
void analogReadResolution(int){}
void noInterrupts(){}
void interrupts(){}
void attachInterrupt(int,void(*callback)(),int){ fakeIsr=callback; }
int digitalPinToInterrupt(int p){return p;}

static void advance(uint32_t d){ fakeMs+=d; }
static void setSeal(bool opened){
 fakePins[Config::SealPin]=opened?HIGH:LOW;
 assert(fakeIsr);
 fakeIsr();
}
int main(){
 FakeNVS::reset();
 // Botões despressionados (INPUT_PULLUP), rede desligada desde o boot.
 for(int &p: fakePins)p=HIGH;
 fakePins[Config::SealPin]=LOW;
 fakePins[Config::NetworkSwitchPin]=HIGH;
 fakePins[Config::PowerSwitchPin]=LOW;
 setup();
 assert(!degradedMode && storageReady && cycleActive);
 assert(pending.count>0);
 assert(pendingSlot>=0);

 // CT18 host: 20 mudancas durando 200 ms, rede indisponivel.
 // As bordas chegam pela ISR; leitura concentrada ao final simula loop ocupado.
 for(int i=0;i<20;i++) {setSeal((i%2)==0);advance(200);}
 readSeal(millis());
 assert(!stableSeal);
 assert(sealEdgeOverruns==0);
 assert(sealEdgeHead==sealEdgeTail);
 // Registros diagnosticos devem incluir as 20 transicoes ate o limite local.
 int diagnostics=0;
 for(const auto &entry:FakeNVS::bytes) if(entry.first.size()>1&&entry.first[0]=='q'&&
      entry.first[1]>='0'&&entry.first[1]<='9') diagnostics++;
 assert(diagnostics==Config::QueueCapacity);
 assert(pending.overflow_count>0); // 10 violacoes, fila cheia com eventos criticos

 // Token/ACK errado nao descarrega a fila e ACK correto conserva a ordem.
 fakePins[Config::NetworkSwitchPin]=LOW;
 updateNetwork();
 flushPending();
 assert(awaitingSimAck);
 const uint32_t correct=simAckToken;
 const uint8_t before=pending.count;
 Serial.inject("ACK 00000000\n");
 readCommands();
 assert(pending.count==before);
 char ok[9];std::snprintf(ok,sizeof(ok),"%08X",correct);
 Serial.inject(std::string("ACK ")+ok+"\n");
 readCommands();
 assert(pending.count==before-1);

 // CT22 parcial: falha injetada na nova gravacao NVS mantem slot anterior.
 auto beforeCount=pending.count;
 auto snapshotKey=pendingSlot==0?"pqd0":"pqd1";
 auto previous=FakeNVS::bytes.at(snapshotKey);
 FakeNVS::failNextWrite=true;
 queue_push(&pending,"{\"type\":\"FALHA_INJETADA\"}",true,clockMs());
 assert(!savePending());
 assert(pendingDirty);
 assert(FakeNVS::bytes.at(snapshotKey)==previous);
 assert(savePending());
 assert(!pendingDirty);

 // Releitura de NVS sem zerar armazenamento simula recarga logica do dispositivo.
 const auto preserved=pending;
 queue_init(&pending);
 pendingGeneration=0;pendingSlot=-1;
 restorePending();
 assert(pending.count==preserved.count);
 assert(pending.overflow_count==preserved.overflow_count);

 StoredContext ctx{};
 assert(restoreContext(ctx) && ctx.cycleActive);
 // Falha no contexto preserva dirty e nao promove geracao ate validar write.
 const uint32_t oldContextGeneration=contextGeneration;
 contextDirty=true;
 FakeNVS::failNextWrite=true;
 saveContext();
 assert(contextDirty && contextGeneration==oldContextGeneration);
 saveContext();
 assert(!contextDirty && contextGeneration==oldContextGeneration+1);
 assert(restoreContext(ctx) && ctx.cycleActive);
 // Corrupcao do slot mais recente: recuperar anterior validado.
 const auto latestSlot=pendingSlot;
 const char* corruptKey=latestSlot==0?"pqd0":"pqd1";
 FakeNVS::bytes.at(corruptKey)[12]^=0xFF;
 queue_init(&pending);
 pendingGeneration=0;pendingSlot=-1;
 restorePending();
 assert(!degradedMode && pendingSlot!=latestSlot);
 assert(pending.count<=beforeCount+1);
 std::puts("PASS: integracao fonte firmware real: 20 bordas ISR, ACK, escrita NVS falha, restauracao e CRC");
}
