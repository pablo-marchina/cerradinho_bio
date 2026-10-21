# Arthur - Personas do fluxo de remessa para conserto

**Autor:** Arthur Loyola (@arthur.loyola)

> **Pergunta investigada:** quem são as pessoas que tocam o ciclo de saída e retorno de um equipamento, o que cada uma precisa da solução e quem é o usuário principal.
> **O que foi feito:** leitura das respostas do parceiro às perguntas sobre pessoas no kick-off (pergunta do G01 em [34:02–37:30] e pergunta do G03 em [56:22–57:30]), cruzadas com o fluxo do relatório 02 e com as falas sobre alertas e canais. Depois, síntese das três personas principais com apoio de IA (NotebookLM) e preenchimento dos campos do template (demografia, letramento digital e cenários), com o uso de IA descrito na seção 6.
> **Evidência:** transcrição do kick-off (16/10/2026). Os nomes das personas são fictícios; os papéis e as falas são do parceiro.
> **Em aberto:** validar as personas com o parceiro na primeira review; confirmar quem cola a TAG/lacre no barracão; saber se o coordenador da área recebe o alerta de prazo hoje ou só o requisitante; entrevistar um analista de PCM real e validar demografia e letramento digital, que hoje são hipóteses sem entrevista; discutir com o parceiro a ideia do aviso de retorno na portaria (seção 3.2).
> **Alimenta:** personas e jornada do usuário na `documentacao.md`; prioridade das telas do dashboard e dos destinatários de alerta.
> **Implicação:** o usuário principal é o **analista de PCM**, não o fiscal nem a gestão patrimonial. A interface e os alertas devem ser desenhados para quem cobra o fornecedor e decide sobre o atraso; fiscal e diretoria são consumidores de alerta e de relatório.

## 1. O que o parceiro respondeu

- "O dono do equipamento é o PCM." O mecânico abre a OM, o analista de PCM cota o fornecedor, abre a solicitação de manutenção externa, entrega o item à transportadora e cobra o fornecedor. Em atraso, **é o analista de PCM quem decide** trazer de volta ou colocar sobressalente [34:28–36:03].
- A NF é emitida pelo **fiscal**; o PCM imprime e junta ao ativo [35:13].
- Existe régua automática de cobrança por e-mail: requisitante, depois coordenador, depois diretor da área [36:09].
- "Tudo que é físico fica com o PCM", inclusive colocar o lacre, porque o fiscal não tem contato com o equipamento [56:44].
- O analista de logística acompanha hoje as remessas e o BI para a diretoria "por senso de dono", mas diz que a cobrança **não é papel da área dele** [50:16–51:41].
- Os alertas devem ir também para o fiscal e para a área de negócio [1:26:51]; por e-mail ou Teams, para e-mails de setor ou encarregados de turno, nunca fora do horário [1:27:12–1:28:30].
- O lacre rompido precisa alertar quem é responsável [57:14].

## 2. Tese e fluxo do ciclo pelas personas

**Tese do grupo** (síntese nossa, não fala do parceiro): um painel bonito desenhado para quem paga a conta não resolve, porque o software não aperta os próprios botões. A solução é desenhada para quem opera o ciclo no dia a dia, o analista de PCM, e para quem hoje não enxerga o físico, o analista fiscal. Diretoria e gestão recebem os relatórios pelo BI, via API, sem telas operacionais próprias.

| # | Etapa | Quem | O que acontece |
|---|---|---|---|
| 1 | Abertura | Joel, mecânico | Abre a OM no SAP e leva o item ao barracão. |
| 2 | Preparação da saída | Geraldo, PCM | Avalia, cota e escolhe a oficina, vincula a TAG, aplica o lacre e entrega à transportadora. |
| 3 | Remessa fiscal | Clarice, fiscal | Emite a NF de remessa; começa a suspensão do ICMS por 270 ou 360 dias. |
| 4 | Retorno físico | Portaria | Hoje o item pode entrar sem o fiscal saber. Ideia do grupo: aviso automático quando a TAG passa pela portaria. |
| 5 | Encerramento | Geraldo e Clarice | Conferência do item e lançamento da NF de retorno; só então o ciclo fecha. |
| — | Consolidação | Izidio, logística | Consome o histórico dos ciclos para o BI da diretoria. |

## 3. Personas

As três primeiras (Geraldo, Clarice e Izidio) são as personas do Artefato 2. Helena e Joel ficam como atores do fluxo (seções 3.4 e 3.5).

