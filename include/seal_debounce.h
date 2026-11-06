#pragma once
#include <stdint.h>

// Nucleo portavel do debounce: nao descarta periodos estaveis apenas porque o
// loop ficou ocupado entre duas bordas. ISR captura bordas, esta rotina decide.
struct SealDebounceEvent {
  bool opened = false;
  uint32_t confirmedAt = 0;
};

// Diferenca com sinal: a ISR pode registrar uma borda depois do now do loop, e a conta sem
// sinal daria um valor enorme, confirmando um pulso curto sem cumprir o debounce (CT18).
inline bool seal_stable_for(uint32_t changedAt, uint32_t at, uint32_t debounceMs) {
  return (int32_t)(at - changedAt) >= (int32_t)debounceMs;
}

// Trata uma borda: confirma o nivel anterior se ele ficou estavel, depois guarda o novo nivel.
inline bool seal_on_edge(bool &raw, bool &stable, uint32_t &changedAt,
                         bool nextRaw, uint32_t edgeAt, uint32_t debounceMs,
                         SealDebounceEvent &out) {
  bool emitted = false;
  if (raw != stable && seal_stable_for(changedAt, edgeAt, debounceMs)) {
    stable = raw;
    out.opened = raw;
    out.confirmedAt = changedAt + debounceMs;
    emitted = true;
  }
  if (nextRaw != raw) { raw = nextRaw; changedAt = edgeAt; }
  return emitted;
}

// Confirma o nivel atual quando ele ficou estavel pelo tempo de debounce.
inline bool seal_settle(bool raw, bool &stable, uint32_t changedAt,
                        uint32_t now, uint32_t debounceMs, SealDebounceEvent &out) {
  if (raw == stable || !seal_stable_for(changedAt, now, debounceMs)) return false;
  stable = raw;
  out.opened = raw;
  out.confirmedAt = changedAt + debounceMs;
  return true;
}
