# CT73: Queda de energia e Last Will

**Card:** #124 · **Seção:** 3.4.3 · **Antes:** [`00-preparacao.md`](00-preparacao.md) · **Índice:** [README](README.md)

**Ordem de execução:** 10 de 10 · anterior: [CT72](ct72-fila-sem-rede.md) · **próximo: fim: atualize a 3.4.3 e a RTM (#126)**

## Objetivo

Ao cortar a alimentação, o broker publica `offline` em `online`; ao religar, volta a `online`.

## Pré-condições

- CT70 aprovado; **faça por último**, porque corta a energia.
- Um cronômetro (o do celular serve).

## Passo a passo

1. No Explorer, deixe `guaratracking/<ID>/online` visível com `online`.
2. Tire o USB e **inicie o cronômetro**. Anote a hora exata da queda.
3. Espere o valor virar `offline`. Pare o cronômetro. **Print do Explorer** com a hora.
4. Esperado: até 45 s (1,5 vez o keep-alive de 30 s).
5. Religue o USB. Confirme `online` de novo e outro print.

## Resultado esperado

- `online = offline` em até 45 s depois da queda; `online` ao religar.

## Evidências (salvar em `assets/3-desenvolvimento-resultados/3.4.3-prototipo-mqtt-i2c/`)

| Marque | Arquivo | O que é |
|---|---|---|
| [x] | `ct73QuedaEnergia3Botoes.mp4` | vídeo da queda de energia (USB desligado e LCD apagando); não mostra o `offline` no Explorer |
| [ ] | `ct73ExplorerOffline.png` | Explorer com `offline` |
| [ ] | `ct73ExplorerOnline.png` | Explorer com `online` de volta |

## Observações

- Anote o tempo medido na tabela de resultado; o ensaio do Luiz deu cerca de 40 s, sem cronômetro.

## Resultado (preencher na execução)

| Campo | Valor |
|---|---|
| Data e hora | |
| Firmware (ambiente e commit) | |
| Situação | [ X ] Aprovado  [ ] Reprovado  [ ] Parcial |
| Observado | O `offline` chegou ao broker cerca de 34 s depois de tirar o USB (estimado pelo Arthur, sem cronômetro; esperado até 45 s). O vídeo mostra a queda, mas não o `offline` no Explorer. |
| Problemas | |

## Depois de executar

- Troque a linha do CT73 na seção Resultados da 3.4.3 (`documentacao.md`) para "Aprovado" e tire o aviso "IMAGEM A INCLUIR" das figuras que você salvou.
- Mascare host, usuário e SSID nos prints antes de salvar.
