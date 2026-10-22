#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// Fila de eventos sem conexao (issue #14): RF010; RN09. Guarda os eventos na ordem
// em que ocorreram; cheia, descarta o nao critico mais antigo e nunca uma violacao.

#ifndef EVENT_QUEUE_CAPACITY
#define EVENT_QUEUE_CAPACITY 8
#endif
#define EVENT_TEXT_MAX 512

typedef struct {
  char text[EVENT_QUEUE_CAPACITY][EVENT_TEXT_MAX];
  bool critical[EVENT_QUEUE_CAPACITY];
  uint8_t count;
  uint32_t lost;  // eventos que nao couberam (todos os guardados eram criticos)
} EventQueue;

enum {
  QUEUE_STORED = 0,
  QUEUE_STORED_DROPPED_OLDEST,  // FILA_CHEIA: saiu o nao critico mais antigo
  QUEUE_REJECTED                // FILA_CHEIA: fila so com criticos
};

static inline void queue_init(EventQueue *queue) {
  queue->count = 0;
  queue->lost = 0;
}

static inline void queue_remove(EventQueue *queue, uint8_t index) {
  for (uint8_t i = index; i + 1 < queue->count; ++i) {
    memcpy(queue->text[i], queue->text[i + 1], EVENT_TEXT_MAX);
    queue->critical[i] = queue->critical[i + 1];
  }
  --queue->count;
}

static inline int queue_push(EventQueue *queue, const char *text, bool critical) {
  int result = QUEUE_STORED;
  if (queue->count == EVENT_QUEUE_CAPACITY) {
    uint8_t oldest = EVENT_QUEUE_CAPACITY;
    for (uint8_t i = 0; i < queue->count; ++i) {
      if (!queue->critical[i]) { oldest = i; break; }
    }
    if (oldest == EVENT_QUEUE_CAPACITY) {
      ++queue->lost;
      return QUEUE_REJECTED;
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
