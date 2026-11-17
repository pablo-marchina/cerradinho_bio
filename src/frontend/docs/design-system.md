# Guarátracking — Arquitetura Visual e Design System

**Versão:** 1.0 — 11/11/2026
**Status:** proposta de produto e especificação de interface, para validação com o grupo e o parceiro.
**Escopo nesta etapa:** frontend React + Vite + TypeScript, com API simulada e dados fictícios. **Não** inclui implementação de broker MQTT, autenticação produtiva, backend ou validação física.
**Referências:** *Guarátracking — Guia de Estilos v2* (outubro/2026; 29 páginas), TAPI Cerradinho final, especificação de 72 requisitos do dashboard (11/11/2026), critérios de Sprint 3, pesquisas de Fleetio, Trackunit, MaintainX, Traccar, ThingsBoard, Carbon Design System e shadcn/ui.

> **Legenda:** [GUIA] é orientação preexistente do manual de marca; [TAPI] é exigência/limite do parceiro; [REQUISITOS] é a especificação de funcionalidades; [PROPOSTA] é nossa decisão de design, ainda sujeita a validação. Não confundir telas simuladas com funcionalidades comprovadas.

## 1. Visão e princípios

**A experiência central do produto:** “Descobrir quais equipamentos em manutenção externa exigem ação, por qual motivo, e qual é a próxima ação responsável.” O Guarátracking não é principalmente um aplicativo de acompanhamento contínuo de pontos em um mapa. É um **painel operacional de gestão de ativos em terceiros** [TAPI; benchmark].

1. **Ação antes de decoração:** prioridades e estados no primeiro scroll; mapa como contexto secundário [PROPOSTA].
2. **Uma fonte de verdade por estado:** lacre, prazo contratado, permanência prolongada, comunicação, posição GNSS, situação fiscal e status de comando são dimensões distintas [TAPI; REQUISITOS].
3. **Dados honestos:** nunca mostrar posição antiga como atual; diferenciar última posição válida de último contato; mostrar horário absoluto e idade; indicar valores simulados, ausentes e estimados [TAPI; REQUISITOS].
4. **Legibilidade industrial:** texto direto e técnico, alta densidade informacional sem ruído visual; tabulações, hierarquia clara e ícones sempre acompanhados de texto para estados críticos [GUIA; PROPOSTA].
5. **Segurança da ação:** comandos críticos precisam de confirmação de usuário e feedback explícito; até existir integração real, apresentar apenas **resultado simulado** [PROPOSTA].
6. **Coerência de marca:** guará-LED oficial, paleta guaraná e tipografia da identidade; não usar o vermelho como fundo dominante do dashboard [GUIA].
7. **Três perfis, uma plataforma:** PCM/Geraldo usa a fila de atenção; Fiscal/Clarice usa retorno e NF; Logística/Izidio usa posição e histórico. Papéis/nomes vêm das personas; as adaptações de tela são proposta, não validação de usuários [PROPOSTA].

## 2. Diretrizes da marca que NÃO devem ser alteradas

