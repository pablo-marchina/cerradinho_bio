# CT77: Rede simulada desligada por tempo

**Card:** #128 · **Seção:** 3.4.3 · **Antes:** [`00-preparacao.md`](00-preparacao.md) · **Índice:** [README](README.md)

**Ordem de execução:** 7 de 9 · anterior: [CT78](ct78-dispositivo-desligado.md) · **próximo: [CT72](ct72-fila-sem-rede.md)**

## Objetivo

O comando `rede` tira a rede simulada por N segundos; os eventos ficam na fila e saem sozinhos ao fim do prazo.

## Pré-condições

- CT74 aprovado; lacre fechado; Wi-Fi e broker no ar.

## Passo a passo

1. Publique `{"id":"13","acao":"rede","valor":"desligada","segundos":60}` e confirme `ok:true`. Anote a hora.
2. Na Serial do monitor, digite `NF` e Enter duas vezes (gera `NF_RETORNO_LANCADA`; sem rede os eventos vão para a fila).
3. Veja a fila subir: linha `fila=` na Serial e, no LCD, a tela que mostra a fila (use AVANÇAR até achar). Tire a foto.
4. Espere os 60 s. A rede volta sozinha: na Serial, `[mqtt]` publicando a fila.
5. No Explorer, confirme os eventos de `NF_RETORNO_LANCADA` chegando **na ordem** e a fila voltando a 0.

## Resultado esperado

- `ok:true`; fila sobe durante o prazo; ao fim, eventos em ordem e fila zerada.

## Evidências (salvar em `assets/3-desenvolvimento-resultados/3.4.3-prototipo-mqtt-i2c/`)

| Marque | Arquivo | O que é |
|---|---|---|
| [ ] | `ct77RedeDesligada.png` | comando, resposta e eventos no Explorer |
| [ ] | `ct77FilaLcd.jpg` | foto do LCD com a fila |
| [ ] | `ct77-rede-desligada.serial.txt` | Serial do reenvio |

## Observações

- `NF` pela Serial é aceito sem ciclo ativo (anomalia conhecida do #141); aqui ele só serve para gerar eventos.

## Resultado (preencher na execução)

| Campo | Valor |
|---|---|
| Data e hora | |
| Firmware (ambiente e commit) | |
| Situação | [ ] Aprovado  [ ] Reprovado  [ ] Parcial |
| Observado | |
| Problemas | |

## Depois de executar

- Troque a linha do CT77 na seção Resultados da 3.4.3 (`documentacao.md`) para "Aprovado" e tire o aviso "IMAGEM A INCLUIR" das figuras que você salvou.
- Mascare host, usuário e SSID nos prints antes de salvar.
