#pragma once

#include <initializer_list>
#include <Arduino.h>
#include <stdint.h>
#include "config.h"

// Classe propria do prototipo: encapsula os quatro indicadores de estado.
// Os sinais de entrada sao calculados pelas regras de negocio, separando
// diagnostico visual e regras do ciclo. Nao faz esperas bloqueantes.
class IndicatorController final {
 public:
  void begin() const {
    for (int pin : {Config::GreenLedPin, Config::YellowLedPin,
                    Config::RedLedPin, Config::BlueLedPin}) {
      pinMode(pin, OUTPUT);
      digitalWrite(pin, LOW);
    }
  }

  void display(bool powered, bool sealViolated, bool removalInProgress,
               bool cycleWarning, bool simulatedNetworkAvailable,
               uint32_t now) const {
    const bool blinkOn = (now / Config::BlinkHalfPeriodMs) % 2 == 0;
    const bool green = !sealViolated && (!removalInProgress || blinkOn);
    digitalWrite(Config::GreenLedPin, powered && green);
    digitalWrite(Config::YellowLedPin, powered && cycleWarning);
    digitalWrite(Config::RedLedPin, powered && sealViolated && blinkOn);
    digitalWrite(Config::BlueLedPin, powered && !simulatedNetworkAvailable && blinkOn);
  }
};
