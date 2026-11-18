# CT84: Abrir e encerrar ciclo pelo dashboard

**Card:** #129 · **Seção:** 3.4.3 · **Antes:** [`00-preparacao.md`](00-preparacao.md) · **Índice:** [README](README.md)

## Objetivo

Validar o ciclo completo por `cmd`, as recusas do RF016, a validação integral do JSON e a convivência com o botão físico de ciclo.

## Pré-condições

- Dispositivo ligado, MQTT conectado e fila zerada.
- Lacre fechado e nenhum ciclo ativo no `status`.

## Passo a passo

1. Publique `{"id":"129-1","acao":"ciclo_encerrar"` sem a chave final. Confirme `ok:false`, `erro:"json_invalido"` e ciclo ainda inativo.
2. Publique `{"id":"129-dup","acao":"autorizar","acao":"ciclo_encerrar"}`. Confirme `ok:false`, `erro:"json_invalido"`, ciclo ainda inativo e nenhuma autorização registrada.
3. Publique `{"id":"129-2","acao":"ciclo_iniciar"}`. Confirme `ok:true`, `CICLO_INICIADO` e ciclo ativo.
4. Publique `{"id":"129-3","acao":"ciclo_encerrar"}` sem NF de retorno. Confirme `encerramento_recusado`, `motivo:"nf_retorno_ausente"` e LCD “Nao encerrado / falta NF retorno”.
5. Publique `{"id":"129-4","acao":"nf_retorno"}`. Confirme `ok:true` e `NF_RETORNO_LANCADA`.
6. Abra o lacre e publique `{"id":"129-5","acao":"ciclo_encerrar"}`. Confirme `motivo:"lacre_aberto"` e ciclo ainda ativo.
7. Feche o lacre e publique `{"id":"129-6","acao":"ciclo_encerrar"}`. Confirme `ok:true`, `CICLO_ENCERRADO`, ciclo inativo e LCD “Ciclo encerrado”.
8. Inicie outro ciclo pelo `cmd`, lance a NF de retorno e encerre segurando o botão CICLO por 3 s. Depois inicie novamente pelo botão, comprovando que os dois caminhos continuam usando o mesmo estado.

## Resultado esperado

- JSON malformado ou com chave repetida não executa ação.
- NF de retorno ausente e lacre aberto impedem o encerramento com motivos distintos.
- O encerramento válido publica o estado já encerrado e atualiza o LCD.
- Comando e pressão longa permanecem compatíveis.

## Evidências

| Marque | Arquivo | O que registrar |
|---|---|---|
| [ ] | `ct84CicloDashboard.mp4` | Explorer, protótipo, LCD e botão durante o fluxo completo |
| [ ] | `ct84-ciclo-dashboard.serial.txt` | eventos e mensagens da Serial |
| [ ] | `ct84CicloDashboard.png` | respostas correlacionadas por `id` no Explorer |
| [x] | `issue131CmdAbrirCiclo.png` | abertura do ciclo confirmada pelo ESP32 no dashboard |
| [x] | `issue131CmdNfRemessa.png` | NF de remessa confirmada pelo ESP32 no dashboard |

## Resultado

| Campo | Valor |
|---|---|
| Data e hora | 18/11/2026, 13:57–14:07 (horário exibido no dashboard) |
| Firmware (ambiente e commit) | `bancada-3botoes`; branch da #131 a partir de `e9727ea5` com alteração local de bateria |
| Situação | [ ] Aprovado  [ ] Reprovado  [x] Parcial |
| Observado | No ESP32 físico, abrir ciclo, recusar encerramento sem NF de retorno, registrar NF de retorno e encerrar passaram pelo dashboard e por `cmd/resposta`. |
| Problemas | Não foram executados JSON inválido/chave repetida, encerramento com lacre aberto nem compatibilidade com o botão físico; vídeo e Serial do roteiro completo continuam pendentes. |