| Elemento | Fonte | Regra a aplicar |
|---|---|---|
| Nome | [GUIA, §10] | Grafia recomendada **Guarátracking**, uma palavra, com acento; repositório também usa “GuaráTracking” por convenção técnica. Alinhar a grafia pública no grupo. |
| Logo | [GUIA, §01] | Assinatura **10b** como principal; logo com sonar **11a** só em capa/loading; **10e** como indicador de dispositivo. Importar o SVG real do kit; não desenhar um pássaro genérico para substituí-lo. |
| Logo mínimo | [GUIA, §01] | Assinatura horizontal >= 140 px; ícone isolado >= 24 px; respeitar margem de proteção definida no guia (3× base do LED). |
| Cores | [GUIA, §03] | Vermelho Guará `#C8232C`, Semente `#161414`, Polpa `#F7F4F1`; estados Folha/Pequi/Guará apenas em dados. |
| Fontes | [GUIA, §04] | Fredoka para títulos/marca, Atkinson Hyperlegible no corpo, JetBrains Mono para IDs, números e telemetria. Tamanhos de slides não valem diretamente para telas de operação. |
| Linguagem | [GUIA, §10] | Frases diretas: “GT-0031 está há 158 dias no fornecedor C”. Evitar mascote/falas brincalhonas em erros. |
| Interações de marca | [GUIA, §§02,08] | Estados guará-LED sem inventar significados; animação opcional, desabilitada sob `prefers-reduced-motion`. |
| Grafismos culturais | [GUIA, §05] | O guia impõe condição de alinhamento/autorização para grafismos inspirados no povo Pataxó em uso público. **Não usar** no dashboard público até isso ser resolvido. |
| Cenas solarpunk | [GUIA, §§11–16] | Reservar para ilustrações, onboarding e material institucional, não como fundo de tabelas/mapas/dados. Cores de cena não substituem a paleta-base de UI. |
| Origem dos dados | [GUIA, §15] | Mostrar dados IoT vs manuais quando útil; ao prototipar, acrescentar categoria **Simulado** sem parecer que o dado veio de hardware. |

**Limites de interpretação do guia:** há alternativas de logo explicitamente “em teste” na página 3. Não assumimos aprovação dessas variantes. Telas ilustrativas do pitch mostram emissão automatizada de NF e trajeto contínuo; o TAPI **não** exige emitir NF no ERP nem rastreamento contínuo. Esses elementos não devem virar funcionalidades por inferência.

## 3. Arquitetura da informação

```text
GUARÁTRACKING
├── Visão geral / Minha atenção           /                         [P0]
│   ├── KPIs clicáveis
│   ├── Alertas prioritários
│   ├── Tabela de equipamentos que exigem ação
│   └── Últimos eventos
├── Equipamentos                         /equipamentos             [P0]
│   ├── Busca e filtros por estado, NF, TAG, fornecedor
│   └── Detalhe do equipamento            /equipamentos/:id         [P0]
│       ├── Resumo do ciclo / fiscal
│       ├── Lacre + bateria + comunicação + fila
│       ├── Última posição válida
│       ├── Timeline de eventos
│       └── Ações contextuais simuladas
├── Alertas                              /alertas                  [P0]
│   ├── Abertos / reconhecidos / tratados
│   └── Detalhe do alerta                 /alertas/:id              [P1]
├── Mapa                                 /mapa                     [P1]
│   └── Localizações válidas e lista sem GNSS
├── Ciclos e retornos                    /ciclos                   [P1]
│   └── NF de remessa, prazo, retorno físico, baixa fiscal
├── Dispositivos                         /dispositivos             [P0 monit.; P1 lista]
│   └── Monitoramento + comandos         /dispositivos/:id          [P0]
└── Relatórios                          /relatorios               [P2]

NÃO criar rota pública de administração produtiva ou login funcional nesta etapa.
```

**Navegação desktop:** barra lateral fixa, agrupada em **Acompanhamento** (Visão geral, Equipamentos, Alertas, Mapa) e **Operação** (Ciclos e retornos, Dispositivos, Relatórios). Ações globais no topo: busca contextual e “Novo envio” **simulado**. Páginas internas não devem duplicar navegação primária. Subpáginas usam breadcrumb e botão “Voltar à lista” com preservação dos filtros [PROPOSTA].

**Acessos rápidos por pessoa (futuros):** Geraldo começa em Visão geral; Clarice pode entrar em Ciclos e retornos; Izidio pode abrir Mapa/Histórico. Nesta fase não há ACL real nem login — não apresentar trocas de perfil como autorização produtiva.

## 4. Shell e grade responsiva [PROPOSTA]

