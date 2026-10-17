# Arthur - Kick-off Cerradinho: problemática, expectativas e respostas

**Autor:** Arthur Loyola (@arthur.loyola)

> **Pergunta investigada:** o que a Cerradinho apresentou como problema e expectativa no kick-off, e o que respondeu às perguntas dos grupos.
> **O que foi feito:** leitura da transcrição automática do kick-off (16/10/2026, ~1h35) e do slide "Fluxos de saída de materiais"; organização das falas por tema, com o minuto aproximado de cada uma.
> **Evidência:** transcrição enviada pelo Arthur na conversa do projeto; minutos entre colchetes, ex.: [34:28]. A transcrição tem erros ("Serradinha", "SMS" no lugar de ICMS); números com transcrição duvidosa estão marcados com (?).
> **Em aberto:** dados da última safra (quantidade de remessas, previsto x realizado, casos críticos, fotos), número de extravios por safra, lista de ferramentas liberadas pela TI e o manual de marca para o dashboard. Todos foram prometidos pelo parceiro.
> **Alimenta:** entendimento do negócio e requisitos (seções de contexto, requisitos funcionais e não funcionais da `documentacao.md`).
> **Implicação:** o "MVP de verdade" para o parceiro são três coisas: alerta de prazo, alerta de rompimento do lacre e geolocalização. Ganho financeiro não é critério de sucesso, então o grupo deve priorizar funcionar de ponta a ponta nesses três pontos antes de qualquer extra.

> Nomes de pessoas do parceiro e de fornecedores foram trocados por papéis (AGENT.md, seção 3). Valores de ICMS são os do TAP apresentado à turma.

## 0. Por que "safra" aparece em todo número

Safra é o ano-safra do setor sucroenergético: o ciclo de colheita e moagem da cana, que vai mais ou menos de abril de um ano a março do seguinte. "Safra 24/25" é esse período entre 2024 e 2025. A Cerradinho mede produção, custos e metas por safra, não por ano civil, então o ICMS pago e as perdas também são somados por safra.

Isso se liga ao problema de três jeitos:

1. **É a régua de comparação.** O TAP pede que a dor seja mostrada "com os dados da última safra", e a diretoria acompanha a evolução safra a safra (cerca de R$ 60 mil na 22/23, R$ 110 mil na seguinte, R$ 602.969 na 24/25 e uma queda na 25/26 depois que o acompanhamento melhorou) [50:42].
2. **Concentra as remessas.** Na entressafra (o período sem moagem, mais ou menos de dezembro a março) a usina para e faz as grandes manutenções, então muitos equipamentos saem juntos e o risco de esquecer algum cresce. O etanol de milho (65% da produção) roda o ano todo e espalha as remessas pelo resto do ano. Esta parte é inferência, não foi dita no kick-off.
3. **O prazo fiscal atravessa safras.** Um item enviado numa entressafra com prazo de 360 dias volta, ou deveria voltar, na entressafra seguinte. Se ninguém lembrar dele durante a safra inteira, o ICMS cai na conta.

## 1. Quem participa do lado da Cerradinho

| Papel | Envolvimento no projeto |
|---|---|
| Gerente de Inovação e Melhoria Contínua | PM do projeto, acompanha o cronograma |
| Liderança do projeto (Inovação) | Ponto focal técnico |
| Analista de logística | Acompanha hoje as remessas para conserto e o BI para a diretoria |
| Time fiscal/logística | Apoio em NF, retorno e ICMS |
| Coordenador da área de negócio | Liderança do negócio |
| Liderança de TI | Dados, infraestrutura e liberação de ferramentas |
| RH e Treinamento & Desenvolvimento | Relação com a turma; não atuam no fluxo |

Áreas de aplicação citadas: gestão patrimonial, fiscal, contabilidade, gestão de ativos e área de negócio [28:20].

## 2. O problema, nas palavras deles

