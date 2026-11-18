# Protocolo MQTT do dashboard

Contrato entre o dashboard e o firmware do protótipo (comandos implementados em `handleDashboardCommand`, `main.cpp`, e lidos por `dashboard_cmd.h`).

**Situação:** vigente na integração física da Sprint 3. O documento `contrato-topicos-dashboard.md` registra um formato expandido futuro, ainda não implementado de ponta a ponta. Nesta versão, `ciclo_divergente` e a presença em objeto JSON não são alegados como concluídos.

## 1. Tópicos

Prefixo: `guaratracking/<device_id>/` (protótipo: `guaratracking/prototipo-001/`). QoS 0. Só `status` e `online` são retidos; as demais mensagens não.

| Tópico | Sentido | Conteúdo |
|---|---|---|
| `status` | dispositivo → dashboard | Estado atual (JSON), a cada 60 s e na conexão |
| `evento` | dispositivo → dashboard | Um JSON por evento do dispositivo (lacre, autorização, ciclo, rede, energia) |
| `online` | dispositivo → dashboard | Presença do dispositivo (conexão e last will) |
| `cmd` | dashboard → dispositivo | Comando (JSON) |
| `cmd/resposta` | dispositivo → dashboard | Resultado de cada comando (JSON) |

O dashboard assina `status`, `evento`, `online` e `cmd/resposta`, e publica em `cmd`. O broker recusa assinar `#` e `$SYS/#`.

## 2. Comando (`cmd`)