| Propriedade | Desktop >=1280 px | Tablet 768–1279 px | Mobile <768 px |
|---|---|---|---|
| Navegação | Sidebar fixa de 248 px | Sidebar recolhível 72 px, ou drawer | Drawer sob botão de menu; rótulos visíveis quando aberto |
| Cabeçalho | 64 px alto, busca, ambiente e ações | 64 px; busca compacta | 60 px; busca em overlay ou campo expansível |
| Área de conteúdo | padding 32 px, máximo 1440 px | padding 24 px | padding 16 px |
| Grade | 12 colunas, gap 20–24 px | 8 colunas | 4 colunas |
| KPIs | 4 por linha em largura típica; 5 se couberem sem truncamento | 2 por linha | 1–2 por linha conforme conteúdo |
| Área de atenção | 8 col tabela / 4 col feed secundário | Empilhada, tabela primeiro | Cards de prioridade; sem tabela com scroll horizontal obrigatório |
| Tabela completa | Cabeçalho sticky, filtros e ordenação | Esconder colunas menos importantes | Substituir por cards; ações via menu e detalhes |
| Alvos de toque | >=44×44 px ações relevantes | >=44×44 px | >=44×44 px |

Ponto de partida para wireframes: desktop **1440×900**, tablet **1024×768**, mobile **390×844**. A interface deve funcionar também em 320px com conteúdo adaptado, sem overflow global.

## 5. Páginas — especificação visual

### 5.1 Visão geral — página principal

**Ordem vertical:** título/breadcrumb/última atualização → faixa de alerta crítico apenas se houver ocorrência → KPIs clicáveis → tabela “Equipamentos que exigem atenção” (bloco principal) + painel de eventos recentes → indicadores secundários/atalhos. **Não abrir com mapa gigante.**

**KPIs:** Em manutenção; Atrasados pelo prazo; Violações não tratadas; Sem atualização esperada; Retorno físico com baixa fiscal pendente; Permanência acima do limiar configurado. Quando houver pouco espaço, selecionar 4 principais e mostrar os demais em indicadores secundários. Card clicável filtra tabela.

**Tabela default:** Prioridade; Equipamento + ID; Fornecedor; Prazo previsto/atraso; Lacre; Última posição válida + idade; Ação “Abrir”. Mostre bateria e fila em detalhe para evitar 12 colunas espremidas. Ordenação inicial **proposta**: violação não tratada → atraso → pendência fiscal → próximo vencimento → falha de comunicação. Validar prioridade com PCM. Não confundir 150 dias com prazo contratado.

### 5.2 Equipamentos

Toolbar com busca única por equipamento, TAG, NF ou fornecedor; filtros persistentes e chips de filtro ativos; tabela ou cards no mobile; coluna Estado separada de “Lacre”. Row click para /equipamentos/:id. Estados vazio, nenhum resultado, erro, carregando e sem posição são diferentes. Dados mock com ao menos 5 equipamentos do piloto, todos fictícios.

### 5.3 Detalhe do equipamento — principal tela de decisão

**Cabeçalho:** nome do equipamento + ID/TAG + fornecedor + situação do ciclo + ação prioritária contextual. **Seções:** (A) ciclo e NF; (B) cartões de Lacre, Bateria, Comunicação/Fila e Localização válida; (C) timeline de eventos por hora; (D) histórico de ciclos/reutilização; (E) ações. Um lacre “íntegro agora” não remove evento de violação no histórico.

**Linha de dados de NF:** NF de remessa e data; prazo previsto; dias fora; dias de atraso; retorno físico; NF de retorno/baixa fiscal. Origem do dado visível (Manual/IoT/Simulado). Não oferecer “Emitir NF” sem integração ERP.

### 5.4 Alertas

Padrão Trackunit adaptado: filtros por severidade/tipo/status; linha com tipo, ativo, data de ocorrência, idade, responsável, estado de tratamento. “Lacre violado” prioridade crítica. Ao clicar: detalhe do alerta e timeline de reconhecimento/tratamento. Estados abertos, reconhecidos, em tratamento, resolvidos. Mock registra localmente o comportamento para demonstração, não cria auditoria real.

### 5.5 Mapa

Painel de lista + mapa em desktop; lista acima no mobile. Pins correspondem **somente** a últimas posições válidas, com data absoluta, idade e qualidade quando disponível. Sidebar com “sem posição válida”; não inventar trajetos nem geofences. Visual de posição anterior sempre diz **“Última posição válida: dd/mm/aaaa HH:mm (há N dias)”**, nunca “posição atual”. Dados de localização do demo devem ser fictícios.

