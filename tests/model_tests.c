#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../chips/nmea.h"
#include "../include/cycle_rules.h"
#include "../include/seal_rules.h"
#include "../include/event_queue.h"
#include "../include/battery_rules.h"

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

  // Lacre (issue #14): autorizacao vale 3 dias e e consumida; sem ela, violacao.
  SealState seal;
  seal_init(&seal, 3);
  assert(seal_opened(&seal, 10) == SEAL_EVENT_VIOLATION && seal.violated);
  assert(seal_closed(&seal) == SEAL_EVENT_CLOSED && !seal.violated);
  seal_authorize(&seal, 20);
  assert(seal_opened(&seal, 23) == SEAL_EVENT_REMOVAL && seal.removal_in_progress);
  assert(seal_closed(&seal) == SEAL_EVENT_RESEALED && !seal.removal_in_progress);
  assert(seal_opened(&seal, 23) == SEAL_EVENT_VIOLATION);  // autorizacao ja usada
  seal_closed(&seal);
  seal_authorize(&seal, 30);
  assert(seal_opened(&seal, 34) == SEAL_EVENT_VIOLATION);  // passou de 3 dias
  assert(seal_closed(&seal) == SEAL_EVENT_CLOSED);
  assert(seal_closed(&seal) == SEAL_EVENT_NONE);

  // Fila (issue #14): 8 eventos, cheia descarta o nao critico mais antigo.
  static EventQueue queue;
  queue_init(&queue);
  char text[16];
  for (int i = 0; i < EVENT_QUEUE_CAPACITY; ++i) {
    snprintf(text, sizeof(text), "e%d", i);
    assert(queue_push(&queue, text, i == 1) == QUEUE_STORED);
  }
  assert(queue_push(&queue, "e8", false) == QUEUE_STORED_DROPPED_OLDEST);
  assert(queue.count == EVENT_QUEUE_CAPACITY);
  assert(strcmp(queue.text[0], "e1") == 0 && queue.critical[0]);  // e0 saiu, violacao ficou
  assert(strcmp(queue.text[EVENT_QUEUE_CAPACITY - 1], "e8") == 0);
  queue_init(&queue);
  for (int i = 0; i < EVENT_QUEUE_CAPACITY; ++i) queue_push(&queue, "v", true);
  assert(queue_push(&queue, "x", false) == QUEUE_REJECTED && queue.lost == 1);
  queue_remove(&queue, 0);
  assert(queue.count == EVENT_QUEUE_CAPACITY - 1);

  // Bateria (issue #14): alerta uma vez abaixo de 20%, rearma acima de 25%; ADC 0 e falha.
  BatteryState battery;
  battery_init(&battery, 20, 25);
  assert(battery_update(&battery, 3000, 73) == 0);
  assert(battery_update(&battery, 700, 17) == BATTERY_EVENT_LOW);
  assert(battery_update(&battery, 600, 15) == 0);
  assert(battery_update(&battery, 900, 22) == 0);   // histerese: ainda nao rearmou
  assert(battery_update(&battery, 700, 17) == 0);
  assert(battery_update(&battery, 1100, 27) == 0);  // rearmou
  assert(battery_update(&battery, 700, 17) == BATTERY_EVENT_LOW);
  assert(battery_update(&battery, 0, 0) == BATTERY_EVENT_SENSOR_FAILURE);
  assert(battery_update(&battery, 0, 0) == 0);
  puts("PASS: coordenadas, distancia, tolerancia, fronteiras NMEA, checksum, fix invalido e regras do ciclo, lacre, fila e bateria");
  return 0;
}