JSON plano, até 256 bytes, sem escapes (`\`) nos textos:

```json
{"id":"a1","acao":"lacre","valor":"aberto","segundos":30}
```

| Campo | Obrigatório | Descrição |
|---|---|---|
| `id` | recomendado | Texto de até 32 caracteres, devolvido na resposta para casar pedido e resposta |
| `acao` | sim | Nome do comando (tabela abaixo) |
| `valor` | só em `lacre`, `rede` e `remessa` | Texto de até 24 caracteres |
| `segundos` | só em `rede` com `desligada` | Inteiro de 1 a 3600 |

### Comandos

| `acao` | `valor` | O que faz (equivale a) |
|---|---|---|
| `avancar` | | Próxima tela do LCD (botão AVANÇAR); acende o visor |
| `voltar` | | Tela anterior do LCD (botão VOLTAR); acende o visor |
| `ligar` | | Liga o dispositivo (chave ON/OFF) |
| `desligar` | | Desliga logicamente; o dispositivo segue ouvindo o `cmd` para poder ligar de novo |
| `autorizar` | | Registra a autorização de retirada (botão AUTORIZAR por 3 s); a resposta informa a janela e o instante de validade no relógio do dispositivo |
| `ciclo_iniciar` (ou `vincular`) | | Inicia o ciclo de remessa (botão CICLO por 3 s sem ciclo ativo) |
| `ciclo_encerrar` | | Encerra o ciclo (botão CICLO por 3 s com ciclo ativo) |
| `remessa` | número da NF (até 15 caracteres: letras, dígitos, `-`, `.` ou `/`) | Registra a NF de remessa do ciclo ativo (equivale a `REMESSA x` na Serial) |
| `nf_retorno` | | Lança a NF de retorno do ciclo ativo, uma vez por ciclo (equivale a `NF` na Serial) |
| `lacre` | `aberto` ou `fechado` | Simula o lacre aberto ou fechado |
| `rede` | `ligada` ou `desligada` | Liga ou desliga a rede simulada; `desligada` exige `segundos` e volta sozinha ao fim do prazo |

Observações:

- `lacre` e `rede` (e `ligar`/`desligar`) só valem no modo com chaves (`Config::SwitchButtons`), que é o do protótipo de 3 botões.
- Com a rede desligada o broker não entrega comandos, por isso o prazo em `segundos` é obrigatório: a rede volta sozinha.
- Com o dispositivo desligado, só `ligar` é aceito.
- `remessa` e `nf_retorno` só valem com ciclo ativo (RN02 e RN11); a NF de retorno é lançada uma vez por ciclo, e o mesmo vale para o comando `NF` da Serial.

## 3. Resposta (`cmd/resposta`)

Cada comando recebido gera exatamente uma resposta, mesmo quando recusado:

```json
{"id":"a1","acao":"lacre","ok":true}
{"id":"a1","acao":"lacre","ok":false,"erro":"lacre_ja_aberto"}
{"id":"a2","acao":"autorizar","ok":true,"janela_dias":3,"valida_ate_ms":123456}
{"id":"a3","acao":"ciclo_encerrar","ok":false,"erro":"encerramento_recusado","motivo":"lacre_aberto"}
```

Em caso de JSON inválido, `id` e `acao` voltam vazios quando não puderam ser lidos.

### Códigos de erro

| `erro` | Quando |
|---|---|
| `json_vazio` | Payload com 0 bytes |
| `json_invalido` | Objeto incompleto, sintaxe inválida, chave repetida, conteúdo depois do `}` ou estrutura fora do contrato plano |
| `payload_grande` | Mais de 256 bytes |
| `acao_ausente` | Sem o campo `acao` ou JSON ilegível |
| `acao_desconhecida` | `acao` fora da tabela |
| `dispositivo_desligado` | Qualquer comando, exceto `ligar`, com o dispositivo desligado |
| `somente_modo_botoes` | `ligar`, `desligar`, `lacre` ou `rede` fora do modo com chaves |
| `valor_invalido` | `valor` diferente dos aceitos em `lacre` ou `rede` |
| `lacre_ja_aberto` / `lacre_ja_fechado` | O lacre já está no estado pedido |
| `segundos_invalido` | `rede` `desligada` sem `segundos` ou fora de 1 a 3600 |
| `ciclo_ativo` | `ciclo_iniciar` com ciclo em andamento |
| `sem_ciclo_ativo` | `ciclo_encerrar`, `remessa` ou `nf_retorno` sem ciclo ativo |
| `nf_invalida` | `remessa` sem `valor` ou com número fora do formato |
| `nf_ja_lancada` | `nf_retorno` quando a NF de retorno do ciclo já foi lançada |
| `encerramento_recusado` | `ciclo_encerrar` recusado (lacre aberto ou sem NF de retorno) |
| `persistencia_pendente` | A ação ocorreu em RAM, mas a NVS falhou; `pendente:true` informa que o firmware continuará tentando salvar. Não repetir o comando |
| `resposta_anterior_pendente` | O comando novo não foi executado porque o reenvio de uma resposta anterior ainda falhou |
| `fila_respostas_cheia` | As quatro vagas regulares estavam ocupadas; a ação nova não foi executada e sua recusa usou a vaga reservada |

Uma resposta `ok:true` significa que o comando foi aceito e executado; o efeito aparece em seguida em `evento` e `status`.

Em `autorizar`, `janela_dias` informa a duração da RN06 e `valida_ate_ms` usa o relógio
acumulado do dispositivo (`sim_clock.h`). Esse relógio sobrevive ao reinício quando o contexto
é salvo, mas não é uma data UTC: no ambiente `demo`, um dia equivale a um segundo; no ambiente
`weekly`, equivale a 24 horas. O dashboard não deve apresentar esse valor como data civil.

Quando `ciclo_encerrar` é recusado com um ciclo ativo, `motivo` distingue `lacre_aberto` de
`nf_retorno_ausente`. Sem ciclo ativo, o próprio `erro` é `sem_ciclo_ativo`.

### Falha ao publicar a resposta

O firmware valida o objeto JSON inteiro antes de executar a ação. Uma mensagem incompleta, como
`{"acao":"ciclo_encerrar"`, ou ambígua, como
`{"acao":"autorizar","acao":"ciclo_encerrar"}`, responde `json_invalido` e não muda o ciclo.

Antes de publicar ou guardar uma resposta `ok:true` para uma ação que muda ciclo, NF, lacre ou
autorização, o firmware salva o novo contexto na NVS. Se a gravação falhar depois que a ação já
alterou a RAM ou emitiu eventos, a resposta é `persistencia_pendente`, com `pendente:true`: não é
uma recusa e o dashboard não deve repetir o comando. O `loop` continua tentando salvar; até isso
ocorrer, `status` e `evento` representam o efeito em RAM, mas um reinício ainda pode restaurar o
estado durável anterior.

Se `mqtt.publish()` falhar depois da execução, o JSON completo de `cmd/resposta` é preservado em
uma fila na NVS e reenviado com o mesmo `id` quando o MQTT estiver disponível. Se o reenvio passar
ao chegar outro comando, o novo comando continua sendo processado na mesma chamada. Se o reenvio
falhar novamente, a nova ação não é executada e uma resposta `resposta_anterior_pendente`, com o
novo `id`, entra na fila para entrega posterior.

A outbox usa dois snapshots completos com geração e checksum. Cada mudança é escrita no slot
inativo e verificada antes de substituir o estado em RAM; durante a remoção, uma falha parcial
preserva o snapshot anterior e pode repetir a resposta já enviada, mas não apaga as respostas
posteriores que já estavam duráveis. Existem
quatro vagas regulares e uma quinta reservada: se as quatro estiverem ocupadas, o comando que já
chegou recebe `fila_respostas_cheia`, não é executado e a leitura de novos comandos fica suspensa
até o escoamento da fila.
O dashboard deve correlacionar e deduplicar respostas pelo `id`, pois uma falha ao limpar a NVS
depois do envio pode causar uma repetição após reinício.

O transporte continua em QoS 0: um retorno `true` de `mqtt.publish()` confirma somente que o
cliente aceitou o envio, não que o dashboard processou a mensagem. Se o `id` não receber resposta
dentro do prazo da interface, o dashboard deve consultar `status`/`evento` antes de decidir se pode
repetir uma ação que altera estado.

## 4. Exemplos

```text
cmd           {"id":"1","acao":"avancar"}
cmd/resposta  {"id":"1","acao":"avancar","ok":true}

cmd           {"id":"2","acao":"rede","valor":"desligada","segundos":120}
cmd/resposta  {"id":"2","acao":"rede","ok":true}

cmd           {"id":"3","acao":"abrir"}
cmd/resposta  {"id":"3","acao":"abrir","ok":false,"erro":"acao_desconhecida"}

cmd           {"id":"4","acao":"autorizar"}
cmd/resposta  {"id":"4","acao":"autorizar","ok":true,"janela_dias":3,"valida_ate_ms":123456}
```

## 5. Fora do protocolo

Credenciais do broker (usuário e senha do EMQX) ficam só no `secrets.h` local e nunca entram no repositório nem na documentação.
