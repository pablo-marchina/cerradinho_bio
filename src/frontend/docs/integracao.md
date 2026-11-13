# Contrato provisório e próxima integração

A interface depende de `services/api.ts`, `services/types.ts` e `useSnapshot.ts`. A API HTTP abaixo existe **apenas como interceptação MSW dentro do browser**, não como serviço em rede. Não é um contrato MQTT acordado com o firmware.

| Método e rota | Entrada | Resultado |
|---|---|---|
| GET `/api/demo/snapshot` | — | Snapshot (asOf, assets, alerts, events, commands, archived) |
| POST `/api/demo/assets` | id, name, tag, description | ativo cadastrado, sem ciclo |
| POST `/api/demo/commands` | assetId, type, payload, scenario | HTTP 202 e registro correlacionado; consultar snapshot até terminal |
| PATCH `/api/demo/alerts/:id` | status, reason | alerta atualizado com ação registrada |
| POST `/api/demo/scenario` | id, scenario | estado sintético após estímulo |
| POST `/api/demo/reset` | — | ok; restaura cenário |

Falhas de validação retornam mensagem e status 409 no simulador. `created`, `published`, `received`, `executed`, `refused`, `timeout`, `expired` são fases fictícias, não confirmação do broker. A autorização e o encerramento têm confirmação explícita e TTL. O histórico associa eventos à TAG e ao ciclo. O relógio simulado avança nas operações, não no hardware. A autorização de retirada usa por padrão 4320 minutos (3 dias), parâmetro de teste da RN06 ainda pendente de homologação. A reaplicação requer o estímulo de fechamento do lacre antes do comando; o encerramento continua exigindo retorno físico, NF de retorno e lacre íntegro.

Para integrar: 1. acordar DTOs e mapeamento com firmware (campos de origem/tempo, qualidade GNSS, fila, ciclo, estados); 2. substituir o cliente HTTP pelo backend autenticado; 3. retirar a inicialização MSW do ambiente real; 4. processar e persistir eventos antes de ACK de aplicação, deduplicar e respeitar expiração de comandos; 5. substituir polling por mecanismo apropriado se necessário; 6. validar permissões, horários, latências, UX, falhas de rede e ensaios físicos. O modo demo deve continuar isolado dos dados reais. Nenhum segredo deve estar no bundle web.

## Evidências para 3.4.3

Capturar telas após instalar e executar: visão geral, violação, posição preservada sem fix, fila após perda de conexão, retorno fiscal pendente, resposta executada/recusada/timeout. Legendar como **interface de demonstração com dados fictícios**. Quando MQTT real existir, repetir com ESP32, broker e logs reais; registrar estímulo, resultado observado, horário e evidência. As imagens do frontend simulado não comprovam pub/sub, LWT ou hardware.
