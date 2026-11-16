# Contrato MQTT entre dispositivo e dashboard — versão 1

- **Issue de origem:** #127 — Contrato de tópicos com o dashboard, parte 1
- **Data da versão:** 16/11/2026
- **Situação:** proposta pronta para validação pelo responsável do dashboard
- **Responsável pelo dashboard:** @pablo.marchina
- **Confirmação do dashboard:** PENDENTE

## 1. Objetivo e limites

Este contrato define os cinco canais MQTT usados entre o dispositivo GuaráTracking, o broker EMQX e a plataforma que alimenta o dashboard:

- `status`;
- `evento`;
- `online`;
- `cmd`;
- `cmd/resposta`.

O contrato descreve o formato alvo da integração. Na implementação integrada até a issue #123, o firmware já publica `status`, `online` e `evento` e assina `cmd`; `cmd/resposta` e a execução dos comandos ainda dependem das issues bloqueadas pela #127. O dashboard visual atual também é uma demonstração estática, sem backend ou MQTT.

O ESP32 não é a fonte de nome do equipamento, fornecedor, responsável, prazo comercial, situação fiscal ou tratamento administrativo de alertas. Esses dados pertencem ao backend. O dispositivo informa somente seu estado físico, contexto local, posição, fila e resultado dos comandos que executa.

Todos os exemplos usam identificadores e dados fictícios.

## 2. Broker e transporte

| Ambiente | Transporte | Endpoint | Situação |
|---|---|---|---|
| Desenvolvimento local | MQTT TCP | `mqtt://IP-do-notebook:1883` | Disponível no EMQX 5.8.9 do `docker/emqx` |
| Dashboard local | MQTT por WebSocket | `ws://IP-do-notebook:8083/mqtt` | Testado nos dois sentidos em 13/11/2026 pela issue #138 |
| Broker online | MQTT com TLS | Host do deployment EMQX Cloud, porta `8883` | Escolhido e testado com o ESP32 na issue #122 |
| Dashboard online | MQTT por WebSocket seguro | Endpoint WSS fornecido pelo deployment | Exige credenciais e validação específica do frontend |

O teste local automatizado está em `scripts/test-emqx-local.cjs`. Ele comprovou que uma mensagem publicada por MQTT TCP chega ao WebSocket e que uma mensagem publicada pelo WebSocket chega ao cliente MQTT TCP. Ele não comprova o ESP32 físico, o Firewall entre equipamentos ou o WSS do Cloud.

## 3. Namespace e regras comuns

O prefixo é configurado por `MQTT_DEVICE_ID`:

```text
guaratracking/{device_id}/
```

Exemplo usado neste contrato:

```text
guaratracking/prototipo-001/
```

Regras comuns:

- payload codificado em UTF-8 e contendo um objeto JSON válido;
- chaves em `snake_case`;
- `device_id` igual ao identificador presente no tópico;
- datas absolutas em UTC e ISO 8601, no formato `YYYY-MM-DDTHH:mm:ssZ`;
- dados desconhecidos representados por `null`, nunca por coordenada zero, string vazia ambígua ou data inventada;
- `uptime_ms` e `clock` preservados quando o dispositivo não conhece a hora absoluta;
- `received_at` é registrado pelo backend ao receber a mensagem e não é inventado pelo ESP32;
- exemplos e testes não usam NF, fornecedor, posição ou credencial real;
- o cliente do dashboard pode assinar `guaratracking/+/status`, `guaratracking/+/evento`, `guaratracking/+/online` e `guaratracking/+/cmd/resposta`, conforme as permissões do broker.

O firmware atual configura um buffer MQTT de 512 bytes. A implementação deve medir tópico mais payload antes de publicar; qualquer schema que ultrapasse esse limite exige aumentar o buffer e repetir os testes de memória e comunicação.

## 4. Resumo dos tópicos

| Sufixo | Publicador | Consumidor | Retido | Finalidade |
|---|---|---|---|---|
| `status` | Dispositivo | Backend e dashboard | Sim | Último estado conhecido do dispositivo |
| `evento` | Dispositivo | Backend e dashboard | Não | Ocorrência histórica, preservando a ordem e o momento em que aconteceu |
| `online` | Dispositivo e Last Will do broker | Backend e dashboard | Sim | Presença da sessão MQTT |
| `cmd` | Backend/dashboard | Dispositivo | Não | Pedido identificado e com validade limitada |
| `cmd/resposta` | Dispositivo | Backend/dashboard | Não | Aceite, recusa ou erro relacionado ao pedido |