### 5.6 Ciclos e retornos

Tabela de ciclos com NF de remessa/retorno, responsável, fornecedor, prazo, status físico e fiscal independentes. Ações de cadastro e encerramento são **simuladas**, com validação e motivo de recusa; fluxo “retorno físico confirmado” não finaliza automaticamente o ciclo quando a NF retorno está pendente.

### 5.7 Dispositivos e comandos

Lista de TAGs e detalhe técnico: lacre, % de bateria/sensor de falha, última conexão LWT se houver, último contato, intervalo esperado e fila local **no último informe**. Estados de comando propostos: `rascunho → solicitado → aguardando_resposta → confirmado/recusado/tempo_esgotado`; em demo, todos identificados “resultado simulado”. Formulários: abrir ciclo, NF, autorizar retirada, encerrar ciclo; confirmação extra para autorizar retirada/encerramento.

### 5.8 Relatórios [P2]

Somente quando existirem fontes suficientes: métricas por fornecedor, permanência, alertas, perdas de comunicação e retornos; gráficos com período/fonte, sem calcular ROI ou atrasos históricos sem data de retorno prevista e real. Não usar valores monetários do parceiro no frontend público.

## 6. Design tokens [GUIA + PROPOSTA]

### 6.1 Paleta primária do guia [GUIA]

| Token | Hex | Uso |
|---|---|---|
| `guara` | `#C8232C` | Marca, botão primário, indicação crítica pontual |
| `urucum` | `#8F1820` | Hover/pressed do CTA, bordas/detalhes de alerta |
| `semente` | `#161414` | Texto principal, ícones em fundos claros |
| `polpa` | `#F7F4F1` | Fundo global |
| `areia` | `#D6D1CC` | Divisórias e bordas neutras |
| `pedra` | `#6B6460` | Texto secundário |
| `folha` | `#3F7D4E` | OK, dentro do prazo |
| `pequi` | `#D89B2B` | Atenção, bateria baixa, vencimento próximo |
| `terra` | `#A65A2E` | Apoio ilustrativo, uso restrito |

### 6.2 Tokens semânticos derivados [PROPOSTA]

| Token | Valor | Uso |
|---|---|---|
| `background` | `#F7F4F1` | Base |
| `surface` | `#FFFFFF` | Tabelas/cards (proposta neutra) |
| `surface-muted` | `#FBFAF5` | Barra e superfícies sutis (Nuvem no guia de cenas) |
| `foreground` | `#161414` | Texto principal |
| `muted-foreground` | `#6B6460` | Secundário |
| `border` | `#D6D1CC` | Borda principal |
| `primary` | `#C8232C` | CTA |
| `primary-hover` | `#8F1820` | Hover/pressed |
| `danger-bg` | `#FFF1F2` | Faixa e chip críticos |
| `danger-text` | `#8F1820` | Texto de crítica |
| `warning-bg` | `#FFF7E7` | Atenção |
| `warning-text` | `#71500A` | Texto legível sobre amarelo suave |
| `success-bg` | `#EEF7EF` | Normalidade |
| `success-text` | `#2B6038` | Texto verde escuro |
| `info-bg` | `#EEF3FA` | Informações, não confundido com estados |
| `focus-ring` | `#161414` | Contorno visível do foco |

As cores claras e textos escuros dos status são **propostos**, não constam literalmente do guia. Contrastes calculados para texto normal: branco sobre Guará `#C8232C` ≈ 5,63:1; Semente sobre Polpa ≈ 16,75:1; Pedreira `#6B6460` sobre Polpa ≈ 5,30:1; **branco sobre Pequi** ≈ 2,43:1 (**não usar como texto pequeno**). Meta WCAG 2.2 AA; validar em UI real e com diferentes tamanhos/estados.

**Regras de uso:** status sempre com texto + ícone; vermelho não deve cobrir toda a interface; não usar cor verde para “localização recente” se o lacre estiver violado, porque são estados diferentes. Sombra só quando necessária para elevação; borda costuma ser suficiente.

