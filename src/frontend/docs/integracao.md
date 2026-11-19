# Integração MQTT do dashboard

O frontend mantém duas fontes de dados explicitamente separadas:

- `demo`: API HTTP interceptada pelo MSW, com dados sintéticos e operações locais;
- `mqtt`: conexão WebSocket com o broker, telemetria recebida nos tópicos do dispositivo e comandos publicados para o firmware.

O modo é selecionado por `VITE_DATA_SOURCE`. O arquivo `.env.mqtt.example` contém a configuração local sem credenciais.

## Contrato vigente

A integração física da Sprint 3 usa o protocolo compacto implementado pelo firmware: pedidos com `id` e `acao`, e respostas com `id`, `acao`, `ok` e `erro`. O formato expandido com `command_id`, `ciclo_id`, `status`, `motivo` e `estado` permanece como alvo futuro e não é usado para alegar o RNF06.3 nesta versão. O adaptador continua capaz de ler alguns campos expandidos para facilitar uma migração posterior, mas não os publica como contrato atual.

## Fluxo MQTT

Para o dispositivo configurado em `VITE_MQTT_DEVICE_ID`, o dashboard:

| Operação | Tópico |
|---|---|
| Assina estado retido | `guaratracking/{device_id}/status` |
| Assina ocorrências | `guaratracking/{device_id}/evento` |
| Assina presença retida | `guaratracking/{device_id}/online` |
| Publica comando identificado | `guaratracking/{device_id}/cmd` |
| Assina o resultado correlacionado | `guaratracking/{device_id}/cmd/resposta` |

O adaptador aceita o formato compacto implementado pelo firmware e o formato expandido previsto no contrato. Em particular:

- `online` e `offline` podem chegar como texto simples ou objeto JSON;
- o adaptador está preparado para posição em `status.posicao` ou nos eventos `location_sample` e `location_unavailable`, mas o firmware atual ainda registra essas amostras somente na Serial/NVS e não as publica no MQTT;
- bateria pode usar `bateria_pct`, `battery_pct` ou `battery_sim_pct`;
- lacre aceita os valores em português publicados no status e os valores em inglês dos eventos;
- uma entrega retida não é contabilizada como contato novo do dispositivo;
- `cmd/resposta` é correlacionado por `id` e uma resposta repetida não cria outro comando.

## Comandos expostos

O formulário MQTT apresenta somente ações implementadas no firmware:

| Ação da interface | JSON publicado |
|---|---|
| Abrir ciclo | `{"id":"...","acao":"ciclo_iniciar"}` |
| Registrar NF de remessa | `{"id":"...","acao":"remessa","valor":"NF-000123"}` |
| Autorizar retirada | `{"id":"...","acao":"autorizar"}` |
| Registrar NF de retorno | `{"id":"...","acao":"nf_retorno"}` |
| Encerrar ciclo | `{"id":"...","acao":"ciclo_encerrar"}` |

O estado visual só muda para **Executado pelo simulador** ou **Executado pelo ESP32**, conforme `VITE_MQTT_SOURCE`, quando chega `cmd/resposta` com o mesmo `id` e resultado positivo. Recusa mostra o código retornado. Após 10 segundos sem resposta, o dashboard mostra **Sem resposta** e orienta consultar status e eventos antes de repetir uma ação que altere estado.

## Simulador e hardware

`npm run simulate:mqtt` executa um cliente MQTT TCP com os mesmos tópicos e o formato compacto do firmware. Ele publica status, violação e uma posição periódica no formato proposto, além de responder aos comandos. A posição é uma antecipação explícita da integração ainda ausente no firmware, não uma reprodução de comportamento já comprovado. O simulador usa apenas dados fictícios e serve para desenvolver a interface sem a placa.

O teste `e2e/mqtt.spec.ts` comprova navegador → WebSocket → broker → simulador → broker → navegador. Execute-o com EMQX e simulador ativos:

```powershell
$env:MQTT_E2E='1'
$env:VITE_DATA_SOURCE='mqtt'
$env:VITE_MQTT_URL='ws://127.0.0.1:8083/mqtt'
$env:VITE_MQTT_DEVICE_ID='prototipo-001'
$env:VITE_MQTT_SOURCE='simulator'
npx playwright test e2e/mqtt.spec.ts --project=desktop
```

Esse ensaio automatizado não comprova o ESP32 físico. A rodada física de 18/11/2026 encerrou o simulador, conectou o protótipo `prototipo-001-rachel` ao mesmo EMQX Cloud e comprovou `status`, bateria por potenciômetro, comandos aceitos e recusados, `cmd/resposta`, Last Will e reconexão. O relatório e as capturas estão em [`issue-131-integracao-dashboard-fisica.md`](../../../document/outros/reports/Sprint03/issue-131-integracao-dashboard-fisica.md).

O ensaio físico não cobriu a abertura do lacre, o GNSS por MQTT nem todos os casos negativos do CT84. Essas partes permanecem explicitamente pendentes e não são inferidas a partir do aceite dos comandos.

## Limites

- O estado MQTT permanece apenas na memória do navegador; recarregar a página perde eventos e respostas não retidos.
- Cadastro, fornecedor, responsável, prazo, tratamento persistente de alertas e histórico entre sessões dependem de backend.
- Usuário e senha em variáveis `VITE_*` ficam visíveis no bundle. O modo atual é destinado ao broker local de demonstração sem credenciais; um broker externo exige backend ou credencial temporária de privilégio mínimo.
- O dashboard preserva a última coordenada recebida, mas não inventa horário absoluto quando o firmware publica apenas `uptime_ms`.
- O modo MQTT não transforma dados do simulador em evidência de hardware; somente a rodada física registrada no relatório da #131 sustenta as funcionalidades nela observadas.