Na biblioteca atual, as publicações normais do ESP32 usam QoS 0. O Last Will de `online` é registrado com QoS 1. Garantias adicionais de entrega ou confirmação pelo servidor continuam pendentes e não devem ser declaradas como implementadas por este contrato.

## 5. Tópico `status`

- **Tópico:** `guaratracking/{device_id}/status`
- **Publicador:** dispositivo
- **Retido:** sim
- **Publicação:** ao conectar, a cada 60 segundos e quando um evento altera o estado, respeitando intervalo mínimo de 1 segundo.

### Exemplo

```json
{
  "schema_version": "1.0",
  "device_id": "prototipo-001",
  "reported_at": "2026-11-13T14:30:00Z",
  "uptime_ms": 120450,
  "clock": "ntp",
  "lacre": "violado",
  "bateria_pct": 68,
  "ciclo_ativo": true,
  "ciclo_id": 12,
  "nf": "NF-000123",
  "dias_fora": 39,
  "gnss_fix": true,
  "posicao": {
    "latitude": -16.686900,
    "longitude": -49.264800,
    "captured_at": "2026-11-11T11:40:00Z"
  },
  "fila": {
    "pendentes": 3,
    "perdidos": 0
  }
}
```

### Campos e uso no dashboard

| Campo | Tipo | Regra |
|---|---|---|
| `schema_version` | string | Versão deste contrato |
| `device_id` | string | Deve coincidir com `{device_id}` do tópico |
| `reported_at` | string ou `null` | Hora absoluta do status; `null` se indisponível |
| `uptime_ms` | inteiro | Tempo ligado segundo o dispositivo |
| `clock` | string | Origem do relógio, por exemplo `ntp`, `gnss` ou `uptime_only` |
| `lacre` | string | `integro`, `violado` ou `indeterminado` |
| `bateria_pct` | inteiro ou `null` | Percentual de 0 a 100; `null` quando a leitura falhar |
| `ciclo_ativo` | booleano | Indica se o dispositivo está vinculado a um ciclo ativo |
| `ciclo_id` | inteiro ou `null` | Identificador do ciclo local |
| `nf` | string ou `null` | Referência anonimizada da NF de remessa |
| `dias_fora` | inteiro | Dias contados no ciclo ativo |
| `gnss_fix` | booleano | Informa se há fix recente; não substitui a última posição válida |
| `posicao` | objeto ou `null` | Última posição válida preservada |
| `posicao.captured_at` | string ou `null` | Base para o dashboard calcular e exibir a idade da posição |
| `fila.pendentes` | inteiro | Eventos que aguardam envio |
| `fila.perdidos` | inteiro | Eventos não críticos que não couberam na fila |

O dashboard calcula a idade como diferença entre o horário atual e `posicao.captured_at`. Quando a hora absoluta não estiver disponível, deve mostrar “idade desconhecida”, sem transformar `uptime_ms` em data civil.

## 6. Tópico `evento`

- **Tópico:** `guaratracking/{device_id}/evento`
- **Publicador:** dispositivo
- **Retido:** não
- **Publicação:** imediatamente quando houver rede ou na ordem da fila quando a conexão voltar.

### Exemplo

```json
{
  "schema_version": "1.0",
  "event_id": "prototipo-001-boot7-0042",
  "device_id": "prototipo-001",
  "type": "VIOLACAO_LACRE",
  "critical": true,
  "occurred_at": "2026-11-13T14:29:51Z",
  "uptime_ms": 111450,
  "clock": "ntp",
  "ciclo_id": 12,
  "dias_fora": 39,
  "seal": "violated",
  "battery_pct": 68
}
```

Regras:

- `event_id` identifica a ocorrência e permite ao backend evitar duplicações;
- `occurred_at` é a hora da ocorrência e pode ser `null` quando houver apenas `uptime_ms`;
- o backend acrescenta `received_at` separadamente;
- o histórico é ordenado por `occurred_at` quando conhecido e preserva também a ordem de recebimento;
- evento recebido depois de período offline não pode receber `received_at` como se fosse a hora em que ocorreu;
- eventos críticos, como `VIOLACAO_LACRE`, alimentam a central de alertas, mas o estado administrativo `aberto`, `reconhecido` ou `resolvido` pertence ao backend.

