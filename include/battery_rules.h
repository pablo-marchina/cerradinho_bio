#pragma once
#include <stdbool.h>
#include <stdint.h>

// Bateria: RF019, RF027; RN10; CT12-CT13. Alerta abaixo de low_pct, rearma acima
// de recover_pct; leitura zero do ADC e falha do sensor, nao bateria vazia.

enum {
  BATTERY_EVENT_LOW = 1u << 0,           // BATERIA_BAIXA
  BATTERY_EVENT_SENSOR_FAILURE = 1u << 1 // SENSOR_BATERIA_FALHA
};

typedef struct {
  uint8_t low_pct;
  uint8_t recover_pct;
  bool low_sent;
  bool failure_sent;
} BatteryState;

static inline void battery_init(BatteryState *battery, uint8_t low_pct, uint8_t recover_pct) {
  battery->low_pct = low_pct;
  battery->recover_pct = recover_pct;
  battery->low_sent = false;
  battery->failure_sent = false;
}

static inline unsigned battery_update(BatteryState *battery, int raw_adc, int pct) {
  if (raw_adc == 0) {
    if (battery->failure_sent) return 0;
    battery->failure_sent = true;
    return BATTERY_EVENT_SENSOR_FAILURE;
  }
  battery->failure_sent = false;
  if (pct > battery->recover_pct) battery->low_sent = false;
  if (pct < battery->low_pct && !battery->low_sent) {
    battery->low_sent = true;
    return BATTERY_EVENT_LOW;
  }
  return 0;
}
