#pragma once
#include <stdint.h>

#ifndef DEMO_MODE
#define DEMO_MODE 1
#endif

// 1 no ambiente bancada: lacre, rede e ON/OFF sao botoes de toque, nao chaves deslizantes.
#ifndef SWITCH_BUTTONS
#define SWITCH_BUTTONS 0
#endif

// secrets.h (ignorado pelo Git; modelo em secrets.example.h) traz Wi-Fi e broker MQTT.
// MQTT so liga com secrets.h e PubSubClient presentes; os testes em computador e o
// Wokwi sem secrets.h continuam sem rede real.
#if __has_include("secrets.h")
#include "secrets.h"
#endif
#if __has_include("secrets.h") && __has_include(<PubSubClient.h>)
#define MQTT_ENABLED 1
#else
#define MQTT_ENABLED 0
#endif

#if __has_include("config.local.h")
#include "config.local.h"
#endif

// Padrao: broker local (HiveMQ no Docker), porta 1883, sem TLS. MQTT_TLS 1 = HiveMQ Cloud (8883).
#ifndef MQTT_TLS
#define MQTT_TLS 0
#endif

// Pinagem padrao = montagem alternada da bancada (secao 3.4.2). Todos os pinos podem ser trocados
// por -D. O diagrama do Wokwi (demo e weekly) usa esta mesma pinagem.
#ifndef LCD_SDA_PIN
#define LCD_SDA_PIN 21
#endif
#ifndef LCD_SCL_PIN
#define LCD_SCL_PIN 23
#endif
// Nome do dispositivo nos topicos (guaratracking/<nome>/status, evento, online, cmd, cmd/resposta).
// Deve ser um texto literal; cada ESP32 que compartilha o broker pode usar um nome proprio.
#ifndef MQTT_DEVICE_ID
#define MQTT_DEVICE_ID "prototipo-001"
#endif

// Identificador do cliente no broker: deve ser unico por ESP32. Vazio = guaratracking-<DeviceId>.
#ifndef MQTT_CLIENT_ID
#define MQTT_CLIENT_ID ""
#endif
#ifndef NETWORK_SWITCH_PIN
#define NETWORK_SWITCH_PIN 16
#endif
// Demais pinos da montagem alternada. Pinos de boot: GPIO5 (CICLO), GPIO12 (buzzer) e GPIO15
// (AUTORIZAR); nao segure CICLO nem AUTORIZAR ao ligar e nada pode puxar o GPIO12 para 3,3 V.
#ifndef GPS_RX_PIN
#define GPS_RX_PIN 36
#endif
#ifndef PREVIOUS_BUTTON_PIN
#define PREVIOUS_BUTTON_PIN 32
#endif
#ifndef BUZZER_PIN
#define BUZZER_PIN 12
#endif
#ifndef AUTHORIZE_BUTTON_PIN
#define AUTHORIZE_BUTTON_PIN 15
#endif
#ifndef CYCLE_BUTTON_PIN
#define CYCLE_BUTTON_PIN 5
#endif

// Prefixo de todos os topicos do contrato MQTT (secao 3.4.3 do Artefato 8).
#define MQTT_TOPIC_PREFIX "guaratracking/" MQTT_DEVICE_ID "/"

#ifndef TRACKER_WIFI_SSID
#define TRACKER_WIFI_SSID "Wokwi-GUEST"
#endif
#ifndef TRACKER_WIFI_PASSWORD
#define TRACKER_WIFI_PASSWORD ""
#endif

namespace Config {
constexpr int GpsRx = GPS_RX_PIN;
constexpr int GpsTx = 17;
constexpr int SealPin = 27;
constexpr int BatteryPin = 34;  // ADC1, usado com Wi-Fi.
// Prototipo V2: LCD 16x2 I2C (SDA 21, SCL 23; 22 no bancada-antiga).
constexpr int LcdSdaPin = LCD_SDA_PIN;
constexpr int LcdSclPin = LCD_SCL_PIN;
constexpr uint8_t LcdAddress = 0x27;
constexpr uint32_t LcdI2cHz = 50000;
constexpr int NextButtonPin = 25;
constexpr int PreviousButtonPin = PREVIOUS_BUTTON_PIN;
constexpr int BuzzerPin = BUZZER_PIN;  // GPIO27 permanece reservado ao lacre.
constexpr int AuthorizeButtonPin = AUTHORIZE_BUTTON_PIN;
constexpr int CycleButtonPin = CYCLE_BUTTON_PIN;
constexpr int NetworkSwitchPin = NETWORK_SWITCH_PIN;
constexpr int PowerSwitchPin = 19;  // ON/OFF: esquerda = ligado, direita = desligado.
// Botoes e lacre usam INPUT_PULLUP; os GPIO34 a 39 so leem e nao tem pull-up, entao um
// botao ali nunca muda de nivel. Erro de pino falha na compilacao, nao na bancada.
constexpr bool hasPullUp(int pin) { return pin < 34 || pin > 39; }
static_assert(hasPullUp(SealPin) && hasPullUp(NextButtonPin) && hasPullUp(PreviousButtonPin) &&
              hasPullUp(AuthorizeButtonPin) && hasPullUp(CycleButtonPin) &&
              hasPullUp(NetworkSwitchPin) && hasPullUp(PowerSwitchPin),
              "Botao em GPIO34-39, que nao tem pull-up interno");
// Botoes no lugar das chaves: toque alterna lacre e rede; ON/OFF exige LongPressMs.
constexpr bool SwitchButtons = SWITCH_BUTTONS;
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
constexpr uint32_t AuthorizationWindowDays = 3;  // RN06: retirada autorizada por tres dias.
constexpr uint32_t ButtonDebounceMs = 50;
constexpr uint32_t LongPressMs = 3000;           // ciclo e autorizacao (V2)
constexpr uint32_t MessageMs = 2000;             // mensagem do LCD apos um botao
constexpr uint32_t LcdTimeoutMs = 15000;         // LCD apaga sem uso (energia)
constexpr uint32_t BuzzerFrequencyHz = 2700;  // ressonancia do buzzer 12085 medida na bancada
constexpr uint8_t BuzzerChannel = 0;             // canal PWM (LEDC) do buzzer
constexpr uint32_t BuzzerBeepMs = 500;           // bipe intermitente a 1 Hz
constexpr uint32_t BuzzerAlarmMs = 30000;        // PENDENTE DE VALIDACAO
// Sem LED desde a Sprint 3: o LCD e o buzzer ja sinalizam a violacao e o LED gastaria energia (secao 3.4.3).
// Falhas do GNSS: sem NMEA por GpsNoDataMs = modulo sem dados; fix invalido com ate
// GpsWeakSatellites satelites = sinal fraco (valor de teste, PENDENTE DE VALIDACAO).
constexpr uint32_t GpsNoDataMs = 5000;
constexpr uint8_t GpsWeakSatellites = 3;
constexpr uint8_t InvoiceMaxLength = 15;         // NF da remessa no LCD
constexpr uint8_t BatteryLowPct = 20;            // PENDENTE DE VALIDACAO
constexpr uint8_t BatteryRecoverPct = 25;        // PENDENTE DE VALIDACAO
constexpr uint32_t BatteryCheckMs = 200;
constexpr const char *DeviceId = MQTT_DEVICE_ID;
}
