#pragma once
// Copie para include/secrets.h e preencha. O secrets.h e ignorado pelo Git:
// nunca coloque credenciais reais neste arquivo de exemplo.
// Com secrets.h presente (e a biblioteca PubSubClient instalada), o firmware liga o MQTT.

// Wi-Fi 2,4 GHz (o ESP32 nao usa 5 GHz nem rede com login por navegador).
#define TRACKER_WIFI_SSID "SEU_WIFI"
#define TRACKER_WIFI_PASSWORD "SUA_SENHA_WIFI"

// --- Modo 1 (padrao): broker local, EMQX no Docker do notebook (docker/emqx) ---
// O ESP32 e o notebook precisam estar na mesma rede. O IP muda se a rede mudar:
// no Windows, confira com `ipconfig` (adaptador Wi-Fi, IPv4).
#define MQTT_HOST "192.168.0.10"
#define MQTT_PORT 1883
#define MQTT_USER ""      // vazio = sem autenticacao (broker local)
#define MQTT_PASSWORD ""

// --- Modo 2: EMQX Cloud Serverless (broker online escolhido), com TLS e login ---
// Troque o bloco do modo 1 por este. O EMQX Cloud so aceita TLS (porta 8883), exige usuario e
// senha (criados no painel do deployment) e nao deixa assinar `#`: use `guaratracking/#`.
// #define MQTT_TLS 1
// #define MQTT_HOST "SEU_DEPLOYMENT.emqxsl.com"
// #define MQTT_PORT 8883
// #define MQTT_USER "SEU_USUARIO_MQTT"
// #define MQTT_PASSWORD "SUA_SENHA_MQTT"
// CA publica que valida o EMQX Cloud (DigiCert Global Root G2), embutida no firmware:
// #include "mqtt_ca_emqx.h"
// #define MQTT_CA_CERT MQTT_CA_EMQX
// So para depurar o TLS: aceita qualquer certificado do servidor.
// #define MQTT_TLS_INSECURE 1
