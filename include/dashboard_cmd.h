#pragma once
// Leitura minima do JSON de comando do dashboard (topico cmd), sem biblioteca externa.
// Aceita objetos planos com texto entre aspas ou numero/booleano sem aspas:
//   {"id":"a1","acao":"lacre","valor":"aberto","segundos":30}
// Texto com barra invertida e recusado (o contrato nao usa escapes).
#include <cstddef>
#include <cstdlib>
#include <cstring>

namespace DashboardCmd {

constexpr size_t MaxPayload = 256;
constexpr size_t MaxId = 32;
constexpr size_t MaxText = 24;

inline bool whitespace(char c) {
  return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

inline void skipWhitespace(const char *json, size_t length, size_t &position) {
  while (position < length && whitespace(json[position])) ++position;
}

// Valida o objeto inteiro antes de qualquer efeito no dispositivo. O contrato aceita somente
// objeto plano, chaves e textos entre aspas sem escapes, numeros JSON e true/false/null.
// Rejeitar escapes e estruturas aninhadas mantem o parser pequeno e coerente com o protocolo.
inline bool validObject(const char *json, size_t length) {
  if (!json || length == 0) return false;
  size_t p = 0;
  const auto quoted = [&]() {
    if (p >= length || json[p] != '"') return false;
    ++p;
    while (p < length && json[p] != '"') {
      const unsigned char c = static_cast<unsigned char>(json[p]);
      if (json[p] == '\\' || c < 0x20) return false;
      ++p;
    }
    if (p >= length) return false;
    ++p;
    return true;
  };
  const auto literal = [&](const char *text) {
    const size_t n = std::strlen(text);
    if (p + n > length || std::strncmp(json + p, text, n) != 0) return false;
    p += n;
    return true;
  };
  const auto number = [&]() {
    const size_t start = p;
    if (p < length && json[p] == '-') ++p;
    if (p >= length) return false;
    if (json[p] == '0') {
      ++p;
      if (p < length && json[p] >= '0' && json[p] <= '9') return false;
    } else {
      if (json[p] < '1' || json[p] > '9') return false;
      while (p < length && json[p] >= '0' && json[p] <= '9') ++p;
    }
    if (p < length && json[p] == '.') {
      ++p;
      const size_t decimals = p;
      while (p < length && json[p] >= '0' && json[p] <= '9') ++p;
      if (p == decimals) return false;
    }
    if (p < length && (json[p] == 'e' || json[p] == 'E')) {
      ++p;
      if (p < length && (json[p] == '+' || json[p] == '-')) ++p;
      const size_t exponent = p;
      while (p < length && json[p] >= '0' && json[p] <= '9') ++p;
      if (p == exponent) return false;
    }
    return p > start;
  };

  skipWhitespace(json, length, p);
  if (p >= length || json[p++] != '{') return false;
  skipWhitespace(json, length, p);
  if (p < length && json[p] == '}') {
    ++p;
    skipWhitespace(json, length, p);
    return p == length;
  }
  while (p < length) {
    if (!quoted()) return false;
    skipWhitespace(json, length, p);
    if (p >= length || json[p++] != ':') return false;
    skipWhitespace(json, length, p);
    if (p >= length) return false;
    const bool valueOk = json[p] == '"' ? quoted()
                         : json[p] == 't' ? literal("true")
                         : json[p] == 'f' ? literal("false")
                         : json[p] == 'n' ? literal("null")
                         : number();
    if (!valueOk) return false;
    skipWhitespace(json, length, p);
    if (p >= length) return false;
    if (json[p] == '}') {
      ++p;
      skipWhitespace(json, length, p);
      return p == length;
    }
    if (json[p++] != ',') return false;
    skipWhitespace(json, length, p);
    if (p >= length || json[p] == '}') return false;
  }
  return false;
}

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
        if (n + 1 >= cap) return false;
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
