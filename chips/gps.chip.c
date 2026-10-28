#include "wokwi-api.h"
#include "nmea.h"
#include <stdlib.h>

typedef struct {
  uart_dev_t uart;
  uint32_t latitude;
  uint32_t longitude;
  uint32_t fixValid;
  char message[192];
} GpsState;

static void sendPosition(void *user_data) {
  GpsState *gps = (GpsState *)user_data;
  int length = makeGga(gps->message, sizeof(gps->message),
                        attr_read_float(gps->latitude),
                        attr_read_float(gps->longitude),
                        attr_read(gps->fixValid) != 0,
                        (uint32_t)(get_sim_nanos() / 1000000000ULL));
  if (length > 0 && (size_t)length < sizeof(gps->message)) {
    uart_write(gps->uart, (uint8_t *)gps->message, (uint32_t)length);
  }
}

void chip_init(void) {
  GpsState *gps = calloc(1, sizeof(GpsState));
  if (!gps) return;
  gps->latitude = attr_init_float("latitude", -23.55f);
  gps->longitude = attr_init_float("longitude", -46.6333f);
  gps->fixValid = attr_init("fixValid", 1);
  const uart_config_t config = {
    .tx = pin_init("TX", INPUT_PULLUP),
    .rx = pin_init("RX", INPUT),
    .baud_rate = 9600,
    .user_data = gps
  };
  gps->uart = uart_init(&config);
  const timer_config_t timerConfig = {
    .callback = sendPosition,
    .user_data = gps
  };
  timer_t timer = timer_init(&timerConfig);
  timer_start(timer, 1000000, true);
  // VCC e GND sao representacao visual; nao ha simulacao eletrica de consumo.
}
