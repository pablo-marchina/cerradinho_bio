# Issue #138: testes locais do EMQX no Docker

> **Pergunta investigada:** o broker local EMQX entrega mensagens nos dois sentidos entre o MQTT TCP usado pelo dispositivo e o WebSocket previsto para o dashboard?
> **O que foi feito:** o EMQX 5.8.9 foi iniciado pelo `docker compose`, as portas e o painel foram verificados e um teste automatizado publicou `status` de MQTT TCP para WebSocket e `cmd` de WebSocket para MQTT TCP.
> **Evidência:** em 13/11/2026, o contêiner permaneceu ativo, as portas 1883 e 8083 aceitaram conexão, o painel respondeu HTTP 200 e `node scripts/test-emqx-local.cjs` confirmou a entrega correta nos dois sentidos. O script versionado reproduz o ensaio com tópicos temporários e payloads fictícios.
> **Em aberto:** o ensaio não usa o ESP32 físico, não verifica acesso por outro equipamento através do Firewall, não testa o EMQX Cloud com WSS e não implementa `evento` ou `cmd/resposta`.
> **Alimenta:** **3.4.3 Contrato MQTT e dashboard** (Artefato 8) e o critério de aceitação do card de contrato que exige WebSocket para o dashboard.
> **Implicação:** o dashboard pode trocar mensagens com dispositivos MQTT pelo broker local usando `ws://IP-do-notebook:8083/mqtt`; portanto, o contrato pode adotar esse transporte no desenvolvimento, mantendo separados os testes futuros do ESP32 e do broker online.

Card: #138. Autora: Rachel Durante Silvestre. Branch: `code/#138/sprint3/testes-locais-docker`.

## Ambiente medido

| Item | Resultado |
|---|---|
| Data | 13/11/2026 |
| Sistema | Windows com Docker Desktop |
| Broker | EMQX 5.8.9, imagem fixa `emqx/emqx:5.8.9` |
| Contêiner | `emqx-emqx-1`, ativo durante o ensaio |
| MQTT TCP | `127.0.0.1:1883`, conexão aceita |
| MQTT por WebSocket | `ws://127.0.0.1:8083/mqtt`, conexão e assinatura aceitas |
| Painel | `http://127.0.0.1:18083`, HTTP 200 |
| Cliente de teste | Node.js v24.13.0, sem pacote externo |

## Fluxos verificados

| Sentido | Publicação | Assinatura | Resultado medido |
|---|---|---|---|
| Dispositivo simulado → dashboard | MQTT TCP, tópico temporário terminado em `/status` | MQTT por WebSocket | Payload JSON recebido sem alteração |
| Dashboard → dispositivo simulado | MQTT por WebSocket, tópico temporário terminado em `/cmd` | MQTT TCP | Payload JSON recebido sem alteração |

O teste cria um prefixo exclusivo `guaratracking/teste-dashboard-<processo>-<instante>/` em cada execução. As mensagens usam QoS 0, não são retidas e não contêm credenciais nem dados reais.

## Como reproduzir

Na raiz do repositório, com Docker Desktop e Node.js 22 ou superior:

```powershell
docker compose -f docker/emqx/docker-compose.yml up -d
node scripts/test-emqx-local.cjs
```

Saída esperada:

```text
PASS: MQTT TCP -> WebSocket entregou status corretamente
PASS: WebSocket -> MQTT TCP entregou comando corretamente
PASS: broker local validado em 127.0.0.1:1883 e ws://127.0.0.1:8083/mqtt
```

Para encerrar o ambiente local:

```powershell
docker compose -f docker/emqx/docker-compose.yml down
```

## Limites

- O teste comprova o roteamento do broker e o caminho WebSocket local, não o firmware nem o protótipo físico.
- O acesso usa loopback. Outro computador ou o ESP32 ainda dependem do IP da rede e das regras do Firewall do Windows.
- O broker local é anônimo e sem TLS; deve ser usado somente com dados fictícios em rede confiável.
- O EMQX Cloud usa TLS/WSS e credenciais próprias e precisa de verificação separada.
- `evento`, `cmd/resposta` e a execução dos comandos permanecem nas fatias posteriores.
