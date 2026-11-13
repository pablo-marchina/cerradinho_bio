# GuaráTracking — Pesquisa de referências para dashboard

> **Escopo da entrega frontend:** as exigências de MQTT, API, autenticação e persistência abaixo permanecem requisitos de produto/integração futura. As telas e comandos iniciais serão simulados; não comprovam a seção 3.4.3.

**Data:** 10/11/2026
**Base interna:** TAPI final Cerradinho, `AGENT.md`, `document/documentacao.md` (seções de personas, jornada, requisitos e RNFs), guia de estilos v2 e auditorias do repositório.
**Base externa:** páginas oficiais e documentação de produtos consultadas em 10/11/2026.
**Aviso:** propostas de design são recomendações; telas de terceiros são referências, não componentes do projeto. Alguns produtos comerciais têm funcionalidades dependentes de planos. Não se presume acesso a contas demo privadas.

## 1. O problema de produto

A Cerradinho precisa acompanhar ativos remetidos a manutenção externa, preservando vínculo com nota fiscal, fornecedor, responsável, data de saída, prazo acordado, última localização válida, telemetria, estado do lacre, alertas e histórico do ciclo até o retorno e baixa fiscal. Atualização semanal configurável, não rastreamento contínuo em tempo real. O MVP requer pelo menos cinco equipamentos reais ou representativos, API documentada e consumo demonstrado em BI/cliente de teste.

**Principal persona:** Geraldo (PCM). A documentação do grupo estabelece especificamente que a plataforma deve privilegiar a lista de "meus itens fora" ordenada pelos dias restantes para vencer, com alertas antecipados, em vez de começar por um painel gerencial sem ação. **Segunda:** Clarice (Fiscal), interessada em NF e retorno físico versus fiscal. **Terceira:** Izidio (Logística/BI), interessado em histórico e indicadores.

**Princípio:** homepage *action-first* (o que precisa ser feito hoje?) e mapa como apoio, não homepage *map-first*. No mapa, nunca apresentar posição histórica como localização atual confirmada. Mostrar data/hora de última posição válida e da última comunicação separadamente.

## 2. Referências de produtos reais e o que aproveitar

| Prioridade | Referência | Padrões de UX a observar | Adaptação ao GuaráTracking | Link |
|---|---|---|---|---|
| Alta | Trackunit Manager / Attention List | Alternância eventos versus ativos; eventos críticos abertos, filtros e ações; Asset Home, lista com última comunicação | Caixa 'Atenção necessária', fila de alertas com resolução auditável, página individual do ativo | https://help.trackunit.com/en/articles/302538-how-do-i-work-with-the-attention-list-in-trackunit-manager |
| Alta | Trackunit Map | Lista dinâmica ao lado do mapa; seleção, clusters, filtros e painel de detalhes | Mapa secundário mostrando **última posição válida** e a idade do dado | https://help.trackunit.com/en/articles/236810-using-the-map-in-trackunit-manager |
| Alta | Fleetio Dashboard | KPIs operacionais, 'open/overdue issues', manutenção, filtros, cards com drill-down | Contadores de atraso, próximos vencimentos, lacre violado, ausência de comunicação | https://help.fleetio.com/en_US/dashboard-overview |
| Alta | MaintainX Assets | Página de ativo com Insights, Details, History e status | Páginas de ativo e histórico de ciclos; anexos e timeline | https://help.getmaintainx.com/viewing-assets |
| Alta | Traccar (open source) | Dispositivos + mapa + relatórios + geocercas e REST API | Inspiração técnica para map/list e rastreamento; não atende NF/ciclos/lacre sem extensão | https://github.com/traccar/traccar-web |
| Alta | ThingsBoard | Widgets de telemetria, mapas, entidades e alarmes; estados de dashboard | Protótipo acelerado para sinais MQTT e mapa; construir fluxo fiscal fora do painel genérico | https://thingsboard.io/docs/concepts/data-visualization/ |
| Média | Hilti ON!Track | Controle por ativo, manutenção, responsável, histórico e devoluções | Associações de ativos/dispositivos, fornecedor e retornos | https://www.hilti.com.br/content/hilti/W2/BR/pt/business/business/equipment/on-track.html |
| Média | Samsara | Visão consolidada com ativos por categoria e localização; controles operacionais | Inventário, filtros e mapa; NÃO copiar rastreamento contínuo ou foco em motoristas | https://kb.samsara.com/hc/en-us/articles/48621492984589-Dashboard-Menus |
| Média | Geotab | Grupos, filtros, mapa e relatórios configuráveis | Agrupar por área, fornecedor, status e qualidade do dado | https://support.geotab.com/help/mygeotab/fleet-activity/map/the-map |
| Média | Wialon | Visão de dispositivos, conexões, notificações, geocercas | Painel de saúde de dispositivos e eventos técnicos | https://help.wialon.com/en/wialon-local/2604/user-guide/monitoring-system/dashboard |
| Média | Tenna | Mapa de equipamentos, sites, dashboards e relatórios | Inventário e avaliação de equipamentos distribuídos | https://www.tenna.com/equipment-management-software-products/ |
| Complementar | UpKeep | Work orders por status, vencimento e prioridade | Prioridades e ações de acompanhamento, sem reproduzir CMMS completo | https://help.onupkeep.com/en/articles/2966098-how-to-navigate-mobile-app-home-page |
| Complementar | Grafana Geomap | Mapas, camadas e séries temporais | Console técnico de desenvolvimento; não ideal como produto principal para PCM | https://grafana.com/docs/grafana/latest/visualizations/panels-visualizations/visualizations/geomap/ |

