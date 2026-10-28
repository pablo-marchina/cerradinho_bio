#pragma once
#include <cstdint>
class LiquidCrystal_I2C {
 public:
  LiquidCrystal_I2C(uint8_t, uint8_t, uint8_t) {}
  void init() {}
  void backlight() {}
  void noBacklight() {}
  void clear() {}
  void setCursor(uint8_t, uint8_t) {}
  template<typename ...Args> void printf(const char*, Args...) {}
};
