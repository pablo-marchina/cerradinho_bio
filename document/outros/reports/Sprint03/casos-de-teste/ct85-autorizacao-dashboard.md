# CT85: Autorizar retirada pelo dashboard

**Card:** #129 · **Seção:** 3.4.3 · **Antes:** [`00-preparacao.md`](00-preparacao.md) · **Índice:** [README](README.md)

## Objetivo

Validar a autorização por `cmd`, a janela de três dias da RN06 e a diferença entre retirada autorizada e violação.

## Pré-condições

- Dispositivo ligado, MQTT conectado e fila zerada.
- Lacre fechado.
- Ambiente `demo`, em que um dia equivale a um segundo, para testar a expiração sem esperar três dias reais.

## Passo a passo

1. Publique `{"id":"129-a1","acao":"autorizar"}`. Confirme `ok:true`, `janela_dias:3`, `valida_ate_ms` e `AUTORIZACAO_REGISTRADA`.
2. Antes de três segundos, abra fisicamente o lacre. Confirme `RETIRADA_AUTORIZADA`, LCD “Retirada / autorizada”, ausência de `VIOLACAO_LACRE` e buzzer desligado.
3. Feche o lacre e confirme `LACRE_REAPLICADO`.
4. Publique outra autorização, aguarde mais de três segundos e abra fisicamente o lacre.
5. Confirme `VIOLACAO_LACRE`, LCD “VIOLACAO / registrada” e buzzer ativo.

## Resultado esperado

- A resposta informa a duração e a validade no relógio do dispositivo.
- A autorização vigente é consumida na primeira abertura e não gera violação.
- A autorização vencida não impede a violação nem o buzzer.

## Evidências

| Marque | Arquivo | O que registrar |
|---|---|---|
| [ ] | `ct85AutorizacaoDashboard.mp4` | resposta, duas aberturas, LCD e buzzer |
| [ ] | `ct85-autorizacao-dashboard.serial.txt` | eventos dentro e fora da janela |
| [ ] | `ct85AutorizacaoDashboard.png` | resposta com `janela_dias` e `valida_ate_ms` |
| [x] | `issue131CmdAutorizar.png` | comando `autorizar` confirmado pelo ESP32 no dashboard |

## Resultado

| Campo | Valor |
|---|---|
| Data e hora | 18/11/2026, 14:03 (horário exibido no dashboard) |
| Firmware (ambiente e commit) | `bancada-3botoes`; branch da #131 a partir de `e9727ea5` com alteração local de bateria |
| Situação | [ ] Aprovado  [ ] Reprovado  [x] Parcial |
| Observado | O comando de autorização chegou ao ESP32 físico e recebeu resposta positiva correlacionada no dashboard. |
| Problemas | A abertura física dentro e fora da janela, `RETIRADA_AUTORIZADA`, violação, LCD e buzzer não foram executados; permanecem planejados. |
