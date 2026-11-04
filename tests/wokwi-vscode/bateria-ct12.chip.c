// Bateria do CT12/13/23 no tempo: depois do boot, 3,5 s em cada nivel
// 17 -> 22 -> 17 -> 31 -> 17 -> 0 -> 50 -> 100% (ADC do Wokwi: fundo de escala ~5,08 V medido; 5,1 V garante a leitura de 100%).
#include "../../chips/wokwi-api.h"
#include <stdlib.h>

static const float NIVEIS[] = {78, 17, 22, 17, 31, 17, 0, 50, 100};
#define INICIO_MS 6000
#define PASSO_MS 3500

typedef struct { pin_t out; } State;

static void tick(void *user_data) {
  State *s = (State *)user_data;
  uint32_t ms = (uint32_t)(get_sim_nanos() / 1000000ULL);
  uint32_t i = ms < INICIO_MS ? 0 : 1 + (ms - INICIO_MS) / PASSO_MS;
  if (i > 8) i = 8;
  pin_dac_write(s->out, NIVEIS[i] * 5.1f / 100.0f);
}

void chip_init(void) {
  State *s = calloc(1, sizeof(State));
  if (!s) return;
  pin_init("GND", INPUT);
  s->out = pin_init("OUT", ANALOG);
  const timer_config_t config = { .callback = tick, .user_data = s };
  timer_start(timer_init(&config), 100000, true);
}