### 6.3 Tipografia [GUIA + PROPOSTA]

| Uso | Família | Tamanho/altura linha (proposta web) | Peso |
|---|---|---|---|
| H1/título principal | Fredoka | 30/36 px | 600 |
| H2/título de seção | Fredoka | 24/30 px | 600 |
| H3/cartão/modal | Fredoka | 18/24 px | 500–600 |
| Corpo | Atkinson Hyperlegible | 16/24 px | 400 |
| Corpo compacto/tabelas | Atkinson Hyperlegible | 14/20 px | 400/700 |
| Rótulo e metadata | Atkinson Hyperlegible | 13/18 px | 400/700 |
| KPI | Fredoka | 30/36 px | 600 |
| ID / TAG / NF / timestamp técnico | JetBrains Mono | 13/20 px | 400/500 |
| Código / JSON | JetBrains Mono | 13/20 px | 400 |

JetBrains Mono **não** deve dominar parágrafos ou longas tabelas. Usar `font-variant-numeric: tabular-nums` para colunas numéricas e contadores. Escala redimensionável com zoom 200%.

### 6.4 Espaçamento e geometria [PROPOSTA]

- Espaços base: 4, 8, 12, 16, 20, 24, 32, 40, 48, 64 px.
- Cards: raio 12 px, borda 1px areia, padding 20–24 px.
- Inputs/botões: raio 8 px, altura normal 44 px, mínimo 44×44 px em áreas de toque.
- Badges: raio total, padding 4×10 px, texto 12–13 px + ícone.
- Tabela desktop: header 44 px, linha 56–64 px; hover discreto e foco visível.
- Sidebar 248 px e topbar 64 px; largura útil máxima 1440 px, sem estender linhas de texto indefinidamente.
- Elevação: `0 1px 3px rgba(22,20,20,.05)` apenas para superfícies que precisem se destacar.
- Motion: 150–200 ms para hover/accordion, **sem animação pulsante permanente em alertas**; `prefers-reduced-motion: reduce` desabilita animações.

## 7. Componentes do design system

| Componente | Variantes/estados | Uso e regras |
|---|---|---|
| `AppShell` | desktop/compacto/mobile | Sidebar, topbar, skip link para conteúdo |
| `PageHeader` | título, descrição, ação, breadcrumb | Uma ação primária por contexto; contexto do ambiente visível |
| `DemoBanner` | mock/ambiente de teste | Sempre visível com dados simulados, sem afirmar sessão real |
| `KpiCard` | normal/attention/critical/neutral | Contagem e filtro; unidade e período claros |
| `PriorityTable` | loading/empty/no-results/error | Estado ordenável; IDs e prazos legíveis |
| `AssetCard` | compact/expanded/mobile | Fallback móvel da tabela |
| `StatusBadge` | critical/warning/success/neutral/unknown | Ícone + rótulo sem dependência exclusiva da cor |
| `SealState` | intact/authorized/violated/reapply/unknown | Status físico; não substitui alertas pendentes |
| `BatteryMeter` | normal/low/sensor-error/unknown/simulated | Percentual explícito, `0` de erro não interpretado como bateria vazia |
| `ConnectivityStatus` | broker-connected/lwt-disconnected/last-check-in/stale/unknown | LWT ≠ atualização semanal; não exibir falso “online” |
| `QueueIndicator` | known/possibly-stale/unknown | Valor e hora do último informe |
| `LocationPanel` | valid/stale/no-fix/never-seen | Última posição válida, idade, data e qualidade |
| `EventTimeline` | chronological/from-queue/out-of-order | Ordem de ocorrência mais recente primeiro; recebido em separado |
| `AlertQueue` | open/acknowledged/handling/resolved | Ação no nível do alerta; histórico imutável conceitualmente |
| `CycleProgress` | shipped/maintenance/physical-return/tax-pending/closed | Não fundir etapas fiscal e física |
| `CommandForm` | idle/validation/pending/simulated-ok/simulated-refused/timeout | Descritivo, confirmação, resposta correlacionada; em mock sempre rótulo SIMULADO |
| `ConfirmDialog` | warning/destructive | Explicitar alvo/efeito, foco retido e escape |
| `FilterBar` | default/active/empty | Chips removíveis, limpar filtros, contagem |
| `EmptyState` | no-data/no-results/no-GNSS/no-events | Explicar motivo e próxima ação |
| `Skeleton` e `ErrorState` | loading/error/retry | Feedback sem piscar e sem falso sucesso |
| `DataOriginTag` | IoT/manual/simulated/estimated/absent | Proveniência; não usar “VIA IoT” em mocks |
| `Toast` | info/success/error | `aria-live` e duração suficiente; respostas importantes ficam persistidas na tela |
| `MapPanel` | map/list/no-position | Pins com tempo; agrupamento com no-data em lista |

