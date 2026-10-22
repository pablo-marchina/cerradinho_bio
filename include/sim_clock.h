#pragma once
#include <stdint.h>

// Relogio do ciclo em ms de 64 bits. O millis() de 32 bits da volta em ~49,7 dias;
// acumular a diferenca entre leituras (subtracao sem sinal) atravessa essa volta e
// permite contar os 150 dias do RN03/RN04 no modo semanal. Sem RTC, o tempo com o
// dispositivo desligado nao e contado; o valor salvo na NVS retoma a contagem.

typedef struct {
  uint32_t last_ms;
  uint64_t elapsed_ms;
} SimClock;

static inline void sim_clock_init(SimClock *clock, uint32_t now_ms, uint64_t elapsed_ms) {
  clock->last_ms = now_ms;
  clock->elapsed_ms = elapsed_ms;
}

static inline uint64_t sim_clock_update(SimClock *clock, uint32_t now_ms) {
  clock->elapsed_ms += (uint32_t)(now_ms - clock->last_ms);
  clock->last_ms = now_ms;
  return clock->elapsed_ms;
}
