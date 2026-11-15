# CT75: Lacre aberto e fechado por comando

**Card:** #128 · **Seção:** 3.4.3 · **Antes:** [`00-preparacao.md`](00-preparacao.md) · **Índice:** [README](README.md)

**Ordem de execução:** 3 de 10 · anterior: [CT74](ct74-comandos-de-tela.md) · **próximo: [CT71](ct71-violacao-do-lacre.md)**

## Objetivo

O comando `lacre` simula a violação e o fechamento, com `evento`, `status` e buzzer.

## Pré-condições

- CT74 aprovado; lacre fechado; dispositivo ligado e ambiente `bancada-3botoes` (as chaves só valem nesse modo).

## Passo a passo

1. Publique `{"id":"3","acao":"lacre","valor":"aberto"}` em `cmd` e confirme `ok:true` em `cmd/resposta`. Anote a hora.
2. Em `evento`, confirme `VIOLACAO_LACRE`. Compare o `uptime_ms` com o da Serial.
3. Em `status`, confirme `"lacre":"violado"` (ele é republicado em até 1 s). **Print do `status` violado.**
4. Ouça o buzzer. Se puder, grave um vídeo curto com o áudio e o LCD ("VIOLACAO registrada").
5. Publique `{"id":"4","acao":"lacre","valor":"fechado"}` e confirme `ok:true`, o evento `LACRE_FECHADO` e o LCD "Lacre fechado / apos violacao".

## Resultado esperado

- `ok:true` nas duas respostas.
- `VIOLACAO_LACRE` e `LACRE_FECHADO` em `evento`.
- `status` com `"lacre":"violado"` e depois íntegro.
- Buzzer e LCD conforme descrito.

## Evidências (salvar em `assets/3-desenvolvimento-resultados/3.4.3-prototipo-mqtt-i2c/`)

| Marque | Arquivo | O que é |
|---|---|---|
| [ ] | `ct75LacreAberto.png` | comando e resposta |
| [ ] | `ct71ExplorerViolacao.png` | evento no Explorer |
| [ ] | `ct71ExplorerStatusViolado.png` | `status` violado |
| [ ] | `ct71LcdViolacao.png` | LCD na violação |
| [ ] | `ct71LcdLacreFechado.png` | LCD com o lacre fechado |
| [ ] | `ct75-lacre-por-comando.serial.txt` | trecho da Serial |
| [x] | `ct75Lacre3Botoes.mp4` | vídeo do `lacre` `aberto` e `fechado`, com o Explorer e o LCD |

## Observações

- Este roteiro cobre também o CT71 no protótipo de 3 botões (o lacre é simulado por comando). Marque os dois CTs com as mesmas evidências.

## Resultado (preencher na execução)

| Campo | Valor |
|---|---|
| Data e hora | 17h:49 - 17h:51 |
| Firmware (ambiente e commit) | |
| Situação | [ s ] Aprovado  [ ] Reprovado  [ ] Parcial |
| Observado | Deu tudo certo conforme o que o ct estipulava, porém eu notei uma função interessante a ser implementado ao dar o comando de fechar pela 2º vez o led n ascendia, arrumei isso no código para o led ascender não apenas na primeira vez. |
| Problemas | |

## Depois de executar

- Troque a linha do CT75 na seção Resultados da 3.4.3 (`documentacao.md`) para "Aprovado" e tire o aviso "IMAGEM A INCLUIR" das figuras que você salvou.
- Mascare host, usuário e SSID nos prints antes de salvar.
