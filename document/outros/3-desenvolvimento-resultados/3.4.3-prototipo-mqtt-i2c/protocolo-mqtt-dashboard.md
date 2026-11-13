# Protocolo MQTT do dashboard

Contrato entre o dashboard e o firmware do protótipo (comandos implementados em `handleDashboardCommand`, `main.cpp`, e lidos por `dashboard_cmd.h`).

## 1. Tópicos

Prefixo: `guaratracking/<device_id>/` (protótipo: `guaratracking/prototipo-001/`). QoS 0. Nenhuma mensagem é retida, exceto `online` (ver abaixo).

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
| `valor` | só em `lacre` e `rede` | Texto de até 24 caracteres |
| `segundos` | só em `rede` com `desligada` | Inteiro de 1 a 3600 |

### Comandos

| `acao` | `valor` | O que faz (equivale a) |
|---|---|---|
| `avancar` | | Próxima tela do LCD (botão AVANÇAR); acende o visor |
| `voltar` | | Tela anterior do LCD (botão VOLTAR); acende o visor |
| `ligar` | | Liga o dispositivo (chave ON/OFF) |
| `desligar` | | Desliga logicamente; o dispositivo segue ouvindo o `cmd` para poder ligar de novo |
| `autorizar` | | Registra a autorização de retirada (botão AUTORIZAR por 3 s) |
| `ciclo_iniciar` (ou `vincular`) | | Inicia o ciclo de remessa (botão CICLO por 3 s sem ciclo ativo) |
| `ciclo_encerrar` | | Encerra o ciclo (botão CICLO por 3 s com ciclo ativo) |
| `lacre` | `aberto` ou `fechado` | Simula o lacre aberto ou fechado |
| `rede` | `ligada` ou `desligada` | Liga ou desliga a rede simulada; `desligada` exige `segundos` e volta sozinha ao fim do prazo |

Observações:

- `lacre` e `rede` (e `ligar`/`desligar`) só valem no modo com chaves (`Config::SwitchButtons`), que é o do protótipo de 3 botões.
- Com a rede desligada o broker não entrega comandos, por isso o prazo em `segundos` é obrigatório: a rede volta sozinha.
- Com o dispositivo desligado, só `ligar` é aceito.

## 3. Resposta (`cmd/resposta`)

Cada comando recebido gera exatamente uma resposta, mesmo quando recusado:

```json
{"id":"a1","acao":"lacre","ok":true}
{"id":"a1","acao":"lacre","ok":false,"erro":"lacre_ja_aberto"}
```

Em caso de JSON inválido, `id` e `acao` voltam vazios quando não puderam ser lidos.

### Códigos de erro

| `erro` | Quando |
|---|---|
| `json_vazio` | Payload com 0 bytes |
| `payload_grande` | Mais de 256 bytes |
| `acao_ausente` | Sem o campo `acao` ou JSON ilegível |
| `acao_desconhecida` | `acao` fora da tabela |
| `dispositivo_desligado` | Qualquer comando, exceto `ligar`, com o dispositivo desligado |
| `somente_modo_botoes` | `ligar`, `desligar`, `lacre` ou `rede` fora do modo com chaves |
| `valor_invalido` | `valor` diferente dos aceitos em `lacre` ou `rede` |
| `lacre_ja_aberto` / `lacre_ja_fechado` | O lacre já está no estado pedido |
| `segundos_invalido` | `rede` `desligada` sem `segundos` ou fora de 1 a 3600 |
| `ciclo_ativo` | `ciclo_iniciar` com ciclo em andamento |
| `sem_ciclo_ativo` | `ciclo_encerrar` sem ciclo |
| `encerramento_recusado` | `ciclo_encerrar` recusado (lacre aberto ou sem NF de retorno) |

Uma resposta `ok:true` significa que o comando foi aceito e executado; o efeito aparece em seguida em `evento` e `status`.

## 4. Exemplos

```text
cmd           {"id":"1","acao":"avancar"}
cmd/resposta  {"id":"1","acao":"avancar","ok":true}

cmd           {"id":"2","acao":"rede","valor":"desligada","segundos":120}
cmd/resposta  {"id":"2","acao":"rede","ok":true}

cmd           {"id":"3","acao":"abrir"}
cmd/resposta  {"id":"3","acao":"abrir","ok":false,"erro":"acao_desconhecida"}
```

## 5. Fora do protocolo

Credenciais do broker (usuário e senha do EMQX) ficam só no `secrets.h` local e nunca entram no repositório nem na documentação.
