#pragma once
#include <stdbool.h>
#include <stdint.h>

// Lacre e retirada autorizada (issue #14): RF006, RF007; RN05, RN06.

enum {
  SEAL_EVENT_NONE = 0,
  SEAL_EVENT_VIOLATION,  // VIOLACAO_LACRE
  SEAL_EVENT_REMOVAL,    // RETIRADA_AUTORIZADA
  SEAL_EVENT_RESEALED,   // LACRE_REAPLICADO
  SEAL_EVENT_CLOSED      // LACRE_FECHADO (fechou depois de uma violacao)
};

typedef struct {
  uint32_t window_days;
  bool authorized;
  uint32_t authorized_day;
  bool removal_in_progress;
  bool violated;
} SealState;

static inline void seal_init(SealState *seal, uint32_t window_days) {
  seal->window_days = window_days;
  seal->authorized = false;
  seal->authorized_day = 0;
  seal->removal_in_progress = false;
  seal->violated = false;
}

static inline void seal_authorize(SealState *seal, uint32_t day) {
  seal->authorized = true;
  seal->authorized_day = day;
}

// A autorizacao vale por window_days a partir do registro e e consumida na abertura.
static inline int seal_opened(SealState *seal, uint32_t day) {
  const bool valid = seal->authorized && day - seal->authorized_day <= seal->window_days;
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