**Ícones:** importar o conjunto oficial para eventos marcários; usar Lucide React em ícones utilitários (`layout-dashboard`, `package`, `shield-check`, `shield-alert`, `battery-low`, `wifi-off`, `map-pin`, `file-text`, `history`, `alert-triangle`, `clock-3`, `check-circle-2`). Tamanho padrão 16–20 px; não usar emoji como indicadores de estado.

## 8. Matriz de estados visuais

| Dimensão | Estados e textos | Tom visual |
|---|---|---|
| Lacre físico | Íntegro; Retirada autorizada; Violado; Aguardando reaplicação; Indeterminado | success / neutral-info / danger / warning / neutral |
| Alerta de violação | Aberto; Reconhecido; Em tratamento; Resolvido | danger enquanto aberto, neutral quando resolvido; nunca apagar o evento |
| Prazo acordado | No prazo; Vence em N dias; Atrasado N dias | success / warning / danger |
| Permanência de referência | N dias fora; ≥ limiar configurado (inicial 150 d) | Condição independente do prazo acordado; danger quando acima |
| Comunicação | Conectado ao broker; Desconexão LWT; Sem atualização esperada; Desconhecido | neutral/success contextual; warning; warning; neutral |
| Bateria | X%; Bateria baixa (<20%); Falha do sensor; Sem dado | texto + medidor; warning; danger/unknown; neutral |
| Posição | Última posição válida há N dias; GNSS sem fix; Sem posição conhecida | texto e horário visíveis; não inventar posição |
| Fila | N pendentes, informados às HH:mm; valor desatualizado; indisponível | semântica explícita; não `N=0` por ausência de dado |
| Fiscal | NF remessa; retorno físico; NF retorno pendente; ciclo encerrado | sequência clara com próximos passos |
| Comando | Preparado; Enviado (simulado); Resposta simulada; Recusado; Timeout | cada estado rótulo + informação de teste |
| Origem | Dispositivo; Manual; Simulado; Estimado | rótulo secundário ao valor; mock sempre evidente |

### Acessibilidade e conteúdo

- Data: `dd/mm/aaaa · HH:mm` e relativo (`há 8 dias`); fuso America/Sao_Paulo quando aplicável, sem inventar timezone de timestamp de uptime.
- Não usar “Atualizado agora” sem timestamp real; mostrar `Último registro recebido` e `Última posição válida` separadamente.
- Tabelas com cabeçalhos semanticamente associados; labels de botões com verbo + objeto (“Ver equipamento”, “Reconhecer alerta”).
- Foco de teclado e ordem lógica; dialogs acessíveis com foco e tecla Esc; inputs com `label`, erros próximos ao campo; `aria-live` para notificações.
- Modo não conectado **não** é igual a perda de sessão MQTT por LWT; no mock rotular estados como cenários.
- Jamais mostrar nome de fornecedor, NF ou coordenada real da Cerradinho em demonstração pública.

## 9. Wireframes textuais

### 9.1 Visão geral — desktop

