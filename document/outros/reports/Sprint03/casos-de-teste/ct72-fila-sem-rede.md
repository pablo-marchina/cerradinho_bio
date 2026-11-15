# CT72: Fila sem rede de verdade (RN09)

**Card:** #124 · **Seção:** 3.4.3 · **Antes:** [`00-preparacao.md`](00-preparacao.md) · **Índice:** [README](README.md)

**Ordem de execução:** 9 de 10 · anterior: [CT77](ct77-rede-desligada-por-tempo.md) · **próximo: [CT73](ct73-queda-de-energia.md)**

## Objetivo

Com o Wi-Fi ou o broker fora, o firmware segue operando e guarda os eventos; ao voltar, envia na ordem e zera a fila.

## Pré-condições

- CT70 aprovado; firmware com a **tela de conexão** (a sexta tela do LCD: do início, aperte VOLTAR uma vez para chegar a ela).
- **Estado do ciclo conhecido** (confira `"ciclo_ativo"` no `status`). O ciclo não é necessário. Os eventos que enchem a fila saem do comando `LIMIAR n` da Serial, que publica `LIMIAR_ALTERADO` a cada uso, com ou sem ciclo (o `NF` não serve mais: ele só vale com ciclo ativo e uma vez por ciclo, RN11). Se quiser o cenário com ciclo, inicie-o antes com `{"id":"62","acao":"ciclo_iniciar"}` (`ok:true` e `CICLO_INICIADO`); o demo entra em atraso em poucos segundos (1 s = 1 dia) e o atraso e o `STATUS_SEMANAL` também entram na fila. Um ciclo esquecido continua depois de reiniciar a placa; para encerrá-lo: lacre fechado, `NF` na Serial e `{"id":"61","acao":"ciclo_encerrar"}`. No fim, volte o limiar ao padrão com `LIMIAR 150`.

## Passo a passo

0. **Comece com a fila zerada.** Com o Wi-Fi e o MQTT conectados, espere a Serial mostrar `fila=0`. A fila fica guardada na memória da placa e sobrevive ao reinício, e ela só esvazia sozinha quando há Wi-Fi e MQTT (`[FILA] rede disponivel; reenviando N evento(s)` e `0 pendentes`). Se ainda houver eventos de um teste anterior: confirme `wifi=on` e `rede_sim=on` (depois do CT77 a rede simulada volta sozinha em 60 s); se continuar, reinicie a placa **com a rede no ar**. Não há comando para limpar a fila na mão.
1. **Derrube a rede de verdade:** desligue o hotspot do celular (ou o roteador). Aguarde até a Serial mostrar a falha do Wi-Fi ou do MQTT.
2. Gere 3 eventos pela Serial: `LIMIAR 100`, `LIMIAR 120` e `LIMIAR 140`, cada um seguido de Enter. Anote a hora de cada um.
3. Veja a fila subir até 3: linha `fila=3` na Serial. No LCD, a mensagem "Sem sinal / na fila: N" aparece por 2 s na queda e, na **tela de conexão** (VOLTAR a partir da tela de NF), ficam `MQTT: sem sinal` e `Fila: 3 eventos`. Tire a foto dessa tela e copie o trecho da Serial.
4. **Print do Explorer antes:** `online = offline` (o Last Will chega cerca de 45 s depois da queda) e o `status` retido, que fica congelado com `fila: 0` porque offline a placa não consegue publicar. A fila só aparece na Serial (`fila=N`) e no LCD.
5. Religue o hotspot. Espere o Wi-Fi e o MQTT reconectarem.
6. Confirme na Serial `[FILA] rede disponivel; reenviando N evento(s) na ordem` e `0 pendentes`. No Explorer, no tópico `evento`, confirme os `LIMIAR_ALTERADO` na ordem (o `uptime_ms` cresce), `online` de volta a `online` e o `status` com `fila: 0` (o reenvio da fila acontece antes de o `status` ser publicado). **Print depois.**

## Resultado esperado

- A tela de conexão do LCD mostra `MQTT: sem sinal` e a fila subindo; ao voltar, mostra `MQTT: conectado` e `Fila: 0`, e os eventos chegam em ordem.
- O firmware não trava sem rede.

## Evidências (salvar em `assets/3-desenvolvimento-resultados/3.4.3-prototipo-mqtt-i2c/`)

| Marque | Arquivo | O que é |
|---|---|---|
| [x] | `ct72FilaSemRede3Botoes.mp4` | vídeo do teste, com a Serial, o LCD e o Explorer |
| [x] | `ct72ExplorerAntes.png` | Explorer antes (coberto pelo vídeo) |
| [x] | `ct72ExplorerDepois.png` | Explorer depois (coberto pelo vídeo) |
| [x] | `ct72FilaLcd.jpg` | foto da tela de conexão do LCD com a fila (coberto pelo vídeo) |
| [x] | `ct72-fila-sem-rede.serial.txt` | Serial com a fila e o reenvio (coberto pelo vídeo) |

## Observações

- **Para repetir o CT72:** volte ao passo 0 (`fila=0`), derrube o Wi-Fi de novo (desligue o hotspot), digite `LIMIAR 100`, `LIMIAR 120` e `LIMIAR 140` na Serial e religue.
- A fila guarda 8 eventos; com mais, o evento não crítico mais antigo é descartado (RN09). Se quiser, teste com 9 depois.
- O card #124 também pede o `status` com a posição GNSS e a idade dela. Isso é outro critério, fora deste CT.

## Resultado (preencher na execução)

| Campo | Valor |
|---|---|
| Data e hora | |
| Firmware (ambiente e commit) | |
| Situação | [ X ] Aprovado  [ ] Reprovado  [ ] Parcial |
| Observado | Executado no protótipo de 3 botões com o firmware da tela de conexão (hotspot desligado, eventos gerados na Serial, hotspot religado); registrado em vídeo. |
| Problemas | |

## Depois de executar

- Troque a linha do CT72 na seção Resultados da 3.4.3 (`documentacao.md`) para "Aprovado" e tire o aviso "IMAGEM A INCLUIR" das figuras que você salvou.
- Mascare host, usuário e SSID nos prints antes de salvar.