Tipos já existentes no firmware incluem `VIOLACAO_LACRE`, `RETIRADA_AUTORIZADA`, `LACRE_REAPLICADO`, `LACRE_FECHADO`, `BATERIA_BAIXA`, `SENSOR_BATERIA_FALHA`, `CICLO_INICIADO`, `CICLO_ENCERRADO`, `ENCERRAMENTO_RECUSADO`, `GNSS_SEM_FIX` e `GNSS_RECUPERADO`. A implementação não deve criar um tipo novo sem atualizar este contrato.

## 7. Tópico `online`

- **Tópico:** `guaratracking/{device_id}/online`
- **Publicador:** dispositivo ao conectar e broker pelo Last Will
- **Retido:** sim

### Exemplo de conexão

```json
{
  "schema_version": "1.0",
  "device_id": "prototipo-001",
  "state": "online",
  "session_id": "boot7",
  "reported_at": "2026-11-13T14:28:00Z"
}
```

### Payload do Last Will

```json
{
  "schema_version": "1.0",
  "device_id": "prototipo-001",
  "state": "offline",
  "session_id": "boot7",
  "reported_at": null
}
```

O broker não conhece a hora em que o firmware montou o Last Will. Por isso, o backend registra `received_at` quando recebe `offline`; não deve preencher `reported_at` retroativamente. O dashboard mostra a presença atual a partir de `state`.

A idade da última comunicação não pode tratar uma entrega retida como uma comunicação nova do dispositivo. Ao receber `status` ou `online` com o indicador MQTT `retain = true`, inclusive depois de o backend ou dashboard se reconectar e refazer a assinatura, o consumidor aplica estas regras:

- preserva o instante de última comunicação já armazenado, sem substituí-lo pelo novo `received_at`;
- se ainda não houver instante armazenado, usa `reported_at` somente quando ele não for `null` e vier de relógio absoluto confiável; caso contrário, registra o instante como desconhecido e o dashboard mostra “idade desconhecida”;
- só uma publicação entregue como não retida atualiza a última comunicação com seu `received_at`.

Assim, a mensagem retida continua útil para reconstruir o último estado e a presença conhecida, mas não comprova atividade nova do dispositivo.

A implementação da #122 ainda publica as strings simples `online` e `offline`. A troca pelos objetos JSON acima é uma alteração necessária para cumprir o exemplo por tópico e deve ser feita junto da implementação do contrato.

## 8. Tópicos `cmd` e `cmd/resposta`

- **Pedido:** `guaratracking/{device_id}/cmd`
- **Resposta:** `guaratracking/{device_id}/cmd/resposta`
- **Retidos:** não

Todo pedido contém `command_id` único, instante do pedido e validade. O dispositivo publica exatamente uma resposta final por `command_id`: `aceito`, `recusado` ou `erro`. O resultado final deve ser preservado para que toda repetição do mesmo `command_id`, inclusive após reconexão, devolva novamente a resposta original, com o mesmo `status`, `motivo`, `responded_at` e `estado`, sem executar nem validar a ação outra vez. A repetição nunca recebe uma nova recusa por duplicidade. Se o mesmo `command_id` reaparecer com conteúdo diferente, a resposta original ainda é republicada e a inconsistência deve ser registrada para auditoria.

Os comandos que alteram um ciclo já existente (`REMESSA`, `NF_RETORNO`, `AUTORIZAR` e `ENCERRAR`) também contêm `ciclo_id` no pedido. Antes de executar, o dispositivo compara esse valor com o ciclo ativo: se não forem iguais, responde `recusado` com `motivo = ciclo_divergente` e não altera estado algum. Isso impede que um pedido produzido por uma tela desatualizada seja aplicado ao ciclo seguinte e atende ao RNF06.3. `VINCULAR` não recebe `ciclo_id`, pois cria um novo ciclo em vez de alterar um ciclo já selecionado.

### 8.1. Abrir ciclo — `VINCULAR`

Pedido:

