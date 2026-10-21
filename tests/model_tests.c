#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../chips/nmea.h"

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
  puts("PASS: coordenadas, distancia, tolerancia, fronteiras NMEA, checksum e fix invalido");
  return 0;
}
