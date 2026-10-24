#pragma once
#include <stdint.h>
#include <string.h>
#include "queue_snapshot.h"

// Somente para a bancada; CRC32 e identificador de demonstracao, NAO
// autenticador criptografico nem chave MQTT. A fila so e removida por ACK explicito.
namespace TransportAck {
inline uint32_t token(const char *payload) {
  return QueuePersistence::crc32(reinterpret_cast<const uint8_t *>(payload), strlen(payload));
}
inline bool parseHex8(const char *text, uint32_t &out) {
  if (!text || strlen(text) != 8) return false;
  uint32_t parsed = 0;
  for (unsigned i = 0; i < 8; ++i) {
    const char c = text[i];
    const uint32_t nibble = c >= '0' && c <= '9' ? c - '0' :
      c >= 'A' && c <= 'F' ? c - 'A' + 10 :
      c >= 'a' && c <= 'f' ? c - 'a' + 10 : 16;
    if (nibble > 15) return false;
    parsed = (parsed << 4) | nibble;
  }
  out = parsed;
  return true;
}
inline bool acceptFront(EventQueue &queue, uint32_t suppliedToken) {
  if (queue.count == 0 || token(queue.text[0]) != suppliedToken) return false;
  queue_remove(&queue, 0);
  return true;
}
}