- Usina grande (unidade em GO), com área fabril extensa e operação agrícola num raio de mais de 200 km. Muitos ativos e muitas pessoas, sem controle centralizado [10:20].
- Equipamentos vão para manutenção externa em SP, MG, GO, MT e MS. Essa é a delimitação regional da geolocalização [12:55].
- O acompanhamento depende de registros e contatos entre áreas e fornecedores. O fornecedor promete uma data, o item "se perde numa planilha" ou é esquecido, principalmente se não for crítico [14:29].
- **Custo fiscal:** a remessa para conserto não paga ICMS se o item voltar no prazo legal (360 dias fora de GO, 270 dias dentro de GO). Passou do prazo, mesmo que o item já tenha voltado, a empresa paga complemento de ICMS (alíquotas de 18%, 7% etc., conforme o destino) [15:31–17:21].
- **Números do TAP:** R$ 975.284,98 de ICMS pago entre as safras 22/23 e 26/27 (até o fim do mês passado); R$ 602.969 só na safra 24/25 [15:09, 18:24].
- **Perdas além do ICMS:** equipamentos sumidos (com boletim de ocorrência), responsável que sai da empresa e o acompanhamento se perde, chassi que voltou trocado pelo fornecedor [21:10, 42:56]. Só de rádios de comunicação a perda passou de R$ 1 milhão na safra passada [22:58].

## 3. O que eles esperam

- Protótipo IoT **reutilizável**, sem tecnologia pré-definida, preso ao equipamento por lacre/alça e transferido para outro equipamento quando o primeiro volta [18:43].
- Vincular cada ciclo à NF, mostrar a **última localização válida**, alertar **permanência** e **retirada/rompimento do lacre** [19:08].
- Atualização semanal (ajustável), prazo de alerta configurável com 150 dias de referência, resistente à água, API para o BI [19:35].
- Escolha técnica considerando cobertura, autonomia, confiabilidade, **custo** e escalabilidade. "Tem que se pagar" frente ao ICMS [20:05, 1:03:47].
- Equipamentos de referência: 1 m³ ou mais (motor, redutor planetário, pneus, bomba, válvula). Se der para escalar a itens pequenos como rádios, "seria sensacional" [22:05].
- MVP com 5 dispositivos (talvez 10 a 20), mais diagnóstico do fluxo atual, matriz comparativa de tecnologias, arquitetura de dados, lista de materiais, plano e evidência de testes, código e documentação ("o book do protótipo") [26:35].
- Reviews a cada 15 dias para ajustar a rota; dúvidas centralizadas no canal com a PM do parceiro, ou às sextas [1:34:29].

## 4. Respostas às perguntas dos grupos

### Pessoas e dados (perguntas do G01)

| Pergunta | Resposta | Minuto |
|---|---|---|
| Querem dados de pessoas (motorista, técnico que repara)? | Sim, para rastreabilidade. Ex.: em solda, por norma é obrigatório saber quem foi o soldador. | [30:05] |
| Esses dados estão na NF? | Parcialmente: a NF traz o fornecedor e o endereço físico. O responsável interno fica no sistema interno (ordem de serviço). O motorista hoje não consta, mas o campo complementar da NF pode trazer motorista, área, nº da OS e nº da TAG. | [30:37] |
| Que sistemas usam? | SAP (remessa, NF, retorno), Gama Soft no fiscal, BI para a diretoria. Na área de negócio, SharePoint/planilhas, e nem todas as áreas têm; há planilhas pessoais. Projeto "360" em outra unidade (MS) vai juntar criação, aprovação, NF de saída e retorno numa transação só. | [32:17–33:58] |
| Quem são as pessoas do dia a dia? | Ver o relatório de personas. O dono do processo é o analista de PCM. | [34:02–37:30] |

### Alertas, prazos e encerramento

- Escalonamento já existe por e-mail: conforme os dias a vencer, o aviso sobe do requisitante para o coordenador e depois para o diretor [36:09]. O alerta do sistema deveria seguir essa escala [44:27].
- Os 150 dias contam **da emissão da NF de saída**, não da saída física [42:03]. A diretoria já cobra a partir de 180 dias (metade de 360) [54:06].
- Os 150 dias são métrica interna para avisar cedo; os prazos legais são 270/360. A área de negócio quer configurar prazos curtos (ex.: 30 dias) quando tem urgência [54:30].
- Um ciclo **não** está encerrado se o item voltou fisicamente mas o retorno não foi lançado no fiscal. Encerrar só depois do lançamento do retorno [47:37].
- Prorrogação: perto do vencimento, faz-se um "retorno simbólico" e uma nova remessa simbólica, o que reinicia o prazo e gera nova NF [37:04].

