#pragma once
#include <stdbool.h>
#include <stdint.h>

// Regras do ciclo de manutencao externa (issue #13): dias fora, status
// periodico e alertas de atraso e permanencia (RF011, RF013, RF014; RN04, RN07).

enum {
  CYCLE_EVENT_STATUS = 1u << 0,  // STATUS_SEMANAL
  CYCLE_EVENT_LATE = 1u << 1,    // ALERTA_ATRASO
  CYCLE_EVENT_STAY = 1u << 2     // ALERTA_PERMANENCIA
};

typedef struct {
  uint32_t cycle_id;
  uint32_t days_out;
  uint32_t expected_return_days;
  uint32_t stay_threshold_days;
  uint32_t status_interval_days;
  bool late_sent;
  bool stay_sent;
} CycleState;

static inline void cycle_start(CycleState *cycle, uint32_t cycle_id, uint32_t expected_return_days,
                               uint32_t stay_threshold_days, uint32_t status_interval_days) {
  cycle->cycle_id = cycle_id;
  cycle->days_out = 0;
  cycle->expected_return_days = expected_return_days;
  cycle->stay_threshold_days = stay_threshold_days;
  cycle->status_interval_days = status_interval_days;
  cycle->late_sent = false;
  cycle->stay_sent = false;
}

// Atraso: passou do prazo previsto. Permanencia: atingiu o limiar. Podem coexistir.
static inline bool cycle_is_late(const CycleState *cycle) {
  return cycle->days_out > cycle->expected_return_days;
}

static inline bool cycle_in_stay(const CycleState *cycle) {
  return cycle->days_out >= cycle->stay_threshold_days;
}

// Avanca um dia e devolve os eventos daquele dia. Cada alerta sai uma vez por ciclo.
static inline unsigned cycle_advance_day(CycleState *cycle) {
  unsigned events = 0;
  ++cycle->days_out;
  if (cycle->status_interval_days > 0 && cycle->days_out % cycle->status_interval_days == 0) {
    events |= CYCLE_EVENT_STATUS;
  }
  if (cycle_is_late(cycle) && !cycle->late_sent) {
    cycle->late_sent = true;
    events |= CYCLE_EVENT_LATE;
  }
  if (cycle_in_stay(cycle) && !cycle->stay_sent) {
    cycle->stay_sent = true;
    events |= CYCLE_EVENT_STAY;
  }
  return events;
}

// Amarelo fixo na permanencia; piscando (blink_on alterna a 1 Hz) no atraso.
static inline bool cycle_yellow_led(const CycleState *cycle, bool blink_on) {
  return cycle_in_stay(cycle) || (cycle_is_late(cycle) && blink_on);
}
