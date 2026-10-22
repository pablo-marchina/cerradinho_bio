#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// Fila de eventos sem conexao: RF010; RN09; CT08-CT10. Guarda os eventos na ordem
// em que ocorreram; cheia, descarta o nao critico mais antigo e nunca uma violacao.
// Com todas as posicoes ocupadas por violacoes, as seguintes entram num unico resumo
// (quantidade, primeira e ultima ocorrencia), enviado depois das posicoes no flush.

#ifndef EVENT_QUEUE_CAPACITY
#define EVENT_QUEUE_CAPACITY 8
#endif
#define EVENT_TEXT_MAX 512

typedef struct {
  char text[EVENT_QUEUE_CAPACITY][EVENT_TEXT_MAX];
  bool critical[EVENT_QUEUE_CAPACITY];
  uint8_t count;
  uint32_t lost;  // eventos nao criticos que nao couberam (todos os guardados eram criticos)
  uint32_t overflow_count;     // violacoes alem da capacidade, guardadas so no resumo
  uint64_t overflow_first_ms;  // relogio do ciclo (sim_clock.h) da primeira e da ultima
  uint64_t overflow_last_ms;
} EventQueue;

enum {
  QUEUE_STORED = 0,
  QUEUE_STORED_DROPPED_OLDEST,  // FILA_CHEIA: saiu o nao critico mais antigo
  QUEUE_REJECTED,               // FILA_CHEIA: fila so com criticos; nao critico perdido
  QUEUE_SUMMARIZED              // FILA_CHEIA: fila so com criticos; violacao no resumo
};

static inline void queue_init(EventQueue *queue) {
  queue->count = 0;
  queue->lost = 0;
  queue->overflow_count = 0;
  queue->overflow_first_ms = 0;
  queue->overflow_last_ms = 0;
}

// Zera o resumo depois de enviado.
static inline void queue_clear_summary(EventQueue *queue) {
  queue->overflow_count = 0;
  queue->overflow_first_ms = 0;
  queue->overflow_last_ms = 0;
}

static inline void queue_remove(EventQueue *queue, uint8_t index) {
  for (uint8_t i = index; i + 1 < queue->count; ++i) {
    memcpy(queue->text[i], queue->text[i + 1], EVENT_TEXT_MAX);
    queue->critical[i] = queue->critical[i + 1];
  }
  --queue->count;
}

static inline int queue_push(EventQueue *queue, const char *text, bool critical, uint64_t at_ms) {
  int result = QUEUE_STORED;
  if (queue->count == EVENT_QUEUE_CAPACITY) {
    uint8_t oldest = EVENT_QUEUE_CAPACITY;
    for (uint8_t i = 0; i < queue->count; ++i) {
      if (!queue->critical[i]) { oldest = i; break; }
    }
    if (oldest == EVENT_QUEUE_CAPACITY) {
      if (!critical) {
        ++queue->lost;
        return QUEUE_REJECTED;
      }
      if (queue->overflow_count == 0) queue->overflow_first_ms = at_ms;
      queue->overflow_last_ms = at_ms;
      ++queue->overflow_count;
      return QUEUE_SUMMARIZED;
    }
    queue_remove(queue, oldest);
    result = QUEUE_STORED_DROPPED_OLDEST;
  }
  strncpy(queue->text[queue->count], text, EVENT_TEXT_MAX - 1);
  queue->text[queue->count][EVENT_TEXT_MAX - 1] = '\0';
  queue->critical[queue->count] = critical;
  ++queue->count;
  return result;
}
