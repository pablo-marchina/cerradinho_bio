# CT71: Violação do lacre publicada no `evento`

**Card:** #123 · **Seção:** 3.4.3 · **Antes:** [`00-preparacao.md`](00-preparacao.md) · **Índice:** [README](README.md)

**Ordem de execução:** 4 de 9 · anterior: [CT75](ct75-lacre-por-comando.md) · **próximo: [CT76](ct76-comandos-recusados.md)**

## Objetivo

A violação gera `VIOLACAO_LACRE` no `evento` no mesmo segundo, e o `status` retido passa a mostrar o lacre violado.

## Pré-condições

- CT75 aprovado (as evidências do CT71 saem do mesmo roteiro).

## Passo a passo

1. Siga o `ct75-lacre-por-comando.md`, passos 2 a 5.
2. Repita **uma vez com ciclo ativo** (envie `{"id":"5","acao":"ciclo_iniciar"}` antes) e confirme `ciclo_ativo=true` no evento.
3. Feche o ciclo no final, se precisar (`ciclo_encerrar` exige lacre fechado e NF de retorno; veja o CT76).

## Resultado esperado

- `VIOLACAO_LACRE` com `ciclo_ativo=false` e com `ciclo_ativo=true`.
- `status` republicado no mesmo segundo.

## Evidências (salvar em `assets/3-desenvolvimento-resultados/3.4.3-prototipo-mqtt-i2c/`)

| Marque | Arquivo | O que é |
|---|---|---|
| [ ] | `ct71ExplorerViolacao.png` | evento no Explorer |
| [ ] | `ct71ExplorerStatusViolado.png` | `status` violado |
| [x] | `ct71Violacao3Botoes.mp4` | vídeo da violação por comando, com o LCD, o buzzer e o Explorer |

## Observações

- Se o `STATUS_SEMANAL` (a cada 7 s no demo) cobrir o `evento` no Explorer, faça a captura sem ciclo ativo.

## Resultado (preencher na execução)

| Campo | Valor |
|---|---|
| Data e hora | 17:52 - 17:53|
| Firmware (ambiente e commit) |  |
| Situação | [ s ] Aprovado  [ ] Reprovado  [ ] Parcial |
| Observado | Tudo conforme o estipulado do ct|
| Problemas | |

## Depois de executar

- Troque a linha do CT71 na seção Resultados da 3.4.3 (`documentacao.md`) para "Aprovado" e tire o aviso "IMAGEM A INCLUIR" das figuras que você salvou.
- Mascare host, usuário e SSID nos prints antes de salvar.
