#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../chips/nmea.h"
#include "../include/cycle_rules.h"
#include "../include/seal_rules.h"
#include "../include/event_queue.h"
#include "../include/battery_rules.h"
#include "../include/sim_clock.h"

#define DAY_MS (24ULL * 60ULL * 60ULL * 1000ULL)

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

  // Ciclo (CT02-CT04): status a cada 7 dias, alertas distintos e emitidos uma vez.
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

  // Relogio (CT04 semanal): o millis() de 32 bits da volta em ~49,7 dias; o relogio
  // acumulado atravessa a volta e o ciclo chega aos 150 dias reais.
  {
    SimClock clock;
    sim_clock_init(&clock, 0xFFFFFF00u, 0);
    assert(sim_clock_update(&clock, 0x00000100u) == 0x200);  // atravessou a volta
    uint32_t millis_now = 0;
    sim_clock_init(&clock, millis_now, 0);
    CycleState cycle;
    cycle_start(&cycle, 1, 5, 150, 7);
    unsigned stay = 0, wraps = 0;
    for (uint32_t hour = 1; hour <= 151 * 24; ++hour) {
      const uint32_t before = millis_now;
      millis_now += 60u * 60u * 1000u;  // uint32_t, como o millis()
      if (millis_now < before) ++wraps;
      const uint64_t elapsed = sim_clock_update(&clock, millis_now);
      while (cycle.days_out < elapsed / DAY_MS) {
        if (cycle_advance_day(&cycle) & CYCLE_EVENT_STAY) ++stay;
      }
    }
    assert(wraps == 3 && cycle.days_out == 151 && stay == 1);
    // Contexto restaurado da NVS: retoma a contagem a partir do valor salvo.
    sim_clock_init(&clock, 12345u, 151 * DAY_MS);
    assert(sim_clock_update(&clock, 12345u + 1000u) == 151 * DAY_MS + 1000u);
  }

  // LIMIAR (RN04): mudar o limiar nao rearma o alerta de permanencia ja emitido.
  {
    CycleState cycle;
    cycle_start(&cycle, 1, 5, 10, 7);
    unsigned stay = 0;
    for (int day = 1; day <= 10; ++day) {
      if (cycle_advance_day(&cycle) & CYCLE_EVENT_STAY) ++stay;
    }
    assert(stay == 1);
    cycle_set_stay_threshold(&cycle, 20);
    for (int day = 11; day <= 12; ++day) cycle_advance_day(&cycle);
    cycle_set_stay_threshold(&cycle, 12);
    for (int day = 13; day <= 30; ++day) {
      if (cycle_advance_day(&cycle) & CYCLE_EVENT_STAY) ++stay;
    }
    assert(stay == 1 && cycle.stay_sent);
    // Antes de emitir, baixar o limiar abaixo de dias_fora emite no dia seguinte, uma vez.
    cycle_start(&cycle, 2, 5, 150, 7);
    for (int day = 1; day <= 30; ++day) cycle_advance_day(&cycle);
    cycle_set_stay_threshold(&cycle, 10);
    assert(cycle_advance_day(&cycle) & CYCLE_EVENT_STAY);
    assert(!(cycle_advance_day(&cycle) & CYCLE_EVENT_STAY));
  }

  // Lacre (CT05-CT07): autorizacao vale 3 dias desde o registro e e consumida.
  SealState seal;
  seal_init(&seal, 3 * DAY_MS);
  assert(seal_opened(&seal, 10 * DAY_MS) == SEAL_EVENT_VIOLATION && seal.violated);
  assert(seal_closed(&seal) == SEAL_EVENT_CLOSED && !seal.violated);
  seal_authorize(&seal, 20 * DAY_MS);
  assert(seal_opened(&seal, 23 * DAY_MS) == SEAL_EVENT_REMOVAL && seal.removal_in_progress);
  assert(seal_closed(&seal) == SEAL_EVENT_RESEALED && !seal.removal_in_progress);
  assert(seal_opened(&seal, 23 * DAY_MS) == SEAL_EVENT_VIOLATION);  // autorizacao ja usada
  seal_closed(&seal);
  seal_authorize(&seal, 30 * DAY_MS);
  assert(seal_opened(&seal, 34 * DAY_MS) == SEAL_EVENT_VIOLATION);  // passou de 3 dias
  assert(seal_closed(&seal) == SEAL_EVENT_CLOSED);
  assert(seal_closed(&seal) == SEAL_EVENT_NONE);
  // Medido em ms: autorizar no fim do dia e abrir 3 dias e 1 ms depois e violacao
  // (com dias truncados, isso passava e a janela chegava a quase 4 dias).
  const uint64_t late_in_day = 40 * DAY_MS + DAY_MS - 1000;
  seal_authorize(&seal, late_in_day);
  assert(seal_opened(&seal, late_in_day + 3 * DAY_MS + 1) == SEAL_EVENT_VIOLATION);
  seal_closed(&seal);
  seal_authorize(&seal, late_in_day);
  assert(seal_opened(&seal, late_in_day + 3 * DAY_MS) == SEAL_EVENT_REMOVAL);  // limite exato
  seal_closed(&seal);
  // Ligar com o lacre aberto: o firmware parte da referencia fechada e chama
  // seal_opened na primeira leitura estavel; sem autorizacao, e violacao (CT05).
  seal_init(&seal, 3 * DAY_MS);
  assert(seal_opened(&seal, 0) == SEAL_EVENT_VIOLATION && seal.violated);

  // Fila (CT08-CT10): 8 eventos, cheia descarta o nao critico mais antigo.
  static EventQueue queue;
  queue_init(&queue);
  char text[16];
  for (int i = 0; i < EVENT_QUEUE_CAPACITY; ++i) {
    snprintf(text, sizeof(text), "e%d", i);
    assert(queue_push(&queue, text, i == 1, (uint64_t)i) == QUEUE_STORED);
  }
  assert(queue_push(&queue, "e8", false, 8) == QUEUE_STORED_DROPPED_OLDEST);
  assert(queue.count == EVENT_QUEUE_CAPACITY);
  assert(strcmp(queue.text[0], "e1") == 0 && queue.critical[0]);  // e0 saiu, violacao ficou
  assert(strcmp(queue.text[EVENT_QUEUE_CAPACITY - 1], "e8") == 0);
  queue_init(&queue);
  for (int i = 0; i < EVENT_QUEUE_CAPACITY; ++i) queue_push(&queue, "v", true, 0);
  assert(queue_push(&queue, "x", false, 0) == QUEUE_REJECTED && queue.lost == 1);
  assert(queue.overflow_count == 0);
  queue_remove(&queue, 0);
  assert(queue.count == EVENT_QUEUE_CAPACITY - 1);

  // CT10 so com violacoes: 12 violacoes sem rede = 8 individuais + resumo de 4 com a
  // primeira e a ultima ocorrencia; nenhuma violacao perdida e o flush entrega tudo.
  queue_init(&queue);
  for (int i = 0; i < 12; ++i) {
    snprintf(text, sizeof(text), "v%d", i);
    const int result = queue_push(&queue, text, true, (uint64_t)(i + 1) * DAY_MS);
    assert(result == (i < EVENT_QUEUE_CAPACITY ? QUEUE_STORED : QUEUE_SUMMARIZED));
  }
  assert(queue.count == EVENT_QUEUE_CAPACITY && queue.lost == 0);
  assert(queue.overflow_count == 4);
  assert(queue.overflow_first_ms == 9 * DAY_MS && queue.overflow_last_ms == 12 * DAY_MS);
  assert(queue_push(&queue, "n", false, 13 * DAY_MS) == QUEUE_REJECTED && queue.lost == 1);
  assert(queue.overflow_count == 4);  // nao critico nao entra no resumo
  unsigned delivered = 0;
  while (queue.count > 0) {  // flush: posicoes na ordem, depois o resumo
    snprintf(text, sizeof(text), "v%u", delivered);
    assert(strcmp(queue.text[0], text) == 0);
    queue_remove(&queue, 0);
    ++delivered;
  }
  delivered += queue.overflow_count;
  queue_clear_summary(&queue);
  assert(delivered == 12 && queue.overflow_count == 0 && queue.overflow_first_ms == 0);
  // Depois do envio, o resumo recomeca do zero.
  for (int i = 0; i < EVENT_QUEUE_CAPACITY + 1; ++i) queue_push(&queue, "v", true, 20 * DAY_MS + i);
  assert(queue.overflow_count == 1 && queue.overflow_first_ms == 20 * DAY_MS + 8 &&
         queue.overflow_last_ms == 20 * DAY_MS + 8);

  // Bateria (CT12-CT13): alerta uma vez abaixo de 20%, rearma acima de 25%; ADC 0 e falha.
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
  puts("PASS: coordenadas, distancia, tolerancia, fronteiras NMEA, checksum, fix invalido e regras do ciclo, relogio, limiar, lacre, fila com resumo e bateria");
  return 0;
}
