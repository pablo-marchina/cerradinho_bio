# Changelog — Guarátracking frontend

Consolidação cumulativa a partir do ZIP v0.1. Os números abaixo representam os incrementos do desenvolvimento; não são releases GitLab publicadas. O pacote entregue é o código da v1.0 de demonstração, com validação de integração ainda pendente.

## 1.0.0 — Candidato a release de frontend
- Correções das regras simuladas: reaplicação exige fechamento prévio do lacre; silêncio começa somente após dois intervalos completos e pode gerar nova ocorrência após recuperação da comunicação.
- Padrão de autorização alinhado à RN06: 4320 minutos (3 dias), ainda pendente de homologação do parceiro.
- Cadastro sem ciclo excluído do filtro de ciclos encerrados.
- Sete módulos de navegação e detalhe por equipamento; nada depende de servidor de aplicação.
- Documentação de execução, matriz de cobertura, cenários de demonstração e plano de integração.
- 37 testes de domínio Node e 5 testes Vitest aprovados em 14/11/2026; build Vite aprovado com aviso de tamanho do bundle. Os 8 roteiros Playwright cobrem desktop e mobile.
- Revisão do MR !83: silêncio referenciado à abertura do ciclo até o primeiro contato; transições GNSS sem duplicação e com motivo, duração e coletas sem fix; bateria na listagem; gráfico de alertas por tipo com dados textuais; pré-vencimento RN19 desabilitado até aprovação de parâmetro por ciclo.
- Revisão da identificação de dados fictícios, datas em Brasília, transições de comandos, vínculo e arquivo de ciclos.

## 0.5 — Qualidade e acessibilidade
- Filtros, paginação e ordenação de tabela; navegação por teclado e foco visível; formulários com rótulos e feedback.
- Estados de falha, recusa, expiração, dados desatualizados e recuperação; aviso de reset da sessão.
- Roteiros automatizados para os fluxos e layout responsivo; conformidade WCAG não certificada.

## 0.4 — Mapa e relatórios
- MapLibre: marcadores de últimas posições válidas, seleção, zoom e filtros GNSS, lista acessível, base OSM opcional e atribuição. Sem inferência de trajetória.
- Recharts: distribuição por fornecedor, atrasos, permanência e contagem de alertas; dados textuais equivalentes e exportação CSV simulada.

## 0.3 — Cadastro, ciclos e alertas
- Formulário de cadastro de equipamento fictício; abertura com NF, emissão, responsável, fornecedor, TAG, prazo e limiar.
- Retorno físico, NF de retorno, reaplicação e fechamento separados; recusa quando faltam pré-condições; proteção contra vínculos simultâneos no simulador.
- Arquivo de ciclos e posições; trilha de tratamento dos alertas, com ciência, tratamento e resolução justificados.

## 0.2 — Telemetria e central de comandos
- BatteryMeter, ConnectivityStatus, QueueIndicator, LocationPanel, Telemetry, CommandForm e ScenarioPanel.
- Comandos correlacionados, estados de publicação/recepção/execução simulados, recusa, timeout e expiração; confirmação explícita para operações sensíveis.
- Cenários para lacre, bateria com histerese, GNSS, LWT, fila, atraso de evento e avanço de relógio; regras puras em `services/simulator.ts`.

## 0.1 — Fundação
- AppShell, sidebar, cabeçalho, cards de KPI, tabela de prioridades, status, timeline e mock API MSW.
- React, TypeScript, Vite, Tailwind, React Router, TanStack Query/Table e design system original.
