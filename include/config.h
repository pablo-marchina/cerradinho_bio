#pragma once
#include <stdint.h>

#ifndef DEMO_MODE
#define DEMO_MODE 1
#endif

#if __has_include("config.local.h")
#include "config.local.h"
#endif

#ifndef TRACKER_WIFI_SSID
#define TRACKER_WIFI_SSID "Wokwi-GUEST"
#endif
#ifndef TRACKER_WIFI_PASSWORD
#define TRACKER_WIFI_PASSWORD ""
#endif

namespace Config {
constexpr int GpsRx = 16;
constexpr int GpsTx = 17;
constexpr int SealPin = 27;
constexpr int BatteryPin = 34;  // ADC1, usado com Wi-Fi.
constexpr int GreenLedPin = 25;
constexpr int YellowLedPin = 26;
constexpr int RedLedPin = 23;  // GPIO27 permanece reservado ao lacre.
constexpr int BlueLedPin = 14;
constexpr int AuthorizeButtonPin = 33;
constexpr int CycleButtonPin = 13;
constexpr int NetworkSwitchPin = 18;
constexpr int PowerSwitchPin = 19;  // ON/OFF: esquerda = ligado, direita = desligado.
constexpr uint32_t GpsBaud = 9600;
constexpr uint32_t ConsoleBaud = 115200;
constexpr uint32_t WeekMs = 7UL * 24UL * 60UL * 60UL * 1000UL;
constexpr uint32_t SampleIntervalMs = DEMO_MODE ? 30000UL : WeekMs;
constexpr uint32_t FixMaxAgeMs = 3000;
constexpr uint32_t GpsWaitMs = 10000;
constexpr uint32_t HeartbeatMs = 2000;
constexpr uint32_t SealDebounceMs = 60;
constexpr uint32_t WifiRetryMs = 30000;
constexpr double MovementThresholdM = 100.0;  // Hipotese inicial, validar em campo.
constexpr uint8_t QueueCapacity = 16;
// Ciclo de manutencao (issue #13). No demo, 1 s real = 1 dia; valores de teste.
constexpr uint32_t DayMs = DEMO_MODE ? 1000UL : 24UL * 60UL * 60UL * 1000UL;
constexpr uint32_t StatusIntervalDays = 7;
constexpr uint32_t ExpectedReturnDays = 5;     // PENDENTE DE VALIDACAO
constexpr uint32_t StayThresholdDays = 150;    // CT04 roda com 10 e com 150
constexpr uint32_t BlinkHalfPeriodMs = 500;    // 1 Hz
// Lacre, autorizacao, fila, botoes e bateria (issue #14). Valores de teste.
constexpr uint32_t AuthorizationWindowDays = 3;  // PENDENTE DE VALIDACAO
constexpr uint32_t ButtonDebounceMs = 50;
constexpr uint32_t LongPressMs = 2000;
constexpr uint8_t BatteryLowPct = 20;            // PENDENTE DE VALIDACAO
constexpr uint8_t BatteryRecoverPct = 25;        // PENDENTE DE VALIDACAO
constexpr uint32_t BatteryCheckMs = 200;
constexpr const char *DeviceId = "prototipo-001";
}
