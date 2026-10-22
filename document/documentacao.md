# Documentação IoTDoc - Módulo 4 - Inteli

**_Os trechos em itálico servem apenas como guia para o preenchimento da seção. Por esse motivo, não devem fazer parte da documentação final_**

## Nome do Grupo

#### Nomes dos integrantes do grupo

<img src="../assets/inteli.png">

## Sumário

[1. Introdução](#c1)

[2. Metodologia](#c2)

[3. Desenvolvimento e Resultados](#c3)

[4. Conclusões e Recomendações](#c4)

[5. Referências](#c5)

<br>


# <a name="c1"></a>1. Introdução (sprint 1)

A CerradinhoBio é uma companhia brasileira de capital aberto, com ações negociadas na B3, e integra o Grupo Cerradinho [3]. Atua em bioenergia a partir de cana-de-açúcar e milho: produz etanol, energia elétrica renovável gerada com biomassa, açúcar VHP e coprodutos do milho, como DDGS, WDG e óleo [4]. Suas operações estão em Chapadão do Céu (GO), onde ficam o negócio de cana e uma unidade de etanol de milho, e em Maracaju (MS), que também abriga uma unidade de etanol de milho [2]. Na safra 2025/26, a companhia registrou receita líquida consolidada de R$ 4,288 bilhões [1] e processou 5,2 milhões de toneladas de cana e 1,5 milhão de toneladas de milho; a capacidade instalada da fábrica de açúcar é de 550 mil toneladas por ano [2]. Como referência de porte de pessoal, a companhia informava cerca de 3.800 empregos diretos e indiretos em sua apresentação institucional publicada durante a safra 2024/25 [5]. A empresa se posiciona entre os maiores complexos industriais sucroenergéticos do Brasil e entre os maiores produtores de etanol do país [3, 4].

**Problema do projeto.** O acompanhamento de equipamentos enviados à manutenção externa depende de registros e contatos entre áreas e fornecedores. A dificuldade de consolidar localização, nota fiscal de remessa, prazo previsto e data de retorno reduz a visibilidade dos ativos em terceiros e exige cobranças manuais. Permanência total e atraso em relação ao prazo combinado são informações distintas e precisam ser analisadas separadamente.

## 1.1 Objetivos

**Objetivo geral.** Melhorar o controle dos equipamentos enviados a fornecedores para manutenção externa, desde a saída até o retorno.

**Objetivos específicos.** Ampliar a visibilidade dos ativos enquanto estão com terceiros; associar cada ciclo de envio à respectiva nota fiscal; distinguir permanência prolongada de atraso em relação ao prazo previsto; reduzir o esforço de consulta e cobrança manual; e preservar o histórico dos ciclos e movimentações.

## 1.2 Proposta da solução

A proposta reúne um dispositivo reutilizável, fixado ao equipamento com um lacre, e uma plataforma para consultar ativos, ciclos de manutenção, última localização e histórico. No cadastro, cada ciclo relaciona equipamento, fornecedor e nota fiscal, preservando os dados de saída e retorno. A plataforma compara o tempo decorrido desde a saída com o prazo previsto: assim, evidencia a permanência total do ativo e identifica separadamente quando o prazo combinado foi ultrapassado. A localização e o estado do ciclo ampliam a visibilidade dos equipamentos em terceiros; alertas configuráveis destacam casos para acompanhamento e pretendem reduzir consultas e cobranças manuais; o histórico registra os ciclos sucessivos; e uma API disponibiliza esses dados para análise em ferramentas de BI. No protótipo, a comunicação será demonstrada por Wi-Fi em cenário controlado. Como a disponibilidade de Wi-Fi nas oficinas de terceiros não é pressuposta, o canal de comunicação para produção será escolhido em sprint posterior, após comparação das alternativas em uma matriz que considere cobertura nos fornecedores, autonomia, confiabilidade, custo e escalabilidade, conforme o TAPI do projeto [6].

## 1.3 Justificativa

A centralização das informações pode facilitar a consulta da situação de cada equipamento, dar rastreabilidade documental aos envios e destacar casos que precisam de acompanhamento. O dispositivo reutilizável, o vínculo entre ativo, ciclo e nota fiscal, o histórico preservado entre usos e a disponibilização dos dados para BI compõem os diferenciais da proposta. A identificação mais rápida de pendências pode contribuir para reduzir o tempo de permanência e as perdas de ativos, mas esses resultados dependem do tratamento dos alertas pelos responsáveis e devem ser medidos; não são garantidos pelo protótipo, conforme os objetivos definidos no TAPI do projeto [6].

# <a name="c2"></a>2. Metodologia (sprint 3)

_Descreva as etapas da metodologia RM-ODP que foram utilizadas para o desenvolvimento, citando o referencial teórico. Você deve apenas enunciar os métodos, sem dizer ainda como ele foi aplicado e quais resultados obtidos._

# <a name="c3"></a>3. Desenvolvimento e Resultados 

## 3.1. Domínio de Fundamentos de Negócio

_Link para os templates de negócios: https://drive.google.com/drive/folders/1lTqisJ_gxxtc5J5aXM8ula2kQ4MsII_M?usp=sharing_

### 3.1.1. Contexto da Indústria (sprint 1)	

**Parceiro:** CerradinhoBio
**Setor analisado:** bioenergia e indústria sucroenergética brasileira, incluindo etanol de cana e de milho, açúcar, bioeletricidade e coprodutos.

#### Concorrência

A CerradinhoBio compete com grupos integrados de açúcar, etanol e bioenergia e, no segmento de etanol de milho, com produtores especializados. Entre os principais concorrentes estão a **Raízen** [19], a **São Martinho** [20] e a **bp bioenergy** [10] (antiga joint venture BP Bunge Bioenergia, adquirida integralmente pela bp em 2024). Para comparar a escala no segmento de cana, as empresas divulgam capacidade anual de moagem de aproximadamente 105 milhões de toneladas para a Raízen [19] e 32 milhões para a bp bioenergy [10]; a São Martinho projeta capacidade plena de 24 milhões de toneladas na safra 2026/27 [20]. No etanol de milho, a **FS** [11] informa capacidade anual de 2,6 bilhões de litros e a **Inpasa** [12] reporta 5,8 bilhões após a expansão de suas unidades. São indicadores de capacidade publicados em períodos diferentes; não representam produção realizada no mesmo ano nem participação de mercado.

#### Modelo de negócio

A CerradinhoBio opera um modelo B2B integrado e intensivo em ativos: transforma cana-de-açúcar e milho em etanol, açúcar VHP, bioeletricidade e coprodutos de milho destinados principalmente à nutrição animal [4], comercializados para distribuidores de combustíveis, clientes industriais e de alimentos para animais, compradores de açúcar e agentes do setor elétrico. A diversificação entre matérias-primas e produtos permite ajustar o mix de produção às condições de mercado; a venda de coprodutos também ajuda a compensar parte do custo do milho. A companhia participa ainda do mercado de créditos de descarbonização (CBIOs) por meio de sua produção certificada no RenovaBio [14]. O desempenho depende de escala e eficiência agrícola e industrial, logística de matéria-prima e produtos, capital para manutenção e expansão e exposição à sazonalidade e à volatilidade dos preços de commodities. Na safra 2025/26, a empresa informou processamento de 5,2 milhões de toneladas de cana e 1,5 milhão de toneladas de milho, além de produção equivalente de 865 mil m³ de etanol hidratado, dos quais 687 milhões de litros vieram do milho [2].

#### Tendências

O setor amplia a produção de etanol de milho, que complementa a oferta de cana e aproveita a disponibilidade de grãos e a demanda por coprodutos; a EPE [13] registrou produção total brasileira de etanol de 36,2 bilhões de litros em 2025, com expansão do milho compensando a queda da cana. O RenovaBio [14] e os CBIOs mantêm um mecanismo de incentivo à descarbonização, enquanto a Lei do Combustível do Futuro [15] cria oportunidades e novas exigências para biometano e combustível sustentável de aviação (SAF): para operadores aéreos, a redução mínima de emissões começa em 1% em 2027 e cresce gradualmente até 10% em 2037 (ANP — SAF [16] e ANP — biometano [17]). Outra frente é digitalizar a manutenção e a gestão de ativos industriais — por exemplo, consolidar registros, localização, prazos e alertas — para dar mais visibilidade à operação e apoiar decisões com dados; os benefícios dependem da qualidade da implantação e da atuação dos responsáveis.

#### Cinco Forças de Porter — perspectiva da CerradinhoBio

Escala qualitativa: **baixa**, **média** ou **alta**. A intensidade indica quanto a força tende a pressionar a atratividade e a rentabilidade da CerradinhoBio no setor, consideradas as condições descritas abaixo. A análise segue o referencial das cinco forças de Porter [18] e a figura foi preenchida sobre o template oficial de cinco forças do Inteli [21].

![Cinco Forças de Porter para a CerradinhoBio, no formato do template oficial do Inteli](../assets/negocios/cinco_forcas.png)

*Figura — Cinco Forças de Porter, preenchidas pela perspectiva da CerradinhoBio. Fonte: template oficial de negócios do Inteli [21]; conteúdo e adaptação: Grupo G01 (2026).*

| Força | Intensidade | Justificativa do ponto de vista da CerradinhoBio |
|---|---|---|
| **Rivalidade entre concorrentes** | **Alta** | Há grandes grupos de cana com escala e portfólios diversificados e produtores de milho em rápida expansão. Etanol e açúcar são produtos amplamente negociados e sujeitos a preços de mercado; isso limita a diferenciação puramente pelo produto e intensifica a disputa por eficiência agrícola e industrial, matéria-prima, logística e contratos de venda. A flexibilidade entre cana, milho, açúcar, energia e coprodutos pode ajudar a CerradinhoBio a responder a ciclos de preço, mas exige disciplina de custos e investimento. |
| **Poder de barganha dos fornecedores** | **Médio** | O milho, a cana adquirida de terceiros onde aplicável, fertilizantes, combustíveis, peças e equipamentos especializados podem pressionar custos, especialmente diante de sazonalidade, distância logística ou fornecedores concentrados. Em contrapartida, há alternativas de fornecimento para vários insumos, contratos e possibilidade de integração/produção agrícola própria; a operação com duas matérias-primas também reduz a dependência de uma única cadeia. O poder varia por insumo, região e safra, portanto recomenda-se acompanhar concentração e custo posto na usina por categoria. |
| **Poder de barganha dos compradores** | **Média** | Parte relevante da produção é vendida em mercados de commodities e passa por distribuidores, tradings e compradores industriais com escala, capacidade de comparar fornecedores e negociar grandes volumes. Isso limita a influência individual do produtor sobre o preço. A diversificação entre etanol, açúcar, energia, coprodutos e CBIOs amplia os canais de receita e reduz a dependência de um único comprador, embora cada mercado tenha sua própria concentração, especificação e dinâmica contratual. |
| **Ameaça de novos entrantes** | **Média** | Uma nova usina de cana ou biorrefinaria de milho exige capital elevado, escala, acesso confiável à matéria-prima, logística, licenciamento, tecnologia e tempo de implantação e ramp-up, o que cria barreiras importantes. Ainda assim, empresas do agronegócio e investidores podem financiar unidades novas, especialmente no milho, onde grãos e infraestrutura regional favorecem projetos. A entrada de novos grupos pode elevar a ameaça; ampliações realizadas por empresas já atuantes não são novos entrantes e reforçam principalmente a rivalidade entre concorrentes. |
| **Ameaça de produtos substitutos** | **Média** | Gasolina e outros combustíveis podem competir com o etanol no transporte; eletrificação, eficiência energética e outras rotas de baixo carbono podem reduzir demanda de longo prazo. Para açúcar, existem adoçantes alternativos; para energia e coprodutos, há outras fontes e ingredientes. Por outro lado, a mistura obrigatória de etanol na gasolina, a frota flex, as metas de descarbonização e a versatilidade do portfólio ajudam a sustentar mercados atuais e a abrir novas aplicações, como SAF e biometano. |

#### Síntese estratégica

O ambiente competitivo é mais pressionado pela rivalidade e pelo poder dos compradores, pois escala, custos e preços de commodities têm peso elevado. Para a CerradinhoBio, as prioridades estratégicas são preservar eficiência e confiabilidade dos ativos, proteger o abastecimento de cana e milho, ampliar a flexibilidade de mix e desenvolver canais de maior valor para coprodutos e combustíveis de baixo carbono. A digitalização da manutenção pode apoiar a disponibilidade e a gestão patrimonial; no projeto de rastreamento, sua contribuição específica é aumentar a visibilidade de equipamentos em manutenção externa, sem pressupor redução de prazo ou perdas antes de medir os resultados do piloto [6].

### 3.1.2. Análise SWOT (sprint 1)	

![Análise SWOT da CerradinhoBio](../assets/negocios/swot.png)

*Figura 3.1.2 — Análise SWOT da CerradinhoBio, com foco em equipamentos enviados à manutenção externa. Fonte: elaboração própria, com base no TAPI do projeto [6], nos dados de produção da CerradinhoBio [2], em seus produtos [4] e nas fontes setoriais vinculadas na seção 3.1.1.*

**Escopo:** análise da CerradinhoBio, com foco nas condições do negócio e na gestão de equipamentos enviados para manutenção externa. Forças e fraquezas representam fatores internos; oportunidades e ameaças, fatores externos. A síntese considera o contexto da indústria descrito na seção 3.1.1 e o TAPI do projeto.

**Leitura para o projeto:** o portfólio diversificado e a escala de processamento da CerradinhoBio oferecem contexto favorável para testar uma rotina de rastreabilidade reutilizável. A dependência de registros e contatos entre áreas e fornecedores é uma fragilidade do processo de manutenção externa descrito no TAPI, a validar com o parceiro. A expansão do etanol e as políticas de descarbonização abrem oportunidades para a companhia; concorrência em escala, volatilidade de insumos e substitutos energéticos compõem ameaças externas. **PENDENTE DE VALIDAÇÃO:** quantificar a dor operacional (volume de remessas, tempo fora, atrasos e esforço de consulta) com os dados da última safra, ainda a serem disponibilizados conforme o TAPI.

### 3.1.3. Descrição da Solução a ser Desenvolvida (sprint 1)	

#### 3.1.3.1 Qual é o problema a ser resolvido

Quando um equipamento da CerradinhoBio é enviado a um fornecedor externo para manutenção, as informações sobre localização, nota fiscal de remessa, prazo previsto e retorno dependem de registros e contatos entre áreas e fornecedor. Sem uma visão consolidada, a Gestão Patrimonial, a Controladoria e a Manutenção podem ter dificuldade para saber onde está cada ativo, há quanto tempo está fora e se o prazo combinado foi ultrapassado, além de precisar fazer consultas e cobranças manuais. O TAPI identifica essa necessidade [6]. **PENDENTE DE VALIDAÇÃO:** demonstrar a dimensão da dor — volume de remessas, permanência, atrasos e casos críticos — com dados da última safra a serem disponibilizados pelo parceiro; não se presume redução de perdas ou de prazo antes dessa análise.

#### 3.1.3.2 Qual a solução proposta (visão de negócios)

Propor um processo de rastreabilidade para equipamentos em manutenção externa, apoiado por dispositivos reutilizáveis e por um registro digital de cada ciclo [6]. A solução reunirá equipamento, fornecedor, responsável, nota fiscal, saída, prazo previsto, retornos de localização, comunicação, bateria, estado do lacre e alertas em uma consulta centralizada, com API para disponibilizar os dados ao BI. O piloto deverá validar a utilidade operacional e a viabilidade técnica e econômica antes de qualquer decisão de expansão. A tecnologia de localização e comunicação será escolhida após comparação de alternativas; a solução não pressupõe rastreamento contínuo ou posicionamento indoor preciso.

#### 3.1.3.3 Como a solução proposta deverá ser utilizada

1. **Solicitação e aprovação:** a área responsável solicita o envio do equipamento à manutenção externa e registra a aprovação conforme o processo da CerradinhoBio.
2. **Cadastro do ciclo:** o operador registra equipamento, fornecedor, responsável, nota fiscal de remessa e prazo previsto de retorno.
3. **Vínculo e lacre:** associa um dispositivo disponível ao equipamento, confere a fixação e registra a identificação do lacre. O sistema mantém um único vínculo ativo por dispositivo e preserva os ciclos anteriores.
4. **Saída e acompanhamento:** na saída, registra a data e inicia o ciclo. Durante a permanência externa, consulta a última localização válida, sua data e hora, a última comunicação, a bateria, o estado do lacre e os dias fora. As atualizações serão periódicas, com frequência semanal configurável como referência do piloto.
5. **Alertas e tratamento:** o sistema sinaliza, separadamente, o vencimento do prazo previsto, a permanência acima do limiar configurado (150 dias como referência inicial), a falta de comunicação, bateria baixa e violação ou retirada do lacre. O responsável verifica o caso, toma as providências e registra o tratamento. Uma retirada necessária ao reparo deve seguir fluxo de autorização, registro e reinstalação com novo lacre. Sem conexão, os eventos ficam armazenados para envio posterior; alertas fora da rotina dependem de conectividade.
6. **Retorno e liberação:** no retorno, o operador confere o equipamento, registra a data e encerra o ciclo. Depois da conferência e retirada do dispositivo, libera-o para outro equipamento sem apagar o histórico.

Os papéis de cadastro, aprovação, instalação, autorização de retirada, tratamento de alertas e conferência do retorno devem ser confirmados com o parceiro.

#### 3.1.3.4 Quais os benefícios trazidos pela solução proposta

- Visão consolidada dos equipamentos que estão fora, seus fornecedores, documentos associados, prazos e última localização válida.
- Mais facilidade para identificar itens atrasados ou acima do limiar de permanência e direcionar a consulta ao responsável adequado.
- Histórico rastreável de saídas, localizações, alertas, retiradas autorizadas, retornos e reutilização dos dispositivos.
- Menos dependência de consultas manuais para responder onde está um equipamento e qual é sua situação, benefício a medir no piloto.
- Base de dados via API para análise em BI e evidências para decidir se a solução deve evoluir.
- Reutilização dos dispositivos entre ciclos, condicionada à integridade, autonomia e viabilidade de operação demonstradas.

Redução do tempo de manutenção, de perdas ou de custos e ganhos ambientais são resultados potenciais, não garantidos pelo protótipo; dependem da resposta às informações e deverão ser avaliados com dados.

#### 3.1.3.5 Qual será o critério de sucesso e qual medida será utilizada para o avaliar

**Critério de sucesso:** demonstrar, no piloto, o ciclo completo de rastreabilidade de equipamentos, com dados consultáveis e históricos preservados, localização periódica útil, alertas tratados por responsáveis definidos e reutilização sem vínculo simultâneo indevido. O TAPI prevê pelo menos cinco equipamentos reais ou representativos e exige que as metas sejam acordadas antes dos testes. Os valores abaixo são **propostas para validação com a CerradinhoBio antes do início dos ensaios**, não resultados já obtidos.

| Indicador proposto | Medida e meta inicial proposta |
|---|---|
| Localização atualizada | Percentual de tentativas semanais programadas que geram uma localização externa válida: **≥ 90%**, em percurso e condições de teste previamente definidos. Registrar também o percentual de ciclos ativos com posição válida nos últimos sete dias. |
| Consulta do ativo | Tempo entre a busca pelo identificador do equipamento e a exibição da situação completa: **≤ 2 minutos em pelo menos 90%** das consultas de teste. |
| Alerta de violação | Tempo entre a violação detectada e a disponibilização do alerta: **≤ 15 minutos em cenário com conexão**. Em cenário sem conexão, confirmar o armazenamento e o envio após o restabelecimento, registrando a latência observada. |
| Falsos alertas | Alertas sem evento correspondente divididos pelo total de alertas avaliados: **≤ 5%** no conjunto de ensaios acordado. |
| Integridade do ciclo | Ciclos de saída e retorno com equipamento, fornecedor, referência documental, datas e vínculo do dispositivo corretamente registrados: **100% dos ciclos de teste**; nenhuma associação ativa duplicada após reutilização. |

O ensaio deve incluir retirada autorizada do lacre, indisponibilidade de conexão, retorno e liberação do dispositivo. A permanência de 150 dias e outros limiares pode ser verificada com datas simuladas, mas ensaios curtos não comprovam autonomia real por 150 dias. O relatório deverá separar valores medidos, estimativas e limitações, e apresentar os resultados também por percurso ou condição de cobertura para evitar que uma média esconda pontos sem localização.
	
### 3.1.4. Value Proposition Canvas (sprint 1)	

_O Canvas da proposta de valor deve ser feito considerando a solução que vocês estão desenvolvendo. Usar o template disponível no drive da turma._     

### 3.1.5. Matriz de Riscos (sprint 1)

<div align="center">
  <p><strong>Figura 1 — Matriz de riscos do projeto</strong></p>
  <img
    src="..\assets\Matriz de Risco - Guará.png"
    alt="Matriz de riscos do projeto"
    width="100%"
  >
  <p><em>Fonte: Elaborado pelos autores (2026).</em></p>
</div>

Probabilidade: 10%, 30%, 50%, 70% ou 90%; impacto: muito baixo a muito alto. Os códigos seguem esses cruzamentos. 150 dias é limiar de alerta, não autonomia.

| ID | Causa e efeito | Prob. | Impacto | Justificativa | Resposta | Responsável |
|---|---|---:|---:|---|---|---|
| R01 | Safra atrasada → diagnóstico impreciso. | 50% | Moderado | Dependência externa. | Mitigar: validar hipóteses. | Negócios |
| R02 | Cobertura ou metal → perda do GNSS. | 70% | Muito alto | Obstáculos degradam sinais. | Mitigar: guardar posição e testar antenas. | Hardware |
| R03 | Ensaio curto → autonomia incerta. | 70% | Alto | Ciclos prolongados. | Mitigar: medir e projetar consumo. | Hardware |
| R04 | Oficina sem Wi-Fi → eventos retidos. | 90% | Alto | Rede externa incerta. | Mitigar: armazenar e avaliar celular. | Conectividade |
| R05 | Ruído ou abertura autorizada → falso alerta. | 50% | Alto | Eventos legítimos parecem violações. | Mitigar: debounce e autorização. | Firmware |
| R06 | Fixação inadequada → dano ou reparo prejudicado. | 50% | Muito alto | Formatos variam. | Evitar: fixação removível testada. | Hardware |
| R07 | Água ou lavagem → falha eletrônica. | 70% | Muito alto | Exposição provável. | Mitigar: vedação e ensaio. | Hardware |
| R08 | Conectividade cara → escala inviável. | 50% | Alto | Custo recorrente. | Mitigar: comparar tecnologias e planos. | Produto |
| R09 | Fornecedores indisponíveis → piloto limitado. | 50% | Moderado | Dependência de terceiros. | Mitigar: antecipar agenda e simular. | PO |
| R10 | Dado restrito publicado → exposição indevida. | 30% | Muito alto | Informações sensíveis. | Evitar: dados fictícios e revisão. | Dados/LGPD |

**Oportunidades:** O01 — rastreador reutilizável reduz custo por ciclo (70%; alto; explorar modularidade; Produto). O02 — integração com BI agiliza decisões (70%; alto; explorar API autenticada; Software).

### 3.1.6. Política de Privacidade de acordo com a LGPD (sprint 1)	

Política preliminar, sem aprovação jurídica da CerradinhoBio.

#### 3.1.6.1 Informações gerais

A solução acompanha equipamentos da CerradinhoBio em manutenção externa.

#### 3.1.6.2 Tratamento de dados

O tratamento seguirá a LGPD. Correspondência com o rascunho de RF do Luis: **PENDENTE DE VALIDAÇÃO**.

#### 3.1.6.3 Dados coletados

Serão tratados nome, e-mail corporativo, cargo, perfil, logs, IP e horário dos usuários; responsáveis por ciclos e alertas; e contatos profissionais de fornecedores. Dados do equipamento não são pessoais por si só; sua localização pode identificar indiretamente o motorista.

#### 3.1.6.4 Fonte dos dados

Fontes: titulares, responsáveis, fornecedores, painel e equipamentos.

#### 3.1.6.5 Finalidades

Servirão à autenticação, acesso, auditoria, ciclos, alertas, suporte e comunicação. Será mantida apenas a última posição válida, atualizada semanalmente, sem rastreamento contínuo, conforme restrições 1 e 5 do TAPI.

#### 3.1.6.6 Armazenamento

Cadastros, logs, ciclos e alertas ficarão no backend/banco em nuvem; mensagens, no broker. Provedor, região e tecnologias: **PENDENTE DE VALIDAÇÃO**.

#### 3.1.6.7 Retenção

Cadastros e permissões serão conservados enquanto ativos; logs, pelo período de auditoria; ciclos e alertas, enquanto necessários ao histórico; contatos, durante a relação operacional; localização, até sua substituição semanal; cookies, durante a sessão. Depois, haverá exclusão ou anonimização. Prazos: **PENDENTE DE VALIDAÇÃO**.

#### 3.1.6.8 Cookies

Se previstos no painel, serão usados somente cookies de sessão necessários.

#### 3.1.6.9 Compartilhamento

O compartilhamento necessário abrangerá provedores de nuvem, broker e conectividade.

#### 3.1.6.10 Segurança

Serão aplicados autenticação, acesso restrito, auditoria e proteção de credenciais.

#### 3.1.6.11 Direitos dos titulares

O titular poderá pedir confirmação, acesso, correção, anonimização, bloqueio, eliminação e informações sobre compartilhamento.

#### 3.1.6.12 Exercício dos direitos

Solicitações serão recebidas pelo canal oficial, terão identidade e escopo verificados, serão registradas, analisadas e respondidas. Canal: **PENDENTE DE VALIDAÇÃO**.

#### 3.1.6.13 DPO/Encarregado

O encarregado e seu contato serão indicados pela CerradinhoBio: **PENDENTE DE VALIDAÇÃO**.


### 3.1.7. Bill of Material (BOM) (sprint 1)
_BOM (Bill of Material) pode ser definido como uma lista completa de materiais, que detalha todos os itens necessários para construir, fabricar ou reparar um produto. Deve-se preencher o template fornecido.

**Componente GNSS definido em 21/10/2026:** Quectel LC76G, variante **PA**, uma unidade por dispositivo. A placa de desenvolvimento ou montagem, antena, circuito de alimentação/backup, fornecedor e preço continuam **PENDENTE DE VALIDAÇÃO**. Esta definição deve integrar a BOM completa; não representa compra ou ensaio físico realizado. A decisão e o contrato de simulação estão em 3.3.5.



## 3.2. Domínio de Fundamentos de Experiência de Usuário 

### 3.2.1. Personas (sprint 1)	

#### Introdução às personas

#### Introdução às personas

As personas orientam o desenvolvimento da solução de rastreamento de equipamentos em manutenção externa da Cerradinho a partir dos objetivos, dificuldades e necessidades dos usuários. Os perfis consideram seus cenários de uso e letramento digital, distinguindo informações fundamentadas na pesquisa de hipóteses pendentes de validação.

### 3.2.2. Jornada do Usuário e Storyboard (sprint 2)	

_Coloque aqui seu Mapa de Jornada do Usuário (seguindo as instruções na descrição do artefato)_

_Coloque aqui seu Storyboard (seguindo as instruções na descrição do artefato)_

## 3.3. Solução Técnica	

_Nesta seção, detalhe a especificação da solução, de acordo com o disposto nas subseções._

### 3.3.1. Requisitos Funcionais (sprint 1, refinar e atualizar até sprint 5)

Os requisitos abaixo saem do TAPI (MVP 1 a 7 e fluxo básico), da avaliação do protótipo e do kick-off com a Cerradinho (16/10/2026). A prioridade segue o que o parceiro chamou de indispensável no kick-off: alerta de lacre, alerta de prazo e geolocalização. Um RF só passa a `Simulado no Wokwi` depois que o caso de teste correspondente (seção 3.4.1) é executado e passa. Os IDs estão congelados: um RF removido não libera o número, e um RF novo entra no fim da lista.

| ID    | Descrição | Origem | Prioridade | Status |
|-------|-----------|--------|------------|--------|
| RF001 | O sistema deve cadastrar o equipamento com identificador próprio, descrição e foto. | TAPI MVP 1 | Alta | Planejado |
| RF002 | O sistema deve abrir um ciclo de manutenção externa com NF de remessa (número e data de emissão), fornecedor, responsável e prazo previsto de retorno. | TAPI MVP 3 | Alta | Planejado |
| RF003 | O sistema deve vincular um dispositivo a um ciclo de manutenção aberto. | TAPI MVP 3 | Alta | Planejado |
| RF004 | O sistema deve recusar o vínculo de um dispositivo que já está em um ciclo ativo. | Fluxo básico | Alta | Planejado |
| RF005 | O sistema deve coletar a posição GNSS na rotina de envio e guardar a última localização válida com data e hora. | TAPI MVP 2 | Alta | Planejado |
| RF006 | O sistema deve monitorar o lacre e detectar sua abertura ou rompimento. | TAPI MVP 5 | Alta | Planejado |
| RF007 | O sistema deve registrar a retirada autorizada e a reaplicação do lacre sem gerar alerta de violação. | TAPI MVP 5 | Alta | Planejado |
| RF008 | O sistema deve conectar-se à rede Wi‑Fi e reconectar automaticamente quando a conexão cair. | Avaliação do protótipo | Alta | Planejado |
| RF009 | O sistema deve publicar status e eventos no broker MQTT, em JSON, no tópico do dispositivo. | TAPI MVP 6 | Alta | Planejado |
| RF010 | O sistema deve guardar os eventos gerados sem conexão e enviá-los na ordem em que ocorreram quando a conexão voltar. | TAPI MVP 5 | Alta | Planejado |
| RF011 | O sistema deve enviar um status periódico, semanal por padrão e configurável. | TAPI MVP 4 | Alta | Planejado |
| RF012 | O sistema deve calcular os dias fora a partir da data de emissão da NF de remessa. | TAPI MVP 4; Kick-off com a Cerradinho (16/10/2026) | Alta | Planejado |
| RF013 | O sistema deve emitir um alerta de atraso quando os dias fora passam do prazo previsto de retorno. | TAPI MVP 3 | Alta | Planejado |
| RF014 | O sistema deve emitir um alerta de permanência quando os dias fora atingem o limiar do ciclo (150 dias por padrão, configurável). | TAPI MVP 4 | Alta | Planejado |
| RF015 | O sistema deve listar os equipamentos fora com última localização válida, dias fora, prazo, bateria e estado do lacre. | TAPI MVP 6 | Alta | Planejado |
| RF016 | O sistema deve encerrar o ciclo somente com a NF de retorno lançada e o lacre fechado. | Fluxo básico | Alta | Planejado |
| RF017 | O sistema deve liberar o dispositivo do ciclo encerrado para um novo vínculo, preservando o histórico. | TAPI MVP 3; fluxo básico | Alta | Planejado |
| RF018 | O sistema deve notificar o responsável do ciclo a cada alerta, com o e-mail como canal inicial. | Fluxo básico; Kick-off com a Cerradinho (16/10/2026) | Alta | Planejado |
| RF019 | O sistema deve medir o nível de bateria e enviá-lo em cada mensagem. | TAPI MVP 6 | Média | Planejado |
| RF020 | O sistema deve cadastrar os dispositivos na plataforma, cada um com identificador e credencial próprios. | TAPI MVP 3 | Média | Planejado |
| RF021 | O sistema deve autenticar os usuários do painel com credencial individual e perfil de acesso. | TAPI MVP 6 | Média | Planejado |
| RF022 | O sistema deve exibir gráficos e mapa com equipamentos por faixa de dias fora, alertas por tipo e última posição de cada equipamento. | TAPI MVP 6 | Média | Planejado |
| RF023 | O sistema deve registrar o tratamento de um alerta com responsável, data e decisão. | Fluxo básico | Média | Planejado |
| RF024 | O sistema deve oferecer uma API autenticada e documentada para consulta dos dados por ferramentas de BI. | TAPI MVP 6 | Média | Planejado |
| RF025 | O sistema deve emitir um alerta quando um dispositivo passa do período esperado sem se comunicar. | Kick-off com a Cerradinho (16/10/2026) | Média | Planejado |
| RF026 | O sistema deve emitir um alerta quando os dias fora atingem a data limite legal do ciclo. | Kick-off com a Cerradinho (16/10/2026) | Média | Planejado |
| RF027 | O sistema deve emitir um alerta quando o nível de bateria fica abaixo do limite mínimo. | TAPI MVP 6 | Média | Planejado |

Itens técnicos da avaliação: coleta de dados via sensor (RF005, RF006, RF019) · definição de devices na plataforma (RF003, RF020) · chave de identificação de usuário e acesso ao dashboard (RF021) · conexão Wi‑Fi (RF008) · formatação de dados para MQTT (RF009) · representação gráfica dos dados coletados (RF015, RF022).

### 3.3.2. Regras de Negócio (sprint 1, refinar até sprint 5)

As regras abaixo restringem o comportamento descrito nos RFs da seção 3.3.1 e vêm das mesmas fontes: TAPI e kick-off com a Cerradinho (16/10/2026), além do regulamento do ICMS de Goiás no caso do prazo legal. Os valores marcados como `PENDENTE DE VALIDAÇÃO` são valores de teste usados no protótipo da seção 3.4.1 e serão definidos com o parceiro. Os casos de teste da 3.4.1 indicam qual RN verificam.

| ID   | Descrição | Origem | RF associado |
|------|-----------|--------|--------------|
| RN01 | Um dispositivo só pode estar vinculado a um ciclo ativo por vez, e um equipamento só pode ter um ciclo ativo por vez. | Fluxo básico | RF003, RF004 |
| RN02 | Um ciclo só pode ser aberto com NF de remessa (número e data de emissão), fornecedor, responsável e prazo previsto de retorno preenchidos. | TAPI MVP 3 | RF002 |
| RN03 | O limiar de permanência é de 150 dias por padrão e pode ser alterado em cada ciclo para qualquer número inteiro de dias maior que zero. | TAPI MVP 4; Kick-off com a Cerradinho (16/10/2026) | RF014 |
| RN04 | Atraso e permanência são alertas distintos, podem estar ativos ao mesmo tempo e cada um é emitido uma única vez por ciclo. | TAPI MVP 3 e 4 | RF013, RF014 |
| RN05 | A abertura do lacre sem autorização vigente é violação crítica e é enviada na hora, fora da rotina de status periódico. | TAPI MVP 5 | RF006 |
| RN06 | A autorização de retirada vale por 3 dias a partir do registro; a abertura do lacre depois desse prazo é tratada como violação. Prazo de 3 dias: `PENDENTE DE VALIDAÇÃO` | TAPI MVP 5 | RF007 |
| RN07 | O status periódico é enviado a cada 7 dias por padrão, com frequência configurável até diária. | TAPI MVP 4 | RF011 |
| RN08 | Uma posição GNSS é válida quando o receptor tem fix, a posição traz data e hora do próprio GNSS e o HDOP é de no máximo 2,0. A última localização válida só é atualizada com uma posição válida; sem ela, a anterior é mantida. Última comunicação e última localização válida são registradas em campos distintos. HDOP máximo de 2,0: `PENDENTE DE VALIDAÇÃO` | TAPI MVP 6 | RF005, RF015 |
| RN09 | Sem conexão, o dispositivo guarda até 8 eventos; com a fila cheia, descarta o evento não crítico mais antigo e nunca descarta uma violação. Cada evento mantém a hora em que ocorreu, e a plataforma registra a hora de recebimento à parte. Capacidade de 8 eventos: `PENDENTE DE VALIDAÇÃO` | TAPI MVP 5 | RF010 |
| RN10 | O alerta de bateria baixa é emitido uma vez quando o nível fica abaixo de 20% e só volta a ser emitido depois que o nível passa de 25%. Leitura zero é falha do sensor, não bateria vazia. Limites de 20% e 25%: `PENDENTE DE VALIDAÇÃO` | TAPI MVP 6 | RF019, RF027 |
| RN11 | O encerramento do ciclo é recusado enquanto o lacre estiver aberto ou a NF de retorno não estiver lançada, e o histórico do ciclo encerrado nunca é apagado. | Fluxo básico | RF016, RF017 |
| RN12 | Uma prorrogação (retorno e nova remessa simbólicos) abre um novo ciclo do mesmo equipamento e do mesmo dispositivo, e os dias fora passam a contar da emissão da nova NF de remessa. | Kick-off com a Cerradinho (16/10/2026) | RF012, RF017 |
| RN13 | A data limite legal do ciclo é de 270 dias para destino em GO e de 360 dias para destino fora de GO, contados da saída, ou seja, da emissão da NF de remessa. Vigência dos prazos: `PENDENTE DE VALIDAÇÃO` com o fiscal da Cerradinho | RCTE/GO (Decreto nº 4.852/1997), art. 79, I, "s", 1 (270 dias, saída interna) e Anexo IX, art. 6º, IV (360 dias, saída interestadual), na redação do Decreto nº 7.815/2013; Kick-off com a Cerradinho (16/10/2026) | RF012, RF026 |
| RN14 | Um dispositivo é considerado sem comunicação quando passa de dois intervalos de status periódico sem enviar nenhuma mensagem (14 dias com o padrão de 7 dias); o alerta é emitido uma vez e só volta a ser emitido depois de uma nova comunicação. Dois intervalos: `PENDENTE DE VALIDAÇÃO` | Kick-off com a Cerradinho (16/10/2026) | RF025 |
| RN15 | O painel e a API só respondem a usuário autenticado; sem credencial válida, o acesso é negado e nenhum dado é exibido. | TAPI MVP 6 | RF021, RF024 |
| RN16 | Mensagem de dispositivo não cadastrado ou com credencial inválida é rejeitada e não altera nenhum ciclo. | TAPI MVP 3 | RF020 |

**Compatibilidade entre requisitos e circuito atual.** RF001–RF027 e RN01–RN16 seguem a `develop`. A integração preserva o circuito e o firmware locais; as diferenças abaixo não representam requisitos já atendidos.

| Regra da develop | Situação atual / tratamento na integração |
|---|---|
| RN05 — envio da violação fora da rotina periódica | Preservar registro local independente de GNSS/rede. Entrega ao servidor ainda não implementada; manter limites de latência pendentes com o parceiro. “Na hora” não define SLA nem garante entrega sem conexão. |
| RN08 — data/hora GNSS e HDOP máximo de 2,0 | O firmware atual verifica validade, idade e satélites, mas não aplica esse limite de HDOP nem persiste data/hora absoluta. Registrar adequação futura do driver LC76G-PA; limite de HDOP ainda pendente. |
| RN09 — fila de 8 com prioridade para críticos | Preservar os 16 registros atuais e o CT21. Os 8 eventos são um valor proposto na regra. Prioridade, reenvio e saturação quando todos forem críticos ainda precisam ser definidos/implementados; fila finita não garante retenção ilimitada. |
| RN10 — leitura zero considerada falha | Preservar o CT23: no potenciômetro atual, zero é mínimo válido. Diagnóstico físico de falha depende do circuito e dos CT13/CT30. |
| RN11 — encerramento exige lacre fechado e NF de retorno | Incorporar a condição aos CT14/CT16 planejados. O botão atual continua apenas em diagnóstico. |

**Regras propostas para operação com bateria (21/10/2026).** Os identificadores `RN-TEC` complementam RN01–RN16 e não representam aprovação do parceiro.

| ID | Regra proposta | Situação |
|---|---|---|
| RN-TEC01 | Registrar a mudança estável do lacre independentemente de GNSS ou conexão. Separar detecção local de entrega do alerta. | Registro local existente; autorização e entrega pendentes. |
| RN-TEC02 | Preservar a última posição válida quando a coleta falhar, identificando-a como histórica e informando sua idade quando conhecida. Ausência de fix não vira coordenadas zero. | Parcial: referência em NVS existente; horário absoluto persistente pendente. |
| RN-TEC03 | Manter eventos até confirmação de recebimento. Dimensionar a fila para o período sem rede e definir saturação, inclusive quando todos os registros forem críticos. | Fila local existente; confirmação, prioridade e política final pendentes. |
| RN-TEC04 | Despertar por eventos e tarefas agendadas, mantendo a detecção do lacre disponível. Limitar aquisições e tentativas de comunicação por orçamentos configurados. | Suspensão e controle de alimentação pendentes. |
| RN-TEC05 | Validar identidade, dispositivo/ciclo, validade e política de revogação antes de aceitar uma autorização. | **PENDENTE DE VALIDAÇÃO:** prazo de chegada e possibilidade de autorização previamente armazenada. Não assumir autorização antecipada como requisito aprovado. |
| RN-TEC06 | Preservar ciclo, eventos e vencimentos durante suspensão e reinício. Despertar por lacre não deve adiar o próximo status periódico. | Hoje só há persistência da referência de posição e dos registros da fila. |

### 3.3.3. Requisitos Não Funcionais — 8 Eixos ISO/IEC 25010 (sprints 2 a 5)

Os critérios mensuráveis ficam nesta seção; procedimentos, resultados e evidências ficam em **3.4**, ligados pela matriz **3.3.4**. Cada critério informa condição de ensaio, métrica, limite, método e situação. Valores atuais do firmware não são SLA aprovado pelo parceiro. Propostas dependem de revisão; metas ausentes ficam como **PENDENTE DE VALIDAÇÃO**, sem resultado de aprovação.

**Situação confirmada pelo grupo em 21/10/2026:** ainda não há metas acordadas de autonomia, entrega de alertas ou duração máxima sem conexão. O tempo de autorização também depende de confirmação com o parceiro.

| ID / Eixo | Requisito e métrica | Critério e situação | Verificação |
|---|---|---|---|
| RNF-USAB01 / USAB | Distinguir posição atual, histórica e ausente. | Proposto: nenhuma coleta sem fix apresentada como atual; idade desconhecida explícita. Interface pendente. | CT19, CT22 e CT32; comparar JSON e interface quando disponível. |
| RNF-CONF01 / CONF | Detectar mudanças estáveis do lacre sem duplicações por ruído. | Atual: debounce do lacre de 60 ms. Proposto: zero duplicações por transição estável e zero eventos para pulsos observados menores que 60 ms no lote controlado. | CT18, CT24 e CT31; contar eventos ausentes, extras e falsos alertas. |
| RNF-CONF02 / CONF | Preservar registros confirmados em memória e a referência de posição. | Proposto: zero registros já confirmados como gravados perdidos no lote de reinícios; zero referência inválida aceita. Sobrecarga deve ser explícita. | CT21, CT22, CT29 e CT33; comparar conteúdo antes/depois. |
| RNF-DES01 / DES | Separar detecção local, entrega de alerta e autorização. | `L_det = t_registro_local − t_estabilizacao_contato`; `L_alerta = t_recebimento_servidor − t_estabilizacao_contato`; `L_aut = t_validacao_dispositivo − t_emissao_plataforma`. Limites e condições de rede: **PENDENTE DE VALIDAÇÃO**. | CT18 e CT32; protocolo inicial proposto de 30 estímulos por condição; registrar p95, máximo, perdas e incerteza temporal. |
| RNF-DES02 / DES | Limitar tempo de GNSS e comunicação ativos, inclusive sem cobertura. | Atual: espera de fix de 10 s por coleta e idade máxima de fix de 3 s; não são metas do módulo físico. Orçamentos futuros de GNSS, rádio, tentativas e espera: **PENDENTE DE VALIDAÇÃO**. | CT19, CT26 e CT28; medir duração por estado, tentativas e falhas. |
| RNF-DES03 / DES | Estimar autonomia pelo consumo do conjunto. | Medir corrente de repouso, picos, carga por aquisição/envio e carga diária equivalente. Autonomia mínima `A_min`, bateria e perfil de eventos: **PENDENTE DE VALIDAÇÃO**. Os 150 dias de permanência não definem autonomia. | CT27; medir na alimentação do conjunto, incluindo regulador, LEDs, sensores, GNSS e comunicação. |
| RNF-SUP01 / SUP | Permitir diagnóstico reproduzível. | Proposto: todos os ensaios identificam versão/configuração e estímulos. Futuro registro inclui estado, causa de despertar, ID do evento e qualidade do relógio. | CT17, CT24 e CT29; conferir logs sem credenciais ou dados reais. |
| RNF-SUP02 / SUP | Simular o Quectel LC76G, variante PA, escolhido para o dispositivo físico. | O mesmo driver GNSS deve funcionar no módulo real e no Wokwi, usando as interfaces, mensagens e transições descritas no contrato de simulação em 3.3.5. Placa e versão de firmware do módulo ainda precisam ser definidas; adaptação do simulador pendente. | CT26 e CT28; comparar sequências de comandos/respostas e estados para todos os cenários do contrato. Tempos sintéticos e consumo estimado devem ser identificados. |
| RNF-SEG01 / SEG | Impedir autorização por comandos inválidos. | Proposto: zero comandos adulterados, de outro dispositivo/ciclo ou expirados aceitos no lote. Política sem rede, revogação e relógio incerto: **PENDENTE DE VALIDAÇÃO**. Botão de bancada não comprova segurança. | CT34; exige canal de autorização implementado. |
| RNF-CAP01 / CAP | Suportar o período acordado sem rede e expor saturação. | Atual: 16 registros persistentes, sem reenvio; o 17º fica apenas na Serial. Capacidade final, volume de eventos e duração sem rede `D_offline`: **PENDENTE DE VALIDAÇÃO**. Dimensionar registros e bytes. | CT21 e CT33; medir ocupação, perdas por classe, retenção e tempo de esvaziamento. |
| RNF-REST01 / REST | Despertar por temporizador/lacre e preservar prazos. | Proposto: um tratamento por estímulo estável, sem despertares repetidos pelo mesmo nível mantido; nenhum prazo reiniciado pelo despertar. Tolerância do relógio e pulso mínimo detectável: **PENDENTE DE VALIDAÇÃO**. | CT24, CT25 e CT29; validar também no ESP32 físico. |
| RNF-REST02 / REST | Adequar fixação, contato e invólucro ao uso. | Limites de água, temperatura, poeira, vibração, força de fixação e posição da antena: **PENDENTE DE VALIDAÇÃO**. Não atribuir classificação IP sem ensaio correspondente. | CT28 e CT31; registrar condições, duração, falhas e infiltração. |
| RNF-ORG01 / ORG | Manter rastreabilidade e separar plano de evidência. | Proposto: todo CT executado contém versão, configuração, procedimento, resultado medido, critério e evidência anonimizada; pendências não aparecem como aprovadas. | Revisão da matriz 3.3.4 e dos registros em 3.4. |

**Como fechar as metas.** O parceiro define a necessidade operacional: latência máxima de autorização e alerta, autonomia, período sem rede, frequência de eventos e condições de uso. A equipe mede a viabilidade antes de acordar limites; registrar data e origem da aprovação junto ao valor. Ensaios temporais usam relógios sincronizados ou a mesma base de captura. Sem isso, informar a incerteza; não apresentar latência ponta a ponta como exata.

**Cálculo de autonomia.** Integrar a corrente medida na bateria: `Q_dia [mAh] = soma(I_i [mA] × t_i [h])` no perfil diário equivalente; `A_estimada [dias] = Q_util [mAh] / Q_dia [mAh/dia]`. Declarar capacidade útil e hipóteses de temperatura, envelhecimento, limite de descarga e autodescarga. Para coleta semanal, distribuir a carga do ciclo completo pelos sete dias e acrescentar eventos e tentativas de rede do perfil. Medir também picos e quedas de tensão. Comparar boa cobertura, falta de GNSS e falta de rede. Wokwi não comprova autonomia; ensaio curto produz estimativa, não comprovação de operação por meses.

### 3.3.4. Matriz de Rastreabilidade (RTM) (sprints 1 a 5)

Rastreabilidade técnica proposta, vinculada aos RF e RN de negócio incorporados da `develop`. A matriz não indica testes executados nem cobre integralmente os 27 RFs: prorrogação, prazo legal, ausência de comunicação e autenticação da plataforma/dispositivo ainda precisam de cobertura específica além dos cenários embarcados abaixo.

| Necessidade | RF | RN de negócio | Regra técnica proposta | RNF | Casos de teste | Evidência a produzir |
|---|---|---|---|---|---|---|
| Detecção do lacre independente de GNSS/rede | RF006, RF009, RF010 | RN05, RN09 | RN-TEC01 | RNF-CONF01, RNF-DES01 | CT05, CT08, CT18, CT24, CT31, CT32 | Estímulos e eventos com tempos. |
| Última localização válida identificável | RF005, RF015 | RN08 | RN-TEC02 | RNF-USAB01, RNF-DES02 | CT19, CT20, CT22, CT28, CT32 | JSON, referência e condições de recepção. |
| Eventos preservados sem rede | RF010 | RN09 | RN-TEC03 | RNF-CONF02, RNF-CAP01 | CT09, CT10, CT21, CT29, CT33 | Fila antes/depois e confirmações do servidor. |
| Autonomia e tentativas limitadas | RF005, RF008, RF011 | RN07, RN08 | RN-TEC04 | RNF-DES02, RNF-DES03, RNF-REST01 | CT24, CT26, CT27, CT28 | Transições e medições de corrente/tempo. |
| Autorização válida no prazo acordado | RF007 | RN05, RN06 | RN-TEC05 | RNF-SEG01, RNF-DES01 | CT06, CT07, CT11, CT32, CT34 | Comandos sintéticos, decisões e latências. |
| Ciclos e prazos preservados | RF004, RF011–RF014, RF016, RF017 | RN01, RN03, RN04, RN07, RN11 | RN-TEC06 | RNF-CONF02, RNF-REST01 | CT02–CT04, CT14–CT16, CT25, CT29 | Estado antes/depois de suspensão e reinício. |
| Diagnóstico reproduzível | Transversal | Transversal | RN-TEC01–RN-TEC06 | RNF-SUP01, RNF-ORG01 | CT17–CT34 | Versão, configuração e registro de execução. |
| Simulador correspondente ao módulo GNSS escolhido | RF005 | RN08 | RN-TEC02, RN-TEC04 | RNF-SUP02 | CT26, CT28 | Contrato de interface e comparação de comandos/respostas entre simulação e hardware. |
| Bateria, fixação e condições físicas | RF006, RF019, RF027 | RN05, RN10 | RN-TEC01, RN-TEC04 | RNF-DES03, RNF-REST02 | CT12, CT13, CT23, CT27–CT31 | Medições, fotos e condições do ensaio. |


### 3.3.5. Arquitetura da Solução (sprint 3)

**Estado atual.** O [firmware principal](../src/firmware/main.cpp) mantém o ESP32 ativo, recebe NMEA continuamente, verifica o lacre por polling, tenta reconectar o Wi-Fi e mantém LEDs de diagnóstico conforme as entradas. Há JSON na Serial e fila em NVS, sem entrega ao servidor. Ainda não há suspensão, controle de alimentação do GNSS, confirmação de entrega ou lógica de autorização/ciclo. Os valores atuais estão em [`include/config.h`](../include/config.h).

#### Máquina de estados proposta para energia e aquisição

A máquina de estados fica na arquitetura, com **diagrama de estados UML em Mermaid** e tabela de transições/condições. Um fluxograma pode complementar as ações internas. Esta proposta ainda não está implementada; lacre, autorização, ciclo, rede e fila são estados de negócio associados, distintos do estado de energia.

```mermaid
stateDiagram-v2
    state "Restaurar contexto" as Restaurar
    state "Classificar evento e prazos" as Classificar
    state "Registrar evento local" as Registrar
    state "Avaliar coleta necessária" as Avaliar
    state "Adquirir posição" as Adquirir
    state "Persistir resultado" as Persistir
    state "Avaliar comunicação" as Rede
    state "Comunicar e aguardar confirmação" as Comunicar
    state "Preparar suspensão" as Preparar
    state "Suspenso" as Suspenso
    state "Tratar falha de armazenamento" as Falha
    [*] --> Restaurar
    Restaurar --> Classificar
    Classificar --> Registrar: evento de lacre ou de negócio
    Classificar --> Avaliar: sem evento novo
    Registrar --> Avaliar: gravação confirmada
    Registrar --> Falha: erro de gravação
    Avaliar --> Adquirir: coleta devida
    Avaliar --> Rede: coleta dispensada
    Adquirir --> Persistir: fix válido ou orçamento esgotado
    Persistir --> Rede: gravação confirmada
    Persistir --> Falha: erro de gravação
    Rede --> Comunicar: janela de rede ou envio permitido
    Rede --> Preparar: sem tarefa de rede devida
    Comunicar --> Preparar: confirmação ou orçamento esgotado
    Falha --> Preparar: falha sinalizada e contingência aplicada
    Preparar --> Classificar: novo evento pendente
    Preparar --> Suspenso: contexto salvo e despertar armado
    Suspenso --> Restaurar: temporizador ou entrada de despertar
```

*Figura 2 — Proposta de estados de energia e aquisição. Fonte: elaboração própria. A transição de falha depende de política de contingência ainda a definir.*

| Condição / transição | Ação e condição de saída | Validação |
|---|---|---|
| Evento de lacre em qualquer estado ativo | Capturar e registrar a transição estável sem esperar GNSS/rede; priorizar seu tratamento. Aquisição e comunicação devem continuar atendendo essa entrada. | CT18, CT24 e CT26. |
| Coleta devida | Alimentar GNSS conforme o módulo; sair com fix válido ou orçamento esgotado. Falha conserva posição histórica. Coleta adicional por violação depende da política acordada. | CT19, CT26 e CT28. |
| Comunicação devida | Aplicar janela, limite de tentativas e espera entre elas; sem confirmação, conservar registros. Periodicidade de escuta depende de `L_aut`, pendente com o parceiro. | CT26, CT32 e CT33. |
| Preparação da suspensão | Salvar contexto, calcular o próximo prazo absoluto, desligar recursos dispensáveis e armar despertar. Entrada mantida não pode causar reinícios repetidos. | CT24, CT25 e CT29. |
| Falha de memória | Expor falha sem declarar o evento persistido. Definir contingência, limite de tentativas e eventual suspensão antes da implementação. | CT29. |

**Integração elétrica proposta.** Avaliar chave de alimentação do GNSS, backup opcional, regulador de baixo consumo e LEDs usados por tempo limitado. Um contato mecânico pode despertar o ESP32; a detecção deve continuar disponível enquanto os demais blocos dormem. Aumentar resistores dos LEDs trata somente essa parcela do consumo. A escolha depende do CT27; os resistores atuais de 220 Ω permanecem no circuito de bancada.

No ESP32 clássico, o GPIO27 do lacre é elegível para despertar por RTC; o GPIO18 da chave de rede não é. O despertar externo é por nível, exigindo rearme com lacre aberto. Os botões nos GPIO33/13 exigem estratégia compatível com as polaridades: EXT1 oferece `ANY_HIGH` ou `ALL_LOW`, não qualquer botão ativo em LOW. A polarização deve permanecer válida na suspensão. [Documentação de suspensão do ESP32](https://docs.espressif.com/projects/esp-idf/en/v4.4.3/esp32/api-reference/system/sleep_modes.html).

O atual `clock="uptime_only"`, baseado em `millis()`, não comprova data/hora após reinício. A evolução precisa preservar prazos, declarar qualidade do relógio e definir sincronização e tolerância à deriva. [Documentação de tempo do ESP32](https://docs.espressif.com/projects/esp-idf/en/v4.4.3/esp32/api-reference/system/system_time.html).

#### GNSS escolhido: Quectel LC76G-PA

**Decisão de 21/10/2026:** adotar o **Quectel LC76G, variante PA**, como módulo GNSS de referência do projeto e do modelo no Wokwi. A escolha considera UART/NMEA, alimentação de 3,3 V e modo de backup; aquisição na instalação e consumo do conjunto ainda exigem ensaio. A comparação abaixo registra a alternativa considerada. GNSS fornece localização; a plataforma ainda exige um meio de comunicação separado.

| Opção | Vantagens para o projeto | Limitações / custo de integração |
|---|---|---|
| GPS customizado atual no Wokwi | Coordenadas e validade do fix controláveis para testes repetíveis. Pode ganhar estados de alimentação, atraso de aquisição e falhas pela Chips API. | Hoje gera GGA por UART a 9600 baud; não processa suspensão nem modela consumo ou backup. Sua extensão valida lógica, não comportamento elétrico de módulo comercial. |
| u-blox MAX-M10S — alternativa considerada | UART/NMEA, modos de economia e backup por hardware. Backup típico de **28 µA**, nas condições `V_BCKP=3,3 V`, `VCC=V_IO=0 V`. | Verificar alimentação, nível de I/O e sequência de desligamento; escolher placa que exponha os sinais necessários. Consumo de placa, regulador e antena entra no ensaio. [Ficha R08](https://content.u-blox.com/sites/default/files/MAX-M10S_DataSheet_UBX-20035208.pdf). |
| **Quectel LC76G, variante PA — escolhido** | UART/NMEA e alimentação de 3,3 V na PA; backup típico de **13 µA**, com `V_BCKP` mantido, nas condições do fabricante. | Backup exige comando `PAIR650` e corte de VCC conforme o fabricante. Conferir a configuração UART da versão adquirida e compatibilizar com o firmware. A variante PB usa 1,8 V e não é substituição elétrica direta. [Hardware Design V1.3](https://www.quectel.com/content/uploads/2023/05/Quectel_LC76G_Series_Hardware_Design_V1.3.pdf). |

Não foi encontrado GPS nativo com esses recursos no [catálogo oficial do Wokwi](https://docs.wokwi.com/getting-started/supported-hardware); isso não exclui projetos comunitários. O [chip próprio do projeto](../chips/gps.chip.c) pode ser evoluído com a [Chips API](https://docs.wokwi.com/chips-api/getting-started).

**Diretriz de integração e simulação:** adaptar o chip do Wokwi ao **LC76G-PA**, reproduzindo os comportamentos usados pelo projeto. O gerador genérico atual ainda não satisfaz essa correspondência. A placa física deve permitir acesso a VCC, V_BCKP e UART para executar as sequências de energia. Validar o módulo escolhido pelo CT28 e medir `Q_ciclo = Q_backup + Q_aquisicao + Q_processamento + Q_comunicacao` na mesma alimentação de referência. Para intervalos semanais, comparar backup e desligamento completo; preservar dados não garante fix rápido após qualquer intervalo. Antena, céu obstruído e metal entram no ensaio. Rever o limite atual de 10 s após medições. Preço, estoque, placa e versão de firmware do módulo: **PENDENTE DE VALIDAÇÃO**.

**Contrato de simulação do LC76G-PA (RNF-SUP02).** Modelo e variante definidos; registrar a versão de firmware/protocolo da unidade adquirida antes de concluir a compatibilidade. A referência de hardware é o documento Quectel V1.3. A implementação do simulador ainda está pendente.

| Aspecto | Correspondência exigida | Como verificar |
|---|---|---|
| Interface e mensagens | Pinagem utilizada, UART e configuração compatíveis com LC76G-PA; sentenças NMEA, validade e checksums consumidos pelo mesmo driver. Comandos PAIR utilizados e respostas devem seguir a versão de protocolo adotada. | Capturar tráfego dos dois ambientes e comparar campos e decisões do driver; não aprovar apenas porque ambos produzem latitude/longitude. Os 9600 baud atuais não comprovam a configuração de fábrica do módulo adquirido. |
| Alimentação e backup | Modelar desligado, aquisição, fix e backup. Para entrar em backup: enviar `$PAIR650,0*25`, cortar VCC e manter V_BCKP; para sair, restaurar VCC, respeitando a sequência do Hardware Design V1.3. Representar também perda do backup e reset. | Conferir disponibilidade da UART, retenção de contexto e retomada. Corte de VCC sem o comando prévio não deve ser tratado como entrada normal em backup. Sinais auxiliares exclusivos do ensaio devem ficar identificados como simulação; não inventar pino físico de controle. |
| Aquisição e falhas | Permitir atraso de aquisição, falta de fix, fix desatualizado e interrupção de UART. Tempos de ficha técnica são parâmetros de cenário, não garantias de recepção física. | Executar CT19 e CT26; calibrar cenários com medições do CT28, mantendo explícito o que é sintético. |
| Limites do modelo | Declarar funções não simuladas, como recepção de RF, desempenho real da antena e corrente elétrica. Valores de consumo atribuídos por estado são estimativas. | CT27 continua obrigatório para consumo físico; não apresentar modelo lógico como validação de autonomia. |

**Arquitetura a complementar:** transporte, diagrama completo com plataforma/API e vídeo de apresentação permanecem pendentes. O diagrama de estados acima cobre apenas o subsistema embarcado proposto.


### 3.3.6. Arquitetura do Protótipo(sprint 4)	

_Vídeo de explicação sobre a arquitetura do protótipo físico: (coloque aqui o link do vídeo)_

_Descreva as tecnologias utilizadas de forma detalhada (visão de tecnologia).

Descreva a arquitetura usando um diagrama de blocos similar à visão anterior, porém especificando as tecnologias utilizadas. O diagrama de arquitetura deve:_
- _Mostrar microcontroladores, incluindo descrições de sua função no sistema (por exemplo: "Irá processar o sinal dos sensores a cada X minutos")_
- _Mostrar sensores, incluindo descrição de função e especificações técnicas do tipo de informação que será coletada_
- _Mostrar apresentadores de informação (displays), incluindo descrição de que tipo de informação será apresentada (por exemplo, "Mostrar temperatura dos sensores")_
- _Mostrar atuadores, caso existam na solução, incluindo descrições do que irão acionar (por exemplo, "Ligar motor de irrigação durante x minutos")_
- _Mostrar o broker MQTT e o dashboard que é a interface do usuário_
- _Mostrar ligações entre os elementos (com fio ou sem fio) – incluindo conexões com sensores e atuadores, conexão com WiFi, entre outros._
_Faça uma tabela dos possíveis componentes utilizados. Todos os componentes devem estar presentes na arquitetura._

| **Componente** | **Descrição** | **Tipo** | 
|----------|----------|----------|
|E.g. ESP32 WROOM 32 Devkit|E.g. Trata-se de um microcontrolador amplamente usado na indústria e em projetos educacionais, que possui interfaces mais amigáveis para conexão de componentes diversos...|E.g. Microcontrolador|
|E.g. sensor XPTO|E.g. Este sensor mede o sinal de XYZ em unidades q/s, e possui tolerância de abcd...|E.g. Sensor|

### 3.3.7. Arquitetura Refinada da Solução (sprint 5)	

_Descreva a revisão da arquitetura técnica da solução de forma detalhada (visão de arquitetura). <br>

Justifique como a arquitetura suporta os requisitos funcionais e não funcionais. <br>

A revisão deverá incluir comentários sobre cada ponto levantado, mostrando como os ajustes foram realizados, além da descrição da arquitetura revisada._


## 3.4. Resultados	

_Nesta seção, detalhe os resultados obtidos com a implementação, de acordo com o disposto nas subseções._

### 3.4.1. Protótipo Inicial do Projeto usando o Simulador Wokwi (sprint 1)

**Link do protótipo:** https://wokwi.com/projects/478276108359405569

**Circuito principal integrado (21/10/2026):** o arquivo local de referência é
[`src/firmware/prototipo_wokwi/diagram.json`](../src/firmware/prototipo_wokwi/diagram.json),
com ESP32 DevKit v1, GPS/GNSS customizado, lacre no GPIO27, potenciômetro no GPIO34,
LEDs verde/amarelo/vermelho/azul nos GPIO25/26/23/14, botões nos GPIO33/13 e chave
de rede no GPIO18. O [guia do circuito](../src/firmware/prototipo_wokwi/README.md)
contém a pinagem e o roteiro de diagnóstico. As configurações `wokwi.toml` e
`wokwi.weekly.toml` ficam nessa mesma pasta e são selecionadas por **Wokwi: Select Config File**.
O projeto público acima e a Figura 1 representam a versão anterior.

Na Sprint 1 o dispositivo é simulado no Wokwi com um ESP32 DevKit. Cada entrada do circuito representa uma situação física do rastreador: o lacre (`SW_LACRE`), a disponibilidade de conexão (`SW_REDE`), a autorização de retirada (`BTN_AUTORIZAR`), o encerramento ou vínculo de ciclo (`BTN_CICLO`) e o nível da bateria (`POT_BATERIA`). Os LEDs representam estados para inspeção e o Monitor Serial mostra os registros JSON. Não há transporte implementado para servidor ou broker. A integração local inclui GNSS simulado; a validação do hardware GNSS permanece para a Sprint 2.

Na integração atual, os botões têm leitura de diagnóstico: amarelo acompanha o
botão de autorização e o botão de ciclo aparece no heartbeat. Verde indica lacre
íntegro, vermelho indica violação e azul indica indisponibilidade da rede simulada.
A chave de rede não desliga o Wi-Fi virtual; os estados são apresentados separadamente.
As regras e os CTs a seguir descrevem o comportamento previsto para as próximas
integrações de firmware; não comprovam execução dessas regras no circuito integrado.

**Tempo e parâmetros atuais.** O firmware principal usa `demo` com coleta a cada 30 s ou `weekly` com sete dias. A escala `1 s = 1 dia` dos CT02–CT04 é uma proposta para a futura lógica de negócio; não está implementada no firmware atual. Hoje o intervalo é contado a partir da conclusão da coleta anterior, incluindo nova espera de fix quando necessária.

| Parâmetro técnico atual | Valor / comportamento | Fonte |
|---|---|---|
| Intervalo de coleta | 30 s (`demo`) / 7 dias (`weekly`) | `Config::SampleIntervalMs` |
| Espera máxima por fix / idade máxima do fix | 10 s / 3 s | `Config::GpsWaitMs` / `Config::FixMaxAgeMs` |
| Debounce do lacre | 60 ms; botões ainda em diagnóstico | `Config::SealDebounceMs` |
| Fila local | 16 registros; antigos preservados, novo apenas na Serial se cheia | `Config::QueueCapacity` e `enqueue()` |
| Movimento entre coletas válidas | Distância estritamente maior que 100 m | `Config::MovementThresholdM` |
| Heartbeat / tentativa de reconexão Wi-Fi | 2 s / 30 s quando desconectado | `Config::HeartbeatMs` / `Config::WifiRetryMs` |

**Valores propostos para testes de negócio.** Nos cenários de ciclo, a contagem de dias fora parte da emissão da NF de remessa, conforme RF012; não corresponde ao `uptime_ms` atual. A tabela seguinte não substitui a configuração atual. O TAPI fixa a referência de 150 dias para permanência e o status semanal; as demais metas dependem do parceiro. Tempo de chegada da autorização e possibilidade de autorização antecipada continuam **PENDENTE DE VALIDAÇÃO**.

| Parâmetro | Valor de teste | Origem |
|---|---|---|
| Status periódico | 7 dias | TAPI ("uma vez por semana, com frequência ajustável") |
| Limiar de permanência | 150 dias (também testado com 10) | TAPI ("150 dias como referência inicial"; MVP 4 pede outros limiares) |
| Prazo previsto de retorno | 5 dias | Valor de teste — `PENDENTE DE VALIDAÇÃO` |
| Janela de autorização de retirada | 3 dias | Valor de teste — `PENDENTE DE VALIDAÇÃO` |
| Capacidade da fila sem conexão | Atual: 16 eventos; dimensionamento final pendente | A proposta anterior de 8 não corresponde ao firmware atual |
| Bateria baixa / recuperada | 20% / 25% | Valor de teste — `PENDENTE DE VALIDAÇÃO` |

**LEDs simultâneos.** Hoje verde/vermelho indicam o estado estável do lacre, amarelo acompanha o botão de autorização e azul indica rede simulada indisponível. As piscadas e os significados de negócio dos CT01–CT16 são planejados.

**Do simulador ao dispositivo real.** O alvo principal compila `src/firmware/main.cpp`. O `sketch.ino` da pasta do circuito é um teste independente de bancada. A migração física exige validar interfaces, alimentação, sensores e firmware; não é apenas trocar fontes de leitura.

| Componente no Wokwi | O que representa no dispositivo real |
|---|---|
| ESP32 DevKit | Microcontrolador do rastreador |
| SW_LACRE (chave deslizante) | Sensor do lacre (laço de continuidade ou reed switch) |
| BTN_AUTORIZAR (botão) | Autorização de retirada registrada pelo responsável (no produto, vinda da plataforma) |
| BTN_CICLO (botão) | Encerramento do ciclo ou novo vínculo, feitos na conferência de retorno |
| POT_BATERIA (potenciômetro) | Percentual sintético; medição física, calibração e diagnóstico do divisor ainda pendentes |
| SW_REDE (chave deslizante) | Disponibilidade simulada, independente do Wi-Fi; hoje não interrompe transporte nem esvazia fila |
| LEDs | Diagnóstico local; política futura de indicação e duração depende da implementação |
| Monitor Serial (JSON) | Registro local; não comprova publicação MQTT ou recebimento pela plataforma |

A versão inicial pública não inclui localização. O circuito principal local inclui
GPS/GNSS customizado nos GPIO16/17; ele não modela recepção de satélites nem consumo real.

**Situação dos CT01–CT16: planejados, não executados nesta revisão.** As colunas de saída descrevem expectativas da futura lógica de negócio. Seus RF/RN seguem as definições de 3.3.1/3.3.2; divergências com a implementação estão explicitadas em 3.3.2. CT17–CT23 caracterizam funções atuais; CT24 em diante tratam evolução e validação física. Compilação e testes matemáticos isolados não comprovam aprovação destes cenários.

![Circuito no Wokwi com o Monitor Serial](../assets/wokwi/circuito.png)

*Figura 1 — Circuito anterior do protótipo no Wokwi, com o Monitor Serial aberto, antes da integração local do GPS.*

Fonte: elaboração própria, captura do simulador Wokwi.

**CT#** | **Bloco** | **Componente de entrada** | **Leitura da entrada** | **Componente de saída** | **Leitura da saída** | **Descrição**
|----------|----------|----------|----------|----------|----------|----------|
|CT01|Inicialização|SW_LACRE e SW_REDE|Lacre fechado e rede disponível ao ligar|LED verde; Serial|Verde fixo, demais apagados; JSON `STATUS` com `dias_fora=0`, `lacre="fechado"`|Esperado: Ao ligar com o lacre íntegro e conexão, o dispositivo indica operação normal e publica o primeiro status (RF009).|
|CT02|Status periódico|Relógio simulado (1 s = 1 dia)|Dias 7 e 14 de ciclo|Serial|Um `STATUS_SEMANAL` no dia 7 e outro no dia 14, nenhum fora desses dias|Esperado: O status é enviado a cada 7 dias, como pede o TAPI (RF011, RF009; RN07).|
|CT03|Prazo e atraso|Relógio simulado (1 s = 1 dia)|`dias_fora` passa do prazo previsto (5) e fica abaixo do limiar|LED amarelo; Serial|Amarelo piscando a 1 Hz; `ALERTA_ATRASO` emitido uma única vez|Esperado: O equipamento passou da data prevista de retorno, o que é diferente de permanência prolongada (RF013, RF012; RN04).|
|CT04|Permanência|Relógio simulado (1 s = 1 dia)|`dias_fora` atinge o limiar (rodar com 10 e com 150)|LED amarelo; Serial|Amarelo fixo; `ALERTA_PERMANENCIA` emitido uma única vez em cada rodada|Esperado: Testa o limiar configurável com o valor de referência e um valor alternativo, como pede o MVP 4 do TAPI (RF014, RF012; RN03, RN04).|
|CT05|Lacre|SW_LACRE|Lacre aberto sem autorização|LED vermelho; Serial|Vermelho piscando; `VIOLACAO_LACRE` é registrado sem esperar GNSS/rede, fora da rotina semanal|Esperado: Abertura não autorizada gera registro local; medir entrega separadamente conforme RNF-DES01 (RF006; RN05).|
|CT06|Retirada autorizada|BTN_AUTORIZAR e SW_LACRE|Botão pressionado; lacre aberto dentro da janela de 3 dias e depois fechado|LED verde; Serial|Verde piscando durante a retirada; `RETIRADA_AUTORIZADA` sem alerta; ao fechar, `LACRE_REAPLICADO` e verde fixo|Esperado: Fluxo de retirada autorizada para o reparo, com reaplicação do lacre (RF007; RN06).|
|CT07|Autorização expirada|BTN_AUTORIZAR e SW_LACRE|Botão pressionado; lacre aberto depois de mais de 3 dias|LED vermelho; Serial|Mesmo resultado do CT05: `VIOLACAO_LACRE`|Esperado: Autorização vencida não libera a abertura (RF006; RN05, RN06).|
|CT08|Sem conexão|SW_REDE e SW_LACRE|Rede indisponível; lacre aberto sem autorização|LEDs azul e vermelho; Serial|Azul e vermelho piscando; nada publicado; `VIOLACAO_LACRE` guardada na fila; Serial mostra `[FILA] 1 pendente`|Esperado: Sem conexão, o evento crítico deve ficar guardado para envio posterior dentro da capacidade disponível; saturação é tratada no CT10 (RF010, RF008; RN05, RN09).|
|CT09|Reconexão|SW_REDE|Rede volta a ficar disponível|LED azul; Serial|Eventos pendentes publicados na ordem, com o dia original de ocorrência; azul apaga|Esperado: Ao reconectar, registros confirmados pelo servidor são removidos sem perder o horário de ocorrência (RF010, RF008; RN09).|
|CT10|Fila cheia (evolução)|SW_REDE e entradas que geram eventos|Exceder a capacidade configurada; repetir com fila mista e só críticos|Serial e memória|Prioridade e descarte conforme política aprovada; contador de perdas e saturação explícitos|Planejado. Definir capacidade e política quando todos forem críticos; não prometer retenção ilimitada. O comportamento atual de 16 registros é caracterizado no CT21 (RF010; RN09).|
|CT11|Ruído de botão|BTN_AUTORIZAR|Pressões rápidas (ruído de contato) e depois o botão segurado por 3 s|Serial|Um único evento por pressão real (debounce do botão a definir; os 60 ms atuais se aplicam ao lacre, detecção por borda); segurar não repete o evento|Esperado: Leituras falsas do botão não geram autorizações em duplicidade (RF007).|
|CT12|Bateria|POT_BATERIA|Nível abaixo de 20%, depois acima de 25%|Serial|`BATERIA_BAIXA` uma única vez; não repete até voltar a 25% (histerese)|Esperado: O alerta de bateria não fica oscilando perto do limite (RF019, RF027; RN10).|
|CT13|Falha do sensor de bateria (evolução)|Circuito físico de medição com diagnóstico|Injetar abertura/curto conforme interface definida|Serial|Distinguir falha somente quando houver evidência elétrica independente|Planejado, depende de hardware. ADC = 0 isoladamente não distingue desconexão de nível mínimo. O potenciômetro atual produz 0% válido no mínimo; verificar no CT23 (RF019, RF027; RN10).|
|CT14|Encerramento|BTN_CICLO e SW_LACRE|NF de retorno lançada; lacre fechado; botão segurado por 2 s|Serial|`CICLO_ENCERRADO`; `dias_fora` zera; `ciclo_id` sobe; contador de ciclos preservado|Esperado: Encerrar o ciclo libera o dispositivo sem apagar o histórico (RF016, RF017; RN11).|
|CT15|Vínculo simultâneo|BTN_CICLO|Ciclo ativo; toque curto (tentativa de novo vínculo)|Serial|`VINCULO_RECUSADO`; o ciclo atual continua igual|Esperado: Impede vínculos simultâneos indevidos, como pede o TAPI (RF004; RN01).|
|CT16|Encerramento recusado|BTN_CICLO e SW_LACRE|Rodada A: lacre aberto e NF lançada. Rodada B: lacre fechado e NF ausente; solicitar encerramento em ambas|Serial|`ENCERRAMENTO_RECUSADO`; o ciclo continua ativo|Esperado: O ciclo só pode ser encerrado com o lacre fechado e a NF de retorno lançada (RF016; RN11).|

#### Casos aplicáveis ao firmware integrado atual

**Situação: planejados; nenhuma execução destes CTs foi realizada nesta revisão documental.** Usar o circuito local e o ambiente `demo`, registrar as alterações locais e as constantes efetivas. Começar com memória de teste conhecida quando exigido, preservando qualquer dado anterior. Registrar versão do firmware, log Serial completo e estímulos; inspecionar NVS quando o critério exigir conteúdo persistido. Uma mensagem de log sozinha não comprova retenção após falta de energia.

| CT | Pré-condição e procedimento | Resultado esperado / medição | Limitação ou preparação |
|---|---|---|---|
| CT17 | Acionar lacre, autorização, ciclo e chave de rede individualmente; repetir com rede simulada indisponível e lacre aberto. | Verde/vermelho acompanham lacre estável; amarelo acompanha autorização; azul acompanha rede simulada. Heartbeat mostra ambos os botões e separa `wifi` de `rede_sim`. Nenhum botão autoriza retirada ou encerra ciclo no firmware atual. | Diagnóstico atual. Não interpretar LED como confirmação de servidor. |
| CT18 | Com captura temporal controlada, aplicar pulsos de 20 e 40 ms no lacre, depois 10 aberturas/fechamentos estáveis de 200 ms; repetir durante espera por fix e com rede indisponível. | Zero eventos nos pulsos curtos observados; um `seal_changed` por transição estável, sem repetição enquanto o nível é mantido. `authorized_removal=null`. Registrar `L_det`, perdas e duplicações; ampliar para 30 estímulos por condição ao medir p95. | Exige injeção temporizada; cliques manuais não comprovam milissegundos. Debounce atual de 60 ms; limite máximo de latência pendente. |
| CT19 | Rodada A: iniciar sem fix. Rodada B: obter posição válida, invalidar o fix e aguardar próxima coleta. Rodada C: interromper NMEA por mais de 3 s antes de solicitar nova coleta. | Sem fix recente, emitir `location_unavailable` após a espera configurada de 10 s, mais latência de processamento medida. Latitude/longitude atuais e `moved` são `null`; histórico é preservado quando disponível. O lacre continua respondendo durante a espera. | Rodada C exige controle da emissão UART no ensaio. Não aceitar só a passagem do tempo com NMEA válido chegando. A referência de comparação é a coleta válida anterior, distinta do último fix observado. |
| CT20 | Com memória de teste sem referência e fix contínuo, coletar primeiro ponto; depois pontos sintéticos separados por aproximadamente 50 m e 150 m da respectiva coleta anterior. Observar ao menos três coletas em `demo`. | Primeiro ponto: `baseline_created=true`, distância e `moved` nulos. Nas seguintes, `moved=false` para 50 m e `true` para 150 m. Com fix disponível, intervalo de 30 s mais latência de processamento; registrar desvio. | Comparar coletas válidas consecutivas. Testar fronteira de 100 m em teste matemático separado, considerando arredondamento; não alegar sete dias medidos com demo de 30 s. |
| CT21 | Iniciar fila vazia em memória de teste; gerar 17 registros, contando também coletas automáticas. Inspecionar chaves `q0` a `q15`; alternar a chave de rede. | Os primeiros 16 ocupam a fila; o 17º aparece na Serial com aviso de limite e não substitui os anteriores. A chave de rede não esvazia a fila. Medir quantos registros ficaram somente na Serial. | Caracteriza a limitação atual, não aprova RN-TEC03. Inspeção de NVS exige ferramenta/instrumentação de teste; capacidade final e prioridade ainda pendentes. |
| CT22 | Salvar posição A e registros conhecidos; reiniciar preservando flash/NVS. Sem novo fix, observar saída; depois coletar B. | Referência A e fila permanecem. Histórico após reinício sem idade conhecida apresenta `last_valid_age_ms=null`; novo ponto B compara com A. `uptime_ms` reinicia e não deve ser tratado como horário absoluto. | Confirmar preservação de flash no ambiente usado; se indisponível no simulador, executar em hardware. Sem acesso à NVS, não aprovar retenção da fila apenas pelo JSON da posição. |
| CT23 | Mover potenciômetro ao mínimo, meio e máximo; observar heartbeat/JSON. | Percentual sintético permanece entre 0 e 100 e acompanha a entrada; zero representa mínimo válido. Não emitir conclusão de bateria desconectada, autonomia ou tensão física com esses dados. | ADC e percentual simulados não são medição de estado de carga de uma bateria real. |

#### Casos para evolução do gerenciamento de energia

**Situação: bloqueados por implementação; não executados.** Exigem máquina de estados, observabilidade e simulação de alimentação/tempo. O chip GNSS atual não modela esses estados. Aplicar primeiro em modelo/teste controlado e depois confirmar no hardware. Metas de negócio pendentes impedem aprovação correspondente.

| CT | Pré-condição e procedimento | Critério proposto | Dependência |
|---|---|---|---|
| CT24 | Suspender; abrir lacre e mantê-lo aberto por 60 s; fechar/reabrir; repetir abertura durante aquisição, comunicação e preparação da suspensão. Testar também temporizador e cada botão habilitado para despertar. | Uma ocorrência por transição estável, nenhuma perda no lote e nenhum ciclo repetido de despertar causado pelo mesmo nível mantido. Identificar causa e estado em log; medir latência. | Definir rearme por nível, polarização, combinações RTC e pulso mínimo detectável. GPIO18 não serve como despertar RTC no ESP32 clássico. Repetir fisicamente. |
| CT25 | Com relógio controlado, atravessar vencimento de status e de autorização durante suspensão; provocar despertar por lacre antes do status e reinício em cada etapa. | Próximo prazo não é adiado pelo evento; vencimentos e ciclo persistem; autorização expirada não vira válida após reinício. Verificar regras nos limites de tempo e quando o relógio é incerto. | Agendamento persistente, relógio confiável e regras de ciclo/autorização; tolerâncias e política para relógio incerto pendentes. |
| CT26 | Manter GNSS sem fix e rede indisponível por vários ciclos; depois restaurar. Injetar evento de lacre durante cada espera. | Aquisição e comunicação terminam dentro dos orçamentos configurados; número de tentativas respeita limite e espera entre elas. Detectar lacre, conservar fila e retomar suspensão quando permitido, sem repetição ilimitada de tentativas. | Orçamentos, política de escuta/autorização e retomada definidos. Verificar comandos de alimentação e duração em log; consumo somente no CT27. |

**Verificação adicional do CT26 — correspondência com LC76G-PA:** após implementar o modelo, executar com o mesmo driver os cenários de partida, comando `$PAIR650,0*25`, corte da alimentação principal com e sem comando prévio, backup mantido/perdido, retorno da alimentação, falta de fix e recuperação. Comparar mensagens e transições com o contrato de 3.3.5 e o CT28. Comandos inválidos devem seguir a resposta documentada do módulo. Funções não suportadas pelo modelo devem ser declaradas; nenhum atalho exclusivo do firmware de simulação pode substituir a interface GNSS real.

**Registro de execução a preencher para cada CT:** data; responsável; versão e alterações locais; configuração; ambiente; pré-condições; estímulos e quantidade de repetições; resultado medido; critério utilizado; situação (`NÃO EXECUTADO`, `BLOQUEADO`, `APROVADO` ou `REPROVADO`); evidência; limitações. Fotos do simulador podem ficar em `assets/wokwi/CTnn.png`; medições e logs anonimizados devem acompanhar o registro, sem usar relatórios internos como evidência. Não criar arquivos de evidência vazios nem marcar teste aprovado antes da execução.

### 3.4.2. Protótipo Físico do Projeto (offline) (sprint 2)	

**Plano físico, não executado.** Definir componentes, condições e metas antes dos ensaios. Estes casos podem evoluir nas sprints seguintes. Fotos da montagem e medições devem ser adicionadas quando produzidas, distinguindo medição de estimativa.

| CT | Procedimento e condições | Métricas / critério | Pendências |
|---|---|---|---|
| CT27 | Medir na alimentação do conjunto, sem alimentação paralela por USB. Capturar repouso, despertar, GNSS, escrita em memória e comunicação. Comparar circuito atual e versão de baixo consumo com o mesmo perfil de eventos; repetir sem cobertura. | Corrente média/de repouso, picos, duração e carga por estado; calcular `Q_dia` e autonomia estimada conforme RNF-DES03. Registrar instrumento, resolução, taxa de amostragem, tensão e temperatura. Medir corrente do contato fechado, LEDs, regulador e divisor de bateria. | Bateria e autonomia-alvo a confirmar. Instrumento deve capturar pulsos de corrente; não somar diretamente correntes medidas em tensões diferentes. LEDs com acionamento limitado e divisor comutado são hipóteses a comparar. |
| CT28 | Ensaiar o Quectel LC76G-PA escolhido em partida sem dados de backup, retomada com backup e desligamento completo, conforme sequência do fabricante. Repetir com antena instalada no invólucro, próximo a metal, a céu aberto e sob cobertura. Registrar placa, firmware, configuração UART e intervalo real entre aquisições; comparar tráfego e estados com o modelo Wokwi do CT26. | Tempo até primeira posição válida, taxa de obtenção de fix dentro do orçamento, erro contra referência conhecida, carga por aquisição e carga de backup. Sem fix, manter histórico identificado. | Critérios de precisão, taxa de sucesso e orçamento pendentes. Antena e placa influenciam resultado; não prometer posicionamento indoor. A simulação de sete dias não substitui retenção física por sete dias. |
| CT29 | Interromper alimentação/reiniciar antes, durante e após gravação, aquisição e preparação da suspensão; testar falha de escrita com injeção controlada. Observar tensão nos picos de rádio/GNSS. | Preservar registros cuja gravação foi confirmada e recuperar estado consistente; dados incompletos devem ser rejeitados ou sinalizados. Falha não aparece como gravação bem-sucedida; medir perdas e reinícios por queda de tensão. | Usar memória de teste e fonte controlada. Estados de ciclo/prazos dependem de implementação; a referência de posição atual já pode ser caracterizada. |
| CT30 | Comparar leitura de bateria com instrumento de referência em vários pontos da faixa permitida; injetar falhas de medição conforme o circuito e acionar limiares de bateria baixa/recuperada. | Erro de tensão e repetibilidade; alarme e histerese conforme limites aprovados. Distinguir diagnóstico elétrico de inferência por ADC; não equiparar tensão linear a percentual sem modelo de bateria. | Divisor, proteção, calibração, bateria, limites e diagnóstico ainda a definir; não aplicar tensão da célula diretamente ao GPIO34. |
| CT31 | Ensaiar fixação, abertura/reaplicação do lacre, vibração e exposição à água nas condições previamente acordadas; verificar contato e operação antes/depois. | Contar violações detectadas, falsos alertas, falhas de contato, deslocamento e infiltração; verificar repetibilidade e latência após ensaio. | Definir método, força, duração, temperatura, água e critério de aceitação antes da execução. Ensaio do protótipo não implica certificação IP. |

### 3.4.3. Protótipo do Projeto com MQTT e I2C (sprint 3)	

**Plano de integração, não executado; bloqueado por transporte e plataforma.** JSON na Serial e Wi-Fi conectado não comprovam recebimento no servidor. A escolha do meio de comunicação deve considerar a cobertura real, sem presumir Wi-Fi disponível nas oficinas. Os ensaios abaixo também se aplicam caso seja escolhido outro transporte.

| CT | Configuração e procedimento | Resultado esperado / medição | Pendências |
|---|---|---|---|
| CT32 | Com relógios sincronizados, gerar abertura de lacre e emitir autorização em momentos distintos do ciclo de escuta. Repetir com dispositivo ativo, suspenso, sem GNSS, com rede e após interrupção de rede. | Medir separadamente `L_det`, `L_alerta` e `L_aut`, p95, máximo e perdas. Em indisponibilidade, registrar duração sem rede e tempo desde reconexão até entrega, sem esconder atraso total. Interface distingue posição histórica e atual. | Limites com parceiro, transporte, observabilidade e política de escuta. Não assumir autorização previamente salva nem prometer resposta em segundos com rádio desligado. |
| CT33 | Acumular eventos mistos e apenas críticos; exceder capacidade; restaurar rede; perder confirmações e reiniciar durante reenvio. | Dentro da capacidade acordada, eventos são entregues sem perda; registros saem da fila somente após confirmação definida. Retransmissões mantêm ID e não duplicam ocorrência no servidor. Saturação segue política aprovada e expõe perdas por classe. | Definir o que a confirmação garante (broker ou persistência da aplicação), IDs, prioridade, política de saturação e `D_offline`. |
| CT34 | Enviar comando válido, expirado, adulterado, repetido e destinado a outro dispositivo/ciclo; testar reinício, relógio incerto e revogação com dispositivo desconectado. | Rejeitar comandos inválidos e registrar motivo. Para comando válido, respeitar janela e vínculo aprovados; reenvio não deve ampliar validade. Operação offline/revogação deve seguir regra expressamente acordada. | Autenticação, autorização, armazenamento e relógio ainda pendentes. Um botão local de diagnóstico não substitui este ensaio. |

### 3.4.4. Protótipo Físico do Projeto (online) (sprint 4)	

_Aqui você deve registrar diversas situações de uso de seu sistema como um todo, indicando exemplos de ação do usuário e resposta do sistema, apontando como o ambiente deverá estar configurado para receber a ação e produzir a resposta. Estes registros serão utilizados para testar seu sistema, portanto, descreva várias situações, incluindo não apenas casos de sucesso, mas também de falha nos comportamentos do sistema. <br>
**Desta vez, utilize diagramas de sequência UML para descrever os fluxos de teste do sistema.** <br>
Siga as nomenclaturas e convenções já utilizadas anteriormente, e não se esqueça dos alinhamentos de negócios e experiência do usuário para pensar em situações representativas. Inclua figuras do protótipo físico e dashboards, além de descrições dos testes realizados para ilustrar o funcionamento do protótipo._

### 3.4.5. Protótipo Final do Projeto (sprint 5)	

_Registre as situações de uso do sistema revisadas utilizando a modelagem UML para descrever os fluxos de teste. <br>
Também inclua figuras da versão final do protótipo físico e dashboards, além de descrições dos testes realizados para ilustrar o funcionamento do protótipo._

## <a name="c4"></a>4. Conclusões e Recomendações

_Escreva, de forma resumida, sobre os principais resultados do seu projeto e faça recomendações formais ao seu parceiro de projeto em relação ao uso dessa solução. Você pode aproveitar este espaço para comentar sobre possíveis materiais extras._

## <a name="c5"></a>5. Referências

1. CERRADINHOBIO. *CerradinhoBio supera R$ 1,5 bilhão de EBITDA e amplia lucro em 90% na Safra 2025/2026*. Disponível em: https://www.cerradinhobio.com.br/news/cerradinhobio-supera-r-15-bilhao-de-ebitda-e-amplia-lucro-em-90-na-safra-2025-2026/. Acesso em: 15 out. 2026.
2. CERRADINHOBIO. *Nossos Números/Dados de Produção*. Disponível em: https://www.cerradinhobio.com.br/a-companhia/nossos-numeros-dados-de-producao/. Acesso em: 19 out. 2026.
3. CERRADINHOBIO. *CerradinhoBio*. Disponível em: https://www.cerradinhobio.com.br/negocios-e-atividades/cerradinhobio/. Acesso em: 15 out. 2026.
4. CERRADINHOBIO. *Nossos Produtos*. Disponível em: https://www.cerradinhobio.com.br/negocios-e-atividades/nossos-produtos/. Acesso em: 15 out. 2026.
5. CERRADINHOBIO. *CerradinhoBio tem 3T24/25 com aumento de receita, expansão da produção de açúcar e melhora nos indicadores financeiros*. Disponível em: https://www.cerradinhobio.com.br/news/cerradinhobio-tem-3t24-25-com-aumento-de-receita-expansao-da-producao-de-acucar-e-melhora-nos-indicadores-financeiros/. Acesso em: 15 out. 2026.
6. CERRADINHO. *TAPI Cerradinho: Rastreamento de Ativos IoT*. Documento de referência do projeto fornecido pelo parceiro, 2026.
7. RAÍZEN. *Relatório integrado — safra 2024/25*. Disponível em: https://www.raizen.com.br/archives_files/archive81-raizen-rs24-25.pdf. Acesso em: 19 out. 2026.
8. CERRADINHOBIO. *Nossos produtos*. Disponível em: https://www.cerradinhobio.com.br/negocios-e-atividades/nossos-produtos/. Acesso em: 19 out. 2026.
9. SÃO MARTINHO. *Relatório da Administração — safra 2024/25*. Disponível em: https://ri.saomartinho.com.br/Download.aspx?Arquivo=MTU+51Kmm5DdaZuzsVXkAw%3D%3D&IdCanal=3kK+JHozjxal5isJwnwwEA%3D%3D&linguagem=pt. Acesso em: 19 out. 2026.
10. BP. *bp bioenergy — operações e capacidade de produção no Brasil*. Disponível em: https://www.bp.com/pt_br/brazil/home/produtos-e-servicos/bp-bunge-bioenergia.html. Acesso em: 19 out. 2026.
11. FS. *Etanol, unidades e capacidade de produção*. Disponível em: https://www.fs.agr.br/. Acesso em: 19 out. 2026.
12. INPASA. *Inauguração da unidade de Balsas e capacidade instalada*. Disponível em: https://www.inpasa.com.br/sala-de-imprensa/inpasa-inaugura-nova-unidade-em-balsas-ma-com-investimento-de-r-2-5-bilhoes-e-reforca-lideranca-no-etanol-de-graos/1023408. Acesso em: 19 out. 2026.
13. EPE. *Análise de Conjuntura dos Biocombustíveis 2025*. Disponível em: https://www.epe.gov.br/pt/publicacoes-dados-abertos/publicacoes/analise-de-conjuntura-dos-biocombustiveis-2025. Acesso em: 19 out. 2026.
14. ANP. *RenovaBio — metas, CBIOs e comprovação*. Disponível em: https://www.gov.br/anp/pt-br/assuntos/renovabio. Acesso em: 19 out. 2026.
15. BRASIL. *Lei nº 14.993, de 8 de outubro de 2024 — Lei do Combustível do Futuro*. Disponível em: https://www.planalto.gov.br/ccivil_03/_ato2023-2026/2024/lei/l14993.htm. Acesso em: 19 out. 2026.
16. ANP. *Combustível Sustentável de Aviação (SAF)*. Disponível em: https://www.gov.br/anp/pt-br/assuntos/saf. Acesso em: 19 out. 2026.
17. ANP. *Biometano*. Disponível em: https://www.gov.br/anp/pt-br/assuntos/producao-e-fornecimento-de-biocombustiveis/biometano. Acesso em: 19 out. 2026.
18. ROCK CONTENT. *As 5 forças de Porter: o que são, para que servem e como aplicar?* Disponível em: https://rockcontent.com/br/blog/5-forcas-de-porter/. Acesso em: 19 out. 2026.
19. RAÍZEN. *Raízen e produtores de cana-de-açúcar debatem perspectivas para o ano-safra no Fórum de Mercado*. Disponível em: https://www.raizen.com.br/sala-de-imprensa/raizen-e-produtores-de-cana-de-acucar-debatem-perspectivas-para-o-ano-safra-no-forum-de-mercado. Acesso em: 20 out. 2026.
20. SÃO MARTINHO. *Formulário de Referência 2025*. Disponível em: https://ri.saomartinho.com.br/Download.aspx?Arquivo=zp29unVyAKR4rxhHtOgKfQ%3D%3D&IdCanal=3kK+JHozjxal5isJwnwwEA%3D%3D&linguagem=pt. Acesso em: 20 out. 2026.
21. INTELI. *5 forças template.jpeg*. Pasta de templates de negócios do projeto, 2022. Disponível em: https://drive.google.com/file/d/1Kr8A2GRrTDUFi5JslJQd7LWVOwyn8E_c/view. Acesso em: 20 out. 2026.
22. ESPRESSIF SYSTEMS. *Sleep Modes — ESP32, ESP-IDF v4.4.3*. Disponível em: https://docs.espressif.com/projects/esp-idf/en/v4.4.3/esp32/api-reference/system/sleep_modes.html. Acesso em: 21 out. 2026.
23. ESPRESSIF SYSTEMS. *System Time — ESP32, ESP-IDF v4.4.3*. Disponível em: https://docs.espressif.com/projects/esp-idf/en/v4.4.3/esp32/api-reference/system/system_time.html. Acesso em: 21 out. 2026.
24. U-BLOX. *MAX-M10S Data Sheet*. UBX-20035208, revisão R08, 30 jan. 2026. Disponível em: https://content.u-blox.com/sites/default/files/MAX-M10S_DataSheet_UBX-20035208.pdf. Acesso em: 21 out. 2026.
25. QUECTEL. *LC76G Series Hardware Design*. Versão 1.3, 25 nov. 2024. Disponível em: https://www.quectel.com/content/uploads/2023/05/Quectel_LC76G_Series_Hardware_Design_V1.3.pdf. Acesso em: 21 out. 2026.
26. WOKWI. *Supported Hardware*. Disponível em: https://docs.wokwi.com/getting-started/supported-hardware. Acesso em: 21 out. 2026.
27. WOKWI. *Custom Chips API — Getting Started*. Disponível em: https://docs.wokwi.com/chips-api/getting-started. Acesso em: 21 out. 2026.
