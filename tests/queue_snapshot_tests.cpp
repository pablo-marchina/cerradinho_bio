#include <cassert>
#include <cstdio>
#include <cstring>
#include "queue_snapshot.h"

// Armazenamento simulado com falha parcial: snapshot anterior jamais e apagado.
struct FakeNvs {
  QueuePersistence::Snapshot slots[2] = {};
  bool written[2] = {false, false};
  bool write(unsigned slot, const QueuePersistence::Snapshot &data, bool powerCut) {
    slots[slot] = data;
    written[slot] = true;
    if (powerCut) {
      reinterpret_cast<unsigned char *>(&slots[slot])[sizeof(data)/2] ^= 0x7F;
      return false;
    }
    return true;
  }
  int recover() const {
    int best = -1;
    for (int i=0;i<2;++i) {
      if (!written[i] || !QueuePersistence::valid(slots[i])) continue;
      if (best < 0 || QueuePersistence::newer(slots[i].generation, slots[best].generation)) best = i;
    }
    return best;
  }
};

int main() {
  EventQueue queue{};
  queue_init(&queue);
  assert(queue_push(&queue, "{\"type\":\"VIOLACAO_LACRE\"}", true, 1) == QUEUE_STORED);
  FakeNvs flash;
  QueuePersistence::Snapshot snapshot{};
  QueuePersistence::prepare(snapshot, queue, 1);
  assert(QueuePersistence::valid(snapshot));
  assert(flash.write(0, snapshot, false));
  assert(flash.recover() == 0);
  assert(queue_push(&queue, "{\"type\":\"STATUS\"}", false, 2) == QUEUE_STORED);
  QueuePersistence::prepare(snapshot, queue, 2);
  assert(!flash.write(1, snapshot, true));
  assert(flash.recover() == 0);  // queda conserva o estado anteriormente confirmado
  assert(flash.write(1, snapshot, false));
  assert(flash.recover() == 1);
  assert(flash.slots[1].queue.count == 2);
  flash.slots[1].queue.text[0][0] ^= 1;
  assert(flash.recover() == 0);  // CRC invalido e descartado
  QueuePersistence::prepare(snapshot, queue, 0);
  snapshot.queue.count = EVENT_QUEUE_CAPACITY + 1;
  assert(!QueuePersistence::valid(snapshot));
  assert(QueuePersistence::newer(0, UINT32_MAX));
  std::puts("PASS: snapshot alternado, queda, checksum, contagem e overflow de geracao");
}
