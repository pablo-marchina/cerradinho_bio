#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../chips/nmea.h"
#include "../include/cycle_rules.h"

int main(void) {
  assert(valid_coordinates(0.0, 0.0));  // Zero nao e ausencia de fix.
  assert(!valid_coordinates(91.0, 0.0));
  assert(!valid_coordinates(0.0, 181.0));
  assert(!valid_coordinates(NAN, 0.0));
  assert(distance_m(-23.55, -46.6333, -23.55, -46.6333) < 0.001);
  double oneDegree = distance_m(0, 0, 1, 0);
  assert(oneDegree > 111000 && oneDegree < 111300);
  assert(distance_m(0, 179.999, 0, -179.999) < 225);
  assert(distance_m(-23.55, -46.6333, -23.5499, -46.6333) < 100);
  assert(distance_m(-23.55, -46.6333, -23.54, -46.6333) > 1000);
  assert(isfinite(distance_m(0, 0, 0, 180)));

  char point[32], sentence[192];
  nmea_coordinate(-23.55, false, point, sizeof(point));
  assert(strcmp(point, "2333.0000") == 0);
  nmea_coordinate(-46.6333333333, true, point, sizeof(point));
  assert(strcmp(point, "04638.0000") == 0);
  nmea_coordinate(12.999999999, false, point, sizeof(point));
  assert(strcmp(point, "1300.0000") == 0);
  int length = make_gga(sentence, sizeof(sentence), -23.55, -46.6333333333, true, 45296);
  assert(length > 0 && length < (int)sizeof(sentence));
  assert(strstr(sentence, "$GPGGA,123456,2333.0000,S,04638.0000,W,1,08,") == sentence);
  char *star = strchr(sentence, '*');
  assert(star && strcmp(sentence + length - 2, "\r\n") == 0);
  unsigned checksum = 0;
  for (char *p = sentence + 1; p < star; ++p) checksum ^= (unsigned char)*p;
  assert(checksum == strtoul(star + 1, NULL, 16));
  make_gga(sentence, sizeof(sentence), 0, 0, false, 0);
  assert(strstr(sentence, ",0,00,1.0,") != NULL);
  make_gga(sentence, sizeof(sentence), NAN, 0, true, 0);
  assert(strstr(sentence, ",0,00,1.0,") != NULL);

  // Ciclo (issue #13): status a cada 7 dias, alertas distintos e emitidos uma vez.
  for (uint32_t threshold = 10; threshold <= 150; threshold += 140) {
    CycleState cycle;
    cycle_start(&cycle, 1, 5, threshold, 7);
    unsigned statuses = 0, late = 0, stay = 0;
    uint32_t lateDay = 0, stayDay = 0;
    for (uint32_t day = 1; day <= threshold + 20; ++day) {
      unsigned events = cycle_advance_day(&cycle);
      if (events & CYCLE_EVENT_STATUS) { ++statuses; assert(day % 7 == 0); }
      if (events & CYCLE_EVENT_LATE) { ++late; lateDay = day; }
      if (events & CYCLE_EVENT_STAY) { ++stay; stayDay = day; }
      if (day == 5) assert(!cycle_yellow_led(&cycle, true));
      if (day == 6) assert(cycle_yellow_led(&cycle, true) && !cycle_yellow_led(&cycle, false));
      if (day == threshold) assert(cycle_yellow_led(&cycle, false));
    }
    assert(statuses == (threshold + 20) / 7);
    assert(late == 1 && lateDay == 6);
    assert(stay == 1 && stayDay == threshold);
    assert(cycle_is_late(&cycle) && cycle_in_stay(&cycle));
  }
  puts("PASS: coordenadas, distancia, tolerancia, fronteiras NMEA, checksum, fix invalido e regras do ciclo");
  return 0;
}