```json
{
  "schema_version": "1.0",
  "command_id": "cmd-20261113-001",
  "device_id": "prototipo-001",
  "cmd": "VINCULAR",
  "requested_at": "2026-11-13T14:31:00Z",
  "expires_at": "2026-11-13T14:32:00Z",
  "params": {}
}
```

Resposta:

```json
{
  "schema_version": "1.0",
  "command_id": "cmd-20261113-001",
  "device_id": "prototipo-001",
  "cmd": "VINCULAR",
  "status": "aceito",
  "responded_at": "2026-11-13T14:31:02Z",
  "motivo": null,
  "estado": {
    "ciclo_id": 13,
    "ciclo_ativo": true
  }
}
```

Recusa prevista: `ciclo_ativo`.

### 8.2. Registrar NF de remessa — `REMESSA`

Pedido:

```json
{
  "schema_version": "1.0",
  "command_id": "cmd-20261113-002",
  "device_id": "prototipo-001",
  "cmd": "REMESSA",
  "ciclo_id": 13,
  "requested_at": "2026-11-13T14:32:00Z",
  "expires_at": "2026-11-13T14:33:00Z",
  "params": {
    "nf": "NF-000123"
  }
}
```

Resposta:

```json
{
  "schema_version": "1.0",
  "command_id": "cmd-20261113-002",
  "device_id": "prototipo-001",
  "cmd": "REMESSA",
  "status": "aceito",
  "responded_at": "2026-11-13T14:32:01Z",
  "motivo": null,
  "estado": {
    "ciclo_id": 13,
    "nf": "NF-000123"
  }
}
```

Recusas previstas: `sem_ciclo_ativo`, `ciclo_divergente` e `nf_invalida`. A referência aceita até 15 caracteres entre letras, números, hífen, ponto e barra, conforme a regra já testada no firmware.

### 8.3. Registrar NF de retorno — `NF_RETORNO`

Pedido:

```json
{
  "schema_version": "1.0",
  "command_id": "cmd-20261113-003",
  "device_id": "prototipo-001",
  "cmd": "NF_RETORNO",
  "ciclo_id": 13,
  "requested_at": "2026-11-13T14:33:00Z",
  "expires_at": "2026-11-13T14:34:00Z",
  "params": {
    "nf": "NF-RET-000123"
  }
}
```

Resposta:

```json
{
  "schema_version": "1.0",
  "command_id": "cmd-20261113-003",
  "device_id": "prototipo-001",
  "cmd": "NF_RETORNO",
  "status": "aceito",
  "responded_at": "2026-11-13T14:33:01Z",
  "motivo": null,
  "estado": {
    "ciclo_id": 13,
    "nf_retorno_registrada": true
  }
}
```

Recusas previstas: `sem_ciclo_ativo`, `ciclo_divergente` e `nf_invalida`. O firmware atual registra apenas a presença da NF de retorno; armazenar sua referência ainda precisa ser confirmado na implementação.

### 8.4. Autorizar retirada — `AUTORIZAR`

Pedido:

```json
{
  "schema_version": "1.0",
  "command_id": "cmd-20261113-004",
  "device_id": "prototipo-001",
  "cmd": "AUTORIZAR",
  "ciclo_id": 13,
  "requested_at": "2026-11-13T14:34:00Z",
  "expires_at": "2026-11-13T14:35:00Z",
  "params": {}
}
```

Resposta:

```json
{
  "schema_version": "1.0",
  "command_id": "cmd-20261113-004",
  "device_id": "prototipo-001",
  "cmd": "AUTORIZAR",
  "status": "aceito",
  "responded_at": "2026-11-13T14:34:01Z",
  "motivo": null,
  "estado": {
    "ciclo_id": 13,
    "janela_dias": 3
  }
}
```

Recusas previstas: `sem_ciclo_ativo` e `ciclo_divergente`. A janela usa a configuração do firmware, atualmente 3 dias. Alterá-la pelo dashboard não faz parte desta versão.

### 8.5. Encerrar ciclo — `ENCERRAR`

Pedido:

```json
{
  "schema_version": "1.0",
  "command_id": "cmd-20261113-005",
  "device_id": "prototipo-001",
  "cmd": "ENCERRAR",
  "ciclo_id": 13,
  "requested_at": "2026-11-13T14:35:00Z",
  "expires_at": "2026-11-13T14:36:00Z",
  "params": {}
}
```

