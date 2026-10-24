#include <cassert>
#include <cstring>
#include <iostream>
#include "transport_ack.h"
int main() {
  EventQueue q{}; queue_init(&q);
  assert(queue_push(&q, "{\"event\":1}", true, 100)==QUEUE_STORED);
  assert(queue_push(&q, "{\"event\":2}", false, 200)==QUEUE_STORED);
  const uint32_t a=TransportAck::token(q.text[0]);
  const uint32_t b=TransportAck::token(q.text[1]);
  assert(a!=b);
  uint32_t v = 0;
  assert(TransportAck::parseHex8("DeAdBeEf",v) && v==0xDEADBEEF);
  assert(!TransportAck::parseHex8("DEADBEE",v));
  assert(!TransportAck::parseHex8("DEADBEEF0",v));
  assert(!TransportAck::parseHex8("DEADBEEG",v));
  assert(!TransportAck::parseHex8(nullptr,v));
  assert(!TransportAck::acceptFront(q,b));
  assert(q.count==2 && strcmp(q.text[0],"{\"event\":1}")==0);
  QueuePersistence::Snapshot persisted{};
  QueuePersistence::prepare(persisted,q,4);
  assert(QueuePersistence::valid(persisted));
  assert(TransportAck::acceptFront(q,a));
  assert(q.count==1 && TransportAck::token(q.text[0])==b);
  assert(!TransportAck::acceptFront(q,a)); // ACK duplicado nao remove outro evento
  QueuePersistence::prepare(persisted,q,5);
  assert(QueuePersistence::valid(persisted));
  assert(TransportAck::acceptFront(q,b));
  assert(q.count==0);
  assert(!TransportAck::acceptFront(q,b));
  std::cout << "PASS: sequencia ACK, rejeicao incorreto/duplicado, fila e CRC\n";
}
