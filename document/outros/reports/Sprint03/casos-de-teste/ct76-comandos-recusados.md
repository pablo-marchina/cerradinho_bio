# CT76: Comandos recusados com motivo

**Card:** #128 · **Seção:** 3.4.3 · **Antes:** [`00-preparacao.md`](00-preparacao.md) · **Índice:** [README](README.md)

**Ordem de execução:** 5 de 10 · anterior: [CT71](ct71-violacao-do-lacre.md) · **próximo: [CT78](ct78-dispositivo-desligado.md)**

## Objetivo

Comandos inválidos recebem `ok:false` com o `erro` certo e não mudam o dispositivo.

## Pré-condições

- CT74 aprovado; lacre **fechado**.

## Passo a passo

1. Publique `{"id":"6","acao":"abrir"}` e confirme `ok:false` com `erro":"acao_desconhecida"`.
2. Publique `{"id":"7","acao":"lacre","valor":"fechado"}` (já está fechado) e confirme `erro":"lacre_ja_fechado"`.
3. Publique `{"id":"8","acao":"lacre","valor":"aberto"}` duas vezes seguidas: a 1ª `ok:true`, a 2ª `erro":"lacre_ja_aberto"`. Depois feche o lacre.
4. Publique com o corpo **vazio** (0 bytes): `erro":"json_vazio"`.
5. Publique `{"id":"9","acao":"lacre","valor":"meio"}`: `erro":"valor_invalido"`.
6. Confirme que o LCD e o `status` não mudaram nas recusas.

## Resultado esperado

- Cada recusa traz `ok:false` e o `erro` esperado, com o `id` devolvido (vazio no `json_vazio`).

## Evidências (salvar em `assets/3-desenvolvimento-resultados/3.4.3-prototipo-mqtt-i2c/`)

| Marque | Arquivo | O que é |
|---|---|---|
| [ ] | `ct76Recusas.png` | respostas de erro no Explorer (uma imagem com todas, ou várias) |
| [x] | `ct76Recusas3Botoes.mp4` | vídeo dos comandos recusados, com o Explorer e o LCD |

## Observações

- Os códigos completos estão no protocolo (13 erros). Estes cinco cobrem os mais prováveis na banca.

## Resultado (preencher na execução)

| Campo | Valor |
|---|---|
| Data e hora | 18:00 - 18:04|
| Firmware (ambiente e commit) | |
| Situação | [ X ] Aprovado  [ ] Reprovado  [ ] Parcial |
| Observado | tudo certo conforme o planejado|
| Problemas | |

## Depois de executar

- Troque a linha do CT76 na seção Resultados da 3.4.3 (`documentacao.md`) para "Aprovado" e tire o aviso "IMAGEM A INCLUIR" das figuras que você salvou.
- Mascare host, usuário e SSID nos prints antes de salvar.
