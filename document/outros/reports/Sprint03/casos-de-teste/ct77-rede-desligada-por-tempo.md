# CT77: Rede simulada desligada por tempo

**Card:** #128 · **Seção:** 3.4.3 · **Antes:** [`00-preparacao.md`](00-preparacao.md) · **Índice:** [README](README.md)

**Ordem de execução:** 8 de 10 · anterior: [CT83](ct83-nf-por-comando.md) · **próximo: [CT72](ct72-fila-sem-rede.md)**

## Objetivo

O comando `rede` tira a rede simulada por N segundos; os eventos ficam na fila e saem sozinhos ao fim do prazo.

## Pré-condições

- CT74 aprovado; lacre fechado; Wi-Fi e broker no ar.
- **Estado do ciclo conhecido** (confira `"ciclo_ativo"` no `status`). O ciclo não é necessário. Os eventos que enchem a fila saem do comando `LIMIAR n` da Serial, que publica `LIMIAR_ALTERADO` a cada uso, com ou sem ciclo (o `NF` não serve mais: ele só vale com ciclo ativo e uma vez por ciclo, RN11). Se quiser o cenário com ciclo, inicie-o antes com `{"id":"62","acao":"ciclo_iniciar"}` (`ok:true` e `CICLO_INICIADO`); o demo entra em atraso em poucos segundos (1 s = 1 dia) e o atraso e o `STATUS_SEMANAL` também entram na fila. Um ciclo esquecido continua depois de reiniciar a placa; para encerrá-lo: lacre fechado, `NF` na Serial e `{"id":"61","acao":"ciclo_encerrar"}`. No fim, volte o limiar ao padrão com `LIMIAR 150`.

## Passo a passo

1. Publique `{"id":"13","acao":"rede","valor":"desligada","segundos":60}` e confirme `ok:true`. Anote a hora.
2. Na Serial do monitor, digite `LIMIAR 100` e `LIMIAR 120`, cada um com Enter (publicam `LIMIAR_ALTERADO`; sem rede os eventos vão para a fila).
3. Veja a fila subir: linha `fila=` na Serial e, no LCD, a mensagem "Sem sinal / na fila: N" (2 s) e a **tela de conexão** (VOLTAR a partir da tela de NF) com `MQTT: sem sinal` e `Fila: N eventos`. Tire a foto.
4. Espere os 60 s. A rede volta sozinha: na Serial, `[mqtt]` publicando a fila.
5. No Explorer, clique no tópico `evento` (dentro de `guaratracking/prototipo-001-arthur`). O tipo do evento vem dentro do JSON (`"type":"LIMIAR_ALTERADO"`), não é um tópico à parte. No histórico do tópico, confirme as mensagens `LIMIAR_ALTERADO` chegando **na ordem** (o `uptime_ms` de cada JSON cresce) e, em `status`, o campo `fila` voltando a `0`. Se houver muita mensagem misturada, use a busca do topo com `LIMIAR`.

## Resultado esperado

- `ok:true`; fila sobe durante o prazo; ao fim, eventos em ordem e fila zerada.

## Evidências (salvar em `assets/3-desenvolvimento-resultados/3.4.3-prototipo-mqtt-i2c/`)

| Marque | Arquivo | O que é |
|---|---|---|
| [x] | `ct77RedeDesligada3Botoes.mp4` | vídeo da rede desligada por tempo, com a fila e o reenvio |
| [ ] | `ct77RedeDesligada.png` | comando, resposta e eventos no Explorer |
| [ ] | `ct77FilaLcd.jpg` | foto do LCD com a fila |
| [ ] | `ct77-rede-desligada.serial.txt` | Serial do reenvio |

## Observações

- O `NF` da Serial só vale com ciclo ativo e uma vez por ciclo (RN11); a anomalia do #141 (NF aceita sem ciclo) foi corrigida. Para encher a fila sem depender de ciclo, use o `LIMIAR n`, que publica `LIMIAR_ALTERADO` a cada uso.

## Resultado (preencher na execução)

| Campo | Valor |
|---|---|
| Data e hora | 18:20 - 18:25 |
| Firmware (ambiente e commit) | `bancada-3botoes` · `bc4cc00` |
| Situação | [ X ] Aprovado  [ ] Reprovado  [ ] Parcial |
| Observado | Tudo certo, conforme o planejado |
| Problemas | Só o vídeo (`ct77RedeDesligada3Botoes.mp4`) foi salvo no repositório; faltam `ct77RedeDesligada.png`, `ct77FilaLcd.jpg` e `ct77-rede-desligada.serial.txt`. |

## Depois de executar

- Troque a linha do CT77 na seção Resultados da 3.4.3 (`documentacao.md`) para "Aprovado" e tire o aviso "IMAGEM A INCLUIR" das figuras que você salvou.
- Mascare host, usuário e SSID nos prints antes de salvar.
