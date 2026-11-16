#pragma once
// Leitura minima do JSON de comando do dashboard (topico cmd), sem biblioteca externa.
// Aceita objetos planos com texto entre aspas ou numero/booleano sem aspas:
//   {"id":"a1","acao":"lacre","valor":"aberto","segundos":30}
// Texto com barra invertida e recusado (o contrato nao usa escapes); aspas no meio de um valor
// sem aspas tambem sao recusadas, porque quebrariam o JSON de cmd/resposta.
#include <cstddef>
#include <cstdlib>
#include <cstring>

namespace DashboardCmd {

constexpr size_t MaxPayload = 256;
constexpr size_t MaxId = 32;
constexpr size_t MaxText = 24;

// Copia o valor de `key` para `out` (terminado em nulo). false = chave ausente, valor vazio,
// maior que `cap` ou texto invalido. Procura so chaves do primeiro nivel (aceita espacos).
inline bool textValue(const char *json, size_t length, const char *key, char *out, size_t cap) {
  if (!json || !key || !out || cap == 0) return false;
  const size_t keyLength = std::strlen(key);
  for (size_t i = 0; i + keyLength + 2 <= length; ++i) {
    if (json[i] != '"' || std::strncmp(json + i + 1, key, keyLength) != 0 || json[i + 1 + keyLength] != '"') {
      continue;
    }
    size_t p = i + keyLength + 2;
    while (p < length && (json[p] == ' ' || json[p] == '\t')) ++p;
    if (p >= length || json[p] != ':') continue;
    ++p;
    while (p < length && (json[p] == ' ' || json[p] == '\t')) ++p;
    if (p >= length) return false;
    size_t n = 0;
    if (json[p] == '"') {
      ++p;
      while (p < length && json[p] != '"') {
        if (json[p] == '\\' || n + 1 >= cap) return false;
        out[n++] = json[p++];
      }
      if (p >= length) return false;  // aspas nao fechadas
    } else {
      while (p < length && json[p] != ',' && json[p] != '}' && json[p] != ' ' && json[p] != '\r' && json[p] != '\n') {
        if (json[p] == '"' || n + 1 >= cap) return false;  // aspas no meio quebrariam o JSON da resposta
        out[n++] = json[p++];
      }
    }
    out[n] = '\0';
    return n > 0;
  }
  return false;
}

// Inteiro de `key` em [minimum, maximum]. false = ausente, nao numerico ou fora da faixa.
inline bool intValue(const char *json, size_t length, const char *key, long minimum, long maximum, long *out) {
  char text[16];
  if (!textValue(json, length, key, text, sizeof(text))) return false;
  char *end = nullptr;
  const long value = std::strtol(text, &end, 10);
  if (end == text || *end != '\0' || value < minimum || value > maximum) return false;
  *out = value;
  return true;
}

}  // namespace DashboardCmd
