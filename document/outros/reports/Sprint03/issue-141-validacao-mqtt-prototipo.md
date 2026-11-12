# Issue #141: validação do MQTT no protótipo físico (CT70, CT71 e CT73)

> **Pergunta investigada:** o MQTT do walking skeleton (#122) e a publicação de eventos (#123, MR !77) funcionam no protótipo físico completo, contra o EMQX Cloud, como descrevem os CT70, CT71 e CT73?
> **O que foi feito:** firmware gravado na montagem alternada da bancada (`bancada-alternada`), conexão TLS ao EMQX Cloud, conferência na Serial e no MQTT Explorer, violação do lacre com e sem ciclo ativo, publicação em `cmd` e queda de energia pelo USB.
> **Evidência:** Serial e prints em `assets/3-desenvolvimento-resultados/3.4.3-prototipo-mqtt-i2c/`; `VIOLACAO_LACRE` com o mesmo `uptime_ms` (146531) na Serial e no Explorer; Last Will em cerca de 40 s.
> **Em aberto:** print do console do EMQX, print do `status` violado, print do Last Will e `cmd` com texto; brownout em loop no boot do rádio; `cmd/resposta` ainda não existe.
> **Alimenta:** **3.4.3 Protótipo do Projeto com MQTT e I2C** (Artefato 8), casos de teste CT70, CT71 e CT73.
> **Implicação:** o caminho dispositivo, broker e cliente funciona no protótipo completo, com o alerta de violação chegando ao broker no mesmo segundo da abertura do lacre, então o dashboard já pode consumir `evento` e `status` reais; mas a alimentação pelo USB do notebook não sustenta o pico do rádio com o GNSS ligado, e isso precisa ser resolvido antes de gravar os CTs oficiais.

Card: #141. Autor: Luiz Cazelatto. Data do ensaio: 12/11/2026. Revisão: Arthur Loyola.

## Ambiente

| Item | Valor |
|---|---|
| Placa | ESP32 DevKitC na montagem alternada (pinos na tabela do relatório de montagem da 3.4.2) |
| Firmware do CT70, `cmd` e CT73 | Ambiente `bancada-alternada` da branch do #132 (commit `7a2dc31`, que inclui o #122) |
| Firmware do CT71 | Mesmo ambiente, com o código do MR !77 (commit `6f275b4`) juntado localmente ao #132, porque a branch do #123 não tem o `bancada-alternada` |
| Broker | EMQX Cloud Serverless, TLS na porta 8883, usuário MQTT do deployment |
| Dispositivo nos tópicos | `MQTT_CLIENT_ID` `guaratracking-luiz` e `MQTT_DEVICE_ID` `prototipo-001-luiz` (tópicos `guaratracking/prototipo-001-luiz/...`) |
| Rede | Wi-Fi de 2,4 GHz fora da rede do Inteli |
| Cliente | MQTT Explorer assinando `guaratracking/#` |

Host, usuário, senha e SSID ficam só no `include/secrets.h` local; nos logs, o host aparece como `<broker-emqx-cloud>`.

## Resultados

| Teste | Situação | Resultado observado | Evidência |
|---|---|---|---|
| CT70: ao ligar, o broker recebe `online` e `status` | APROVADO | Boot às 18:20:07; `[mqtt] conectado` e `status publicado (138 bytes)` 6 s depois; no Explorer, `online = online` e `status` retidos com `device_id` `prototipo-001-luiz`; "MQTT ok / TLS" no LCD | [Serial](../../../../assets/3-desenvolvimento-resultados/3.4.3-prototipo-mqtt-i2c/ct70-conexao-mqtt.serial.txt) · [Explorer](../../../../assets/3-desenvolvimento-resultados/3.4.3-prototipo-mqtt-i2c/ct70ExplorerOnlineStatus.png) · [LCD](../../../../assets/3-desenvolvimento-resultados/3.4.3-prototipo-mqtt-i2c/ct70LcdMqttOk.jpg) · [Protótipo](../../../../assets/3-desenvolvimento-resultados/3.4.3-prototipo-mqtt-i2c/prototipoMontagemAlternada.jpg) |
| CT71: violação com ciclo ativo | APROVADO | 19:03:25: `VIOLACAO_LACRE` com `ciclo_ativo=true` e `critico=true`, sem `[mqtt] evento nao publicado`, e `status` republicado no mesmo segundo; 10 heartbeats com `lacre=violado`; 19:03:44: `LACRE_FECHADO`, com o ciclo seguindo ativo | [Serial](../../../../assets/3-desenvolvimento-resultados/3.4.3-prototipo-mqtt-i2c/ct71-violacao-ciclo-ativo.serial.txt) |
| CT71: violação sem ciclo, conferida no Explorer | APROVADO | Ciclo encerrado (`ENCERRAMENTO_RECUSADO` com o lacre aberto e `CICLO_ENCERRADO` depois de fechar); 19:27:40: `VIOLACAO_LACRE` com `uptime_ms` 146531 na Serial e o mesmo `uptime_ms` no tópico `evento` do Explorer; 19:28:17: `LACRE_FECHADO`. No vídeo, o LCD mostra "VIOLACAO registrada", volta à tela de prazo, apaga pelo tempo limite e, ao fechar, mostra "Lacre fechado / apos violacao"; o buzzer está no áudio³. O `status` retido mostrou `"lacre":"violado"` com `"ciclo_ativo":true` numa violação anterior | [Serial](../../../../assets/3-desenvolvimento-resultados/3.4.3-prototipo-mqtt-i2c/ct71-encerramento-e-violacao-sem-ciclo.serial.txt) · [Explorer](../../../../assets/3-desenvolvimento-resultados/3.4.3-prototipo-mqtt-i2c/ct71ExplorerViolacao.png) · [Vídeo](../../../../assets/3-desenvolvimento-resultados/3.4.3-prototipo-mqtt-i2c/ct71ViolacaoSemCiclo.mp4) · [LCD violação](../../../../assets/3-desenvolvimento-resultados/3.4.3-prototipo-mqtt-i2c/ct71LcdViolacao.png) · [LCD fechado](../../../../assets/3-desenvolvimento-resultados/3.4.3-prototipo-mqtt-i2c/ct71LcdLacreFechado.png) |
| Recebimento de `cmd` | APROVADO, com ressalva¹ | `[mqtt] cmd recebido em guaratracking/prototipo-001-luiz/cmd` às 18:17:12 e 18:17:18 | [Serial](../../../../assets/3-desenvolvimento-resultados/3.4.3-prototipo-mqtt-i2c/cmd-recebido.serial.txt) |
| CT73: queda de energia e Last Will | APROVADO, com ressalva² | USB tirado; `online` virou `offline` cerca de 40 s depois (esperado: até 45 s, 1,5 vez o keep-alive de 30 s); ao religar, voltou a `online` | Observação no Explorer |

1. As duas mensagens publicadas no teste chegaram vazias (0 bytes), porque o Publish do Explorer foi enviado sem texto. Falta repetir com texto.
2. O tempo foi observado no Explorer, sem print nem cronômetro registrado.
3. O som do buzzer ainda não foi conferido: a análise dos quadros do vídeo confere só o LCD, e falta ouvir o áudio. O vídeo tem 47 s, e os quadros do LCD foram extraídos aos 8 s e aos 46 s.

## Anomalias

| Anomalia | O que se viu | Hipótese e próximo passo |
|---|---|---|
| Brownout em loop | `Brownout detector was triggered` logo depois de `[memoria] NVS=ok`, a cada 1 a 2 s, três vezes no dia (18:12, 18:55 e 19:24); a placa não sai do loop sozinha | O pico de corrente da partida do rádio, com LCD e GNSS no mesmo 5 V do USB do notebook, derruba a tensão. Baixar a potência de transmissão para 8,5 dBm não resolveu (33 brownouts em 60 s), porque o pico é na partida do rádio. Recolocar o 5 V do GNSS tirou a placa do loop nas três vezes. Próximo passo: fonte de 5 V e 2 A ou capacitor de 470 a 1000 µF entre 5 V e GND perto do ESP32, e trocar os jumpers do GNSS. [Serial](../../../../assets/3-desenvolvimento-resultados/3.4.3-prototipo-mqtt-i2c/brownout-loop.serial.txt) |
| `STATUS_SEMANAL` a cada 7 s | No demo (1 s = 1 dia), o ciclo ativo gera um `STATUS_SEMANAL` a cada 7 s, que cobre o último valor de `evento` no Explorer e republica o `status` | Gasta a cota do EMQX e dificulta capturar o `VIOLACAO_LACRE`; por isso a captura no Explorer foi feita sem ciclo |
| Mensagem de boot desatualizada | Com o MQTT ligado, o boot ainda imprime `[transporte] JSON na serial + fila local; envio HTTP/MQTT pendente` | Atualizar a mensagem quando `MQTT_ENABLED` |
| NF de retorno sem ciclo | `NF_RETORNO_LANCADA` aceita às 19:26:58, com `ciclo_ativo=false` | Avaliar se deve ser recusada, como o `REMESSA` sem ciclo |
| Assinatura de `cmd` sem conferência | O firmware não confere se o broker aceitou a assinatura | Se a ACL do usuário recusar, a falha fica invisível |
| Credencial errada | Com o e-mail da conta no lugar do usuário MQTT, `estado=5` (não autorizado) a cada 10 s | O usuário é o criado em Access Control do deployment; vale citar no guia do grupo |

## Evidências que faltam para a 3.4.3

- **CT70:** print do console do EMQX com o cliente `guaratracking-luiz` conectado.
- **CT71:** print do `status` com `"lacre":"violado"` e conferência do buzzer no áudio do vídeo.
- **CT73:** print do Explorer com `offline` e depois `online`, com a hora da queda.
- **`cmd`:** repetir com o texto `teste`.

## Limites

- Ensaio de bancada: os CTs oficiais da 3.4.3 precisam ser regravados com as evidências completas e com a alimentação estável.
- O CT71 rodou com o código do MR !77 juntado localmente ao #132; depois do merge dos dois na `develop-Art8`, vale repetir o CT71 com o firmware da branch.
- Os trechos da Serial do CT71 sem ciclo foram filtrados pelo script de captura (só eventos) e cortados em 260 caracteres por linha.
