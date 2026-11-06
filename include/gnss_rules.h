#pragma once
#include <stdbool.h>
#include <stdint.h>

// Falhas do GNSS (regioes remotas, interferencia, modulo sem dados). Um evento por
// ocorrencia: GNSS_SEM_FIX na primeira coleta sem posicao e GNSS_RECUPERADO na
// primeira coleta valida depois dela, com a duracao e as coletas perdidas.

enum {
  GNSS_EVENT_NONE = 0,
  GNSS_EVENT_LOST,      // GNSS_SEM_FIX
  GNSS_EVENT_RECOVERED  // GNSS_RECUPERADO
};

enum {
  GNSS_REASON_NO_FIX = 0,  // NMEA chegando, sem posicao (ceu encoberto, area remota)
  GNSS_REASON_NO_DATA,     // nenhum NMEA recente: modulo, cabo ou alimentacao
  GNSS_REASON_WEAK_SIGNAL  // poucos satelites visiveis (interferencia, obstaculos)
};

typedef struct {
  bool outage;
  uint8_t reason;
  uint64_t lostAtMs;
  uint32_t failedSamples;
  uint64_t lastOutageMs;          // preenchidos na recuperacao
  uint32_t lastFailedSamples;
} GnssState;

// Comeca sem falha registrada.
static inline void gnssInit(GnssState *gnss) {
  gnss->outage = false;
  gnss->reason = GNSS_REASON_NO_FIX;
  gnss->lostAtMs = 0;
  gnss->failedSamples = 0;
  gnss->lastOutageMs = 0;
  gnss->lastFailedSamples = 0;
}

// Separa a causa da falha: sem dados, sinal fraco ou sem fix.
static inline uint8_t gnssClassify(bool receivingData, bool satellitesKnown,
                                   uint32_t satellites, uint32_t weakMax) {
  if (!receivingData) return GNSS_REASON_NO_DATA;
  if (satellitesKnown && satellites > 0 && satellites <= weakMax) return GNSS_REASON_WEAK_SIGNAL;
  return GNSS_REASON_NO_FIX;
}

static inline const char *gnssReasonName(uint8_t reason) {
  switch (reason) {
    case GNSS_REASON_NO_DATA: return "sem_dados";
    case GNSS_REASON_WEAK_SIGNAL: return "sinal_fraco";
    default: return "sem_fix";
  }
}

// Chamada ao fim de cada coleta, valida ou nao.
static inline int gnssSample(GnssState *gnss, bool valid, uint8_t reason, uint64_t nowMs) {
  if (valid) {
    if (!gnss->outage) return GNSS_EVENT_NONE;
    gnss->outage = false;
    gnss->lastOutageMs = nowMs >= gnss->lostAtMs ? nowMs - gnss->lostAtMs : 0;
    gnss->lastFailedSamples = gnss->failedSamples;
    gnss->failedSamples = 0;
    return GNSS_EVENT_RECOVERED;
  }
  ++gnss->failedSamples;
  if (gnss->outage) return GNSS_EVENT_NONE;
  gnss->outage = true;
  gnss->reason = reason;
  gnss->lostAtMs = nowMs;
  return GNSS_EVENT_LOST;
}
