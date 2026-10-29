#pragma once
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "../include/tracking_math.h"

// Arredonda em unidades de 0,0001 minuto antes de separar graus e minutos:
// isso evita produzir o minuto invalido 60.0000 na fronteira entre graus.
static inline void nmeaCoordinate(double value, bool longitude,
                                   char *out, size_t capacity) {
  int64_t units = (int64_t)llround(fabs(value) * 600000.0);
  int degrees = (int)(units / 600000);
  double minutes = (double)(units % 600000) / 10000.0;
  snprintf(out, capacity, longitude ? "%03d%07.4f" : "%02d%07.4f",
           degrees, minutes);
}

static inline int makeGga(char *out, size_t capacity, double lat, double lon,
                           bool fix, uint32_t seconds) {
  char latitude[24], longitude[24], body[150];
  if (!validCoordinates(lat, lon)) {
    lat = 0.0;
    lon = 0.0;
    fix = false;
  }
  nmeaCoordinate(lat, false, latitude, sizeof(latitude));
  nmeaCoordinate(lon, true, longitude, sizeof(longitude));
  seconds %= 86400;
  snprintf(body, sizeof(body),
           "GPGGA,%02u%02u%02u,%s,%c,%s,%c,%d,%02d,1.0,0.0,M,0.0,M,,",
           (unsigned)(seconds / 3600), (unsigned)((seconds / 60) % 60),
           (unsigned)(seconds % 60), latitude, lat < 0 ? 'S' : 'N',
           longitude, lon < 0 ? 'W' : 'E', fix ? 1 : 0, fix ? 8 : 0);
  uint8_t checksum = 0;
  for (const char *p = body; *p; ++p) checksum ^= (uint8_t)*p;
  return snprintf(out, capacity, "$%s*%02X\r\n", body, (unsigned)checksum);
}
