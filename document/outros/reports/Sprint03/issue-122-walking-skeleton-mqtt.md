# Issue #122: walking skeleton MQTT (F0)

> **Pergunta investigada:** o ESP32 consegue ligar ao broker MQTT, publicar `status`, assinar `cmd` e avisar presença (`online` com LWT) seguindo o contrato da seção 3.4.3, de ponta a ponta e com o mínimo de código?
> **O que foi feito:** biblioteca PubSubClient no `platformio.ini`; `setBufferSize(512)`; cliente MQTT no `src/firmware/main.cpp` com `status` retido, `online` retido com LWT `offline`, assinatura de `cmd` e mensagem "MQTT ok" no LCD; credenciais em `include/secrets.h` (ignorado pelo Git, modelo em `include/secrets.example.h`); `docker/hivemq/docker-compose.yml` para subir o broker local; compilação dos ambientes `demo`, `weekly` e `bancada`.
> **Evidência:** `pio run -e demo` com broker local: SUCCESS, flash 62,3% (816305 de 1310720 bytes), RAM 15,7%. `pio run -e bancada` com TLS ligado (modo Cloud): SUCCESS, flash 78,7%. `pio run -e weekly` sem `secrets.h`: SUCCESS, flash 60,5%, sem MQTT. `scripts/check-firmware-syntax.sh`: PASS nos dois modos. Todos **medidos** no computador; nada disso foi executado na placa.
> **Em aberto:** conexão real do ESP32 ao broker não testada (falta gravar na bancada e conferir no MQTT Explorer); TLS com HiveMQ Cloud não testado; tópicos `evento` e `cmd/resposta` e a execução dos comandos recebidos em `cmd` ficam para as fatias seguintes; liberação da porta 1883 no Firewall do Windows para outros dispositivos não foi verificada com um segundo aparelho.
> **Alimenta:** **3.4.3 Contrato MQTT e dashboard** (Artefato 8) e o Art. 10, que recebe broker, tópicos e ligações.
> **Implicação:** a arquitetura passa a ter o caminho firmware → broker → cliente provado em código, então o dashboard e as demais fatias já podem ser desenvolvidos contra tópicos reais. Como o broker local não tem autenticação e só alcança dispositivos na mesma rede, ele serve ao desenvolvimento e à gravação dos CTs com dados fictícios; para a operação na Cerradinho, o broker precisa de TLS e login, e é por isso que o modo HiveMQ Cloud (ou outro serviço) fica registrado como demanda futura.

Card: #122 (Walking skeleton, F0). Autor: Arthur Loyola. Branch: `122-walking-skeleton-mqtt`.

## Decisão de broker

| Opção | Estado | Motivo |
|---|---|---|
| HiveMQ no Docker do notebook, porta 1883, sem TLS e sem login | **Padrão agora** | Já roda no Docker do Arthur, não depende de conta nem de internet, atende ao contrato (aceita WebSocket na 8000 quando a porta é publicada) |
| HiveMQ Cloud (TLS, porta 8883, usuário e senha) | **Demanda futura** | Funciona fora do Inteli e é seguro por padrão, mas exige conta e credenciais. O firmware já tem o modo TLS, ativado por `MQTT_TLS 1` |

**Decisão pendente do grupo:** o broker do Docker só serve ao desenvolvimento e à bancada (depende do notebook de quem o roda, da mesma rede do ESP32 e não tem autenticação). O grupo precisa decidir a **plataforma do broker online** (HiveMQ Cloud ou outro serviço) antes de gravar CTs fora do Inteli e de qualquer uso na Cerradinho.

## Como o firmware escolhe o modo

Tudo vem do `include/secrets.h`, copiado de `include/secrets.example.h`:

- Modo local (padrão): `MQTT_HOST` com o IP do notebook, `MQTT_PORT 1883`, `MQTT_USER ""`. Usuário vazio significa conexão sem autenticação, sem TLS.
- Modo Cloud: `MQTT_TLS 1`, host do cluster, porta 8883, usuário e senha. O firmware valida o certificado pelo bundle do ESP32 (precisa de NTP para a data); `MQTT_TLS_INSECURE 1` serve só para depurar.
- Sem `secrets.h` (ou sem PubSubClient), o MQTT não compila no firmware. O Wokwi e os testes em computador seguem como antes.

## Comportamento (contrato 3.4.3)

Prefixo `guaratracking/prototipo-001/`.

| Tópico | Ação do firmware |
|---|---|
| `online` | Publica `online` retido ao conectar; LWT `offline` retido, QoS 1 |
| `status` | Publica retido ao conectar e a cada 60 s: lacre, bateria, ciclo, NF, dias fora, GNSS e fila |
| `cmd` | Assina; por enquanto escreve o comando recebido no Serial |

O LCD mostra "MQTT ok" por 2 s ao conectar. As tentativas de reconexão ocorrem a cada 10 s, só com Wi-Fi ligado e a chave de rede simulada disponível.

## Como testar

1. Suba o broker: em `docker/hivemq`, copie `.env.example` para `.env` com a licença (ignorado pelo Git) e rode `docker compose up -d`, ou use o contêiner `hivemq` já em execução. Confira http://localhost:8080. Passos completos em `docker/hivemq/README.md`.
2. Descubra o IP do notebook (`ipconfig`; no dia da implementação foi 10.150.1.141, mas muda com a rede) e ponha em `include/secrets.h`, com o Wi-Fi de 2,4 GHz.
3. Grave: `pio run -e bancada -t upload`; abra `pio device monitor -b 115200`. Esperado: `[mqtt] conectado a ...` e "MQTT ok" no LCD.
4. No MQTT Explorer, conecte em `IP:1883` (sem TLS, sem usuário) e assine `guaratracking/prototipo-001/#`. Esperado: `online = online` e `status` retidos.
5. Publique algo em `guaratracking/prototipo-001/cmd` e confira a linha `[mqtt] cmd recebido` no monitor serial.
6. Desligue o ESP32 sem avisar: após o keep-alive (30 s) o broker deve publicar `offline` em `online`.

## Limites e pendências

- Nada foi executado na placa ou contra o broker; as verificações acima estão por fazer e, quando feitas, devem virar CT gravado.
- O broker local não tem autenticação: só dados fictícios, em rede confiável.
- Outros dispositivos só alcançam a 1883 se o Firewall do Windows permitir; não foi criada nenhuma regra.
- Plataforma do broker online a ser decidida pelo grupo (ver "Decisão de broker").
- O IP do notebook muda com a rede; o `secrets.h` precisa ser atualizado.
- Próximas fatias: tópico `evento`, `cmd/resposta`, execução dos comandos e o dashboard via WebSocket (porta 8000).
