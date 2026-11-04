// GPS do CT45 com estimulos no tempo: sem fix ate 60 s; NET (GPIO18) em HIGH,
// rede indisponivel, ate 110 s. Fora isso, igual ao chips/gps.chip.c.
#include "../../chips/wokwi-api.h"
#include "../../chips/nmea.h"
#include <stdlib.h>

#define FIX_EM_S 60
#define REDE_EM_S 110

typedef struct {
  uart_dev_t uart;
  pin_t net;
  char message[192];
} GpsState;

static void tick(void *user_data) {
  GpsState *gps = (GpsState *)user_data;
  uint32_t s = (uint32_t)(get_sim_nanos() / 1000000000ULL);
  pin_write(gps->net, s >= REDE_EM_S ? LOW : HIGH);
  int length = makeGga(gps->message, sizeof(gps->message), -23.55f, -46.6333f, s >= FIX_EM_S, s);
  if (length > 0 && (size_t)length < sizeof(gps->message)) {
    uart_write(gps->uart, (uint8_t *)gps->message, (uint32_t)length);
  }
}

void chip_init(void) {
  GpsState *gps = calloc(1, sizeof(GpsState));
  if (!gps) return;
  gps->net = pin_init("NET", OUTPUT_HIGH);
  const uart_config_t config = {
    .tx = pin_init("TX", INPUT_PULLUP),
    .rx = pin_init("RX", INPUT),
    .baud_rate = 9600,
    .user_data = gps
  };
  gps->uart = uart_init(&config);
  const timer_config_t timerConfig = { .callback = tick, .user_data = gps };
  timer_start(timer_init(&timerConfig), 1000000, true);
}
