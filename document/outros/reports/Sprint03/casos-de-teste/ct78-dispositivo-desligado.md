# CT78: Dispositivo desligado: só `ligar` é aceito

**Card:** #128 · **Seção:** 3.4.3 · **Antes:** [`00-preparacao.md`](00-preparacao.md) · **Índice:** [README](README.md)

**Ordem de execução:** 6 de 9 · anterior: [CT76](ct76-comandos-recusados.md) · **próximo: [CT77](ct77-rede-desligada-por-tempo.md)**

## Objetivo

Com o dispositivo desligado logicamente, só `ligar` é aceito; o resto recebe `dispositivo_desligado`.

## Pré-condições

- CT74 aprovado; dispositivo ligado.

## Passo a passo

1. Publique `{"id":"10","acao":"desligar"}`: `ok:true`, e o LCD apaga.
2. Publique `{"id":"11","acao":"avancar"}`: `ok:false` com `erro":"dispositivo_desligado"`.
3. Publique `{"id":"12","acao":"ligar"}`: `ok:true`, e o LCD volta.
4. Confirme na Serial que o dispositivo ouviu o `cmd` o tempo todo.
5. **Teste adicional, feito pelo Arthur:** repita o desligar e o ligar **pelo botão ON/OFF** (segurar 3 s) em vez do comando, e olhe o tópico `evento` no Explorer. O `evento` mudou nesse teste, e o tipo observado foi `STATUS_SEMANAL`. Anote se o resultado é o mesmo do comando.

## Resultado esperado

- `desligar` e `ligar` com `ok:true`; `avancar` desligado com `dispositivo_desligado`.
- Pelo botão ON/OFF, o `evento` registra o desligamento e o religamento, como no comando.

## Evidências (salvar em `assets/3-desenvolvimento-resultados/3.4.3-prototipo-mqtt-i2c/`)

| Marque | Arquivo | O que é |
|---|---|---|
| [ ] | `ct78Desligado.png` | as três respostas no Explorer |
| [ ] | `ct78DesligadoBotaoEvento.png` | o `evento` no Explorer ao desligar e ligar pelo botão ON/OFF |
| [x] | `ct78Desligado3Botoes.mp4` | vídeo do desligar e ligar por comando e pelo botão ON/OFF |

## Resultado (preencher na execução)

| Campo | Valor |
|---|---|
| Data e hora | |
| Firmware (ambiente e commit) | |
| Situação | [ X ] Aprovado  [ ] Reprovado  [ ] Parcial |
| Observado | Tudo certo. Testado também com o botão ON/OFF além do comando, e o `evento` mudou, e o tipo observado foi `STATUS_SEMANAL` |
| Problemas | |

## Depois de executar

- Troque a linha do CT78 na seção Resultados da 3.4.3 (`documentacao.md`) para "Aprovado" e tire o aviso "IMAGEM A INCLUIR" das figuras que você salvou.
- Mascare host, usuário e SSID nos prints antes de salvar.
