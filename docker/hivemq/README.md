# Broker MQTT local (HiveMQ no Docker)

Broker do protótipo durante o desenvolvimento. HiveMQ Cloud ou outro serviço é demanda futura (o firmware já tem o modo TLS, ver `include/secrets.example.h`).

```bash
cp .env.example .env   # preencha HIVEMQ_LICENSE_KEY (o .env é ignorado pelo Git)
docker compose up -d
```

- MQTT: `IP-do-notebook:1883` (sem TLS, sem usuário). Confira o IP com `ipconfig`; ele muda se a rede mudar.
- Control Center: http://localhost:8080
- WebSocket para o dashboard (mqtt.js): `ws://IP-do-notebook:8000/mqtt`
- Se um contêiner `hivemq` criado com `docker run` já ocupa as portas, pare-o antes (`docker stop hivemq`).
- Outros dispositivos na rede só conectam se o Firewall do Windows liberar a porta 1883 (e a 8000 para o WebSocket).
- Teste: MQTT Explorer em `IP-do-notebook:1883`, assinando `guaratracking/prototipo-001/#`.
