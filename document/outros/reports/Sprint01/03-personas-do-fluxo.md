# Arthur - Personas do fluxo de remessa para conserto

**Autor:** Arthur Loyola (@arthur.loyola)

> **Pergunta investigada:** quem são as pessoas que tocam o ciclo de saída e retorno de um equipamento, o que cada uma precisa da solução e quem é o usuário principal.
> **O que foi feito:** leitura das respostas do parceiro às perguntas sobre pessoas no kick-off (pergunta do G01 em [34:02–37:30] e pergunta do G03 em [56:22–57:30]), cruzadas com o fluxo do relatório 02 e com as falas sobre alertas e canais.
> **Evidência:** transcrição do kick-off (16/10/2026). Os nomes das personas são fictícios; os papéis e as falas são do parceiro.
> **Em aberto:** validar as personas com o parceiro na primeira review; confirmar quem cola a TAG/lacre no barracão; saber se o coordenador da área recebe o alerta de prazo hoje ou só o requisitante; entrevistar um analista de PCM real.
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

## 2. Personas

### 2.1 Persona principal: Geraldo, analista de PCM

| | |
|---|---|
| **Papel** | Planejamento e Controle da Manutenção. Dono do ativo enquanto ele está fora. |
| **No fluxo** | Recebe o item quebrado, cota o fornecedor, abre a solicitação no SAP, entrega à transportadora, vincula e lacra a TAG, cobra o fornecedor, decide em caso de atraso, confirma o retorno. |
| **Ferramentas hoje** | SAP, planilha ou lista no SharePoint da área (quando existe), telefone e e-mail com fornecedores. |
| **Objetivos** | Saber, sem ligar para ninguém, onde está cada item e quando volta; ser avisado antes do prazo estourar; ter um histórico para defender a decisão de trocar de fornecedor. |
| **Dores** | Muitos itens ao mesmo tempo; o não crítico some da memória; o fornecedor promete data e não cumpre; não sabe se o fornecedor mandou o item para outra unidade. |
| **O que precisa da solução** | Lista de "meus itens fora" ordenada por dias para vencer; alerta por e-mail com antecedência configurável (ex.: 30 dias para item urgente); alerta imediato de lacre; última localização e se está no endereço da NF; registro de abertura autorizada do lacre. |
| **Frase** | "Pode se perder numa planilha, pode esquecer que esse equipamento tá aqui, principalmente se não for um ativo tão crítico." [14:35] |

### 2.2 Persona secundária: Clarice, analista fiscal

| | |
|---|---|
| **Papel** | Emite a NF de remessa e lança o retorno; paga complemento de ICMS quando o prazo legal estoura. |
| **No fluxo** | Etapas de emissão da NF, retorno simbólico e prorrogação, lançamento da NF de retorno. |
| **Objetivos** | Nenhum item passar de 270/360 dias sem decisão; nenhum item voltar "por fora" sem lançamento. |
| **Dores** | Não tem contato com o equipamento; descobre o atraso pelo relatório, não pelo campo; item que voltou sem passar no fiscal deixa a remessa aberta. |
| **O que precisa da solução** | Alerta quando o item entra de volta na cerca da usina, para cobrar o lançamento; ciclo que só encerra com o retorno lançado; NF, nº da OS e nº da TAG ligados no mesmo registro. |
| **Frase** | "O material tá na Cerradinho, ok, mas o processo como um todo não foi finalizado." [48:27] |

### 2.3 Persona secundária: Izidio, analista de logística (acompanhamento de remessas)

| | |
|---|---|
| **Papel** | Consolida tudo o que saiu e voltou e reporta à diretoria pelo BI. Hoje faz o follow-up "por senso de dono". |
| **Objetivos** | Mostrar a evolução do ICMS por safra; manter na base todo item que não voltou, mesmo com ICMS já pago. |
| **Dores** | Dados espalhados entre SAP e planilhas das áreas; não consegue dizer agora quantas remessas saíram na safra. |
| **O que precisa da solução** | API para alimentar o BI; histórico de ciclos com datas de saída, previsto e retorno; permanência e atraso calculados separadamente. |
| **Frase** | "Eu só vou dar baixa nele quando ele realmente retornou." [53:16] |

### 2.4 Persona de decisão: Helena, coordenadora/diretora da área requisitante

| | |
|---|---|
| **Papel** | Recebe o escalonamento quando o item se aproxima do vencimento. Responde pelo custo do ICMS e pelas perdas da área. |
| **O que precisa da solução** | Poucos alertas, só os escalonados (coordenador entre 31 e 61 dias para vencer, diretor entre 1 e 30, conforme a régua atual [36:26]); visão consolidada por área e por fornecedor no BI. |

### 2.5 Ator de apoio: Joel, mecânico (manutentor)

Abre a OM no SAP e leva o item ao barracão [34:41]. Não usa o painel. Pode ser quem prende a TAG, se o PCM delegar (a confirmar).

## 3. Atores externos (não são usuários do sistema)

| Ator | Relação com a solução |
|---|---|
| Transportadora (>90% terceirizada) | Leva o item; não tem internet garantida no caminhão. O dispositivo precisa guardar e enviar depois. |
| Fornecedor de manutenção | Pode abrir o lacre para reparar, mudar o item de unidade ou terceirizar o serviço. Gera eventos legítimos que não podem virar alerta falso. |
| TI | Libera ferramentas e integrações (Python, BI, SharePoint); qualquer outra passa por avaliação de cyber. |
| Gestão patrimonial e contabilidade | Consomem o histórico para extravios e imobilizado; não operam o ciclo no dia a dia. |

## 4. Quem usa, quem consulta, quem decide

| | Cadastra a saída | Aplica o lacre | Cobra o fornecedor | Recebe alerta | Consulta | Decide |
|---|---|---|---|---|---|---|
| Analista de PCM | Sim | Sim | Sim | Prazo, lacre, cerca | Painel | Atraso, sobressalente, prorrogação |
| Analista fiscal | Emite NF | Não | Não | Retorno sem lançamento, prazo legal | Painel/BI | Complemento de ICMS |
| Analista de logística | Não | Não | Hoje sim (informal) | Resumo | BI via API | Não |
| Coordenação/diretoria | Aprova | Não | Não | Escalonamento | BI | Prioridade e custo |
| Mecânico | Abre OM | Talvez | Não | Não | Não | Não |
