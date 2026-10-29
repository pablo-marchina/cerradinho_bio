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
  uint64_t windowMs;
  bool authorized;
  uint64_t authorizedAtMs;
  bool removalInProgress;
  bool violated;
} SealState;

static inline void sealInit(SealState *seal, uint64_t windowMs) {
  seal->windowMs = windowMs;
  seal->authorized = false;
  seal->authorizedAtMs = 0;
  seal->removalInProgress = false;
  seal->violated = false;
}

static inline void sealAuthorize(SealState *seal, uint64_t nowMs) {
  seal->authorized = true;
  seal->authorizedAtMs = nowMs;
}

// A autorizacao vale windowMs a partir do instante do registro.
static inline bool sealAuthorizationValid(const SealState *seal, uint64_t nowMs) {
  return seal->authorized && nowMs >= seal->authorizedAtMs &&
         nowMs - seal->authorizedAtMs <= seal->windowMs;
}

// A autorizacao vigente e consumida na abertura.
static inline int sealOpened(SealState *seal, uint64_t nowMs) {
  const bool valid = sealAuthorizationValid(seal, nowMs);
  seal->authorized = false;
  if (valid) {
    seal->removalInProgress = true;
    return SEAL_EVENT_REMOVAL;
  }
  seal->violated = true;
  return SEAL_EVENT_VIOLATION;
}

static inline int sealClosed(SealState *seal) {
  if (seal->removalInProgress) {
    seal->removalInProgress = false;
    return SEAL_EVENT_RESEALED;
  }
  if (seal->violated) {
    seal->violated = false;
    return SEAL_EVENT_CLOSED;
  }
  return SEAL_EVENT_NONE;
}
