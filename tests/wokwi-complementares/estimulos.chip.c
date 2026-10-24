// Estímulos elétricos virtuais e observação UART; não produz eventos do firmware.
#include "../../chips/wokwi-api.h"
#include "../../chips/nmea.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
  uart_dev_t gps, trace, monitor;
  pin_t seal, net, auth;
  uint32_t caso, ms;
  uint64_t last_edge_us, first_byte_us, record_edge_us;
  char line[900], txbuf[64][240], gpsbuf[192];
  unsigned line_len, head, tail;
  bool busy;
} state_t;

static void drain(state_t *s) {
  if (!s->busy && s->head != s->tail) {
    s->busy = uart_write(s->trace, (uint8_t *)s->txbuf[s->head], strlen(s->txbuf[s->head]));
  }
}
static void trace(state_t *s, const char *text) {
  unsigned next = (s->tail + 1) % 64;
  if (next == s->head) { printf("FALHA: fila da instrumentacao cheia\n"); return; }
  snprintf(s->txbuf[s->tail], 240, "%s\n", text);
  s->tail = next;
  drain(s);
}
static void written(void *data) {
  state_t *s = data;
  s->head = (s->head + 1) % 64;
  s->busy = false;
  drain(s);
}
static void observe(void *data, uint8_t byte) {
  state_t *s = data;
  if (!s->line_len) {
    s->first_byte_us = get_sim_nanos() / 1000;
    s->record_edge_us = s->last_edge_us;
  }
  if (byte == '\n') {
    s->line[s->line_len] = 0;
    if (s->line[0] == '{' && strstr(s->line, "\"type\":\"seal_changed\"")) {
      char msg[240];
      snprintf(msg, sizeof(msg), "MEDICAO ct=%u borda_us=%llu registro_us=%llu latencia_us=%llu nivel=%u autorizado=%u",
        s->caso, s->record_edge_us, s->first_byte_us, s->first_byte_us - s->record_edge_us,
        pin_read(s->seal), strstr(s->line, "\"authorized_removal\":true") != NULL);
      trace(s, msg);
    }
    s->line_len = 0;
  } else if (s->line_len < sizeof(s->line) - 1) s->line[s->line_len++] = byte;
}
static void edge(state_t *s, unsigned level) {
  pin_write(s->seal, level);
  s->last_edge_us = get_sim_nanos() / 1000;
  char msg[160];
  snprintf(msg, sizeof(msg), "ESTIMULO ct=%u borda_us=%llu lacre=%u", s->caso, s->last_edge_us, level);
  trace(s, msg);
}
static void gps_send(state_t *s) {
  const unsigned t = s->ms;
  bool fix = true, mute = false;
  double lat = -23.55, lon = -46.6333;
  if (s->caso == 18) fix = false;
  if (s->caso == 19) {
    fix = (t >= 20000 && t < 55000) || (t >= 90000 && t < 120000) || t >= 160000;
    mute = t >= 120000 && t < 160000;
  }
  if (s->caso == 20) lat += t >= 50000 ? 0.0018 : (t >= 20000 ? 0.00045 : 0);
  if (s->caso == 22) {
    mute = t >= 35000 && t < 65000;
    if (t >= 65000) lat += 0.00135;
  }
  if (!mute) {
    int n = make_gga(s->gpsbuf, sizeof(s->gpsbuf), lat, lon, fix, t / 1000);
    uart_write(s->gps, (uint8_t *)s->gpsbuf, n);
  }
}
static void tick(void *data) {
  state_t *s = data;
  const unsigned t = ++s->ms;
  if (t == 1000) printf("Timer do gerador ativo, CT%u\n", s->caso);
  if (t % 1000 == 0) gps_send(s);
  if (s->caso == 18) {
    // Duas condições: espera por fix com rede disponível; mesma espera sem rede.
    unsigned local = t >= 40000 ? t - 40000 : t;
    if (t == 42000) { pin_write(s->net, HIGH); trace(s, "REDE indisponivel em 42000000 us"); }
    if (local == 3000 || local == 3200) edge(s, HIGH);
    if (local == 3020 || local == 3240) edge(s, LOW);
    if (local >= 4000 && local <= 7800 && (local - 4000) % 200 == 0) edge(s, ((local - 4000) / 200) % 2 == 0);
    if (t == 60000) trace(s, "FIM CT18");
  }
  if (s->caso == 19) {
    if (t == 6000 || t == 76000 || t == 146000) edge(s, HIGH);
    if (t == 6500 || t == 76500 || t == 146500) edge(s, LOW);
    if (t == 20000) trace(s, "GPS fix valido A a partir de 20 s");
    if (t == 55000) trace(s, "GPS fix invalido, NMEA continua a partir de 55 s");
    if (t == 90000) trace(s, "GPS fix valido novamente a partir de 90 s");
    if (t == 120000) trace(s, "GPS UART SEM NMEA entre 120 e 160 s");
    if (t == 160000) trace(s, "GPS UART retomada em 160 s");
    if (t == 185000) trace(s, "FIM CT19");
  }
  if (s->caso == 20 && t == 70000) trace(s, "FIM CT20: A, B (+50 m), C (+150 m em relacao a B)");
  if (s->caso == 21) {
    if (t >= 4000 && t <= 19000 && t % 1000 == 0) edge(s, (t / 1000) % 2 == 0);
    if (t == 21000) trace(s, "?NVS depois_17");
    if (t == 24000) { pin_write(s->net, HIGH); trace(s, "REDE indisponivel em 24 s"); }
    if (t == 26000) { pin_write(s->net, LOW); trace(s, "REDE disponivel em 26 s"); }
    if (t == 28000) trace(s, "?NVS depois_rede");
    if (t == 30000) trace(s, "FIM CT21: 1 coleta e 16 transicoes = 17 registros");
  }
  if (s->caso == 22) {
    if (t == 20000) { pin_write(s->net, HIGH); trace(s, "REDE indisponivel em 20 s"); }
    if (t == 25000) edge(s, HIGH);
    if (t == 40000) trace(s, "?NVS antes_reset");
    if (t == 44000) trace(s, "?RESET");
    if (t == 60000) trace(s, "?NVS apos_reset_sem_fix");
    if (t == 98000) trace(s, "?NVS apos_B");
    if (t == 100000) { pin_write(s->net, LOW); trace(s, "REDE disponivel em 100 s"); }
    if (t == 105000) trace(s, "FIM CT22");
  }
}
void chip_init(void) {
  static state_t state;
  state_t *s = &state;
  s->caso = attr_read(attr_init("caso", 18));
  printf("Gerador CT%u inicializado\n", s->caso);
  s->seal = pin_init("SEAL", OUTPUT_LOW);
  s->net = pin_init("NET", OUTPUT_LOW);
  s->auth = pin_init("AUTH", OUTPUT_HIGH);
  printf("Pinos inicializados\n");
  const uart_config_t gps = {.tx=pin_init("TX", INPUT_PULLUP), .rx=pin_init("RX", INPUT), .baud_rate=9600, .user_data=s};
  s->gps = uart_init(&gps);
  printf("UART GPS inicializada\n");
  const uart_config_t trace_uart = {.tx=pin_init("TRACE", INPUT_PULLUP), .rx=pin_init("MONITOR", INPUT), .baud_rate=115200, .rx_data=observe, .write_done=written, .user_data=s};
  s->trace = uart_init(&trace_uart);
  printf("UART sonda inicializada\n");
  const timer_config_t timer = {.callback=tick, .user_data=s};
  timer_t clock_timer = timer_init(&timer);
  printf("Timer id=%u\n", clock_timer);
  timer_start(clock_timer, 1000, true);
}
