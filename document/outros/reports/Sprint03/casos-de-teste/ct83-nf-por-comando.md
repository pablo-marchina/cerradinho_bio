# CT83: NF por comando (remessa e retorno)

**Card:** #128 · **Seção:** 3.4.3 · **Antes:** [`00-preparacao.md`](00-preparacao.md) · **Índice:** [README](README.md)

## Objetivo

O dashboard registra a NF de remessa e a NF de retorno pelo `cmd`, com as mesmas regras da Serial: só com ciclo ativo (RN02) e a NF de retorno uma vez por ciclo (RN11).

## Pré-condições

- CT74 aprovado; dispositivo ligado; lacre fechado; **sem ciclo ativo** (`"ciclo_ativo":false` no `status`; se houver um ciclo esquecido, encerre-o antes).

## Passo a passo

1. **Sem ciclo:** publique `{"id":"80","acao":"remessa","valor":"NF-000123"}`: `ok:false` com `erro":"sem_ciclo_ativo"`. Depois `{"id":"81","acao":"nf_retorno"}`: o mesmo erro.
2. Inicie o ciclo: `{"id":"82","acao":"ciclo_iniciar"}` (`ok:true`).
3. **NF inválida:** `{"id":"83","acao":"remessa","valor":"NF 123"}` (tem espaço) e `{"id":"84","acao":"remessa"}` (sem `valor`): `ok:false` com `erro":"nf_invalida"`. A tela de NF do LCD continua "nao informada".
4. **NF válida:** `{"id":"85","acao":"remessa","valor":"NF-000123"}`: `ok:true`, o LCD mostra "NF registrada" e, na tela de NF, `NF-000123`. O `status` traz `"nf":"NF-000123"`.
5. **NF de retorno:** `{"id":"86","acao":"nf_retorno"}`: `ok:true` e `NF_RETORNO_LANCADA` no `evento`. Repita: `ok:false` com `erro":"nf_ja_lancada"`.
6. **Serial:** digite `NF` e Enter. Não deve aparecer outro `NF_RETORNO_LANCADA` (a Serial segue a mesma regra e imprime `NF de retorno recusada`).
7. **Encerre:** `{"id":"87","acao":"ciclo_encerrar"}`: `ok:true` e `CICLO_ENCERRADO`. Sem ciclo de novo, `remessa` e `nf_retorno` voltam a responder `sem_ciclo_ativo`.

## Resultado esperado

- Sem ciclo, as duas ações respondem `sem_ciclo_ativo`.
- Com ciclo, a NF de remessa válida aparece no LCD; a inválida responde `nf_invalida`.
- A NF de retorno é aceita uma vez por ciclo (`nf_ja_lancada` na segunda) e só então o ciclo encerra.

## Evidências (salvar em `assets/3-desenvolvimento-resultados/3.4.3-prototipo-mqtt-i2c/`)

| Marque | Arquivo | O que é |
|---|---|---|
| [ ] | `ct83Nf3Botoes.mp4` | vídeo com as respostas no Explorer e a tela de NF do LCD |
| [ ] | `ct83ExplorerErros.png` | respostas `sem_ciclo_ativo`, `nf_invalida` e `nf_ja_lancada` |

## Resultado (preencher na execução)

| Campo | Valor |
|---|---|
| Data e hora | |
| Firmware (ambiente e commit) | |
| Situação | [ ] Aprovado  [ ] Reprovado  [ ] Parcial |
| Observado | |
| Problemas | |

## Depois de executar

- Troque a linha do CT83 na seção Resultados da 3.4.3 (`documentacao.md`) para "Aprovado" e tire o aviso "IMAGEM A INCLUIR" das figuras que você salvou.
- Mascare host, usuário e SSID nos prints antes de salvar.
