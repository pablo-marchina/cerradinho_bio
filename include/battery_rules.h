#pragma once
#include <stdbool.h>
#include <stdint.h>

// Bateria: RF019, RF027; RN10; CT12-CT13. Alerta abaixo de lowPct, rearma acima
// de recoverPct; leitura zero do ADC e falha do sensor, nao bateria vazia.

enum {
  BATTERY_EVENT_LOW = 1u << 0,           // BATERIA_BAIXA
  BATTERY_EVENT_SENSOR_FAILURE = 1u << 1 // SENSOR_BATERIA_FALHA
};

typedef struct {
  uint8_t lowPct;
  uint8_t recoverPct;
  bool lowSent;
  bool failureSent;
} BatteryState;

// Comeca sem alerta enviado, com os limiares de alerta e de rearme.
static inline void batteryInit(BatteryState *battery, uint8_t lowPct, uint8_t recoverPct) {
  battery->lowPct = lowPct;
  battery->recoverPct = recoverPct;
  battery->lowSent = false;
  battery->failureSent = false;
}

// Devolve o evento da leitura: falha com ADC zero, ou alerta uma vez ate rearmar.
static inline unsigned batteryUpdate(BatteryState *battery, int rawAdc, int pct) {
  if (rawAdc == 0) {
    if (battery->failureSent) return 0;
    battery->failureSent = true;
    return BATTERY_EVENT_SENSOR_FAILURE;
  }
  battery->failureSent = false;
  if (pct > battery->recoverPct) battery->lowSent = false;
  if (pct < battery->lowPct && !battery->lowSent) {
    battery->lowSent = true;
    return BATTERY_EVENT_LOW;
  }
  return 0;
}