### Capturas ilustrativas úteis

- Trackunit — overview de ativos: https://gdm-catalog-fmapi-prod.imgix.net/ProductScreenshot/158a9d21-7c72-4a5a-9510-cde61a3a8db0.png
- Fleetio — situação de manutenção: https://d33wubrfki0l68.cloudfront.net/c054da48f1846319adb14810a93ed75f1be577c4/d8423/assets/images/manage/maintenance-status.png
- MaintainX — reporting de ordens: https://assets-global.website-files.com/65afd8dc16873da38e2b5ecb/66017379960f138d1b129ab3_2023_04_Reporting-Main.png
- Samsara — visão de ativos: https://images.ctfassets.net/bx9krvy0u3sx/549e4vZgCz0oBhdVl1w6bG/adece17b6c54f86097f732b850c04ce8/left-behind-dashboard-mx.png
- Traccar — map/list: https://selfh.st/content/images/2023/04/traccar-screenshot.png
- ThingsBoard — asset management: https://img.thingsboard.io/iot-articles/asset-management_2_2180x1364.png
- Hilti ON!Track — alerta de ativos sem atualizações: https://help.ontrack3.hilti.com/hc/article_attachments/34401138554129

## 3. Bibliotecas, templates e padrões de interface

| Ferramenta | Uso recomendado | Link |
|---|---|---|
| shadcn/ui `dashboard-01` | Base de layout: sidebar, header, KPI cards, gráfico e tabela; código reutilizável | https://ui.shadcn.com/blocks/dashboard |
| Carbon Design System — Data Table | Guia de busca, ordenação, filtros, expansão, ações e toolbar em tabelas operacionais | https://www.carbondesignsystem.com/building-blocks/core/components/data-table/guidelines |
| TanStack Table | Lógica de filtros, ordenação, paginação, colunas e expansão com React | https://tanstack.com/table/latest |
| MapLibre GL JS | Mapa vetorial interativo com dados GeoJSON; consultar licenças e provedor de tiles | https://maplibre.org/maplibre-gl-js/docs/ |
| Tremor Blocks | Ideias para KPI, barras, tabelas, status monitoring, filtros e estados vazios | https://blocks.tremor.so/blocks |
| Ant Design Pro | Templates prontos de aplicações empresariais, alternativa se equipe preferir um kit completo | https://github.com/ant-design/ant-design-pro |
| ThingsBoard Widgets | Bibliotecas de alarmes, mapas, séries temporais e controles IoT | https://thingsboard.io/docs/user-guide/widgets/ |
| WCAG 2.2 pt-BR | Não codificar estados exclusivamente por cor, contraste e navegação por teclado | https://www.w3.org/Translations/WCAG22-pt-BR/ |
| GOV.UK Design System Tags | Indicadores de status com texto + cor consistente | https://design-system.service.gov.uk/components/tag/ |

