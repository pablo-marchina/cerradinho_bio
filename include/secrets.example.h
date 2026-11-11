#pragma once
// Copie para include/secrets.h e preencha. O secrets.h e ignorado pelo Git:
// nunca coloque credenciais reais neste arquivo de exemplo.
// Com secrets.h presente (e a biblioteca PubSubClient instalada), o firmware liga o MQTT.

// Wi-Fi 2,4 GHz da bancada (o ESP32 nao usa 5 GHz).
#define TRACKER_WIFI_SSID "SEU_WIFI"
#define TRACKER_WIFI_PASSWORD "SUA_SENHA_WIFI"

// --- Modo 1 (padrao): broker local, HiveMQ no Docker do notebook ---
// O ESP32 e o notebook precisam estar na mesma rede. O IP muda se a rede mudar:
// no Windows, confira com `ipconfig` (adaptador Wi-Fi, IPv4).
#define MQTT_HOST "192.168.0.10"
#define MQTT_PORT 1883
#define MQTT_USER ""      // vazio = sem autenticacao (broker local)
#define MQTT_PASSWORD ""

// --- Modo 2 (futuro): HiveMQ Cloud, com TLS e login ---
// Troque o bloco acima por este:
// #define MQTT_TLS 1
// #define MQTT_HOST "SEU_CLUSTER.s1.eu.hivemq.cloud"
// #define MQTT_PORT 8883
// #define MQTT_USER "SEU_USUARIO_MQTT"
// #define MQTT_PASSWORD "SUA_SENHA_MQTT"
// So para depurar o TLS: aceita qualquer certificado do servidor.
// #define MQTT_TLS_INSECURE 1
