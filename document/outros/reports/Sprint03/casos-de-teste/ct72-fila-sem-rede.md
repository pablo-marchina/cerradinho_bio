# CT72: Fila sem rede de verdade (RN09)

**Card:** #124 · **Seção:** 3.4.3 · **Antes:** [`00-preparacao.md`](00-preparacao.md) · **Índice:** [README](README.md)

**Ordem de execução:** 8 de 9 · anterior: [CT77](ct77-rede-desligada-por-tempo.md) · **próximo: [CT73](ct73-queda-de-energia.md)**

## Objetivo

Com o Wi-Fi ou o broker fora, o firmware segue operando e guarda os eventos; ao voltar, envia na ordem e zera a fila.

## Pré-condições

- CT70 aprovado; CT77 aprovado (para você já conhecer a tela da fila).

## Passo a passo

1. **Derrube a rede de verdade:** desligue o hotspot do celular (ou o roteador). Aguarde até a Serial mostrar a falha do Wi-Fi ou do MQTT.
2. Gere 3 eventos pela Serial: `NF`, `NF`, `NF`. Anote a hora de cada um.
3. Veja a fila subir até 3: linha `fila=3` na Serial e a tela do LCD. Tire a foto e copie o trecho da Serial.
4. **Print do Explorer antes:** `status` retido com a fila no valor anterior.
5. Religue o hotspot. Espere o Wi-Fi e o MQTT reconectarem.
6. Confirme no Explorer os 3 eventos chegando na ordem e o `status` com a fila em 0. **Print depois.**

## Resultado esperado

- O LCD mostra a fila subindo; ao voltar, os eventos chegam em ordem e a fila zera.
- O firmware não trava sem rede.

## Evidências (salvar em `assets/3-desenvolvimento-resultados/3.4.3-prototipo-mqtt-i2c/`)

| Marque | Arquivo | O que é |
|---|---|---|
| [ ] | `ct72ExplorerAntes.png` | Explorer antes |
| [ ] | `ct72ExplorerDepois.png` | Explorer depois |
| [ ] | `ct72FilaLcd.jpg` | foto do LCD com a fila |
| [ ] | `ct72-fila-sem-rede.serial.txt` | Serial com a fila e o reenvio |

## Observações

- A fila guarda 8 eventos; com mais, o evento não crítico mais antigo é descartado (RN09). Se quiser, teste com 9 depois.
- O card #124 também pede o `status` com a posição GNSS e a idade dela. Isso é outro critério, fora deste CT.

## Resultado (preencher na execução)

| Campo | Valor |
|---|---|
| Data e hora | |
| Firmware (ambiente e commit) | |
| Situação | [ ] Aprovado  [ ] Reprovado  [ ] Parcial |
| Observado | |
| Problemas | |

## Depois de executar

- Troque a linha do CT72 na seção Resultados da 3.4.3 (`documentacao.md`) para "Aprovado" e tire o aviso "IMAGEM A INCLUIR" das figuras que você salvou.
- Mascare host, usuário e SSID nos prints antes de salvar.
