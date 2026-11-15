# CT74: Comandos `avancar` e `voltar`

**Card:** #128 · **Seção:** 3.4.3 · **Antes:** [`00-preparacao.md`](00-preparacao.md) · **Índice:** [README](README.md)

**Ordem de execução:** 2 de 9 · anterior: [CT70](ct70-conexao-e-status.md) · **próximo: [CT75](ct75-lacre-por-comando.md)**

## Objetivo

Um comando em `cmd` troca a tela do LCD e a resposta chega em `cmd/resposta` com o mesmo `id`.

## Pré-condições

- CT70 aprovado, com o dispositivo ligado.

## Passo a passo

1. No Explorer, publique em `guaratracking/<ID>/cmd` (QoS 0, **com texto no corpo**): `{"id":"1","acao":"avancar"}`.
2. Em `cmd/resposta`, confirme `{"id":"1","acao":"avancar","ok":true}`. Tire o print mostrando o Publish e a resposta.
3. Olhe o LCD: ele deve estar na tela seguinte. Tire a foto.
4. Publique `{"id":"2","acao":"voltar"}` e confirme `ok:true` e o LCD de volta à tela anterior.

## Resultado esperado

- `ok:true` com o `id` devolvido nas duas respostas.
- O LCD muda para a tela seguinte e volta.

## Evidências (salvar em `assets/3-desenvolvimento-resultados/3.4.3-prototipo-mqtt-i2c/`)

| Marque | Arquivo | O que é |
|---|---|---|
| [ X ] | `ct74PublishAvancar.png` | Publish e resposta |
| [ X ] | `ct74LcdAvancar.jpg` | foto do LCD na tela seguinte |
| [x] | `ct74Comandos3Botoes.mp4` | vídeo de `avancar` e `voltar`, com o Explorer e o LCD |

## Observações

- Se o LCD estiver apagado, o comando acende o visor sem trocar de tela; repita.

## Resultado (preencher na execução)

| Campo | Valor |
|---|---|
| Data e hora | 22:47 - 22:50 |
| Firmware (ambiente e commit) | |
| Situação | [ X ] Aprovado  [ ] Reprovado  [ ] Parcial |
| Observado | Caso de teste conforme o esperado, ao postar aprimeira ação avança, a segunda retorna, conformo o esperado, ambas são mexem com o LCD e aparecem no mqtt explorer |
| Problemas | |

## Depois de executar

- Troque a linha do CT74 na seção Resultados da 3.4.3 (`documentacao.md`) para "Aprovado" e tire o aviso "IMAGEM A INCLUIR" das figuras que você salvou.
- Mascare host, usuário e SSID nos prints antes de salvar.
