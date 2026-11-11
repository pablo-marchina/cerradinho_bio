#pragma once
// Copie para include/secrets.h e preencha. O secrets.h e ignorado pelo Git:
// nunca coloque credenciais reais neste arquivo de exemplo.
// Com secrets.h presente (e a biblioteca PubSubClient instalada), o firmware liga o MQTT.

// Wi-Fi 2,4 GHz da bancada (o ESP32 nao usa 5 GHz).
#define TRACKER_WIFI_SSID "SEU_WIFI"
#define TRACKER_WIFI_PASSWORD "SUA_SENHA_WIFI"

// HiveMQ Cloud (cluster serverless): host do painel do cluster, porta TLS 8883 e
// credenciais criadas em Access Management.
#define MQTT_HOST "SEU_CLUSTER.s1.eu.hivemq.cloud"
#define MQTT_PORT 8883
#define MQTT_USER "SEU_USUARIO_MQTT"
#define MQTT_PASSWORD "SUA_SENHA_MQTT"

// So para depurar: 1 aceita qualquer certificado do servidor (sem validacao TLS).
// #define MQTT_TLS_INSECURE 1
