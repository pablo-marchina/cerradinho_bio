# Planning Sprint 2 — tópicos dos artefatos 5, 6 e 7

São 47 cards sem pessoas atribuídas: 13 do Art. 5, 12 do Art. 6, 10 do Art. 7 e 12 transversais (repositório, apresentação, revisão e merge). Seg 26/10 é só a planning. O build e os ajustes finos vão de ter 27/10 a qua 4/11. Na qui 5/11 acontecem a revisão final contra a rubrica, o merge da develop na main e a entrega. A apresentação é na sex 6/11 de manhã e fica pronta na quinta.

Enunciados e rubricas: [ArtefatosDaSp2.md](ArtefatosDaSp2.md).

O documento segue a ordem em que é usado: primeiro **quando** (calendário), depois **o quê** (cards), **em que ordem** (dependências) e por fim **como a sprint roda** (planning, revisões, sincronização e entrega).

## Sumário

- [1. Calendário](#1-calendário)
- [2. Visão por dia](#2-visão-por-dia)
- [3. Cards por artefato](#3-cards-por-artefato)
  - [3.1 Artefato 5 — Protótipo físico offline (seção 3.4.2)](#31-artefato-5--protótipo-físico-offline-seção-342)
  - [3.2 Artefato 6 — RNFs (3.3.3) e atualização de RF, RN e RTM](#32-artefato-6--rnfs-333-e-atualização-de-rf-rn-e-rtm)
  - [3.3 Artefato 7 — Jornadas atual e futura e storyboard (seção 3.2.2)](#33-artefato-7--jornadas-atual-e-futura-e-storyboard-seção-322)
  - [3.4 Cards transversais — repositório, apresentação, revisão e merge](#34-cards-transversais--repositório-apresentação-revisão-e-merge)
- [4. Dependências](#4-dependências)
  - [4.1 Fluxo dos artefatos 5, 6 e 7](#41-fluxo-dos-artefatos-5-6-e-7)
  - [4.2 Fluxo de repositório, apresentação, revisão e entrega](#42-fluxo-de-repositório-apresentação-revisão-e-entrega)
  - [4.3 Por que cada card espera e quanto sobra de folga](#43-por-que-cada-card-espera-e-quanto-sobra-de-folga)
- [5. Como a sprint roda](#5-como-a-sprint-roda)
  - [5.1 Planning — seg 26/10](#51-planning--seg-2610)
  - [5.2 Revisões ao longo da sprint](#52-revisões-ao-longo-da-sprint)
  - [5.3 Reunião de sincronização — sex 30/10](#53-reunião-de-sincronização--sex-3010)
  - [5.4 Revisão final e entrega — qui 5/11](#54-revisão-final-e-entrega--qui-511)
- [6. Decisões da planning](#6-decisões-da-planning)

## 1. Calendário

Os 9 dias úteis contam de seg 26/10 a sex 6/11, sem o feriado de Finados (seg 2/11). Cada card tem um prazo; a tabela mostra o que fecha em cada dia.

| Dia | Data | Fase | Art. 5 — Protótipo físico | Art. 6 — RNF, RF, RN, RTM | Art. 7 — Jornadas e storyboard | Transversais | Gate no fim do dia |
| --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | Seg 26/10 | Planning | — | — | — | Responsáveis, equipes e cards de cada artefato (5.R1, 6.R1, 7.R1) | Cards criados e atribuídos |
| 2 | Ter 27/10 | Alinhamento | 5.1 inventário · 5.5 qualidade do código (início) | 6.1 taxonomia · 6.10 prints do Wokwi (início) | 7.1 template · 7.2 insumos | X1 pastas e nomes de assets | Ferramentas escolhidas e primeiros cards em andamento |
| 3 | Qua 28/10 | Base | 5.R2 dúvidas ao professor · 5.2 pinagem · 5.3 montagem · 5.6 CTs escritos | 6.R2 dúvidas ao professor · 6.2 demandas de negócio | 7.R2 dúvidas ao professor · 7.3 jornada atual (início) | X2 limpeza de branches e MRs | CTs numerados e publicados para o Art. 6 e o Art. 7 |
| 4 | Qui 29/10 | Construção | 5.4 firmware em cada versão · 5.5 fecha · 5.11 comparação e escolha da versão | 6.3 a 6.6 RNFs em paralelo · 6.10 fecha | 7.3 jornada atual fecha | — | ESP32 lê sensores e acende LEDs no protoboard; versão final escolhida; jornada atual pronta |
| 5 | Sex 30/10 | Construção | 5.7 execução dos CTs | 6.3 a 6.6 fecham | 7.4 jornada futura · 7.6 roteiro do storyboard | A1 roteiro da apresentação · S1 reunião de sincronização | 8 RNFs escritos; CTs executados; responsáveis revisam os cards |
| — | Seg 2/11 | Feriado | — | — | — | — | — |
| 6 | Ter 3/11 | Integração | 5.8 correções · 5.9 texto da 3.4.2 | 6.7 RF e RN · 6.8 RTM (início) | 7.5 insights · 7.7 cenas do storyboard (início) | A2 slides (início) · A3 vídeo do protótipo | Todo conteúdo existe em branch |
| 7 | Qua 4/11 | Ajustes finos | 5.10 CTs na RTM | 6.8 RTM fecha · 6.9 consistência cruzada | 7.7 fecha · 7.8 texto da 3.2.2 | X3 README · A2 slides fecham | Tudo mergeado na develop |
| 8 | Qui 5/11 | Revisão e entrega | R5 revisão contra a rubrica | R6 revisão contra a rubrica | R7 revisão contra a rubrica | M1 merge develop → main · A4 ensaio e versão final | Entregue; apresentação pronta |
| 9 | Sex 6/11 | Apresentação | — | — | — | Apresentação de manhã | — |

## 2. Visão por dia

Cada coluna é um dia, e cada barra vai do início ao prazo do card. A legenda explica cores e símbolos.

```mermaid
%%{init: {"theme": "base", "themeVariables": {"fontFamily": "Segoe UI, Arial, sans-serif", "fontSize": "13px", "sectionBkgColor": "#7D8A84", "altSectionBkgColor": "#7D8A84", "sectionBkgColor2": "#7D8A84", "excludeBkgColor": "#E3E7E4", "gridColor": "#D5DBD6", "taskBkgColor": "#A8E6B8", "taskBorderColor": "#2E7D32", "taskTextColor": "#182420", "taskTextLightColor": "#182420", "taskTextDarkColor": "#182420", "taskTextOutsideColor": "#7D8A84", "taskTextClickableColor": "#182420", "critBkgColor": "#F8B4B4", "critBorderColor": "#C62828", "doneTaskBkgColor": "#E0E0E0", "doneTaskBorderColor": "#8A968F", "activeTaskBkgColor": "#A8E6B8", "activeTaskBorderColor": "#1F5C52", "titleColor": "#7D8A84", "textColor": "#7D8A84", "todayLineColor": "#9B3A34"}, "gantt": {"barHeight": 22, "barGap": 6, "topPadding": 56, "leftPadding": 190, "gridLineStartPadding": 40, "fontSize": 13, "sectionFontSize": 13, "numberSectionStyles": 4}}}%%
gantt
    title Sprint 2 — cards por dia (← indica de quais cards depende)
    dateFormat YYYY-MM-DD
    axisFormat %d/%m
    tickInterval 1day
    todayMarker off

    section Planning
    5.R1, 6.R1, 7.R1 Alinhamento dos responsáveis :done, r1, 2026-10-26, 1d

    section Art. 5 Protótipo
    5.1 Inventário                          :crit, a51, 2026-10-27, 1d
    5.5 Qualidade do código                 :a55, 2026-10-27, 3d
    5.R2 Dúvidas ao professor               :a5r2, 2026-10-27, 2d
    5.2 Pinagem ← 5.1                       :crit, a52, 2026-10-28, 1d
    5.3 Montagem de cada versão ← 5.2       :crit, a53, 2026-10-28, 1d
    5.6 CTs escritos ← 5.1                  :a56, 2026-10-28, 1d
    5.4 Firmware em cada versão ← 5.3, 5.5  :crit, a54, 2026-10-29, 1d
    5.11 Comparação e escolha ← 5.4, 5.6    :crit, a511, 2026-10-29, 1d
    5.7 Execução dos CTs ← 5.11, 5.6        :crit, a57, 2026-10-30, 1d
    5.8 Correções ← 5.7                     :a58, 2026-11-03, 1d
    5.9 Texto 3.4.2 ← 5.7                   :a59, 2026-11-03, 1d
    5.10 CTs na RTM ← 5.6, 6.8              :a510, 2026-11-04, 1d

    section Art. 6 RNF, RF, RN, RTM
    6.1 Estrutura da tabela RNF             :a61, 2026-10-27, 1d
    6.10 Prints pendentes do Wokwi          :a610, 2026-10-27, 3d
    6.R2 Dúvidas ao professor               :a6r2, 2026-10-27, 2d
    6.2 Demandas de negócio ← 6.1           :a62, 2026-10-28, 1d
    6.3 a 6.6 Oito RNFs em paralelo ← 6.2   :a63, 2026-10-29, 2d
    6.7 RF e RN ← 5.4, 5.7                  :a67, 2026-11-03, 1d
    6.8 RTM ← 5.6, 6.3 a 6.7, 6.10          :a68, 2026-11-03, 2d
    6.9 Consistência cruzada ← 6.8, 7.5     :a69, 2026-11-04, 1d

    section Art. 7 Jornadas e storyboard
    7.1 Template e fases                    :a71, 2026-10-27, 1d
    7.2 Insumos da persona                  :a72, 2026-10-27, 1d
    7.R2 Dúvidas ao professor               :a7r2, 2026-10-27, 2d
    7.3 Jornada atual ← 7.1, 7.2            :a73, 2026-10-28, 2d
    7.4 Jornada futura ← 7.3                :a74, 2026-10-30, 1d
    7.6 Roteiro do storyboard ← 7.3, 5.6    :a76, 2026-10-30, 1d
    7.5 Insights e oportunidades ← 7.4      :a75, 2026-11-03, 1d
    7.7 Ilustração do storyboard ← 7.6      :a77, 2026-11-03, 2d
    7.8 Texto 3.2.2 ← 7.5, 7.7              :a78, 2026-11-04, 1d

    section Transversais
    X1 Pastas e nomes de assets             :x1, 2026-10-27, 1d
    X2 Limpeza de branches e MRs            :x2, 2026-10-27, 2d
    S1 Reunião de sincronização             :milestone, s1, 2026-10-30, 0d
    A1 Roteiro da apresentação ← 5.6, 7.3   :ap1, 2026-10-30, 1d
    A3 Vídeo do protótipo ← 5.7             :ap3, 2026-11-03, 1d
    A2 Slides ← A1, 5.7, 6.3 a 6.6, 7.4     :ap2, 2026-11-03, 2d
    X3 README ← X1, 5.4                     :x3, 2026-11-04, 1d

    section Revisão e entrega
    R5, R6, R7 Revisão contra a rubrica     :rv, 2026-11-05, 1d
    M1 Merge develop → main ← R5 a R7       :m1, 2026-11-05, 1d
    A4 Ensaio e versão final ← A2, A3, R5 a R7 :ap4, 2026-11-05, 1d
    Apresentação                            :milestone, apr, 2026-11-06, 0d
```

**Legenda**

| Símbolo | Significado |
| --- | --- |
| 🟩 Barra verde | Card; vai do dia em que começa até o prazo |
| 🟥 Barra rosa com borda vermelha | Caminho crítico do protótipo: sem folga, um dia de atraso empurra o resto |
| ⬜ Barra cinza | Feito na própria planning (alinhamento dos responsáveis) |
| ◆ Losango | Marco de um dia: reunião de sincronização e apresentação |
| `←` no nome | Cards de que este depende (ex.: `5.4 ← 5.3, 5.5`) |
| Dia sem barras | Fim de semana ou feriado de 2/11 |
| Faixas à esquerda | Grupo do card: Planning, Art. 5, Art. 6, Art. 7, Transversais, Revisão e entrega |

## 3. Cards por artefato

Cada card nasce de uma frase do enunciado ou de uma exigência da rubrica; a coluna "Critério" mostra qual. Os cards R1 e R2 são do responsável de cada artefato.

### 3.1 Artefato 5 — Protótipo físico offline (seção 3.4.2)

O firmware do Wokwi já existe (`src/firmware/main.cpp` e os `include/*_rules.h`), então o trabalho é levar esse circuito para o protoboard, testar e registrar. A equipe do Art. 5 decidiu que **cada integrante monta a própria versão física** do protótipo (5.3 e 5.4); depois as versões são comparadas e o grupo escolhe a melhor (5.11), que é a usada nos CTs, nas evidências e na seção 3.4.2. No nível Avançado a rubrica pede quatro coisas: pelo menos 3 CTs completos, ESP32 com sensor no protoboard e fios organizados, resultados documentados com prints e vídeos, e código com nomes descritivos e funções comentadas.

| # | Tópico (vira card) | Pronto quando | Critério da rubrica | Depende de | Prazo | Tamanho |
| --- | --- | --- | --- | --- | --- | --- |
| 5.R1 | Responsável: alinhar o artefato com o time | Enunciado e rubrica apresentados ao grupo; cards divididos; o que roda em paralelo combinado; ao longo da sprint, informações trocadas entre cards e cards criados ou excluídos quando preciso (checagem na sex 30/10) | Todos | — | Seg 26/10, na planning (divisão) · Ter 3/11 (último ajuste de cards) | M |
| 5.R2 | Responsável: levar ao professor o que o enunciado não diz | Respostas registradas no card: numeração dos CTs físicos (CT36 em diante, como diz o texto-guia, ou CT27 a CT31 já planejados), se o GNSS precisa estar no protótipo offline, onde ficam os vídeos | Todos | 5.R1 | Qua 28/10 | P |
| 5.1 | Inventário dos componentes físicos e recorte do protótipo offline | Lista do que o grupo tem (reed switch ou chave, pot ou divisor, LEDs, botões, GNSS LC76G) e do que fica fora (Wi‑Fi, MQTT); BOM da 3.1.7 conferida; confirmado que há kit (ESP32, protoboard e componentes) para cada integrante montar a sua versão | Sensor conectado ao ESP32 | — | Ter 27/10 | P |
| 5.2 | Pinagem e esquema elétrico do circuito físico | Esquema (Fritzing ou foto anotada) mantendo os GPIOs do Wokwi (27, 34, 25, 26, 23, 14, 33, 13, 18, 19) | Organização dos fios | 5.1 | Qua 28/10 | M |
| 5.3 | Montagem no protoboard com fios organizados, uma versão por integrante | Cada integrante do Art. 5 com o seu circuito montado a partir do mesmo esquema (5.2), cores de fio padronizadas, foto de cima e de lado de cada versão | Protótipo no protoboard | 5.2 | Qua 28/10 | G |
| 5.5 | Auditoria de qualidade do código | Nomes descritivos, uma responsabilidade por função, comentário em cada função; `tests/model_tests.c` continua passando | Nomes, funções e comentários (separa o 75% do 100%) | — | Qui 29/10 | M |
| 5.6 | Casos de teste do protótipo físico | Tabela no formato do template (descrição, pré-condição, procedimento, pós-condição) com sucesso e falha: lacre aberto, ruído no contato, divisor desconectado, botão com ruído, GNSS sem fix | 3 CTs ou mais com mudança de sensor e resultado nos LEDs | 5.1 | Qua 28/10 | G |
| 5.4 | Firmware rodando no hardware real, em cada versão | Ambiente PlatformIO para a placa física compila e grava em todas as versões; leituras reais de lacre, bateria e botões aparecem no Serial de cada uma | Leituras e saídas | 5.3, 5.5 | Qui 29/10 | G |
| 5.11 | Comparação das versões e escolha da melhor | Tabela comparando as versões (fios organizados, leituras corretas no Serial, LEDs reagindo aos sensores, firmeza dos contatos, facilidade de demonstrar), com foto de cada uma; versão escolhida e motivo registrados no card | Protótipo no protoboard com fios organizados | 5.4, 5.6 | Qui 29/10 | M |
| 5.7 | Execução dos CTs e registro de evidências | Cada CT, executado na versão escolhida, com print do Serial, foto e vídeo curto em `assets/` e situação APROVADO ou REPROVADO | Resultados documentados | 5.11, 5.6 | Sex 30/10 | GG |
| 5.8 | Correção das falhas e reexecução | CTs reprovados corrigidos ou registrados como falha com motivo | Resultados alinhados ao esperado | 5.7 | Ter 3/11 | M |
| 5.9 | Texto da seção 3.4.2 | Introdução, resumo da comparação das versões e motivo da escolha, figuras com legenda e fonte, tabela de CTs, links dos vídeos, texto-guia removido | Figuras e descrição dos testes | 5.7 | Ter 3/11 | M |
| 5.10 | CTs novos na RTM | Cada CT da 3.4.2 ligado a RF e RN na 3.3.4, feito junto com o 6.8 | Atualizar a RTM | 5.6, 6.8 | Qua 4/11 | P |

### 3.2 Artefato 6 — RNFs (3.3.3) e atualização de RF, RN e RTM

O Avançado pede 8 RNFs diferentes conforme a ISO 25010, cada um com métrica ou critério de teste e justificativa, e RF, RN e RTM atualizados "de forma satisfatória". Seguimos o que o enunciado escreve: ISO 25010 e IDs `RNF01`, `RNF02`… A tabela do template usa eixos FURPS+ (USAB, CONF, DES…), então o responsável confirma com o professor se pode trocá-la (6.R2). Os 14 RNFs propostos no `planejamento-energia-gnss.md` servem de matéria-prima.

| # | Tópico (vira card) | Pronto quando | Critério da rubrica | Depende de | Prazo | Tamanho |
| --- | --- | --- | --- | --- | --- | --- |
| 6.R1 | Responsável: alinhar o artefato com o time | Enunciado e rubrica apresentados; cards divididos; os 4 cards de RNF (6.3 a 6.6) combinados para rodar em paralelo; ao longo da sprint, informações trocadas entre cards e cards criados ou excluídos quando preciso (checagem na sex 30/10) | Todos | — | Seg 26/10, na planning (divisão) · Ter 3/11 (último ajuste de cards) | M |
| 6.R2 | Responsável: levar ao professor o que o enunciado não diz | Respostas registradas no card: se a tabela FURPS+ do template pode virar as 8 características da ISO 25010; o que conta como atualização "satisfatória" de RF, RN e RTM; se a RTM deve ganhar coluna de RNF | Todos | 6.R1 | Qua 28/10 | P |
| 6.1 | Estrutura da tabela de RNFs | Tabela da 3.3.3 com as 8 características da ISO 25010 e IDs `RNF01`…`RNF08`, colunas de métrica e justificativa | Conforme a ISO 25010 | — | Ter 27/10 | P |
| 6.10 | Prints pendentes do Wokwi (dívida da Sprint 1) | Os prints marcados `PENDENTE DE EVIDÊNCIA` na RTM (CT01 a CT23, CT35) capturados em `assets/wokwi/` | RTM satisfatória | — | Qui 29/10 | M |
| 6.2 | Demandas de negócio que geram RNF | Lista ligando TAPI, kick-off e personas a cada característica (autonomia da bateria, operação sem cobertura, lacre, LGPD, BI) | Justificativa referenciando o negócio | 6.1 | Qua 28/10 | M |
| 6.3 | RNFs de Adequação funcional e Eficiência de desempenho | 2 RNFs com métrica verificável e justificativa | 8 RNFs diferentes | 6.2 | Sex 30/10 | M |
| 6.4 | RNFs de Compatibilidade e Usabilidade | Idem | 8 RNFs diferentes | 6.2 | Sex 30/10 | M |
| 6.5 | RNFs de Confiabilidade e Segurança | Idem, aproveitando RNF-CONF01, CONF02 e SEG01 do planejamento | 8 RNFs diferentes | 6.2 | Sex 30/10 | M |
| 6.6 | RNFs de Manutenibilidade e Portabilidade | Idem, aproveitando RNF-SUP01 e SUP02 | 8 RNFs diferentes | 6.2 | Sex 30/10 | M |
| 6.7 | Atualização de RF (3.3.1) e RN (3.3.2) | Status dos RFs verificados no protótipo físico; RF ou RN novos que o hardware revelou; `PENDENTE DE VALIDAÇÃO` resolvidos com o que o parceiro já respondeu; tabela de compatibilidade revista | RFs e RNs atualizados de forma satisfatória | 5.4, 5.7 | Ter 3/11 | G |
| 6.8 | Atualização da RTM (3.3.4) | CTs da 3.4.2 incluídos, RNFs ligados, linhas "CT a definir" revistas, prints da 6.10 no lugar dos pendentes | RTM atualizada de forma satisfatória | 5.6, 6.3 a 6.7, 6.10 | Qua 4/11 | G |
| 6.9 | Consistência cruzada | Cada RF, RN, RNF e CT citado existe e bate entre 3.3.1, 3.3.2, 3.3.3, 3.3.4 e 3.4.2; RFs novos vindos das oportunidades da jornada (7.5) incluídos; texto-guia removido | Atualização satisfatória | 6.8, 7.5 | Qua 4/11 | M |

### 3.3 Artefato 7 — Jornadas atual e futura e storyboard (seção 3.2.2)

O foco é a persona principal, Geraldo (analista de PCM). A avaliação é qualitativa: coerência, profundidade, contraste entre atual e futura, qualidade dos templates, insights e a descrição do processo de construção, incluindo o uso de IA. Os reports do kick-off ajudam a entender o fluxo, mas pela regra do grupo não podem ser citados como evidência.

| # | Tópico (vira card) | Pronto quando | Critério avaliado | Depende de | Prazo | Tamanho |
| --- | --- | --- | --- | --- | --- | --- |
| 7.R1 | Responsável: alinhar o artefato com o time | Enunciado apresentado; ferramenta escolhida (Claude Design, Figma ou Canva); cards divididos; o que roda em paralelo combinado (7.4 e 7.6 na sex 30/10); ao longo da sprint, informações trocadas entre cards e cards criados ou excluídos quando preciso | Todos | — | Seg 26/10, na planning (divisão) · Ter 3/11 (último ajuste de cards) | M |
| 7.R2 | Responsável: levar ao professor o que o enunciado não diz | Respostas registradas no card: quantidade esperada de cenas no storyboard, se as cenas podem ser geradas por IA, se as jornadas cobrem só a persona principal ou também as outras | Todos | 7.R1 | Qua 28/10 | P |
| 7.1 | Template das jornadas e fases comuns | Template montado na ferramenta escolhida; mesmas fases nas duas jornadas, com início e fim definidos, para o contraste ficar lado a lado | Qualidade dos templates | 7.R1 | Ter 27/10 | P |
| 7.2 | Insumos da persona | Dores, objetivos e situações do Geraldo tirados da 3.2.1, 3.1.3 e do fluxo de remessa para conserto | Relação com a persona | — | Ter 27/10 | M |
| 7.3 | Jornada atual | Fases, ações, pensamentos, sentimentos, touchpoints, curva emocional com picos e vales, oportunidades | Coerência e profundidade | 7.1, 7.2 | Qui 29/10 | G |
| 7.4 | Jornada futura | Mesma estrutura, mostrando como alertas, lacre e painel mudam cada fase | Contraste atual × futura | 7.3 | Sex 30/10 | G |
| 7.6 | Roteiro do storyboard | 6 a 8 cenas, cada uma com ambiente, circunstância, ação ou decisão do usuário e reação do sistema, coerente com os CTs (LEDs, alertas) | Sequência lógica | 7.3, 5.6 | Sex 30/10 | M |
| 7.5 | Insights e oportunidades | Texto curto com os insights e uma tabela de contraste por fase; oportunidades que pedem RF novo enviadas ao Art. 6 | Qualidade dos insights | 7.4 | Ter 3/11 | M |
| 7.7 | Ilustração e montagem do storyboard | Cenas desenhadas ou geradas, com estilo coerente com as personas, exportadas para `assets/` | Clareza visual | 7.6 | Qua 4/11 | GG |
| 7.8 | Texto da seção 3.2.2 | Figuras com legenda e fonte, descrição do processo de construção, parágrafo de uso de IA, texto-guia removido | Descrição do processo e do uso de IA | 7.5, 7.7 | Qua 4/11 | M |

### 3.4 Cards transversais — repositório, apresentação, revisão e merge

Estes cards não pertencem a um artefato. A apresentação é na sexta 6/11 de manhã, então tudo dela fecha na quinta, e o ensaio acontece depois da revisão contra a rubrica para os slides mostrarem a versão final.

| # | Tópico (vira card) | Pronto quando | Depende de | Prazo | Tamanho |
| --- | --- | --- | --- | --- | --- |
| X1 | Pastas e nomes dos assets da Sprint 2 | Pastas criadas (ex.: `assets/fisico/`, `assets/ux/`) e padrão de nomes combinado, para todo mundo salvar no lugar certo desde o início | — | Ter 27/10 | P |
| X2 | Limpeza de branches e MRs da Sprint 1 | Branches já mergeadas removidas do remoto, MRs abertos fechados ou mergeados, cards da Sprint 1 encerrados | — | Qua 28/10 | M |
| S1 | Reunião de sincronização do time | Cada equipe contou o que fez, o que falta e o que travou; tabela de dependências revista; cards trocados, divididos ou criados | Cards de todos os artefatos | Sex 30/10 (ou ter 3/11 de manhã) | P |
| X3 | README do repositório atualizado | Estrutura de pastas, como gravar o firmware no ESP32 físico, status da Sprint 2 | 5.4, X1 | Qua 4/11 | M |
| A1 | Roteiro da apresentação | Sequência, tempo por parte, o que cada artefato mostra e quem fala | Escopo dos artefatos (5.6, 7.3) | Sex 30/10 | M |
| A2 | Slides da apresentação | Slides no estilo da Sprint 1 com protótipo físico, RNFs e jornadas | A1, 5.7, 6.3 a 6.6, 7.4 | Qua 4/11 | GG |
| A3 | Vídeo e fotos do protótipo para a apresentação | Demo curta gravada (ou roteiro de demo ao vivo) e fotos boas do circuito | 5.7 | Ter 3/11 | M |
| R5 | Revisão do Art. 5 contra a rubrica | Feita por quem não trabalhou no Art. 5; cada exigência do Avançado marcada; o que faltar corrigido no dia | Todos os cards do Art. 5 | Qui 5/11 | M |
| R6 | Revisão do Art. 6 contra a rubrica | Idem para o Art. 6 | Todos os cards do Art. 6 | Qui 5/11 | M |
| R7 | Revisão do Art. 7 contra a rubrica | Idem para o Art. 7 | Todos os cards do Art. 7 | Qui 5/11 | M |
| M1 | Merge da develop na main e entrega | MRs revisados, develop mergeada na main, entrega feita | R5, R6, R7 | Qui 5/11 | P |
| A4 | Ensaio e versão final da apresentação | Slides ajustados ao que ficou depois da revisão; ensaio cronometrado | A2, A3, R5 a R7 | Qui 5/11 | M |

## 4. Dependências

O caminho crítico é o protótipo físico: 5.1 → 5.2 → 5.3 → 5.4 → 5.11 → 5.7. Ele não tem folga, e um dia de atraso na montagem atrasa a RTM (6.7, 6.8), o vídeo (A3) e os slides (A2).

Como ler: uma seta vai do card que entrega para o card que espera. A legenda fica abaixo de cada diagrama. Os arquivos `.mmd` desta pasta têm os mesmos diagramas para abrir no [Mermaid Live](https://mermaid.live).

### 4.1 Fluxo dos artefatos 5, 6 e 7

```mermaid
%%{init: {"theme": "base", "themeVariables": {"fontFamily": "Segoe UI, Arial, sans-serif", "primaryColor": "#D9CCF2", "primaryBorderColor": "#6B4A86", "primaryTextColor": "#182420", "lineColor": "#7D8A84", "clusterBkg": "#7D8A8414", "clusterBorder": "#7D8A84", "titleColor": "#7D8A84", "edgeLabelBackground": "#FFFFFF"}}}%%
flowchart LR
    %% Sprint 2 - fluxo de dependências entre os cards
    %% Linha cheia: dependência dentro do artefato. Tracejada: dependência entre artefatos.
    %% Verde: pode começar sem esperar outro card. Vermelho: caminho crítico (sem folga).

    subgraph ART5["Art. 5 — Protótipo físico (3.4.2)"]
        direction LR
        A5R1["5.R1 Alinhamento<br/>Seg 26/10"]
        A5R2["5.R2 Dúvidas ao professor<br/>Qua 28/10"]
        A5_1["5.1 Inventário<br/>Ter 27/10"]
        A5_2["5.2 Pinagem<br/>Qua 28/10"]
        A5_3["5.3 Montagem de cada versão<br/>Qua 28/10"]
        A5_5["5.5 Qualidade do código<br/>Qui 29/10"]
        A5_6["5.6 CTs escritos<br/>Qua 28/10"]
        A5_4["5.4 Firmware em cada versão<br/>Qui 29/10"]
        A5_11["5.11 Comparação e escolha<br/>Qui 29/10"]
        A5_7["5.7 Execução dos CTs<br/>Sex 30/10"]
        A5_8["5.8 Correções<br/>Ter 3/11"]
        A5_9["5.9 Texto 3.4.2<br/>Ter 3/11"]
        A5_10["5.10 CTs na RTM<br/>Qua 4/11"]
    end

    subgraph ART6["Art. 6 — RNF, RF, RN, RTM (3.3.x)"]
        direction LR
        A6R1["6.R1 Alinhamento<br/>Seg 26/10"]
        A6R2["6.R2 Dúvidas ao professor<br/>Qua 28/10"]
        A6_1["6.1 Estrutura da tabela RNF<br/>Ter 27/10"]
        A6_10["6.10 Prints Wokwi pendentes<br/>Qui 29/10"]
        A6_2["6.2 Demandas de negócio<br/>Qua 28/10"]
        A6_3["6.3 RNF Adequação + Desempenho<br/>Sex 30/10"]
        A6_4["6.4 RNF Compatibilidade + Usabilidade<br/>Sex 30/10"]
        A6_5["6.5 RNF Confiabilidade + Segurança<br/>Sex 30/10"]
        A6_6["6.6 RNF Manutenibilidade + Portabilidade<br/>Sex 30/10"]
        A6_7["6.7 RF e RN<br/>Ter 3/11"]
        A6_8["6.8 RTM<br/>Qua 4/11"]
        A6_9["6.9 Consistência cruzada<br/>Qua 4/11"]
    end

    subgraph ART7["Art. 7 — Jornadas e storyboard (3.2.2)"]
        direction LR
        A7R1["7.R1 Alinhamento + ferramenta<br/>Seg 26/10"]
        A7R2["7.R2 Dúvidas ao professor<br/>Qua 28/10"]
        A7_1["7.1 Template e fases<br/>Ter 27/10"]
        A7_2["7.2 Insumos da persona<br/>Ter 27/10"]
        A7_3["7.3 Jornada atual<br/>Qui 29/10"]
        A7_4["7.4 Jornada futura<br/>Sex 30/10"]
        A7_6["7.6 Roteiro do storyboard<br/>Sex 30/10"]
        A7_5["7.5 Insights e oportunidades<br/>Ter 3/11"]
        A7_7["7.7 Ilustração do storyboard<br/>Qua 4/11"]
        A7_8["7.8 Texto 3.2.2<br/>Qua 4/11"]
    end


    %% Art. 5
    A5R1 --> A5R2
    A5_1 -->A5_2
    A5_2 -->A5_3
    A5_3 -->A5_4
    A5_5 -->|mesmo main.cpp| A5_4
    A5_1 -->A5_6
    A5_4 -->A5_11
    A5_6 -->|CTs como base| A5_11
    A5_11 -->|versão escolhida| A5_7
    A5_6 -->A5_7
    A5_7 --> A5_8
    A5_7 -->A5_9
    A5_6 --> A5_10
    A6_8 -.->|mesma tabela| A5_10

    %% Art. 6
    A6R1 --> A6R2
    A6_1 --> A6_2
    A6_2 -->A6_3 & A6_4 & A6_5 & A6_6
    A6_3 & A6_4 & A6_5 & A6_6 --> A6_8
    A5_4 -.->|o que o hardware revelou| A6_7
    A5_7 -.->|status dos RFs| A6_7
    A6_7 --> A6_8
    A5_6 -.->|IDs dos CTs| A6_8
    A6_10 -->A6_8
    A6_8 --> A6_9
    A7_5 -.->|oportunidades viram RF| A6_9

    %% Art. 7
    A7R1 --> A7R2
    A7R1 -->A7_1
    A7_1 --> A7_3
    A7_2 --> A7_3
    A7_3 -->A7_4
    A7_4 --> A7_5
    A7_3 --> A7_6
    A5_6 -.->|reações do sistema| A7_6
    A7_6 --> A7_7
    A7_5 --> A7_8
    A7_7 --> A7_8
    classDef livre fill:#A8E6B8,stroke:#2E7D32,color:#182420,stroke-width:2px
    classDef critico fill:#F8B4B4,stroke:#C62828,color:#182420,stroke-width:3px
    class A5R1,A6R1,A7R1,A5_1,A5_5,A6_1,A6_10,A7_2 livre
    class A5_2,A5_3,A5_4,A5_11,A5_7 critico
    classDef espera fill:#D9CCF2,stroke:#6B4A86,color:#182420
    class A5R2,A5_6,A5_8,A5_9,A5_10,A6R2,A6_2,A6_3,A6_4,A6_5,A6_6,A6_7,A6_8,A6_9,A7R2,A7_1,A7_3,A7_4,A7_6,A7_5,A7_7,A7_8 espera
```

**Legenda**

| Símbolo | Significado |
| --- | --- |
| 🟩 Caixa verde | Pode começar sem esperar nenhum outro card |
| 🟥 Caixa com borda vermelha | Caminho crítico do protótipo, sem folga |
| 🟪 Caixa lilás | Card que espera outro card terminar |
| Seta cheia `→` | Dependência dentro do mesmo artefato |
| Seta tracejada `⇢` | Dependência entre artefatos; o texto na seta diz o que é entregue |
| Data na caixa | Prazo do card |

### 4.2 Fluxo de repositório, apresentação, revisão e entrega

```mermaid
%%{init: {"theme": "base", "themeVariables": {"fontFamily": "Segoe UI, Arial, sans-serif", "primaryColor": "#D9CCF2", "primaryBorderColor": "#6B4A86", "primaryTextColor": "#182420", "lineColor": "#7D8A84", "clusterBkg": "#7D8A8414", "clusterBorder": "#7D8A84", "titleColor": "#7D8A84", "edgeLabelBackground": "#FFFFFF"}}}%%
flowchart LR
    %% Sprint 2 - repositório, apresentação, revisão e entrega
    %% Retângulo cinza: card de artefato que alimenta o transversal.

    subgraph ENTRADAS["Vem dos artefatos"]
        direction TB
        E5_4["5.4 Firmware em cada versão<br/>Qui 29/10"]
        E5_6["5.6 CTs escritos<br/>Qua 28/10"]
        E7_3["7.3 Jornada atual<br/>Qui 29/10"]
        E5_7["5.7 Execução dos CTs<br/>Sex 30/10"]
        E6_RNF["6.3 a 6.6 RNFs<br/>Sex 30/10"]
        E7_4["7.4 Jornada futura<br/>Sex 30/10"]
        E5F["Art. 5 completo<br/>5.8, 5.9, 5.10"]
        E6F["Art. 6 completo<br/>6.9"]
        E7F["Art. 7 completo<br/>7.8"]
    end

    X1["X1 Pastas e nomes de assets<br/>Ter 27/10"]
    X2["X2 Limpeza de branches e MRs<br/>Qua 28/10"]
    S1["S1 Reunião de sincronização<br/>Sex 30/10"]
    X3["X3 README<br/>Qua 4/11"]
    AP1["A1 Roteiro da apresentação<br/>Sex 30/10"]
    AP3["A3 Vídeo do protótipo<br/>Ter 3/11"]
    AP2["A2 Slides<br/>Qua 4/11"]
    R5["R5 Revisão Art. 5 x rubrica<br/>Qui 5/11"]
    R6["R6 Revisão Art. 6 x rubrica<br/>Qui 5/11"]
    R7["R7 Revisão Art. 7 x rubrica<br/>Qui 5/11"]
    M1["M1 Merge develop → main<br/>Qui 5/11"]
    AP4["A4 Ensaio e versão final<br/>Qui 5/11"]
    APR(["Apresentação<br/>Sex 6/11 de manhã"])

    X1 --> X3
    E5_4 -.->|como gravar o firmware| X3
    E5_6 -.-> AP1
    E7_3 -.-> AP1
    AP1 --> AP2
    E5_7 -.-> AP2
    E6_RNF -.-> AP2
    E7_4 -.-> AP2
    E5_7 -.-> AP3
    E5F -.-> R5
    E6F -.-> R6
    E7F -.-> R7
    R5 & R6 & R7 --> M1
    R5 & R6 & R7 --> AP4
    AP2 & AP3 --> AP4
    AP4 --> APR
    classDef livre fill:#A8E6B8,stroke:#2E7D32,color:#182420,stroke-width:2px
    classDef fim fill:#FFD59E,stroke:#E08A00,color:#182420,stroke-width:2px
    classDef entrada fill:#E0E0E0,stroke:#8A8A8A,color:#333333
    class X1,X2,S1 livre
    class R5,R6,R7,M1,AP4,APR fim
    class E5_4,E5_6,E7_3,E5_7,E6_RNF,E7_4,E5F,E6F,E7F entrada
    classDef espera fill:#D9CCF2,stroke:#6B4A86,color:#182420
    class X3,AP1,AP3,AP2 espera
```

**Legenda**

| Símbolo | Significado |
| --- | --- |
| ⬜ Caixa cinza (coluna "Vem dos artefatos") | Card de um artefato que alimenta um card transversal |
| 🟩 Caixa verde | Pode começar sem esperar nenhum outro card |
| 🟪 Caixa lilás | Card transversal que espera outro card |
| 🟧 Caixa laranja | Revisão contra a rubrica, merge e ensaio, todos na qui 5/11 |
| ⬭ Forma arredondada | Apresentação, sex 6/11 de manhã |
| Seta cheia `→` | Dependência entre cards transversais |
| Seta tracejada `⇢` | Card de artefato alimentando um transversal |

**Podem começar já na ter 27/10, sem esperar ninguém:** 5.1 Inventário, 5.5 Qualidade do código, 6.1 Estrutura da tabela de RNFs, 6.10 Prints pendentes do Wokwi, 7.2 Insumos da persona, X1 Pastas e nomes de assets, X2 Limpeza de branches e MRs. Os cards 7.1 e os R2 só precisam do alinhamento feito na planning.

### 4.3 Por que cada card espera e quanto sobra de folga

Cada linha diz quem espera, por quê e quanto tempo sobra entre a entrega e o uso.

| Card que espera | Espera por | Por que depende | O que precisa chegar | Entregue até | Usado a partir de | Folga |
| --- | --- | --- | --- | --- | --- | --- |
| 5.2 Pinagem | 5.1 Inventário | Só dá para desenhar o esquema com os componentes que o grupo tem | Lista de componentes | Ter 27/10 | Qua 28/10 | 0 |
| 5.3 Montagem | 5.2 Pinagem | Todas as versões seguem o mesmo esquema, para a comparação ser justa | Esquema elétrico | Qua 28/10 (manhã) | Qua 28/10 (tarde) | Mesmo dia |
| 5.4 Firmware no hardware | 5.3 Montagem | O firmware só lê sensores reais com o circuito montado | Protoboard montado | Qua 28/10 | Qui 29/10 | 0 |
| 5.4 Firmware no hardware | 5.5 Qualidade do código | Os dois mexem no `main.cpp`; refatorar depois geraria conflito e evidências de código antigo | Refatoração mergeada | Qui 29/10 (manhã) | Qui 29/10 (tarde) | Mesmo dia |
| 5.11 Comparação e escolha | 5.4 e 5.6 | Só dá para comparar versões que já rodam o firmware; os CTs escritos servem de roteiro da comparação | Firmware gravado em cada versão e tabela de CTs | Qui 29/10 (manhã) | Qui 29/10 (tarde) | Mesmo dia |
| 5.7 Execução dos CTs | 5.11 e 5.6 | As evidências saem da versão escolhida, com os CTs escritos | Versão escolhida e tabela de CTs | Qui 29/10 | Sex 30/10 | 0 |
| 6.7 RF e RN | 5.4 e 5.7 | Um RF só muda de status depois que o CT físico passou | Resultado dos CTs | Sex 30/10 | Ter 3/11 | 0 |
| 6.3 a 6.6 RNFs | 6.2 Demandas | A justificativa de cada RNF cita uma demanda de negócio | Lista de demandas | Qua 28/10 | Qui 29/10 | 0 |
| 6.8 RTM e 5.10 | 5.6 CTs escritos | A RTM liga cada CT a RF e RN, então precisa dos IDs | Numeração dos CTs | Qua 28/10 | Ter 3/11 | 2 dias |
| 6.8 RTM | 6.3 a 6.6 e 6.10 | A RTM liga os RNFs e troca os prints pendentes | RNFs escritos e prints | Sex 30/10 | Ter 3/11 | 0 |
| 7.4 Jornada futura | 7.3 Jornada atual | A futura usa as mesmas fases para o contraste aparecer | Jornada atual | Qui 29/10 | Sex 30/10 | 0 |
| 7.6 Roteiro do storyboard | 5.6 CTs escritos | As reações do sistema no storyboard têm de ser as mesmas dos CTs (vermelho na violação, amarelo no atraso) | Tabela de CTs | Qua 28/10 | Sex 30/10 | 1 dia |
| 6.9 Consistência | 7.5 Insights | Oportunidades da jornada futura podem virar RF novo | Lista de oportunidades | Ter 3/11 | Qua 4/11 | 0 |
| A2 Slides | 5.7, 6.3 a 6.6, 7.4 | Os slides mostram conteúdo real, não rascunho | Resultados, RNFs e jornada futura | Sex 30/10 | Ter 3/11 | 0 |
| A3 Vídeo do protótipo | 5.7 Execução | A demo mostra o protótipo já testado | CTs executados | Sex 30/10 | Ter 3/11 | 0 |
| R5 a R7 Revisão | Todos os cards do artefato | A revisão confere a versão final contra a rubrica | Tudo na develop | Qua 4/11 | Qui 5/11 | 0 |
| M1 Merge e A4 Ensaio | R5 a R7 | Merge e slides finais só depois das correções da revisão | Revisão concluída | Qui 5/11 (manhã) | Qui 5/11 (tarde) | Mesmo dia |

Como 3.2.2, 3.3.x e 3.4.2 ficam no mesmo `documentacao.md`, cada card edita só a sua seção e o MR é pequeno. Os responsáveis olham esta tabela na reunião de sex 30/10: se algum "entregue até" escorregou, é ali que se troca ou divide card.

## 5. Como a sprint roda

### 5.1 Planning — seg 26/10

Na planning cada responsável já faz o alinhamento do seu artefato (5.R1, 6.R1 e 7.R1): cada equipe sai com os próprios cards, e o build começa no dia seguinte. São cerca de 2 horas.

1. **Abertura, 15 min.** Apresentar este documento: calendário, prazo da quinta 5/11 e caminho crítico do protótipo. As tabelas são ponto de partida, não os cards definitivos.
2. **Equipes, 5 min.** Definir os 3 responsáveis e as equipes. Sugestão: 3 pessoas no Art. 5, que tem os cards mais pesados e nenhuma folga, e 2 em cada um dos outros.
3. **Trabalho por equipe, 40 min.** Cada equipe lê enunciado e rubrica, ajusta os tópicos e cria os próprios cards com tamanho e prazo. Cada uma preenche duas listas: "o que espero de outro artefato e até quando" e "o que entrego para outro artefato e até quando". A tabela abaixo serve de ponto de partida.
4. **Rodada geral, 30 min.** Cada responsável apresenta as duas listas em 5 minutos. Onde o "preciso até" de uma equipe cai antes do "entrego até" de outra, resolve-se ali. No fim, distribuir os cards transversais (repositório, apresentação, revisores da quinta).
5. **Fechamento, 10 min.** Cards no GitLab até o fim do dia; reunião de sincronização na sex 30/10.

#### Sugestão para as duas listas

Ponto de partida para cada equipe conferir as listas que montar no bloco 3 da planning. Foi tirado da tabela 4.3.

| Artefato | O que espera de outro artefato | Até quando | O que entrega para outro artefato | Até quando |
| --- | --- | --- | --- | --- |
| Art. 5 — Protótipo físico | Nada de outro artefato; só as linhas dos seus CTs na RTM, feitas junto com o Art. 6 (6.8) | Qua 4/11 | Ao Art. 6: numeração e tabela dos CTs (5.6) | Qua 28/10 |
| | | | Ao Art. 7: reações do sistema em cada CT, para o storyboard (5.6) | Qua 28/10 |
| | | | Ao Art. 6 e à apresentação: versão escolhida (5.11) e resultados dos CTs, fotos e vídeos (5.7) | Sex 30/10 |
| | | | Ao README (X3): como gravar o firmware no ESP32 físico (5.4) | Qui 29/10 |
| Art. 6 — RNF, RF, RN, RTM | Do Art. 5: numeração dos CTs (5.6) | Qua 28/10 | Ao Art. 5: linhas dos CTs físicos na RTM (6.8) | Qua 4/11 |
| | Do Art. 5: resultado dos CTs, para mudar o status dos RFs (5.7) | Sex 30/10 | À apresentação: os 8 RNFs (6.3 a 6.6) | Sex 30/10 |
| | Do Art. 7: oportunidades que viram RF novo (7.5) | Ter 3/11 | | |
| Art. 7 — Jornadas e storyboard | Do Art. 5: reações do sistema em cada CT (5.6) | Qua 28/10 | Ao Art. 6: oportunidades que pedem RF novo (7.5) | Ter 3/11 |
| | | | À apresentação: jornada futura (7.4) | Sex 30/10 |

### 5.2 Revisões ao longo da sprint

As revisões acontecem ao longo da sprint, card a card, e não se acumulam na quinta.

1. Quem termina um card abre o MR, move o card para **Waiting Review** e avisa no grupo.
2. Quem estiver livre pega a revisão, de preferência alguém da mesma equipe, que conhece o conteúdo. Se o card entrega algo para outro artefato (CTs, oportunidades), revisa alguém da equipe que vai usar.
3. A revisão sai em até 1 dia útil e confere o "Pronto quando" e o critério da rubrica do card, não só o texto. O autor corrige, e o MR entra na develop.
4. Ninguém revisa o próprio card.

### 5.3 Reunião de sincronização — sex 30/10

Cada equipe conta, em 5 a 10 minutos, o que fez, o que falta e o que ficou travado, para o time inteiro entender os outros artefatos. A sexta é o meio da sprint e o último dia antes do feriado; se não der, a reunião passa para ter 3/11 de manhã. Ela termina com a tabela de dependências revista: o que escorregou vira troca, divisão ou novo card. É o card S1.

### 5.4 Revisão final e entrega — qui 5/11

Na quinta, cada artefato é revisado por quem não trabalhou nele, com a rubrica aberta, marcando cada exigência do nível Avançado. O que faltar vira ajuste no mesmo dia, antes do merge. Como tudo já foi revisado card a card (5.2), a quinta só confere o artefato inteiro. Depois do merge, o ensaio (A4) fecha a apresentação com a versão final.

## 6. Decisões da planning

| Tema | Decisão | Onde entrou |
| --- | --- | --- |
| Calendário | Seg 26/10 só planning; build até qua 4/11; entrega e merge na qui 5/11; apresentação sex 6/11 de manhã | Calendário |
| RNF | Seguir o enunciado (ISO 25010, `RNF01`…); o que ele não diz vai ao professor | 6.1, 6.R2 |
| Componentes do protótipo | Em aberto | 5.1 e 5.R2 |
| Protótipo físico | Cada integrante do Art. 5 monta a própria versão; o grupo compara as versões e escolhe a melhor, que vai para os CTs e a 3.4.2 | 5.3, 5.4, 5.11 |
| Numeração de CT | Em aberto; o texto-guia da 3.4.2 sugere continuar no CT36 | 5.R2 |
| Prints pendentes do Wokwi | Entra na sprint | 6.10 |
| Ferramenta das jornadas | O responsável decide (Claude Design, Figma ou Canva) | 7.R1 |
| Revisões | Contínuas, card a card; a quinta é só a conferência final contra a rubrica | Como funcionam as revisões |
