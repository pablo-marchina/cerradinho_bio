#include "wokwi-api.h"
#include "nmea.h"
#include <stdlib.h>

typedef struct {
  uart_dev_t uart;
  uint32_t latitude;
  uint32_t longitude;
  uint32_t fix_valid;
  char message[192];
} gps_state_t;

static void send_position(void *user_data) {
  gps_state_t *gps = (gps_state_t *)user_data;
  int length = make_gga(gps->message, sizeof(gps->message),
                        attr_read_float(gps->latitude),
                        attr_read_float(gps->longitude),
                        attr_read(gps->fix_valid) != 0,
                        (uint32_t)(get_sim_nanos() / 1000000000ULL));
  if (length > 0 && (size_t)length < sizeof(gps->message)) {
    uart_write(gps->uart, (uint8_t *)gps->message, (uint32_t)length);
  }
}

void chip_init(void) {
  gps_state_t *gps = calloc(1, sizeof(gps_state_t));
  if (!gps) return;
  gps->latitude = attr_init_float("latitude", -23.55f);
  gps->longitude = attr_init_float("longitude", -46.6333f);
  gps->fix_valid = attr_init("fixValid", 1);
  const uart_config_t config = {
    .tx = pin_init("TX", INPUT_PULLUP),
    .rx = pin_init("RX", INPUT),
    .baud_rate = 9600,
    .user_data = gps
  };
  gps->uart = uart_init(&config);
  const timer_config_t timer_config = {
    .callback = send_position,
    .user_data = gps
  };
  timer_t timer = timer_init(&timer_config);
  timer_start(timer, 1000000, true);
  // VCC e GND sao representacao visual; nao ha simulacao eletrica de consumo.
}
