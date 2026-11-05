#pragma once
#include <stdint.h>

// Nucleo portavel do debounce: nao descarta periodos estaveis apenas porque o
// loop ficou ocupado entre duas bordas. ISR captura bordas, esta rotina decide.
struct SealDebounceEvent {
  bool opened = false;
  uint32_t confirmedAt = 0;
};

inline bool seal_on_edge(bool &raw, bool &stable, uint32_t &changedAt,
                         bool nextRaw, uint32_t edgeAt, uint32_t debounceMs,
                         SealDebounceEvent &out) {
  bool emitted = false;
  if (raw != stable && (uint32_t)(edgeAt - changedAt) >= debounceMs) {
    stable = raw;
    out.opened = raw;
    out.confirmedAt = changedAt + debounceMs;
    emitted = true;
  }
  if (nextRaw != raw) { raw = nextRaw; changedAt = edgeAt; }
  return emitted;
}

inline bool seal_settle(bool raw, bool &stable, uint32_t changedAt,
                        uint32_t now, uint32_t debounceMs, SealDebounceEvent &out) {
  if (raw == stable || (uint32_t)(now - changedAt) < debounceMs) return false;
  stable = raw;
  out.opened = raw;
  out.confirmedAt = changedAt + debounceMs;
  return true;
}
