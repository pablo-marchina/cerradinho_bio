#pragma once
#include <stdint.h>

// Relogio do ciclo em ms de 64 bits. O millis() de 32 bits da volta em ~49,7 dias;
// acumular a diferenca entre leituras (subtracao sem sinal) atravessa essa volta e
// permite contar os 150 dias do RN03/RN04 no modo semanal. Sem RTC, o tempo com o
// dispositivo desligado nao e contado; o valor salvo na NVS retoma a contagem.

typedef struct {
  uint32_t lastMs;
  uint64_t elapsedMs;
} SimClock;

static inline void simClockInit(SimClock *clock, uint32_t nowMs, uint64_t elapsedMs) {
  clock->lastMs = nowMs;
  clock->elapsedMs = elapsedMs;
}

static inline uint64_t simClockUpdate(SimClock *clock, uint32_t nowMs) {
  clock->elapsedMs += (uint32_t)(nowMs - clock->lastMs);
  clock->lastMs = nowMs;
  return clock->elapsedMs;
}
