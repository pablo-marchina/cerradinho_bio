#include <cassert>
#include <cstdio>
#include "seal_debounce.h"
int main() {
  bool raw=false, stable=false;
  uint32_t changed=0;
  SealDebounceEvent event{};
  unsigned emitted=0, opened=0, closed=0;
  auto record=[&](){++emitted; if(event.opened) ++opened; else ++closed;};
  // CT18: 20 transicoes estaveis de 200 ms, lidas somente depois de todas
  // as bordas — simula um loop ocupado com Serial/NVS.
  for (unsigned i=0;i<20;++i) {
    const bool next=(i%2)==0;
    if (seal_on_edge(raw,stable,changed,next,i*200+100,60,event))record();
  }
  if (seal_settle(raw,stable,changed,20*200+100,60,event))record();
  assert(emitted==20 && opened==10 && closed==10);
  // Pulsos de 20 e 40 ms nao sao estaveis, logo nao devem contar.
  raw=false;stable=false;changed=0; emitted=0;
  for(unsigned i=0;i<10;++i){
    uint32_t at=i*200+100;
    if(seal_on_edge(raw,stable,changed,true,at,60,event))++emitted;
    if(seal_on_edge(raw,stable,changed,false,at+(i%2?40:20),60,event))++emitted;
  }
  assert(!seal_settle(raw,stable,changed,3000,60,event));
  assert(emitted==0);
  // Wrap millis de 32 bits: duracao calculada modulo 2^32.
  raw=false;stable=false;changed=0;
  assert(!seal_on_edge(raw,stable,changed,true,UINT32_MAX-30,60,event));
  assert(seal_settle(raw,stable,changed,40,60,event));
  assert(event.opened);
  std::puts("PASS: debounce 20/20 estados estaveis em backlog, pulsos curtos e rollover");
}