**Por que não as hipóteses do planning.** O planning da Sprint 1 partia de P1 Gestão Patrimonial/Controladoria e P2 Manutenção. O kick-off mostrou outra divisão: "o dono do equipamento é o PCM" [34:28], é o PCM quem decide em caso de atraso [34:28–36:03], o fiscal emite e lança as notas sem contato com o equipamento [56:44], e a gestão patrimonial só consome o histórico (seção 4). Por isso o PCM vira a persona principal, o fiscal a secundária, e Gestão Patrimonial passa a ator.

**Campos que não vêm do kick-off.** O parceiro descreveu papéis, ferramentas, dores e necessidades, mas não falou de idade, formação nem de familiaridade com tecnologia. Por isso **demografia e letramento digital estão marcados como hipótese** e foram inferidos do papel e das ferramentas citadas; precisam de validação com o parceiro. Os cenários são do grupo, escritos a partir do fluxo e das falas, e usam apenas o que o produto planeja fazer.

**Escala de letramento digital (1 a 5).**

| Nível | Critério |
|---|---|
| 1 | Usa telefone e papel; depende de outra pessoa para qualquer sistema. |
| 2 | Usa e-mail e mensagens; usa sistema corporativo só com roteiro passo a passo. |
| 3 | Usa um sistema corporativo no dia a dia (SAP) e planilhas; resolve o rotineiro sozinho e pede ajuda no fora do comum. |
| 4 | Navega bem em vários sistemas, filtra, cruza dados e emite relatórios. |
| 5 | Integra sistemas, automatiza rotinas ou consome dados por API. |

### 3.1 Persona principal: Geraldo, analista de PCM

| | |
|---|---|
| **Papel** | Planejamento e Controle da Manutenção. Dono do ativo enquanto ele está fora. |
| **No fluxo** | Recebe o item quebrado, cota o fornecedor, abre a solicitação no SAP, entrega à transportadora, vincula e lacra a TAG, cobra o fornecedor, decide em caso de atraso, confirma o retorno. |
| **Ferramentas hoje** | SAP, planilha ou lista no SharePoint da área (quando existe), telefone e e-mail com fornecedores. |
| **Objetivos** | Saber, sem ligar para ninguém, onde está cada item e quando volta; ser avisado antes do prazo estourar; ter um histórico para defender a decisão de trocar de fornecedor. |
| **Dores** | Muitos itens ao mesmo tempo; o não crítico some da memória; o fornecedor promete data e não cumpre; não sabe se o fornecedor mandou o item para outra unidade; sem dados, é difícil decidir entre trazer o item de volta, colocar sobressalente ou pedir prorrogação. |
| **O que precisa da solução** | Lista de "meus itens fora" ordenada por dias para vencer; alerta por e-mail com antecedência configurável (ex.: 30 dias para item urgente); alerta imediato de lacre; última localização e se está no endereço da NF; registro de abertura autorizada do lacre; histórico de cumprimento de prazo por fornecedor, para decidir se renova ou troca de oficina. |
| **Expectativas** | Que o sistema lhe diga, sem ligações, onde está cada item e quando volta, e que o avise antes do prazo estourar, para decidir com dados e não de memória. |
| **O que é sucesso para ela** | Nenhum item sai do radar: todo atraso é visto a tempo de trazer de volta, colocar sobressalente ou prorrogar. |
| **Demografia (hipótese)** | Fonte: papel e área (Manutenção, barracão da usina, dono do ativo). Hipótese do grupo, sem fonte do parceiro: formação técnica ou em engenharia; faixa etária e tempo de casa a validar. |
| **Letramento digital (hipótese)** | Nível 3. Usa SAP, planilha ou lista no SharePoint, e-mail e telefone [14:35, 34:28], mas controla itens fora por planilha e memória. |
| **Frase** | "Pode se perder numa planilha, pode esquecer que esse equipamento tá aqui, principalmente se não for um ativo tão crítico." [14:35] |

**Cenários de Geraldo.**

- **P1-C1, saída com TAG e lacre:** Geraldo recebe o item quebrado, escolhe a oficina, abre o ciclo com NF, fornecedor, data de saída e prazo, vincula a TAG e aplica o lacre.
- **P1-C2, acompanhamento:** abre "meus itens fora", vê a lista ordenada por dias para vencer, recebe o alerta com antecedência configurável e decide entre cobrar, trazer de volta, colocar sobressalente ou pedir prorrogação.
- **P1-C3, lacre aberto:** recebe o alerta imediato de lacre aberto; confere se foi retirada autorizada pelo fornecedor ou violação e registra o tratamento.
- **P1-C4, falha:** o equipamento fica sem posição válida ou o alerta parece falso; Geraldo vê a última posição válida e o horário e decide se aciona o fornecedor.

### 3.2 Persona secundária: Clarice, analista fiscal