**Comparação de caminhos de implementação**
- **Customizado React/TypeScript + shadcn/ui + TanStack Table + MapLibre + gráficos:** melhor adaptação a notas fiscais, retorno físico/baixa fiscal, lacre e controle de prazos; exige construir API, autenticação e persistência.
- **ThingsBoard:** ótimo para demonstrar telemetria MQTT com widgets em pouco tempo; workflows de negócio específicos precisam de extensão ou aplicação própria.
- **Traccar:** ótima referência open source para localização/dispositivos; não substitui o modelo de ciclos fiscais e lacres.
- **Grafana:** apropriado para métricas técnicas; não é a experiência operacional principal do PCM.

## 4. Arquitetura da informação — páginas sugeridas

1. **Visão Geral / Minha atenção (P0):** cards clicáveis de em manutenção, atrasados, perto do vencimento, lacre violado, sem comunicação; tabela de itens que exigem ação priorizada.
2. **Equipamentos (P0):** listagem pesquisável/filtrável; filtros por situação, equipamento, fornecedor, responsável e NF; colunas configuráveis.
3. **Detalhe de equipamento/ciclo (P0):** cadastro, fornecedor, NF saída/retorno, previsto/realizado, status do lacre, bateria, última posição/último contato, ações disponíveis, timeline de eventos e histórico de reutilização.
4. **Alertas (P0):** fila de eventos abertos, ciência, responsável, decisão e justificativa; filtros por severidade, tipo e prazo; trilha de auditoria.
5. **Mapa (P1):** pins de última posição válida com timestamp e qualidade; lista sincronizada, busca e filtros; estado 'sem posição' representado fora do mapa.
6. **Retornos e Fiscal (P1):** diferencia recebimento físico de baixa/nota fiscal de retorno; bloqueia encerramento indevido e preserva pendência.
7. **Relatórios/API (P1):** ciclos por período e fornecedor, contagem de atrasos e permanências longas, exportação e contrato documentado de API/cliente BI.
8. **Dispositivos e Configurações (P2):** inventário de TAGs, bateria/último contato, lacre, vínculo ativo e limiares de alerta; autorização por papéis.

## 5. Homepage recomendada: estrutura e prioridades

- Header: GuaráTracking, contexto 'Meu acompanhamento', pesquisa por equipamento/NF, data da última sincronização da página, usuário.
- Cards com drill-down: **Em manutenção**, **Atrasados**, **Vencem em breve**, **Lacres violados**, **Sem atualização esperada** (números derivados do sistema, não da planilha histórica de saídas).
- **Tabela primária (tamanho visual dominante):** 'Prioridades de hoje / Equipamentos fora'. Ordenada por urgência e data de vencimento; primeiro os eventos de lacre não tratados, depois vencidos, depois próximos do prazo e pendências fiscais. A regra exata de ordenação deve ser acordada com o parceiro.
- Mapa pequeno opcional ou acesso 'Ver no mapa'; nunca fazer do mapa o único meio de localizar um registro.
- Feed de eventos/alertas recentes, com autor, horário, status de entrega e ação.

**Colunas visíveis (mínimo útil):** Prioridade; Equipamento e ID; Fornecedor; Vencimento/dias de atraso; Última posição válida + horário; Lacre; Ação. Em detalhes/colunas opcionais: NF de remessa, responsável, bateria, último contato, dias fora, limiar 150 d, retorno físico, baixa fiscal.

## 6. Estados que nunca devem ser confundidos

- **Prazo previsto:** `no_prazo`, `proximo_vencimento`, `atrasado`.
- **Permanência longa:** condição independente `dias_fora >= limiar_configurado` (150 dias apenas como valor inicial, configurável).
- **Lacre:** `integro`, `retirada_autorizada`, `violado`, `aguardando_reaplicacao`, `indeterminado`.
- **Conectividade:** `ultima_comunicacao_em`, `mensagem_pendente`, `sem_comunicacao_esperada` (não usar online/offline instantâneo como estado primário em um sistema que reporta semanalmente).
- **Localização:** coordenadas válidas, `capturado_em`, `recebido_em`, fonte e precisão/HDOP se disponíveis; `desconhecida` quando não há fix válido.
- **Ciclo:** `cadastrado`, `remetido`, `em_manutencao`, `retorno_fisico_confirmado`, `baixa_fiscal_pendente`, `encerrado`.
- **Alerta:** `aberto`, `reconhecido`, `em_tratamento`, `resolvido`, com responsável, timestamps e justificativa; não sobrescrever histórico.
- **Natureza da evidência:** dado medido, simulado, estimado ou ausente. A interface de demonstração deve sinalizar dados fictícios.

