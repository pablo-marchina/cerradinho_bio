#pragma once
#include <stdbool.h>
#include <stdint.h>

// Regras do ciclo de manutencao externa: dias fora, status periodico e alertas
// de atraso e permanencia (RF011, RF013, RF014; RN03, RN04, RN07; CT02-CT04).

enum {
  CYCLE_EVENT_STATUS = 1u << 0,  // STATUS_SEMANAL
  CYCLE_EVENT_LATE = 1u << 1,    // ALERTA_ATRASO
  CYCLE_EVENT_STAY = 1u << 2     // ALERTA_PERMANENCIA
};

typedef struct {
  uint32_t cycleId;
  uint32_t daysOut;
  uint32_t expectedReturnDays;
  uint32_t stayThresholdDays;
  uint32_t statusIntervalDays;
  bool lateSent;
  bool staySent;
} CycleState;

static inline void cycleStart(CycleState *cycle, uint32_t cycleId, uint32_t expectedReturnDays,
                               uint32_t stayThresholdDays, uint32_t statusIntervalDays) {
  cycle->cycleId = cycleId;
  cycle->daysOut = 0;
  cycle->expectedReturnDays = expectedReturnDays;
  cycle->stayThresholdDays = stayThresholdDays;
  cycle->statusIntervalDays = statusIntervalDays;
  cycle->lateSent = false;
  cycle->staySent = false;
}

// Atraso: passou do prazo previsto. Permanencia: atingiu o limiar. Podem coexistir.
static inline bool cycleIsLate(const CycleState *cycle) {
  return cycle->daysOut > cycle->expectedReturnDays;
}

static inline bool cycleInStay(const CycleState *cycle) {
  return cycle->daysOut >= cycle->stayThresholdDays;
}

// Avanca um dia e devolve os eventos daquele dia. Cada alerta sai uma vez por ciclo.
static inline unsigned cycleAdvanceDay(CycleState *cycle) {
  unsigned events = 0;
  ++cycle->daysOut;
  if (cycle->statusIntervalDays > 0 && cycle->daysOut % cycle->statusIntervalDays == 0) {
    events |= CYCLE_EVENT_STATUS;
  }
  if (cycleIsLate(cycle) && !cycle->lateSent) {
    cycle->lateSent = true;
    events |= CYCLE_EVENT_LATE;
  }
  if (cycleInStay(cycle) && !cycle->staySent) {
    cycle->staySent = true;
    events |= CYCLE_EVENT_STAY;
  }
  return events;
}

// Muda o limiar sem rearmar o alerta ja emitido no ciclo (RN04): cada alerta sai
// uma vez por ciclo, mesmo que o limiar suba e depois volte a ser atingido.
static inline void cycleSetStayThreshold(CycleState *cycle, uint32_t stayThresholdDays) {
  cycle->stayThresholdDays = stayThresholdDays;
}

// Amarelo fixo na permanencia; piscando (blinkOn alterna a 1 Hz) no atraso.
static inline bool cycleYellowLed(const CycleState *cycle, bool blinkOn) {
  return cycleInStay(cycle) || (cycleIsLate(cycle) && blinkOn);
}
