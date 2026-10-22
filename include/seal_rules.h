#pragma once
#include <stdbool.h>
#include <stdint.h>

// Lacre e retirada autorizada: RF006, RF007; RN05, RN06; CT05-CT07.

enum {
  SEAL_EVENT_NONE = 0,
  SEAL_EVENT_VIOLATION,  // VIOLACAO_LACRE
  SEAL_EVENT_REMOVAL,    // RETIRADA_AUTORIZADA
  SEAL_EVENT_RESEALED,   // LACRE_REAPLICADO
  SEAL_EVENT_CLOSED      // LACRE_FECHADO (fechou depois de uma violacao)
};

// Tempos em ms do relogio do ciclo (sim_clock.h), nao em dias truncados.
typedef struct {
  uint64_t window_ms;
  bool authorized;
  uint64_t authorized_at_ms;
  bool removal_in_progress;
  bool violated;
} SealState;

static inline void seal_init(SealState *seal, uint64_t window_ms) {
  seal->window_ms = window_ms;
  seal->authorized = false;
  seal->authorized_at_ms = 0;
  seal->removal_in_progress = false;
  seal->violated = false;
}

static inline void seal_authorize(SealState *seal, uint64_t now_ms) {
  seal->authorized = true;
  seal->authorized_at_ms = now_ms;
}

// A autorizacao vale window_ms a partir do instante do registro e e consumida na abertura.
static inline int seal_opened(SealState *seal, uint64_t now_ms) {
  const bool valid = seal->authorized && now_ms >= seal->authorized_at_ms &&
                     now_ms - seal->authorized_at_ms <= seal->window_ms;
  seal->authorized = false;
  if (valid) {
    seal->removal_in_progress = true;
    return SEAL_EVENT_REMOVAL;
  }
  seal->violated = true;
  return SEAL_EVENT_VIOLATION;
}

static inline int seal_closed(SealState *seal) {
  if (seal->removal_in_progress) {
    seal->removal_in_progress = false;
    return SEAL_EVENT_RESEALED;
  }
  if (seal->violated) {
    seal->violated = false;
    return SEAL_EVENT_CLOSED;
  }
  return SEAL_EVENT_NONE;
}