## 7. Identidade visual do GuaráTracking (referência interna)

O guia v2 do projeto recomenda: `#F7F4F1` Polpa (fundo), `#161414` Semente (texto), `#C8232C` vermelho Guará (marca/alerta), `#D6D1CC` Areia (divisórias), `#6B6460` Pedra (texto secundário), `#3F7D4E` Folha (OK), `#D89B2B` Pequi (atenção). Tipografia: **Fredoka** para títulos/branding, **Atkinson Hyperlegible** para corpo e **JetBrains Mono** para dados. O guia alerta para não sobrecarregar tudo de vermelho; usar cores de status apenas onde há significado.

Arquivo-fonte: `assets/guia-de-estilos-v2.pdf`. Figma apontado no repositório: https://www.figma.com/design/hSXL4R1B2bOS91uXfL0ZKh/Sem-t%C3%ADtulo?node-id=50-3429 . Para interface web, aplicar escala de texto adequada ao conteúdo, não literalmente as medidas para slides.

## 8. Roteiros de teste UX (a medir; não comprovados)

- Geraldo identifica o primeiro equipamento que vencerá, identifica seu responsável e registra o tratamento do alerta.
- Geraldo consulta o lacre violado, diferencia evento local e evento entregue ao servidor, encontra evidência/horário da ocorrência.
- Clarice identifica ativo que retornou fisicamente mas não possui NF de retorno regularizada e confirma o fechamento apenas depois das condições necessárias.
- Izidio filtra ciclos encerrados por fornecedor e período, verifica fonte dos dados e exporta dados anonimizados para cliente BI.
- Dado sem localização é exibido corretamente; dado de uma semana atrás é marcado explicitamente como histórico.
- Usuário de acesso restrito não visualiza nem altera dados fora de sua permissão; verificar diretamente API e interface.

O repositório propõe metas de usabilidade RNF04.1 de pelo menos 15 acertos em 18 tarefas, distribuídos com pelo menos 5 acertos por perfil, e RNF04.3 com mediana de até 3 minutos por perfil; esses objetivos seguem **não medidos**. KPI de consulta simples até 2 min em 90% das buscas é uma métrica diferente. Testar antes de afirmar conformidade.

## 9. Ordem de construção do dashboard

1. Contrato de dados, estados/ciclo, massas fictícias de 5 equipamentos e cenários de atraso/lacre/sem rede/retorno parcial.
2. Visão geral e tabela com pesquisa, filtros, ordenação, detalhe e estados vazios; teste funcional dos cartões.
3. Alertas com ciência, justificativa e trilha auditável; autorização por papéis.
4. Cadastro e detalhe de equipamentos/dispositivos, vínculo sem simultaneidade e histórico.
5. Mapa de última posição válida com carimbo de horário e qualidade.
6. Integração via API autenticada, ingestão MQTT com confirmações, dados persistentes e testes de falhas.
7. Fluxo Fiscal, relatórios, exportação/cliente BI e ensaio UX com representantes dos três perfis.

**Não confundir demonstração com produção:** ao usar exemplos fictícios do piloto, marcar 'DADOS SIMULADOS'. As planilhas históricas reais são restritas e não devem ser copiadas para um deploy acadêmico público.

## 10. Referências adicionais para aprofundamento

- Trackunit Asset Home: https://help.trackunit.com/en/articles/141031-how-do-i-navigate-in-asset-home
- Trackunit alertas de bateria/comunicação: https://help.trackunit.com/en/articles/163484-how-do-i-set-up-telematics-device-alerts-in-trackunit-manager
- Trackunit exportação e fuso horário: https://help.trackunit.com/en/articles/495978-how-do-i-export-asset-data
- Fleetio dashboard customizável: https://www.fleetio.com/blog/custom-dashboard
- MaintainX reporting: https://help.getmaintainx.com/about-reporting
- UpKeep ordens em atraso: https://help.onupkeep.com/en/articles/4642169-analytics-work-orders
- Wialon monitoramento: https://help.wialon.com/en/wialon-local/2504/user-guide/monitoring-system
- ThingsBoard mapa: https://thingsboard.io/docs/reference/widgets/widget-library/maps/
- MapLibre: https://maplibre.org/maplibre-gl-js/docs/
- WCAG 2.2: https://www.w3.org/Translations/WCAG22-pt-BR/
