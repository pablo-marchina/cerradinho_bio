# Issue #122: walking skeleton MQTT (F0)

> **Pergunta investigada:** o ESP32 consegue ligar ao broker MQTT, publicar `status`, assinar `cmd` e avisar presença (`online` com Last Will) seguindo o contrato da seção 3.4.3, de ponta a ponta, com um broker online que o grupo possa adotar?
> **O que foi feito:** cliente MQTT no firmware (`status` e `online` retidos com Last Will, assinatura de `cmd`, "MQTT ok" no LCD), `secrets.h` ignorado pelo Git, TLS com CA embutida, `docker/emqx` e teste real contra o EMQX Cloud Serverless (detalhes abaixo).
> **Evidência:** medida na bancada com o ESP32 avulso: conexão TLS ao EMQX Cloud, `status`/`online` retidos, `cmd` recebido, Last Will em cerca de 47 s e "MQTT ok" no LCD; compilação de `demo`, `weekly` e `bancada` com SUCCESS (seção "Resultados medidos").
> **Em aberto:** teste com o protótipo completo, `evento`, `cmd/resposta`, WebSocket com mqtt.js e Firewall (seção "Limites e pendências").
> **Alimenta:** **3.4.3 Contrato MQTT e dashboard** (Artefato 8) e o Art. 10, que recebe broker, tópicos e ligações.
> **Implicação:** o caminho firmware, broker e cliente está provado de ponta a ponta no broker escolhido (EMQX Cloud), então dashboard e demais fatias já podem usar tópicos reais; o Cloud exige TLS, login e assinar `guaratracking/#`, com cota mensal gratuita, e o broker local serve só ao desenvolvimento.

Card: #122 (Walking skeleton, F0). Autor: Arthur Loyola. Branch: `122-walking-skeleton-mqtt`.

## Resultados medidos

medida na bancada com o ESP32 avulso (sem o protótipo do grupo). EMQX Cloud (TLS, porta 8883, usuário e senha): `[wifi] conectado`, `[mqtt] conectado` e `status` publicado; `online` e `status` retidos vistos no MQTT Explorer e em um cliente de teste; `cmd` recebido pelo ESP32 (linha `[mqtt] cmd recebido` no Serial); Last Will: `online` virou `offline` cerca de 47 s depois da queda de energia da placa e voltou a `online` ao reconectar (keep-alive de 30 s); LCD mostrou "MQTT ok / TLS" por 2 s após conectar (confirmado a olho). Teste controlado com keep-alive de 10 s: o broker publicou o Last Will cerca de 20 s depois. EMQX local 5.8.9 no Docker: conexão anônima na 1883 aceita, assinatura de `guaratracking/#` aceita e de `#` recusada, mensagem entregue, Last Will em 7,5 s com keep-alive de 5 s, painel respondeu HTTP 200 e o listener WebSocket (8083) respondeu. Compilação: `pio run -e demo`, `-e weekly` e `-e bancada` com SUCCESS (flash 62,3% em `weekly` sem MQTT e 73,9% com TLS); `scripts/check-firmware-syntax.sh` e `scripts/verify-host.sh` passam.

## Decisão de broker

| Opção | Estado | Motivo |
|---|---|---|
| EMQX Cloud Serverless (TLS 8883, usuário e senha) | **Broker online escolhido pelo grupo** | Funciona fora do Inteli, aceita WebSocket seguro e já foi testado com o ESP32. As credenciais ficam só no `include/secrets.h` de cada pessoa, nunca no repositório |
| EMQX 5.8.9 no Docker (`docker/emqx`), porta 1883, sem TLS e sem login | **Broker local de desenvolvimento** | Mesmo software do Cloud, sem licença e com versão fixa; permite testar sem internet nem conta e reproduzir o teste em outro notebook |
| HiveMQ Enterprise no Docker (versão anterior deste report) | **Retirado** | Dependia de licença pessoal e de `latest`, o que impedia outros integrantes de subir o mesmo broker (revisão do MR) |
| HiveMQ Cloud, Mosquitto e outros | Não adotados | Sem necessidade agora; o firmware fala MQTT padrão e funciona com qualquer broker |

