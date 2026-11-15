# CT70: Conexão e `status` retido

**Card:** #123 · **Seção:** 3.4.3 · **Antes:** [`00-preparacao.md`](00-preparacao.md) · **Índice:** [README](README.md)

**Ordem de execução:** 1 de 9 · anterior: [`00-preparacao.md`](00-preparacao.md) · **próximo: [CT74](ct74-comandos-de-tela.md)**

## Objetivo

Ao ligar, o broker recebe `online` e o `status` retidos, e o LCD mostra "MQTT ok".

## Pré-condições

- `00-preparacao.md` feito; o EMQX Cloud no ar; Explorer assinando `guaratracking/#`.
- Protótipo **sem energia** (USB desconectado).

## Passo a passo

1. Com o monitor serial aberto, conecte o USB.
2. Observe a Serial: `[wifi] conectado` e `[mqtt] conectado`, depois `[mqtt] status publicado`. Anote a hora do boot.
3. Olhe o LCD: "MQTT ok / TLS" por cerca de 2 s. **Tire a foto nesse intervalo** (ou grave um vídeo curto e extraia o quadro).
4. No Explorer, abra `guaratracking/<ID>/online` e `guaratracking/<ID>/status`. Confirme que aparecem com o ícone de retido.
5. No console do EMQX, abra a lista de clientes e confirme que o seu `MQTT_CLIENT_ID` está conectado. Tire o print com host e usuário cobertos.

## Resultado esperado

- `online = online` e `status` com JSON (`device_id` correto).
- LCD com "MQTT ok / TLS".
- Cliente conectado no console do EMQX.

## Evidências (salvar em `assets/3-desenvolvimento-resultados/3.4.3-prototipo-mqtt-i2c/`)

| Marque | Arquivo | O que é |
|---|---|---|
| [ ] | `ct70LcdMqttOk.jpg` | foto do LCD |
| [x] | `ct70ExplorerOnlineStatus3Botoes.png` | Explorer com `online` e `status` (host mascarado) |
| [ ] | `ct70ConsoleEmqx.png` | console do EMQX com o cliente |
| [ ] | `ct70-conexao-mqtt.serial.txt` | trecho da Serial do boot |
| [x] | `ct70Serial3Botoes.png` | Serial no boot: conexão, reenvio da fila e `0 pendentes` (host mascarado) |

## Observações

- Se a placa reiniciar em loop com `Brownout detector was triggered`, troque o cabo, use porta direta e, se precisar, ponha um capacitor de 470 a 1000 µF entre VIN e GND.
- O Luiz já ensaiou este CT no protótipo dele (relatório do #141); falta o print do EMQX e regravar no seu.

## Resultado (preencher na execução)

| Campo | Valor |
|---|---|
| Data e hora | 22:35 - 22:40|
| Firmware (ambiente e commit) | |
| Situação | [ X ] Aprovado  [ ] Reprovado  [ ] Parcial |
| Observado | Após ligar e dar upload ao ESP32, o lcd liga e aparece MQTT ok, conforme o esperado, além disso é percepetível por meio do MQTT explorer os status esperados desse caso de teste. |
| Problemas | |

## Depois de executar

- Troque a linha do CT70 na seção Resultados da 3.4.3 (`documentacao.md`) para "Aprovado" e tire o aviso "IMAGEM A INCLUIR" das figuras que você salvou.
- Mascare host, usuário e SSID nos prints antes de salvar.