Resposta de recusa:

```json
{
  "schema_version": "1.0",
  "command_id": "cmd-20261113-005",
  "device_id": "prototipo-001",
  "cmd": "ENCERRAR",
  "status": "recusado",
  "responded_at": "2026-11-13T14:35:01Z",
  "motivo": "nf_retorno_ausente",
  "estado": {
    "ciclo_id": 13,
    "ciclo_ativo": true
  }
}
```

Recusas previstas: `sem_ciclo_ativo`, `ciclo_divergente`, `lacre_aberto` e `nf_retorno_ausente`.

### Estados da interface de comandos

| Estado visual | Origem |
|---|---|
| Solicitado | Dashboard publicou o pedido em `cmd` |
| Aguardando resposta | Ainda não chegou mensagem correspondente em `cmd/resposta` |
| Confirmado | Resposta com `status = aceito` |
| Recusado | Resposta com `status = recusado` e `motivo` conhecido |
| Erro | Resposta com `status = erro` |
| Timeout | `expires_at` passou sem resposta; é estado calculado pelo dashboard/backend |

Reconhecer ou resolver um alerta não é comando MQTT nesta versão. Essa ação altera o registro administrativo do backend e não apaga o evento físico do dispositivo.

## 9. Mapeamento das telas

| Tela ou bloco | Dados MQTT | Dados do backend ou cálculo |
|---|---|---|
| Lacre e alerta de violação | `status.lacre`; evento `VIOLACAO_LACRE` | Situação `aberto`, `reconhecido` ou `resolvido`; responsável pelo tratamento |
| Histórico de eventos | `evento` com `event_id`, `type` e hora de ocorrência | `received_at`, persistência, ordenação e filtros |
| Bateria | `status.bateria_pct`; eventos `BATERIA_BAIXA` e `SENSOR_BATERIA_FALHA` | Limiares de apresentação e histórico |
| Ciclo e NF | `status.ciclo_ativo`, `ciclo_id`, `nf` e `dias_fora` | Equipamento, fornecedor, prazo acordado, retorno físico e situação fiscal |
| Posição com idade | `status.posicao` e `captured_at` | Idade calculada; mapa; aviso de posição ausente ou antiga |
| Online e fila | `online.state`; `status.fila` | Última comunicação calculada apenas com publicação não retida; entrega retida preserva o instante conhecido ou produz idade desconhecida |
| Formulário de comandos | Publicação em `cmd` e retorno em `cmd/resposta` | Permissão do usuário, confirmação da ação, timeout e auditoria |
| Indicadores da visão geral | Mensagens dos dispositivos | Contagens e agregações produzidas pelo backend |

## 10. Compatibilidade e implementação pendente

| Item | Situação integrada até #123 | Necessário para cumprir este contrato |
|---|---|---|
| `status` | Publicado e retido com campos resumidos | Acrescentar schema, tempo, ciclo, posição e detalhamento da fila sem ultrapassar o buffer |
| `online` | Strings `online` e `offline` retidas | Trocar por JSON; manter Last Will retido |
| `cmd` | Assinado, mas apenas registrado na Serial | Validar JSON, validade e `ciclo_id`; preservar e republicar o resultado original de `command_id` repetido; executar comandos permitidos |
| `evento` | Publicado no tópico; falhas ficam na fila local e são reenviadas na ordem | Adequar o payload ao schema, incluindo identidade e tempos previstos neste contrato |
| `cmd/resposta` | Não implementado | Publicar uma resposta final correlacionada por `command_id` |
| Dashboard | Mock visual estático | Consumir WebSocket e confirmar os campos deste contrato |

Mudanças incompatíveis depois da aprovação desta versão devem atualizar `schema_version`, este documento, os exemplos e os testes do dashboard e do firmware.

## 11. Checklist de aceite

- [x] Broker escolhido: EMQX.
- [x] WebSocket local comprovado nos dois sentidos.
- [x] Cinco tópicos definidos.
- [x] Um exemplo JSON para cada tópico.
- [x] Formato de pedido e resposta para cada comando desta versão.
- [x] Telas mapeadas para MQTT, backend e cálculos.
- [ ] Responsável pelo dashboard confirmou que consegue consumir o contrato.
- [ ] Revisor aprovou a versão 1.