```text
┌──────────────248px────────┬─────────────────────────────────────── 1440px ──────────────────────────────────────┐
│ [LOGO 10b]               │ [Buscar equipamento, TAG ou NF...]              DEMONSTRAÇÃO | Novo envio [CTA]  │
│                         ├───────────────────────────────────────────────────────────────────────────────────┤
│ ACOMPANHAMENTO           │ Visão geral                                         [atualização simulada]       │
│  ◉ Visão geral           │ [ALERTA CRÍTICO: Lacre violado — abrir equipamento]                            │
│  □ Equipamentos          │ [Em manutenção] [Atrasados] [Violações] [Sem atualização] [Pendência fiscal]   │
│  □ Alertas               │                                                                    │
│  □ Mapa                  │ [ EQUIPAMENTOS QUE EXIGEM ATENÇÃO       8 col ] [ EVENTOS RECENTES   4 col ] │
│                         │ [ busca + filtros + ordenação             ] [ timestamp ocorrência ] │
│ OPERAÇÃO                 │ [ prioridade / ativo / fornecedor / prazo ] [ lacre / atraso      ] │
│  □ Ciclos e retornos     │ [ ------------------------------------------------------------- ] │
│  □ Dispositivos          │ [ mais linhas + paginação                   ] [ Ver todos ]        │
│  □ Relatórios            │                                                                    │
└──────────────────────────┴────────────────────────────────────────────────────────────────────┘
```

### 9.2 Detalhe

```text
[Voltar] / Equipamentos / GT-0042
Motor de bomba — AT-0002           [Em manutenção] [Ações ▾]
[FAIXA CRÍTICA caso exista violação não tratada]

[ NF + prazo + dias fora + atraso + fornecedor + responsável ]
[LACRE] [BATERIA] [COMUNICAÇÃO + FILA] [ÚLTIMA POSIÇÃO VÁLIDA / há N dias]
[ HISTÓRICO DE EVENTOS — ocorrido em, recebido em ] [ INFORMAÇÕES DO CICLO ]
[ AÇÕES SIMULADAS: NF / autorizar retirada / abrir / encerrar ]
```

### 9.3 Mobile

```text
[☰] Guarátracking              [demo]
Visão geral
[alerta crítico, se aplicável]
[Em manutenção] [Atrasados]
[Violações]    [Sem atualização]
Equipamentos que exigem atenção
[card ativo #1 · prazo · lacre · abrir]
[card ativo #2 · prazo · lacre · abrir]
Últimos eventos
[evento + hora + ativo]
```

## 10. Interações e transições para o protótipo frontend

1. KPI “Violações” → `/alertas?tipo=lacre&status=aberto`, com filtro ativo e botão “Limpar”.
2. Selecionar registro → `/equipamentos/:id`; preservar busca e filtros ao voltar.
3. Troca de equipamento → timeline, estado do lacre, NF, posição e fila pertencem ao mesmo ID; nunca misturar outro ciclo.
4. Botão `Autorizar retirada` → dialog contextual “Você está autorizando a retirada do dispositivo GT-XXXX?” → status **simulado** com registro visual.
5. Botão `Encerrar ciclo` sem NF de retorno → exibir **recusa simulada**, motivo específico e campo faltante; não alterar status.
6. Filtro `Sem posição` → ativos aparecem na lista, não pinados em coordenadas inventadas.
7. Contexto offline → indicador de último estado conhecido, sem alegar dados em tempo real.
8. Evento recebido depois da ocorrência → posicioná-lo por `occurred_at`, mostrar `received_at` ao expandir.
9. Bateria 0 com falha sensor → `Falha na leitura da bateria`, sem barra 0% falsamente crítica.
10. Troca de perfil demonstrativo → UX pode mudar, mas jamais representar controle de acesso real.

## 11. Componentização React e estrutura sugerida

```text
src/frontend/
  src/
    app/            router.tsx, providers.tsx, app-shell.tsx
    design-system/  tokens.css, typography.css, status.ts, icons.ts
    components/
      ui/           shadcn primitives
      layout/       sidebar, header, breadcrumbs, page-header
      feedback/     badges, banners, toasts, empty/error/skeleton
      data-display/ kpi, data-table, timeline, location, device-status
    features/
      overview/
      equipments/
      alerts/
      map/
      cycles/
      devices/
      reports/
    domain/         types.ts, predicates.ts, date-formatters.ts
    services/       contracts.ts, mock.ts
    mocks/          scenarios.ts, handlers.ts
    styles/         globals.css
```

