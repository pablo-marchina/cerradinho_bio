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
// Prototipo V2: LCD 16x2 I2C nos pinos padrao do Wire (SDA 21, SCL 22).
constexpr int LcdSdaPin = 21;
constexpr int LcdSclPin = 22;
constexpr uint8_t LcdAddress = 0x27;
constexpr int NextButtonPin = 25;
constexpr int PreviousButtonPin = 26;
constexpr int BuzzerPin = 23;  // GPIO27 permanece reservado ao lacre.
constexpr int AuthorizeButtonPin = 33;
constexpr int CycleButtonPin = 14;
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
// Registro local de diagnostico na NVS (q0..q15): amostras de GPS e seal_changed
// (CT18-CT22). Separado da fila de eventos de negocio (event_queue.h, RN09).
constexpr uint8_t QueueCapacity = 16;
// Ciclo de manutencao (CT02-CT04). No demo, 1 s real = 1 dia; valores de teste.
constexpr uint32_t DayMs = DEMO_MODE ? 1000UL : 24UL * 60UL * 60UL * 1000UL;
constexpr uint32_t StatusIntervalDays = 7;
constexpr uint32_t ExpectedReturnDays = 5;     // PENDENTE DE VALIDACAO
constexpr uint32_t StayThresholdDays = 150;    // CT04 roda com 10 e com 150
// Lacre, autorizacao, fila, botoes e bateria (CT05-CT16). Valores de teste.
constexpr uint32_t AuthorizationWindowDays = 3;  // PENDENTE DE VALIDACAO
constexpr uint32_t ButtonDebounceMs = 50;
constexpr uint32_t LongPressMs = 3000;           // ciclo e autorizacao (V2)
constexpr uint32_t MessageMs = 2000;             // mensagem do LCD apos um botao
constexpr uint32_t LcdTimeoutMs = 15000;         // LCD apaga sem uso (energia)
constexpr uint32_t BuzzerFrequencyHz = 2000;
constexpr uint32_t BuzzerBeepMs = 500;           // bipe intermitente a 1 Hz
constexpr uint32_t BuzzerAlarmMs = 30000;        // PENDENTE DE VALIDACAO
constexpr uint8_t InvoiceMaxLength = 15;         // NF da remessa no LCD
constexpr uint8_t BatteryLowPct = 20;            // PENDENTE DE VALIDACAO
constexpr uint8_t BatteryRecoverPct = 25;        // PENDENTE DE VALIDACAO
constexpr uint32_t BatteryCheckMs = 200;
constexpr const char *DeviceId = "prototipo-001";
}
