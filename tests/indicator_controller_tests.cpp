#include <cassert>
#include <cstdio>
#include "indicator_controller.h"

static int leds[40] = {};
static int outputCount = 0;
void pinMode(int pin, int mode) { assert(pin >= 0 && pin < 40); assert(mode == OUTPUT); ++outputCount; }
void digitalWrite(int pin, int value) { assert(pin >= 0 && pin < 40); leds[pin] = value; }

int main() {
  IndicatorController controller;
  controller.begin();
  assert(outputCount == 4);
  controller.display(true, false, false, false, true, 0);
  assert(leds[Config::GreenLedPin] && !leds[Config::YellowLedPin]
         && !leds[Config::RedLedPin] && !leds[Config::BlueLedPin]);
  controller.display(true, true, false, true, false, 0);
  assert(!leds[Config::GreenLedPin] && leds[Config::YellowLedPin]
         && leds[Config::RedLedPin] && leds[Config::BlueLedPin]);
  controller.display(true, true, false, false, false, Config::BlinkHalfPeriodMs);
  assert(!leds[Config::RedLedPin] && !leds[Config::BlueLedPin]);
  controller.display(true, false, true, false, true, Config::BlinkHalfPeriodMs);
  assert(!leds[Config::GreenLedPin]);
  controller.display(true, false, true, false, true, 2 * Config::BlinkHalfPeriodMs);
  assert(leds[Config::GreenLedPin]);
  controller.display(false, true, false, true, false, 0);
  assert(!leds[Config::GreenLedPin] && !leds[Config::YellowLedPin]
         && !leds[Config::RedLedPin] && !leds[Config::BlueLedPin]);
  std::puts("PASS: classe Indicadores, inicializacao, alerta, rede, retirada e desligamento");
}