| | |
|---|---|
| **Papel** | Emite a NF de remessa e lança o retorno; paga complemento de ICMS quando o prazo legal estoura. |
| **No fluxo** | Etapas de emissão da NF, retorno simbólico e prorrogação, lançamento da NF de retorno. |
| **Objetivos** | Nenhum item passar de 270/360 dias sem decisão; nenhum item voltar "por fora" sem lançamento. |
| **Dores** | Não tem contato com o equipamento; descobre o atraso pelo relatório, não pelo campo; item que voltou sem passar no fiscal deixa a remessa aberta; quando o prazo estoura, a empresa paga complemento de ICMS (R$ 602.969 na safra 24/25, segundo o TAPI). |
| **O que precisa da solução** | Aviso quando o item entra de volta na usina, para cobrar o lançamento (ideia do grupo: detectar a TAG na portaria, ainda não discutida com o parceiro); ciclo que só encerra com o retorno lançado; NF, nº da OS e nº da TAG ligados no mesmo registro. |
| **Ferramentas hoje (hipótese)** | Sistema de emissão de NF e portal da SEFAZ; o parceiro não detalhou as demais [35:13]. |
| **Expectativas** | Saber do retorno físico sem depender do PCM e ter TAG, NF e OS no mesmo registro, para fechar a remessa a tempo. |
| **O que é sucesso para ela** | Zero complemento de ICMS por prazo vencido sem baixa: todo ciclo fecha com a NF de retorno lançada. |
| **Demografia (hipótese)** | Fonte: papel e área (Fiscal, relação com a SEFAZ). Hipótese do grupo, sem fonte do parceiro: formação em ciências contábeis ou áreas afins; faixa etária e tempo de casa a validar. |
| **Letramento digital (hipótese)** | Nível 4. Emite NF eletrônica e lança retorno em sistema fiscal [35:13, 48:27]; a hipótese é de que cruza dados e relatórios com facilidade. |
| **Frase** | "O material tá na Cerradinho, ok, mas o processo como um todo não foi finalizado." [48:27] |

**Cenários de Clarice.**

- **P2-C1, retorno sem aviso:** o item volta à usina sem passar no fiscal; Clarice recebe o aviso de que a TAG do equipamento entrou (ideia do grupo, ainda não discutida com o parceiro) e cobra o lançamento da NF de retorno.
- **P2-C2, encerramento do ciclo:** com o item conferido, lança a NF de retorno; o ciclo só fecha com esse lançamento e a TAG fica livre para outro ciclo.
- **P2-C3, prazo legal:** recebe o alerta de que o prazo de 270 ou 360 dias está perto de vencer e decide por prorrogação ou retorno simbólico antes do complemento de ICMS.
- **P2-C4, falha:** o aviso de retorno não chega por falta de conexão; o evento fica guardado e sai depois, e o ciclo continua aberto até o lançamento.

### 3.3 Persona de inteligência: Izidio, analista de logística (acompanhamento de remessas)

| | |
|---|---|
| **Papel** | Consolida tudo o que saiu e voltou e reporta à diretoria pelo BI. Hoje faz o follow-up "por senso de dono". |
| **Objetivos** | Mostrar a evolução do ICMS por safra; manter na base todo item que não voltou, mesmo com ICMS já pago. |
| **Dores** | Dados espalhados entre SAP e planilhas das áreas; não consegue dizer agora quantas remessas saíram na safra; cobra oficinas e prazos manualmente, uma tarefa operacional que não é da área dele [50:16–51:41]. |
| **O que precisa da solução** | API para alimentar o BI; histórico de ciclos com datas de saída, previsto e retorno; permanência e atraso calculados separadamente. |
| **Ferramentas hoje** | SAP, planilhas das áreas e BI para a diretoria [50:16–51:41]. |
| **Expectativas** | Receber os ciclos prontos por API no BI, sem montar planilha à mão nem cobrar oficina fora do papel da área. |
| **O que é sucesso para ela** | Mostrar à diretoria a evolução do ICMS por safra e os gargalos de oficina com dados confiáveis e sem trabalho manual. |
| **Demografia (hipótese)** | Fonte: papel e área (Logística, acompanhamento de remessas, relatórios à diretoria). Hipótese do grupo, sem fonte do parceiro: formação em logística, engenharia ou administração; faixa etária e tempo de casa a validar. |
| **Letramento digital (hipótese)** | Nível 4. Trabalha com SAP, planilhas das áreas e BI [50:16–51:41]; a hipótese é de que consome dados por relatório e por integração. |
| **Frase** | "Eu só vou dar baixa nele quando ele realmente retornou." [53:16] |

**Cenários de Izidio.**