### Lacre e segurança

- Rompimento ou retirada pelo fornecedor deve gerar alerta; só a Cerradinho deveria retirar [42:26].
- Alerta de lacre: **o mais imediato possível** (10 a 30 minutos é aceitável), não esperar o ciclo semanal [1:29:11].
- Querem registro tanto de abertura autorizada quanto de violação, para distinguir acidente de má-fé [1:17:20].
- Ideia levantada por um grupo: abertura remota autorizada para o mecânico do fornecedor. Achado trabalhoso ("50 mecânicos te ligando"), mas pode entrar no 360 [1:10:35].
- **Cerca eletrônica** agradou: alerta se sair da rota ou do endereço da NF, e aviso quando o equipamento entra de volta na usina [46:47, 49:17, 1:02:39].

### Localização e conectividade

- Cidade e uma posição por dia já bastam; a TAP diz uma por semana no mínimo [58:54].
- O pior caso não é extravio, é **esquecer** o equipamento no fornecedor [59:29].
- O fornecedor pode mandar o item para outra unidade ou terceirizar o reparo; isso muda a localização sem ser violação [1:00:37].
- Transporte: 99% rodoviário, mais de 90% por transportadora terceirizada [1:26:12]. O destino mais comum é o interior e a Grande SP; GO, MS, MT e SP cobrem ~90% [1:25:34].
- Caminhões não têm internet garantida. A frota leve usa comunicação via satélite, mas os equipamentos vão para longe dela [1:31:43].
- Tempo fora: varia muito; projetar autonomia de até 1 ano cobre ~99% dos casos [44:05].

### Escopo, valor e sucesso

- **Critério de sucesso:** protótipo funcionando, alertas chegando e as premissas atendidas. Ganho financeiro não é condição [55:29].
- **Indispensável:** alerta de rompimento, alerta de prazo e geolocalização. O resto é melhoria [1:09:48].
- Foco em ativos acima de R$ 1 milhão (TAP), mas podem baixar para R$ 200–500 mil, e **pneus** entram pelo volume (~5.000 pneus no parque) [1:11:57, 1:13:04].
- Faixa de valores enviada: de peça de R$ 20 a colhedora de R$ 2 milhões. Itens baratos vão em lote e podem voltar fracionados (ex.: saíram 50, faltam 2) [1:07:53].
- Mais de 6.000 ativos só no setor de uma das pessoas; podem levantar quantos itens foram para manutenção externa na safra [1:05:55].
- Equipamento crítico ganha sobressalente; os demais seguem com bypass ou entram em parada planejada [1:04:51].
- Orçamento por dispositivo: não há teto definido; "quanto mais barato, melhor" [1:03:34].
- Não conhecem solução de mercado para isso. Já estudaram RFID na portaria para pneus (entrada e saída) [1:20:33].

### Canal e ferramentas

- Canal oficial dos alertas: **e-mail** (ou Teams), para e-mails de setor ou encarregados de turno. Evitar WhatsApp e alertas fora do horário por risco trabalhista [1:27:12, 1:28:05].
- Liberados: Python, BI, SharePoint, planilhas, código em geral. Outras ferramentas passam por avaliação de cyber com a TI [1:21:47].
- Existe manual de marca para o layout do dashboard [1:21:11].

## 5. O que o parceiro prometeu enviar

1. Dados de remessas da última safra (quantidade, retornos, prazos) [50:16].
2. Número de extravios de imobilizado por safra [1:07:34].
3. Itens mais relevantes enviados para manutenção na safra, com fotos [1:07:01].
4. Um passo a passo guiado de um caso real, com NF de exemplo (vídeo na semana seguinte) [1:03:00].
5. Manual de marca e a posição da TI sobre ferramentas permitidas.