**Stack mantida:** React/Vite/TypeScript/Tailwind/shadcn/ui; React Router, TanStack Table, TanStack Query opcional se MSW for uma API simulada, Zustand para estado de UI transitório, RHF+Zod em forms, MapLibre e Recharts quando as telas os exigirem. Não introduzir backend apenas para o protótipo. Os mocks devem ser coerentes e ter estados alternáveis com indicadores de `DEMONSTRAÇÃO`.

## 12. Contrato de design com implementação

- **Nome de rotas e tipos:** documentar antes de montar componentes.
- **Tokens:** todos os componentes utilizam variáveis semânticas, não hex codes espalhados.
- **shadcn/ui:** customizar `Button`, `Badge`, `Card`, `Dialog`, `Input`, `Table`, `Sheet`, `Tabs`, `Tooltip` por tema, não alterar comportamento de acessibilidade da base.
- **Fonte:** usar pacote/URL oficial com fallback; nunca repassar arquivos de fontes proprietárias.
- **Ícones:** Lucide React de interface, guará-LED apenas via recurso oficial.
- **Responsividade:** testar 390, 768, 1024, 1440 px; zoom 200%; teclado e leitor de tela em fluxos críticos.
- **No demo:** nenhum comando é apresentado como executado no hardware real; nenhum mapa utiliza localização do parceiro.
- **Documentação:** capturar visuais na seção 3.4.3, mas as evidências de MQTT e I2C requerem integrações/hardware reais e permanecem independentes desta definição visual.

## 13. Definition of Done do design system

- [ ] Paleta, tipografia, spacing e componentes-base documentados com componentes React.
- [ ] Ação primária clara e não ambígua por tela/contexto.
- [ ] Wireframes da Visão geral, Equipamentos, Detalhe, Alertas, Mapa, Ciclos/Retornos e Dispositivos.
- [ ] Estado crítico, atenção, normal, ausente, stale, error, loading e empty previstos em cada componente aplicável.
- [ ] Posição antiga jamais apresentada como atual; atraso e permanência 150d visivelmente separados.
- [ ] Dados mock, rótulos e informações de parceiro fictícios; marca de demonstração em todas as telas.
- [ ] Falha de comando e recusa de encerramento com explicação e preservação do estado anterior.
- [ ] Fluxos de teclado, foco, contraste, labels e zoom AA auditados no navegador.
- [ ] Design revisado com PCM, Fiscal, Logística e guia de marca antes da apresentação final.

## 14. Referências

**Projeto:**
- *Guarátracking — Guia de Estilos v2* (out/2026), arquivo do grupo no Drive. Identifica diretamente a paleta, logotipos, tipografia, padrões de dispositivos, amostra de site/painel e restrições de uso cultural.
- TAPI final Cerradinho: mínimo de 5 equipamentos reais/representativos; periodicidade semanal configurável; prazo de 150 dias como referência; alerta de lacre; NF remessa e retorno; última posição válida; histórico e API BI; sem rastreamento indoor preciso ou tempo real contínuo.
- `requisitos_dashboard_guaratracking_v1_2026-11-11.md`: 72 requisitos com rastreabilidade de RF/RN/RNF. Nesta fase, aqueles que dependem de backend/API são **visualizados apenas por mock**.

**Padrões externos usados como inspiração, não como prova do funcionamento do projeto:**
- Trackunit: https://help.trackunit.com/en/articles/141031-how-do-i-navigate-in-asset-home
- Fleetio: https://www.fleetio.com/blog/custom-dashboard
- MaintainX: https://www.getmaintainx.com/
- Traccar: https://www.traccar.org/
- shadcn: https://ui.shadcn.com/blocks/dashboard
- Carbon data table: https://carbondesignsystem.com/components/data-table/usage/
- WCAG 2.2: https://www.w3.org/TR/WCAG22/
