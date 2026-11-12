# Broker MQTT local (EMQX no Docker)

Broker do protótipo **só para desenvolvimento e bancada**. Roda no notebook de quem o sobe, só alcança dispositivos na mesma rede do ESP32, não tem TLS nem usuário e some quando o notebook desliga. É o mesmo software do **EMQX Cloud Serverless**, que o grupo escolheu como broker online; assim o teste local vale para o online. As credenciais do EMQX Cloud ficam só no `include/secrets.h` de cada pessoa, nunca no repositório.

A imagem é `emqx/emqx` com versão fixa (`5.8.9`) e não precisa de licença.

## Como subir

1. Com o Docker em execução, em `docker/emqx`: `docker compose up -d`.
2. Abra o painel em http://localhost:18083 (primeiro acesso: usuário `admin`, senha `public`; o painel pede para trocá-la). Só o painel usa esse acesso; o broker MQTT local não pede usuário.
3. Se outro contêiner já usa a 1883, pare-o antes (`docker ps` e `docker stop <nome>`).
4. Para parar: `docker compose down`.

## Como apontar o firmware

1. Descubra o IP do notebook com `ipconfig` (adaptador Wi-Fi, IPv4). Ele muda se a rede mudar. Notebook e ESP32 precisam estar na mesma rede, em 2,4 GHz.
2. Copie `include/secrets.example.h` para `include/secrets.h` e use o **modo 1 (broker local)**: `MQTT_HOST` com esse IP, `MQTT_PORT 1883`, `MQTT_USER ""` e `MQTT_PASSWORD ""`, sem `MQTT_TLS`.
3. Grave com `pio run -e bancada -t upload` e abra `pio device monitor -b 115200`. Esperado: `[mqtt] conectado a ...` e "MQTT ok" no LCD.

## Como conferir

- No MQTT Explorer (`IP-do-notebook:1883`, sem TLS e sem usuário), assine `guaratracking/#`. Aparecem `online` e `status` (retidos). Publique um texto em `guaratracking/prototipo-001/cmd` e veja `[mqtt] cmd recebido` no Serial.
- No painel (http://localhost:18083) há a lista de clientes e de assinaturas.
- WebSocket para o dashboard (mqtt.js): `ws://IP-do-notebook:8083/mqtt`.

## Vários ESP32 no mesmo broker

Cada ESP32 precisa de um nome de conexão único, senão um derruba o outro: defina `MQTT_CLIENT_ID` (e, se quiser separar os tópicos, `MQTT_DEVICE_ID`) no `include/secrets.h`, como mostra `include/secrets.example.h`. Quem muda o `MQTT_DEVICE_ID` publica em `guaratracking/<nome>/...` e continua visível para quem assina `guaratracking/#`.

## Portas

| Porta | Uso |
|---|---|
| 1883 | MQTT do ESP32 |
| 8083 | WebSocket |
| 18083 | Painel de administração |

Outros dispositivos só conectam se o Firewall do Windows liberar a 1883 (e a 8083 para o WebSocket).

## Diferenças para o EMQX Cloud

O Cloud exige TLS (porta 8883), usuário e senha, e nega assinar `#` (assine `guaratracking/#`). Para esse modo, ver `include/secrets.example.h` (modo 2, com a CA em `include/mqtt_ca_emqx.h`).
