#pragma once
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "event_queue.h"

// Snapshot autocontido: a gravacao ocorre no slot alternativo. O slot antigo
// permanece valido ate que a nova gravacao seja integral e verificavel.
// Nao garante energia ininterrupta nem entrega remota: somente recuperacao local.
namespace QueuePersistence {
constexpr uint32_t Magic = 0x47545131UL;  // 'GTQ1'
constexpr uint32_t Version = 1;
struct Snapshot {
  uint32_t magic;
  uint32_t version;
  uint32_t generation;
  EventQueue queue;
  uint32_t checksum;
};

inline uint32_t crc32(const uint8_t *data, size_t length) {
  uint32_t crc = 0xFFFFFFFFUL;
  for (size_t i = 0; i < length; ++i) {
    crc ^= data[i];
    for (unsigned bit = 0; bit < 8; ++bit) {
      crc = (crc >> 1) ^ ((crc & 1U) ? 0xEDB88320UL : 0U);
    }
  }
  return ~crc;
}

inline void prepare(Snapshot &snapshot, const EventQueue &queue, uint32_t generation) {
  memset(&snapshot, 0, sizeof(snapshot));
  snapshot.magic = Magic;
  snapshot.version = Version;
  snapshot.generation = generation;
  snapshot.queue = queue;
  snapshot.checksum = crc32(reinterpret_cast<const uint8_t *>(&snapshot),
                            offsetof(Snapshot, checksum));
}

inline bool valid(const Snapshot &snapshot) {
  if (snapshot.magic != Magic || snapshot.version != Version ||
      snapshot.queue.count > EVENT_QUEUE_CAPACITY) return false;
  for (uint8_t i = 0; i < snapshot.queue.count; ++i) {
    if (!memchr(snapshot.queue.text[i], '\0', EVENT_TEXT_MAX) ||
        snapshot.queue.text[i][0] == '\0') return false;
  }
  return snapshot.checksum == crc32(reinterpret_cast<const uint8_t *>(&snapshot),
                                    offsetof(Snapshot, checksum));
}

// Comparacao segura do contador com retorno circular, assumindo distancia
// entre geracoes inferior a 2^31, verdade para dois slots alternados.
inline bool newer(uint32_t candidate, uint32_t current) {
  return static_cast<int32_t>(candidate - current) > 0;
}
}
