# Rastreabilidade dos 72 requisitos — recorte frontend v1.0

**Simulado** = representação em código e dados sintéticos; não significa aceite do requisito completo. **Integrado com simulador MQTT** = publicação/assinatura real no broker e resposta correlacionada sem placa. **Integrado com ESP32 físico** = comportamento observado na rodada física documentada da #131; não amplia a evidência para casos não executados. **Parcial** = existe suporte visual ou modelo incompleto. **Fora/pendente** = não implementado no recorte, requer integração ou evolução. Nenhuma linha representa homologação.

**Atualização da integração:** o frontend possui um modo MQTT separado do MSW. Além do teste automatizado com simulador, a rodada física de 18/11/2026 comprovou no EMQX Cloud a recepção de `status`, bateria por potenciômetro, presença/LWT, reconexão e o fluxo `cmd`/`cmd/resposta` com aceite e recusa. A posição continua vindo apenas do simulador no formato proposto; o firmware físico ainda não publica coordenadas no broker. A abertura física do lacre também não foi executada nessa rodada.

| ID | Estado | Observação |
|---|---|---|
| DASH-VIS-01 | Simulado | Dashboard, Equipment, PriorityTable, AppShell, filtros e rotas. |
| DASH-VIS-02 | Simulado | Dashboard, Equipment, PriorityTable, AppShell, filtros e rotas. |
| DASH-VIS-03 | Simulado | Dashboard, Equipment, PriorityTable, AppShell, filtros e rotas. |
| DASH-VIS-04 | Simulado | Dashboard, Equipment, PriorityTable, AppShell, filtros e rotas. |
| DASH-VIS-05 | Simulado | Dashboard, Equipment, PriorityTable, AppShell, filtros e rotas. |
| DASH-VIS-06 | Simulado | Dashboard, Equipment, PriorityTable, AppShell, filtros e rotas. |
| DASH-VIS-07 | Parcial | Dashboard, Equipment, PriorityTable, AppShell, filtros e rotas. |
| DASH-CIC-01 | Simulado | RegisterAsset, Cycles, CommandForm e simulator.ts; cadastro de fornecedor/responsável por texto, sem catálogo homologado. |
| DASH-CIC-02 | Parcial | RegisterAsset, Cycles, CommandForm e simulator.ts; cadastro de fornecedor/responsável por texto, sem catálogo homologado. |
| DASH-CIC-03 | Simulado | RegisterAsset, Cycles, CommandForm e simulator.ts; cadastro de fornecedor/responsável por texto, sem catálogo homologado. |
| DASH-CIC-04 | Simulado | RegisterAsset, Cycles, CommandForm e simulator.ts; cadastro de fornecedor/responsável por texto, sem catálogo homologado. |
| DASH-CIC-05 | Simulado | RegisterAsset, Cycles, CommandForm e simulator.ts; cadastro de fornecedor/responsável por texto, sem catálogo homologado. |
| DASH-CIC-06 | Simulado | RegisterAsset, Cycles, CommandForm e simulator.ts; cadastro de fornecedor/responsável por texto, sem catálogo homologado. |
| DASH-CIC-07 | Simulado | RegisterAsset, Cycles, CommandForm e simulator.ts; cadastro de fornecedor/responsável por texto, sem catálogo homologado. |
| DASH-CIC-08 | Simulado | RegisterAsset, Cycles, CommandForm e simulator.ts; cadastro de fornecedor/responsável por texto, sem catálogo homologado. |
| DASH-CIC-09 | Simulado | RegisterAsset, Cycles, CommandForm e simulator.ts; cadastro de fornecedor/responsável por texto, sem catálogo homologado. |
| DASH-CIC-10 | Fora/pendente | RegisterAsset, Cycles, CommandForm e simulator.ts; cadastro de fornecedor/responsável por texto, sem catálogo homologado. |
| DASH-CIC-11 | Fora/pendente | RegisterAsset, Cycles, CommandForm e simulator.ts; cadastro de fornecedor/responsável por texto, sem catálogo homologado. |
| DASH-TEL-01 | Simulado | Telemetry, ScenarioPanel, simulator.ts; confirmação de fila/estado é só simulação, sem qualidade GNSS medida. |
| DASH-TEL-02 | Simulado | Telemetry, ScenarioPanel, simulator.ts; confirmação de fila/estado é só simulação, sem qualidade GNSS medida. |
| DASH-TEL-03 | Integrado com ESP32 físico | Potenciômetro no GPIO34 alterou a bateria no LCD, no `status` do broker e no dashboard; sequência de alerta/rearme 20%/25% executada na rodada física. |
| DASH-TEL-04 | Simulado | Telemetry, ScenarioPanel, simulator.ts; confirmação de fila/estado é só simulação, sem qualidade GNSS medida. |
| DASH-TEL-05 | Simulado | Telemetry, ScenarioPanel, simulator.ts; confirmação de fila/estado é só simulação, sem qualidade GNSS medida. |
| DASH-TEL-06 | Simulado | Telemetry, ScenarioPanel, simulator.ts; confirmação de fila/estado é só simulação, sem qualidade GNSS medida. |
| DASH-TEL-07 | Parcial | Telemetry, ScenarioPanel, simulator.ts; confirmação de fila/estado é só simulação, sem qualidade GNSS medida. |
| DASH-TEL-08 | Integrado com simulador MQTT | A interface recebe coordenadas no formato proposto e preserva horário desconhecido; o firmware físico ainda não publica posição no MQTT. |
| DASH-TEL-09 | Parcial | Última comunicação e última posição válida são exibidas separadamente, sem inventar horário de coleta. RN08 ainda não está validada: faltam publicação física, HDOP medido e data absoluta do GNSS. |
| DASH-TEL-10 | Simulado | Telemetry, ScenarioPanel, simulator.ts; confirmação de fila/estado é só simulação, sem qualidade GNSS medida. |
| DASH-TEL-11 | Fora/pendente | Telemetry, ScenarioPanel, simulator.ts; confirmação de fila/estado é só simulação, sem qualidade GNSS medida. |
| DASH-EVT-01 | Simulado | Alerts, EventTimeline, simulator.ts; sem deduplicação real, escalonamento, regras fiscais especiais ou SLA homologado. |
| DASH-EVT-02 | Parcial | Alerts, EventTimeline, simulator.ts; sem deduplicação real, escalonamento, regras fiscais especiais ou SLA homologado. |
| DASH-EVT-03 | Parcial | Alerts, EventTimeline, simulator.ts; pré-vencimento RN19 permanece desabilitado até validação de parâmetro por ciclo. Sem escalonamento, regras fiscais especiais ou SLA homologado. |
| DASH-EVT-04 | Simulado | Alerts, EventTimeline, simulator.ts; sem deduplicação real, escalonamento, regras fiscais especiais ou SLA homologado. |
| DASH-EVT-05 | Fora/pendente | Alerts, EventTimeline, simulator.ts; sem deduplicação real, escalonamento, regras fiscais especiais ou SLA homologado. |
| DASH-EVT-06 | Parcial | Alerts, EventTimeline, simulator.ts; sem deduplicação real, escalonamento, regras fiscais especiais ou SLA homologado. |
| DASH-EVT-07 | Parcial | Alerts, EventTimeline, simulator.ts; sem deduplicação real, escalonamento, regras fiscais especiais ou SLA homologado. |
| DASH-EVT-08 | Fora/pendente | Alerts, EventTimeline, simulator.ts; sem deduplicação real, escalonamento, regras fiscais especiais ou SLA homologado. |
| DASH-CMD-01 | Integrado com ESP32 físico | O formulário publicou abrir ciclo, NFs, autorização e encerramento para o firmware físico. |
| DASH-CMD-02 | Integrado com ESP32 físico | O alvo `prototipo-001-rachel` e o prefixo contratado foram conferidos no EMQX Cloud e no MQTTX. |
| DASH-CMD-03 | Parcial | CommandForm, api.ts, MSW e simulator.ts; contrato provisório, não compatibilidade comprovada com firmware; sem publicação real. |
| DASH-CMD-04 | Parcial | CommandForm, api.ts, MSW e simulator.ts; contrato provisório, não compatibilidade comprovada com firmware; sem publicação real. |
| DASH-CMD-05 | Integrado com ESP32 físico | A interface diferenciou publicação, execução confirmada e recusa por falta de NF de retorno; timeout permanece coberto pelo teste automatizado. |
| DASH-CMD-06 | Integrado com ESP32 físico | Respostas positivas e negativa do ESP32 atualizaram o comando correlacionado por `id`. |
| DASH-CMD-07 | Simulado | CommandForm, api.ts, MSW e simulator.ts; contrato provisório, não compatibilidade comprovada com firmware; sem publicação real. |
| DASH-CMD-08 | Simulado | CommandForm, api.ts, MSW e simulator.ts; contrato provisório, não compatibilidade comprovada com firmware; sem publicação real. |
| DASH-CMD-09 | Simulado | CommandForm, api.ts, MSW e simulator.ts; contrato provisório, não compatibilidade comprovada com firmware; sem publicação real. |
| DASH-CMD-10 | Parcial | CommandForm, api.ts, MSW e simulator.ts; contrato provisório, não compatibilidade comprovada com firmware; sem publicação real. |
| DASH-CMD-11 | Parcial | CommandForm, api.ts, MSW e simulator.ts; contrato provisório, não compatibilidade comprovada com firmware; sem publicação real. |
| DASH-HIS-01 | Simulado | Positions, EquipmentDetail, arquivo de ciclo e histórico de posições; sem rota inferida. |
| DASH-HIS-02 | Simulado | Positions, EquipmentDetail, arquivo de ciclo e histórico de posições; sem rota inferida. |
| DASH-HIS-03 | Simulado | Positions, EquipmentDetail, arquivo de ciclo e histórico de posições; sem rota inferida. |
| DASH-HIS-04 | Parcial | Positions, EquipmentDetail, arquivo de ciclo e histórico de posições; sem rota inferida. |
| DASH-HIS-05 | Parcial | Positions, EquipmentDetail, arquivo de ciclo e histórico de posições; sem rota inferida. |
| DASH-API-01 | Parcial | Tipos e API interceptada MSW; nenhum backend, autenticação real, banco ou controle de acesso. |
| DASH-API-02 | Fora/pendente | Tipos e API interceptada MSW; nenhum backend, autenticação real, banco ou controle de acesso. |
| DASH-API-03 | Parcial | Tipos e API interceptada MSW; nenhum backend, autenticação real, banco ou controle de acesso. |
| DASH-API-04 | Fora/pendente | Tipos e API interceptada MSW; nenhum backend, autenticação real, banco ou controle de acesso. |
| DASH-API-05 | Fora/pendente | Tipos e API interceptada MSW; nenhum backend, autenticação real, banco ou controle de acesso. |
| DASH-API-06 | Parcial | Tipos e API interceptada MSW; nenhum backend, autenticação real, banco ou controle de acesso. |
| DASH-API-07 | Fora/pendente | Tipos e API interceptada MSW; nenhum backend, autenticação real, banco ou controle de acesso. |
| DASH-API-08 | Fora/pendente | Tipos e API interceptada MSW; nenhum backend, autenticação real, banco ou controle de acesso. |
| DASH-API-09 | Parcial | Tipos e API interceptada MSW; nenhum backend, autenticação real, banco ou controle de acesso. |
| DASH-API-10 | Parcial | Tipos e API interceptada MSW; nenhum backend, autenticação real, banco ou controle de acesso. |
| DASH-UX-01 | Parcial | CSS, tokens, DataState, testes Node; navegador e validações com usuários/hardware pendentes. |
| DASH-UX-02 | Simulado | CSS, tokens, DataState, testes Node; navegador e validações com usuários/hardware pendentes. |
| DASH-UX-03 | Parcial | CSS, tokens, DataState, testes Node; navegador e validações com usuários/hardware pendentes. |
| DASH-UX-04 | Simulado | CSS, tokens, DataState, testes Node; navegador e validações com usuários/hardware pendentes. |
| DASH-UX-05 | Fora/pendente | CSS, tokens, DataState, testes Node; navegador e validações com usuários/hardware pendentes. |
| DASH-UX-06 | Simulado | CSS, tokens, DataState, testes Node; navegador e validações com usuários/hardware pendentes. |
| DASH-UX-07 | Fora/pendente | CSS, tokens, DataState, testes Node; navegador e validações com usuários/hardware pendentes. |
| DASH-UX-08 | Fora/pendente | CSS, tokens, DataState, testes Node; navegador e validações com usuários/hardware pendentes. |
| DASH-UX-09 | Parcial | CSS, tokens, DataState, testes Node; navegador e validações com usuários/hardware pendentes. |

Permanecem pendentes de execução/homologação: testes de usabilidade CT-U01, CT-F24, métricas RNF, permissões, lacre físico na janela de autorização, coordenadas GNSS no MQTT e os demais casos de hardware não registrados no relatório da #131. A integração MQTT com simulador continua sendo evidência apenas de software e broker; a evidência física limita-se aos cenários efetivamente executados em 18/11/2026.

O simulador usa a abertura do ciclo como referência inicial para o alerta de silêncio, sem preencher ficticiamente o campo de último contato. Mensagens de telemetria recebidas atualizam o último contato; a passagem do tempo e a perda de conexão não o atualizam. A cobertura de RN08 continua **parcial** até existir medição de HDOP e validação do horário absoluto do GNSS. A regra de pré-vencimento RN19 exige um parâmetro aprovado para cada ciclo antes de afetar alertas ou prioridades.