- **P3-C1, histórico para o BI:** o BI consulta a API e recebe os ciclos com datas de saída, previsão e retorno, sem Izidio digitar nada.
- **P3-C2, evolução do ICMS por safra:** monta o relatório de permanência em oficina e de atraso por fornecedor, separados, e apresenta à diretoria.
- **P3-C3, falha:** um ciclo sem retorno lançado aparece como pendente, não como encerrado; Izidio avisa o fiscal em vez de cobrar a oficina.

### 3.4 Ator de decisão: Helena, coordenadora/diretora da área requisitante

| | |
|---|---|
| **Papel** | Recebe o escalonamento quando o item se aproxima do vencimento. Responde pelo custo do ICMS e pelas perdas da área. |
| **O que precisa da solução** | Poucos alertas, só os escalonados (coordenador entre 31 e 61 dias para vencer, diretor entre 1 e 30, conforme a régua atual [36:26]); visão consolidada por área e por fornecedor no BI. |

### 3.5 Ator de apoio: Joel, mecânico (manutentor)

Abre a OM no SAP e leva o item ao barracão [34:41]. Não usa o painel. Pode ser quem prende a TAG, se o PCM delegar (a confirmar).

## 4. Atores externos (não são usuários do sistema)

| Ator | Relação com a solução |
|---|---|
| Transportadora (>90% terceirizada) | Leva o item; não tem internet garantida no caminhão. O dispositivo precisa guardar e enviar depois. |
| Fornecedor de manutenção | Pode abrir o lacre para reparar, mudar o item de unidade ou terceirizar o serviço. Gera eventos legítimos que não podem virar alerta falso. |
| TI | Libera ferramentas e integrações (Python, BI, SharePoint); qualquer outra passa por avaliação de cyber. |
| Gestão patrimonial e contabilidade | Consomem o histórico para extravios e imobilizado; não operam o ciclo no dia a dia. |

## 5. Quem usa, quem consulta, quem decide

| | Cadastra a saída | Aplica o lacre | Cobra o fornecedor | Recebe alerta | Consulta | Decide |
|---|---|---|---|---|---|---|
| Analista de PCM | Sim | Sim | Sim | Prazo, lacre, cerca | Painel | Atraso, sobressalente, prorrogação |
| Analista fiscal | Emite NF | Não | Não | Retorno sem lançamento, prazo legal | Painel/BI | Complemento de ICMS |
| Analista de logística | Não | Não | Hoje sim (informal) | Resumo | BI via API | Não |
| Coordenação/diretoria | Aprova | Não | Não | Escalonamento | BI | Prioridade e custo |
| Mecânico | Abre OM | Talvez | Não | Não | Não | Não |

## 6. Como as personas foram construídas e o uso de IA

**Fontes.** Transcrição do kick-off de 16/10/2026, os reports 01 e 02, o planning da Sprint 1 e o TAPI. Não houve entrevista com usuários; por isso demografia e letramento digital são hipóteses e as falas citadas são só do parceiro, com minuto da transcrição.

**Etapas e onde a IA entrou.**

1. **Reports do kick-off:** o Arthur transcreveu o encontro e usou uma ferramenta de IA para organizar a transcrição em três reports (problemática, fluxo e personas), com o minuto aproximado de cada fala.
2. **Síntese no NotebookLM:** os reports foram carregados no NotebookLM, que gerou um podcast de cerca de 20 minutos e, com o Arthur, um texto de síntese das três personas. A IA ajudou a sintetizar: a tese ("desenhar para quem opera o ciclo"), o fluxo do ciclo pelas pessoas e, por persona, o papel, as dores e as necessidades.
3. **Template e cenários:** os campos de demografia, letramento digital e cenários foram escritos a partir do fluxo e das falas, com a escala da seção 3; a IA ajudou a redigir e organizar, e o grupo marcou o que é hipótese.
4. **Identidade visual:** a apresentação visual das personas foi feita com apoio de IA, descrita em `05-Processo-com-IA.md`.

**Como o conteúdo foi conferido.**

- Toda dor e necessidade vem de uma fala do parceiro com o minuto da transcrição; o que o texto do NotebookLM trouxe sem fala correspondente do parceiro aparece rotulado como ideia ou hipótese do grupo.
- O que é ideia do grupo está dito como ideia (aviso de retorno na portaria) e o que é inferência está dito como hipótese (demografia, letramento digital).
- Os nomes das personas são fictícios e nenhum nome real do parceiro ou de fornecedor aparece.
- O material passou por revisão no merge request (Pablo, !4). Falta validar as personas com o parceiro na review.

**Limites.** A IA não foi fonte de dados do parceiro: ela organizou e redigiu o que veio da transcrição. Qualquer número ou fala deve ser conferido na transcrição, não no podcast.