## Como o firmware escolhe o modo

Tudo vem do `include/secrets.h`, copiado de `include/secrets.example.h`:

- Modo 1 (broker local): `MQTT_HOST` com o IP do notebook, `MQTT_PORT 1883`, `MQTT_USER ""`. Usuário vazio significa conexão sem autenticação e sem TLS.
- Modo 2 (EMQX Cloud): `MQTT_TLS 1`, host do deployment, porta 8883, usuário e senha, e a CA pública DigiCert Global Root G2 embutida (`include/mqtt_ca_emqx.h`); `MQTT_TLS_INSECURE 1` serve só para depurar.
- Sem `secrets.h` (ou sem PubSubClient), o MQTT não compila no firmware. O Wokwi e os testes em computador seguem como antes.
- Os pinos do LCD e da chave de rede podem ser trocados por `-D` (`LCD_SDA_PIN`, `LCD_SCL_PIN`, `NETWORK_SWITCH_PIN`); o padrão é SDA 21, SCL 22 e chave de rede no 18.

## Comportamento (contrato 3.4.3)

Prefixo `guaratracking/prototipo-001/`.

| Tópico | Ação do firmware |
|---|---|
| `online` | Publica `online` retido ao conectar; Last Will `offline` retido, QoS 1 |
| `status` | Publica retido ao conectar, a cada 60 s e quando um evento muda o estado (mínimo de 1 s entre duas publicações): lacre, bateria, ciclo, NF, dias fora, GNSS e fila |
| `cmd` | Assina; por enquanto escreve o comando recebido no Serial |

O LCD mostra "MQTT ok" por 2 s ao conectar, depois volta à tela anterior e apaga após 15 s sem uso (comportamento já existente). As tentativas de reconexão ocorrem a cada 10 s, só com Wi-Fi ligado e a chave de rede simulada disponível.

## Como testar

1. **Broker local:** em `docker/emqx`, `docker compose up -d`. Painel em http://localhost:18083. Passos completos em `docker/emqx/README.md`. **Broker online:** crie um deployment Serverless no EMQX Cloud, com usuário e senha, e use o modo 2.
2. Descubra o IP do notebook (`ipconfig`; muda com a rede) para o modo 1, ou preencha host, usuário e senha para o modo 2, em `include/secrets.h`, com Wi-Fi de 2,4 GHz.
3. Grave com `pio run -e bancada -t upload` e abra `pio device monitor -b 115200`. Esperado: `[mqtt] conectado a ...` e "MQTT ok" no LCD.
4. No MQTT Explorer, conecte no broker (no Cloud, com TLS ligado) e assine `guaratracking/#`; em Advanced, retire `#` e `$SYS/#` da lista de assinaturas. Esperado: `online = online` e `status` retidos.
5. Publique um texto em `guaratracking/prototipo-001/cmd` e confira `[mqtt] cmd recebido` no monitor serial.
6. Tire a energia do ESP32: após cerca de 45 s (1,5 vez o keep-alive de 30 s) o broker publica `offline` em `online`; ao religar, volta a `online`.

## Limites e pendências

- Ainda não houve teste com o protótipo completo do grupo; o teste foi feito com o ESP32 avulso e o LCD.
- O broker local não tem autenticação nem TLS: só dados fictícios, em rede confiável. O EMQX Cloud tem cota mensal gratuita.
- Outros dispositivos só alcançam a 1883 do notebook se o Firewall do Windows permitir; não foi criada nenhuma regra.
- O IP do notebook muda com a rede; o `secrets.h` precisa ser atualizado.
- Próximas fatias: tópico `evento`, `cmd/resposta` (#128), execução dos comandos e o dashboard via WebSocket (8083 no local, 8084 no Cloud).
