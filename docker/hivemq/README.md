# Broker MQTT local (HiveMQ no Docker)

Broker do protótipo **só para desenvolvimento e bancada**: roda no notebook de quem o sobe, só alcança dispositivos na mesma rede do ESP32, não tem autenticação nem TLS e some quando o notebook desliga. A plataforma do broker online (HiveMQ Cloud ou outro serviço) é uma **decisão pendente do grupo**; o firmware já tem o modo TLS (`MQTT_TLS 1`, ver `include/secrets.example.h`).

## Como testar com o Docker

1. Em `docker/hivemq`, copie `.env.example` para `.env` e cole a licença do HiveMQ em `HIVEMQ_LICENSE_KEY`. O `.env` é ignorado pelo Git; nunca publique a licença.
2. Rode `docker compose up -d`. Se já houver um contêiner `hivemq` ocupando as portas, pare-o antes (`docker stop hivemq`).
3. Confira o Control Center em http://localhost:8080.
4. Descubra o IP do notebook com `ipconfig` (adaptador Wi-Fi, IPv4). Ele muda se a rede mudar. Notebook e ESP32 na mesma rede, em 2,4 GHz.
5. Copie `include/secrets.example.h` para `include/secrets.h` e preencha o Wi-Fi e `MQTT_HOST` com esse IP (porta 1883, usuário e senha vazios).
6. Grave com `pio run -e bancada -t upload` e abra `pio device monitor -b 115200`. Esperado: `[mqtt] conectado a ...` e "MQTT ok" no LCD.
7. No MQTT Explorer (`IP:1883`, sem TLS e sem usuário) ou no Control Center, assine `guaratracking/prototipo-001/#`: aparecem `online` e `status` (retidos). Publique um texto em `guaratracking/prototipo-001/cmd` e veja `[mqtt] cmd recebido` no Serial.

## Portas

- 1883: MQTT (ESP32); 8080: Control Center; 8000: WebSocket para o dashboard (`ws://IP:8000/mqtt`).
- Outros dispositivos só conectam se o Firewall do Windows liberar a porta 1883 (e a 8000 para o WebSocket).
