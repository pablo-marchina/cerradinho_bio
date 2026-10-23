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

A matriz orienta prioridades. Escalas: probabilidade 10%–90%; impacto muito baixo–muito alto. Classificações vêm do TAPI, *kick-off* ou hipóteses. Alerta: 150 dias; prazos fiscais: 270 dias em Goiás e 360 fora, sem relação com autonomia.

<div align="center">
  <p><strong>Figura 1 — Matriz de riscos do projeto</strong></p>
  <img
    src="../assets/negocios/matriz_riscos.png"
    alt="Matriz de riscos do projeto"
    width="100%"
  >
  <p><em>Fonte: Elaborado pelos autores (2026).</em></p>
</div>

| ID | Causa e efeito | Prob. | Impacto | Justificativa | Resposta | Responsável |
|---|---|---:|---:|---|---|---|
| R01 | Nova NF não vinculada à TAG → falso vencimento. | 50% | Moderado | *Kick-off*: retorno simbólico renova o prazo. | Mitigar: associar a NF ao ciclo. | Software/Fiscal |
| R02 | Metal/galpões atenuam GNSS → perda de localização. | 70% | Muito alto | Hipótese: Sistema Global de Navegação por Satélite sofre atenuação. | Mitigar: guardar posição e testar antenas. | Hardware |
| R03 | Ensaio curto de bancada → autonomia anual incerta. | 70% | Alto | TAPI: ciclos chegam a um ano. | Mitigar: projetar autonomia pelo consumo. | Hardware |
| R04 | Sem Wi-Fi, NB-IoT ou LTE-M → eventos retidos. | 90% | Alto | TAPI: cobertura incerta em oficinas/rodovias. | Mitigar: armazenar e comparar redes. | Conectividade |
| R05 | Ruído no sensor ou abertura autorizada do lacre → falso alerta. | 50% | Alto | *Kick-off*: retiradas legítimas ocorrem no reparo. | Mitigar: *debounce* e autorização prévia. | Firmware |
| R06 | Fixação inadequada → dano ou reparo prejudicado. | 50% | Muito alto | TAPI: formatos variam. | Evitar: fixação removível. | Hardware |
| R07 | Água ou lavagem → falha eletrônica. | 70% | Muito alto | TAPI: exposição prevista. | Mitigar: vedação e ensaio. | Hardware |
| R08 | Conectividade cara → escala inviável. | 50% | Alto | TAPI: custo orienta a escolha. | Mitigar: comparar tecnologias e planos. | Produto |
| R09 | Fornecedores indisponíveis → piloto limitado. | 50% | Moderado | Hipótese: dependência de terceiros. | Mitigar: agendar e simular. | Projeto |
| R10 | Dado restrito publicado → exposição indevida. | 30% | Muito alto | TAPI: localizações e notas são restritas. | Evitar: dados fictícios e revisão. | Dados/LGPD |
| R11 | Retorno sem NF registrada → remessa fiscal aberta. | 70% | Muito alto | *Kick-off*: encerramento requer registros físico e fiscal. | Mitigar: cerca e confirmação fiscal. | Processos/Logística |

**Oportunidades**

| ID | Oportunidade | Prob. | Impacto | Tratamento | Responsável |
|---|---|---:|---|---|---|
| O01 | Rastreador reutilizável reduz custo por ciclo. | 70% | Alto | Explorar modularidade. | Produto |
| O02 | Histórico permite avaliar prazo e desempenho das oficinas. | 70% | Alto | Criar indicador para seleção e negociação. | Produto/BI |

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

**Componente GNSS definido em 21/10/2026:** Quectel LC76G, variante **PA**, uma unidade por dispositivo. A placa de desenvolvimento ou montagem, antena, circuito de alimentação/backup, fornecedor e preço continuam **PENDENTE DE VALIDAÇÃO**. Esta definição deve integrar a BOM completa; não representa compra ou ensaio físico realizado. A decisão e o contrato de simulação estão no [planejamento de energia e GNSS](outros/planejamento-energia-gnss.md#5-gnss-escolhido-quectel-lc76g-pa).



## 3.2. Domínio de Fundamentos de Experiência de Usuário 

### 3.2.1. Personas (sprint 1)	

#### Introdução às personas

As personas orientam o desenvolvimento da solução de rastreamento de equipamentos em manutenção externa da Cerradinho a partir dos objetivos, dificuldades e necessidades dos usuários. Os perfis consideram seus cenários de uso e letramento digital, distinguindo informações fundamentadas na pesquisa de hipóteses pendentes de validação.

#### Metodologia de construção das personas

A construção das personas partiu da análise do TAPI e das respostas do parceiro no kickoff de 16/10/2026. As informações foram organizadas por etapa do fluxo de manutenção externa, relacionando cada responsável às suas atividades, ferramentas, dificuldades e necessidades. Foram selecionados três perfis porque representam interações distintas com a solução: o PCM acompanha o equipamento, cobra o fornecedor e decide como agir diante de atrasos; o Fiscal emite a nota de remessa e registra o retorno, podendo permanecer sem informação mesmo quando o equipamento já voltou fisicamente; e a Logística consolida os dados para acompanhamento pelo BI. Essas evidências fundamentaram as personas Geraldo, Clarice e Izidio e orientaram a elaboração de seus objetivos, dores e cenários de uso.

O NotebookLM apoiou a síntese das informações do kickoff, e o Codex auxiliou na organização e redação da documentação. A IA também gerou alternativas de layout e cenas para as personas; a equipe selecionou as opções e orientou ajustes de composição e coerência com cada função. Os nomes são fictícios, os cenários foram elaborados a partir do fluxo identificado.

##### P1. Geraldo, analista de PCM (persona principal)

![Persona 1, Geraldo, analista de PCM, 38 anos, técnico em mecânica, com dores, necessidades, objetivos, cenários P1-C1 a P1-C4 e letramento digital 3 de 5](../assets/personas/persona-01-geraldo.png)

Geraldo, analista de PCM de 38 anos e técnico em mecânica, trabalha no barracão da manutenção da usina e é, na prática, o dono do equipamento enquanto ele está fora: é ele quem avalia a peça que o mecânico trouxe, escolhe e cota a oficina, vincula a TAG, aplica o lacre, entrega o item à transportadora e depois passa horas ao telefone cobrando o fornecedor, apoiado em SAP, planilhas e e-mail (letramento digital 3 de 5). Esse trabalho, porém, acontece com dezenas de itens ao mesmo tempo, e por isso os equipamentos menos críticos acabam saindo da memória, enquanto os fornecedores prometem datas que nem sempre cumprem; sem dados à mão, Geraldo também não consegue saber se um atraso pede trazer o item de volta, usar uma peça sobressalente ou prorrogar o prazo. É por isso que o que ele espera da solução é simples: saber, sem ligar para ninguém, onde está cada item e quando ele volta, ser avisado antes de o prazo estourar e contar com o histórico de cumprimento de cada fornecedor para decidir se renova o contrato ou troca de oficina. Para a plataforma, isso significa pôr no centro uma lista de "meus itens fora" ordenada por dias para vencer, com alertas de prazo configuráveis, alerta imediato de lacre aberto e a última localização e o histórico por fornecedor ao alcance de um clique, em vez de painéis gerenciais que não ajudam quem precisa agir todos os dias.

##### P2. Clarice, analista fiscal (persona secundária)

![Persona 2, Clarice, analista fiscal, 34 anos, formada em ciências contábeis, com dores, necessidades, objetivos, cenários P2-C1 a P2-C4 e letramento digital 4 de 5](../assets/personas/persona-02-clarice.png)

Clarice representa a analista fiscal responsável pela emissão das notas fiscais de remessa e pelo lançamento das notas de retorno, utilizando sistemas como SAP e Gama Soft. Diferentemente de Geraldo, não acompanha fisicamente os equipamentos: sua principal dificuldade ocorre quando um item retorna sem que o Fiscal seja informado, deixando a remessa aberta. Por isso, precisa consultar de forma integrada a identificação do dispositivo (TAG), a nota fiscal e a ordem de serviço, acompanhar prazos e identificar pendências documentais. Seu objetivo é reduzir a busca manual por informações e agir antes que essas pendências gerem consequências fiscais. Na solução, consulta alertas de prazo, verifica retornos ainda não registrados e confirma o lançamento da nota fiscal de retorno, necessário para encerrar o ciclo após a conferência do equipamento. Um aviso automático de chegada à portaria poderá apoiar esse acompanhamento, mas permanece como proposta do grupo a ser discutida com o parceiro.

##### P3. Izidio, analista de logística (persona de inteligência)

![Persona 3, Izidio, analista de logística, 29 anos, engenheiro de produção, com dores, necessidades, objetivos, cenários P3-C1 a P3-C3 e letramento digital 4 de 5](../assets/personas/persona-03-izidio.png)

Izidio, analista de logística de 29 anos e engenheiro de produção, consolida o que saiu e o que voltou para a diretoria e acompanha o custo de ICMS por safra, trabalhando com SAP, planilhas, BI e e-mail ou Teams (letramento digital 4 de 5). O problema é que os dados vivem espalhados entre o SAP e as planilhas de cada área, de modo que ele não consegue dizer, em um instante, quantas remessas saíram na safra; para compensar, e movido por um senso de dono, acaba cobrando oficinas e prazos à mão, uma tarefa operacional que não é da sua área e que o afasta da otimização logística que deveria ser o seu foco. Por isso ele precisa de uma API que alimente o BI automaticamente, de um histórico de ciclos com datas de saída, previsão e retorno e de permanência e atraso calculados separadamente, e espera, com isso, mostrar à diretoria a evolução do ICMS por safra e manter na base todo item que não voltou, mesmo quando o ICMS já foi pago. Para a plataforma, isso reforça a prioridade de uma API robusta e de dados confiáveis, em lugar de telas operacionais, e a regra de tratar como pendente, nunca como encerrado, o ciclo sem retorno lançado, para que o número que chega à diretoria seja o que realmente aconteceu.

#### Conclusão

As personas indicam que a solução deve apresentar informações e ações adequadas às responsabilidades de cada usuário. Para Geraldo, isso significa priorizar a consulta dos equipamentos sob sua responsabilidade, os prazos, a última localização válida e os alertas que exigem intervenção. Para Clarice, o foco está na associação entre equipamento e documentos e na identificação de retornos pendentes de lançamento. Para Izidio, a necessidade é acessar históricos consistentes para consolidar dados e analisar o acompanhamento das remessas.

Essas diferenças orientam o GuaráTracking a combinar uma visão operacional dos equipamentos com o controle das pendências de cada ciclo e a disponibilização de dados para BI. A interface deve facilitar a identificação do que precisa de atenção e de quem deve agir, enquanto o histórico deve preservar a relação entre saída, acompanhamento e retorno. Assim, o rastreamento apoia decisões e a continuidade do processo entre as áreas.

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
| RN09 — fila de 8 com prioridade para críticos | Implementada na fila de eventos sem rede (8 eventos, descarte do não crítico mais antigo, reenvio na ordem com `dia_ocorrencia`). Com a fila só de violações, a 9ª violação em diante entra num único registro de resumo (`VIOLACAO_LACRE_RESUMO`, com quantidade e primeira/última ocorrência), persistido na NVS e enviado depois das 8 no reenvio; nenhuma violação é perdida. Só eventos não críticos que não cabem são contados em `perdidos`. A política de agregação deve ser confirmada com o parceiro. O registro local de 16 entradas na NVS (CT21) continua separado. |
| RN10 — leitura zero considerada falha | Implementada: ADC = 0 gera `SENSOR_BATERIA_FALHA` e nenhum `BATERIA_BAIXA` (CT13). No dispositivo físico, o diagnóstico elétrico do divisor ainda depende do CT30 previsto no [planejamento de energia e GNSS](outros/planejamento-energia-gnss.md). |
| RN11 — encerramento exige lacre fechado e NF de retorno | As duas recusas foram verificadas no CT16. No simulador, `NF` no Serial representa o lançamento da NF de retorno e segurar `BTN_CICLO` por 2 s solicita o encerramento. No reteste de 23/10/2026, o CT14 confirmou JSON com ciclo inativo e dias zero, reutilização e contador acumulado. A persistência do histórico após corte de energia não foi verificada. |

### 3.3.3. Requisitos Não Funcionais — 8 Eixos ISO/IEC 25010 (sprints 2 a 5)

*Preencha os 8 eixos. Cada eixo deve ter ao menos um RNF verificável (com métrica, limite ou critério concreto) ou justificativa explícita de ausência. Evolua do conceitual (sprint 2) ao técnico mensurável (sprint 5).*

| Eixo                     | Requisito | Métrica / Critério | Justificativa |
|--------------------------|-----------|--------------------|---------------|
| USAB — Usabilidade       | ...       | ...                | ...           |
| CONF — Confiabilidade    | ...       | ...                | ...           |
| DES — Desempenho         | ...       | p95 < X ms         | ...           |
| SUP — Suportabilidade    | ...       | ...                | ...           |
| SEG — Segurança          | ...       | ...                | ...           |
| CAP — Capacidade         | ...       | ...                | ...           |
| REST — Restrições Design | ...       | ...                | ...           |
| ORG — Organizacionais    | ...       | ...                | ...           |


### 3.3.4. Matriz de Rastreabilidade (RTM) (sprints 1 a 5)

A Matriz de Rastreabilidade (RTM) relaciona os requisitos funcionais (RF) e as regras de negócio (RN) aos casos de teste e às evidências de verificação. Ela permite identificar a cobertura de cada requisito e acompanhar as pendências de validação.

| Persona | RF    | RN   | Caso de Teste | Evidência        |
|---------|-------|------|---------------|------------------|
| P1 Geraldo — P1-C1 Saída com TAG e lacre | RF001 | Não se aplica — sem RN específica | Planejado — CT a definir | Planejado, sprint a definir |
| P1 Geraldo — P1-C1 Saída com TAG e lacre | RF002 | RN02 | Planejado — CT a definir | Planejado, sprint a definir |
| P1 Geraldo — P1-C1 Saída com TAG e lacre | RF003 | RN01 | Planejado — CT a definir | Planejado, sprint a definir |
| P1 Geraldo — P1-C1 Saída com TAG e lacre | RF004 | RN01 | CT15 | [CT15](../assets/wokwi/CT15.png) |
| P1 Geraldo — P1-C4 Falha | RF005 | RN08 | CT19, CT20, CT22 | [CT19](../assets/wokwi/CT19.png) e [CT20](../assets/wokwi/CT20.png) — APROVADOS no simulador; [CT22](../assets/wokwi/CT22.png) — **REPROVADO no ambiente**: referência e NVS retidas, mas `uptime_ms` não reiniciou ([log](../assets/wokwi/CT22.serial.txt)) |
| P1 Geraldo — P1-C3 Lacre aberto | RF006 | RN05 | CT05, CT07, CT18 | [CT05](../assets/wokwi/CT05-evento.png), [CT07](../assets/wokwi/CT07.png); [CT18](../assets/wokwi/CT18-original.png) — **REPROVADO**: quatro de 20 transições estáveis de 200 ms perdidas sem rede ([log original](../assets/wokwi/CT18-original.serial.txt)) |
| P1 Geraldo — P1-C3 Lacre aberto | RF007 | RN06 | CT06, CT11 | [CT06](../assets/wokwi/CT06-retirada.png), [CT11](../assets/wokwi/CT11.png) |
| P2 Clarice — P2-C4 Falha (associação direta) | RF008 | Não se aplica — sem RN específica | CT08, CT09 | [CT08](../assets/wokwi/CT08.png), [CT09](../assets/wokwi/CT09.png) (BLOQUEADO para confirmação do servidor) |
| P1 Geraldo — P1-C2 Acompanhamento | RF009 | Não se aplica — sem RN específica | CT01, CT02 | [CT01](../assets/wokwi/CT01.png), [CT02](../assets/wokwi/CT02.png) — diagnóstico e saída Serial observados; publicação MQTT não comprovada |
| P2 Clarice — P2-C4 Falha | RF010 | RN09 | CT08, CT09, CT10, CT21, CT22 | [CT08](../assets/wokwi/CT08.png), [CT09](../assets/wokwi/CT09.png) (BLOQUEADO para confirmação do servidor), [CT10](../assets/wokwi/CT10.png); [CT21](../assets/wokwi/CT21.png) — APROVADO no limite de 16 diagnósticos NVS ([log](../assets/wokwi/CT21.serial.txt)); [CT22](../assets/wokwi/CT22.png) — **REPROVADO no ambiente** no critério de reinício de `uptime_ms`, com retenção das filas verificada ([log](../assets/wokwi/CT22.serial.txt)) |
| P1 Geraldo — P1-C2 Acompanhamento | RF011 | RN07 | CT02 | [CT02](../assets/wokwi/CT02.png) |
| P1 Geraldo — P1-C2 Acompanhamento; P2 Clarice — P2-C3 Prazo legal; P3 Izidio — P3-C2 ICMS por safra | RF012 | RN12, RN13 | CT03, CT04 | [CT03](../assets/wokwi/CT03.png), [CT04](../assets/wokwi/CT04.png) |
| P1 Geraldo — P1-C2 Acompanhamento; P3 Izidio — P3-C2 ICMS por safra | RF013 | RN04 | CT03 | [CT03](../assets/wokwi/CT03.png) |
| P1 Geraldo — P1-C2 Acompanhamento; P3 Izidio — P3-C2 ICMS por safra | RF014 | RN03, RN04 | CT04 | [CT04](../assets/wokwi/CT04.png) |
| P1 Geraldo — P1-C2 Acompanhamento; P1-C4 Falha | RF015 | RN08 | CT19, CT20, CT22 | [CT19](../assets/wokwi/CT19.png) e [CT20](../assets/wokwi/CT20.png) — APROVADOS no simulador; [CT22](../assets/wokwi/CT22.png) — **REPROVADO no ambiente**: referência e NVS retidas, mas `uptime_ms` não reiniciou ([log](../assets/wokwi/CT22.serial.txt)) |
| P2 Clarice — P2-C2 Encerramento do ciclo; P3 Izidio — P3-C3 Falha | RF016 | RN11 | CT14, CT16 | [CT14 corrigido](../assets/wokwi/CT14-corrigido.png) — APROVADO com dois encerramentos, ciclo inativo e dias zero ([log do reteste de 23/10](../assets/wokwi/CT14-2026-10-23.serial.txt)); CT16 APROVADO nas recusas por [lacre aberto](../assets/wokwi/CT16-corrigido-lacre-aberto.png) e [NF ausente](../assets/wokwi/CT16-corrigido-nf-ausente.png) |
| P2 Clarice — P2-C2 Encerramento do ciclo; P3 Izidio — P3-C1 Histórico para o BI | RF017 | RN11, RN12 | CT14 | [CT14 corrigido](../assets/wokwi/CT14-corrigido.png) — APROVADO: [reutilização no ciclo 2](../assets/wokwi/CT14-corrigido-reutilizado.png) e contador de encerramentos 1/2 ([log do reteste de 23/10](../assets/wokwi/CT14-2026-10-23.serial.txt)); histórico após corte físico de energia ainda pendente |
| P1 Geraldo — P1-C2 Acompanhamento; P2 Clarice — P2-C1 Retorno sem aviso | RF018 | Não se aplica — sem RN específica | Planejado — CT a definir | Planejado, sprint a definir |
| P1 Geraldo — P1-C2 Acompanhamento (associação indireta) | RF019 | RN10 | CT12, CT13, CT23 | [CT12](../assets/wokwi/CT12.png), [CT13](../assets/wokwi/CT13-isolado.png); [CT23](../assets/wokwi/CT23.png) — APROVADO com percentuais sintéticos 0/50/100%, uma falha do sensor em zero e nenhum alerta falso de bateria baixa ([log CT17/CT23](../assets/wokwi/CT17-CT23.serial.txt)) |
| P1 Geraldo — P1-C1 Saída com TAG e lacre | RF020 | RN16 | Planejado — CT a definir | Planejado, sprint a definir |
| P1 Geraldo — P1-C2 Acompanhamento (associação transversal) | RF021 | RN15 | Planejado — CT a definir | Planejado, sprint a definir |
| P1 Geraldo — P1-C2 Acompanhamento; P3 Izidio — P3-C2 ICMS por safra | RF022 | Não se aplica — sem RN específica | Planejado — CT a definir | Planejado, sprint a definir |
| P1 Geraldo — P1-C3 Lacre aberto | RF023 | Não se aplica — sem RN específica | Planejado — CT a definir | Planejado, sprint a definir |
| P3 Izidio — P3-C1 Histórico para o BI | RF024 | RN15 | Planejado — CT a definir | Planejado, sprint a definir |
| P1 Geraldo — P1-C4 Falha | RF025 | RN14 | Planejado — CT a definir | Planejado, sprint a definir |
| P2 Clarice — P2-C3 Prazo legal | RF026 | RN13 | Planejado — CT a definir | Planejado, sprint a definir |
| P1 Geraldo — P1-C2 Acompanhamento | RF027 | RN10 | CT12, CT13, CT23 | [CT12](../assets/wokwi/CT12.png), [CT13](../assets/wokwi/CT13-isolado.png); [CT23](../assets/wokwi/CT23.png) — APROVADO com percentuais sintéticos 0/50/100%, uma falha do sensor em zero e nenhum alerta falso de bateria baixa ([log CT17/CT23](../assets/wokwi/CT17-CT23.serial.txt)) |

**Casos de teste transversais:** [CT17](../assets/wokwi/CT17.png) (APROVADO no simulador, [log](../assets/wokwi/CT17-CT23.serial.txt)) e CT35 ([desligado](../assets/wokwi/CT35-desligado.png), [religado](../assets/wokwi/CT35.png)) verificam aspectos transversais e, por isso, não são atribuídos a um RF ou a uma RN específicos nesta matriz.

Os resultados dos CT17–CT23 correspondem às execuções de 23/10/2026 descritas na seção 3.4.1, com capturas, logs, configuração e limitações. As aprovações são funcionais no simulador; CT18 e CT22 permanecem reprovados, e CT09 depende da confirmação de entrega pelo servidor.

A proposta de RNFs, a máquina de estados de energia e os CT24–CT34 estão no [planejamento de energia e GNSS](outros/planejamento-energia-gnss.md), para revisão nas próximas sprints.

### 3.3.5. Arquitetura da Solução (sprint 3)

_Vídeo de explicação sobre a arquitetura da solução: (coloque aqui o link do vídeo)_

_Descreva a arquitetura técnica da solução de forma detalhada (visão de arquitetura).

Justifique como a arquitetura suporta os requisitos funcionais e não funcionais.

O diagrama de arquitetura deve:_
- _Mostrar microcontroladores, incluindo descrições de sua função no sistema (por exemplo: "Irá processar o sinal dos sensores a cada X minutos")_
- _Mostrar sensores, incluindo descrição de função e especificações técnicas do tipo de informação que será coletada_
- _Mostrar apresentadores de informação (displays), incluindo descrição de que tipo de informação será apresentada (por exemplo, "Mostrar temperatura dos sensores")_
- _Mostrar atuadores, caso existam na solução, incluindo descrições do que irão acionar (por exemplo, "Ligar motor de irrigação durante x minutos")_
- _Mostrar o broker MQTT e o dashboard que é a interface do usuário_
- _Mostrar ligações entre os elementos (com fio ou sem fio) – incluindo conexões com sensores e atuadores, conexão com WiFi, entre outros_


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
O projeto público acima representa a versão anterior; a Figura 1 e as evidências dos CTs abaixo mostram o circuito local integrado usado nas execuções de 22/10/2026.

Na Sprint 1 o dispositivo é simulado no Wokwi com um ESP32 DevKit. Cada entrada do circuito representa uma situação física do rastreador: o lacre (`SW_LACRE`), a disponibilidade de conexão (`SW_REDE`), a autorização de retirada (`BTN_AUTORIZAR`), o encerramento ou vínculo de ciclo (`BTN_CICLO`) e o nível da bateria (`POT_BATERIA`). Os LEDs representam estados para inspeção e o Monitor Serial mostra os registros JSON. Não há transporte implementado para servidor ou broker. A integração local inclui GNSS simulado; a validação do hardware GNSS permanece para a Sprint 2.

Na integração atual, o firmware principal já tem o ciclo (CT02–CT04): conta os dias
fora, envia o `STATUS` ao ligar e o `STATUS_SEMANAL` a cada 7 dias, e emite os alertas
de atraso e de permanência. Verde indica lacre íntegro, amarelo pisca no atraso e fica
fixo na permanência, vermelho pisca na violação e azul pisca com a rede simulada
indisponível. O firmware também trata o lacre com retirada autorizada (CT05–CT16),
a fila de eventos sem rede, o debounce dos botões, o alerta de bateria e o encerramento
e o vínculo de ciclo, e a chave ON/OFF (`SW_ENERGIA`, GPIO19) registra o desligamento lógico (CT35).
Os dias do ciclo são contados por um relógio de 64 bits que acumula os intervalos do `millis()`, então a volta do contador de 32 bits (~49,7 dias) não trava o `weekly` antes dos 150 dias. O contexto do ciclo e do lacre e a fila de eventos sem rede ficam na NVS e são restaurados ao reiniciar: o ciclo continua com o mesmo `ciclo_id` e `dias_fora`, e o `STATUS` de inicialização informa `contexto_restaurado`. Sem RTC, o tempo com o ESP32 sem energia não é contado. Ligar com o lacre aberto passa pela mesma detecção e gera `VIOLACAO_LACRE`.
A chave de rede não desliga o Wi-Fi virtual; os estados são apresentados separadamente.

**Tempo e parâmetros atuais.** O firmware principal usa `demo` com coleta a cada 30 s ou `weekly` com sete dias. No `demo`, o ciclo usa a escala `1 s = 1 dia` dos CT02–CT04 (`Config::DayMs`); no `weekly`, o dia é real. O intervalo de coleta é contado a partir da conclusão da coleta anterior, incluindo nova espera de fix quando necessária.

| Parâmetro técnico atual | Valor / comportamento | Fonte |
|---|---|---|
| Intervalo de coleta | 30 s (`demo`) / 7 dias (`weekly`) | `Config::SampleIntervalMs` |
| Espera máxima por fix / idade máxima do fix | 10 s / 3 s | `Config::GpsWaitMs` / `Config::FixMaxAgeMs` |
| Debounce do lacre / dos botões | 60 ms / 50 ms, com detecção por borda | `Config::SealDebounceMs` / `Config::ButtonDebounceMs` |
| Registro local de diagnóstico na NVS (`q0`–`q15`) | 16 registros de amostras de GPS e `seal_changed`; antigos preservados, novo apenas na Serial se cheio. Separado da fila de eventos de negócio | `Config::QueueCapacity` e `enqueue()` |
| Fila de eventos sem rede | 8 eventos, persistidos na NVS (`p0`–`p7`); cheia, descarta o não crítico mais antigo; reenvio na ordem ao reconectar | `EVENT_QUEUE_CAPACITY` em `include/event_queue.h` |
| Janela de autorização | 3 dias medidos em ms a partir do registro da autorização | `Config::AuthorizationWindowDays` e `seal_opened()` |
| Movimento entre coletas válidas | Distância estritamente maior que 100 m | `Config::MovementThresholdM` |
| Heartbeat / tentativa de reconexão Wi-Fi | 2 s / 30 s quando desconectado | `Config::HeartbeatMs` / `Config::WifiRetryMs` |

**Valores propostos para testes de negócio.** O RF012 exige contar os dias fora desde a emissão da NF de remessa. No protótipo atual, a contagem parte do início do ciclo e usa tempo acelerado no `demo`; os ensaios não validam leitura de uma NF real nem data/hora absoluta. A tabela seguinte informa os valores usados nos testes de negócio. O TAPI fixa a referência de 150 dias para permanência e o status semanal; as demais metas dependem do parceiro. Tempo de chegada da autorização e possibilidade de autorização antecipada continuam **PENDENTE DE VALIDAÇÃO**.

| Parâmetro | Valor de teste | Origem |
|---|---|---|
| Status periódico | 7 dias | TAPI ("uma vez por semana, com frequência ajustável") |
| Limiar de permanência | 150 dias (também testado com 10) | TAPI ("150 dias como referência inicial"; MVP 4 pede outros limiares) |
| Prazo previsto de retorno | 5 dias | Valor de teste — `PENDENTE DE VALIDAÇÃO` |
| Janela de autorização de retirada | 3 dias | Valor de teste — `PENDENTE DE VALIDAÇÃO` |
| Capacidade da fila sem conexão | 8 eventos | Valor de teste — `PENDENTE DE VALIDAÇÃO` (RN09) |
| Bateria baixa / recuperada | 20% / 25% | Valor de teste — `PENDENTE DE VALIDAÇÃO` |

**LEDs simultâneos.** Cada LED representa um estado independente e podem acender juntos: verde e vermelho seguem o estado estável do lacre, amarelo segue o ciclo (piscando no atraso, fixo na permanência) e azul pisca com a rede simulada indisponível. O verde pisca durante a retirada autorizada, e todos os LEDs apagam com a chave ON/OFF desligada.

**Do simulador ao dispositivo real.** O alvo principal compila `src/firmware/main.cpp`. O `sketch.ino` da pasta do circuito é um teste independente de bancada. A migração física exige validar interfaces, alimentação, sensores e firmware; não é apenas trocar fontes de leitura.

| Componente no Wokwi | O que representa no dispositivo real |
|---|---|
| ESP32 DevKit | Microcontrolador do rastreador |
| SW_LACRE (chave deslizante) | Sensor do lacre (laço de continuidade ou reed switch) |
| BTN_AUTORIZAR (botão) | Autorização de retirada registrada pelo responsável (no produto, vinda da plataforma) |
| SW_ENERGIA (chave deslizante) | Desligamento lógico no simulador; corte físico entre bateria e ESP32 ainda a validar |
| BTN_CICLO (botão) | Encerramento do ciclo ou novo vínculo, feitos na conferência de retorno |
| POT_BATERIA (potenciômetro) | Percentual sintético; medição física, calibração e diagnóstico do divisor ainda pendentes |
| SW_REDE (chave deslizante) | Disponibilidade simulada, independente do Wi-Fi; sem rede, guarda eventos de negócio; ao reconectar, reenvia pela Serial e esvazia a fila, sem confirmação de servidor |
| LEDs | Diagnóstico local; política futura de indicação e duração depende da implementação |
| Monitor Serial (JSON) | Registro local; não comprova publicação MQTT ou recebimento pela plataforma |

A versão inicial pública não inclui localização. O circuito principal local inclui
GPS/GNSS customizado nos GPIO16/17; ele não modela recepção de satélites nem consumo real.

**Situação dos CT01–CT16 e do CT35:** executados no Wokwi em 22/10/2026, com resultados resumidos abaixo, prints e logs externos. A aprovação indicada se limita ao comportamento funcional observado no simulador. O CT09 permanece **BLOQUEADO** para confirmação de entrega pelo servidor; o CT14 foi corrigido e **APROVADO no reteste de 23/10/2026**, com confirmação das duas recusas do CT16 na mesma versão. As medições exatas de 1 Hz e das fronteiras de debounce não foram realizadas. Os RF/RN e IDs foram preservados, sem alterar o esperado para aprovar falhas. CT17–CT23 foram executados em 23/10/2026, com resultados na tabela complementar abaixo, e CT24–CT34 permanecem reservados no [planejamento de energia e GNSS](outros/planejamento-energia-gnss.md). Testes de lógica em `tests/model_tests.c` são evidência separada da execução no Wokwi e da validação física.

![CT01: circuito integrado com verde aceso e STATUS de inicialização no Monitor Serial](../assets/wokwi/CT01.png)

*Figura 1 — CT01: inicialização do circuito integrado no Wokwi, com lacre íntegro e dias fora iguais a zero. Fonte: captura real do simulador Wokwi for VS Code, realizada nesta sessão de testes em 22/10/2026.*

**CT#** | **Bloco** | **Componente de entrada** | **Leitura da entrada** | **Componente de saída** | **Leitura da saída** | **Descrição**
|----------|----------|----------|----------|----------|----------|----------|
|CT01|Inicialização|SW_LACRE e SW_REDE|Lacre fechado e rede disponível ao ligar|LED verde; Serial|Verde fixo, demais apagados; JSON `STATUS` com `dias_fora=0`, `seal="intact"`|Esperado: Ao ligar com o lacre íntegro e conexão, o dispositivo indica operação normal e publica o primeiro status (RF009).|
|CT02|Status periódico|Relógio simulado (1 s = 1 dia)|Dias 7 e 14 de ciclo|Serial|Um `STATUS_SEMANAL` no dia 7 e outro no dia 14, nenhum fora desses dias|Esperado: O status é enviado a cada 7 dias, como pede o TAPI (RF011, RF009; RN07).|
|CT03|Prazo e atraso|Relógio simulado (1 s = 1 dia)|`dias_fora` passa do prazo previsto (5) e fica abaixo do limiar|LED amarelo; Serial|Amarelo piscando a 1 Hz; `ALERTA_ATRASO` emitido uma única vez|Esperado: O equipamento passou da data prevista de retorno, o que é diferente de permanência prolongada (RF013, RF012; RN04).|
|CT04|Permanência|Relógio simulado (1 s = 1 dia)|`dias_fora` atinge o limiar (rodar com 10 e com 150)|LED amarelo; Serial|Amarelo fixo; `ALERTA_PERMANENCIA` emitido uma única vez em cada rodada|Esperado: Testa o limiar configurável com o valor de referência e um valor alternativo, como pede o MVP 4 do TAPI (RF014, RF012; RN03, RN04).|
|CT05|Lacre|SW_LACRE|Lacre aberto sem autorização|LED vermelho; Serial|Vermelho piscando; `VIOLACAO_LACRE` é registrado sem esperar GNSS/rede, fora da rotina semanal|Esperado: Abertura não autorizada gera registro local; medir entrega separadamente da detecção local, com limite a validar com o parceiro (RF006; RN05).|
|CT06|Retirada autorizada|BTN_AUTORIZAR e SW_LACRE|Botão pressionado; lacre aberto dentro da janela de 3 dias e depois fechado|LED verde; Serial|Verde piscando durante a retirada; `RETIRADA_AUTORIZADA` sem alerta; ao fechar, `LACRE_REAPLICADO` e verde fixo|Esperado: Fluxo de retirada autorizada para o reparo, com reaplicação do lacre (RF007; RN06).|
|CT07|Autorização expirada|BTN_AUTORIZAR e SW_LACRE|Botão pressionado; lacre aberto depois de mais de 3 dias|LED vermelho; Serial|Mesmo resultado do CT05: `VIOLACAO_LACRE`|Esperado: Autorização vencida não libera a abertura (RF006; RN05, RN06).|
|CT08|Sem conexão|SW_REDE e SW_LACRE|Rede indisponível; lacre aberto sem autorização|LEDs azul e vermelho; Serial|Azul e vermelho piscando; nada publicado; `VIOLACAO_LACRE` guardada na fila; Serial mostra `[FILA] 1 pendente`|Esperado: Sem conexão, o evento crítico deve ficar guardado para envio posterior dentro da capacidade disponível; saturação é tratada no CT10 (RF010, RF008; RN05, RN09).|
|CT09|Reconexão|SW_REDE|Rede volta a ficar disponível|LED azul; Serial|Eventos pendentes publicados na ordem, com o dia original de ocorrência; azul apaga|Esperado: Ao reconectar, registros confirmados pelo servidor são removidos sem perder o horário de ocorrência (RF010, RF008; RN09).|
|CT10|Fila cheia|SW_REDE, SW_LACRE e entradas que geram eventos|Rodada A: rede indisponível; mais de 8 eventos gerados, com uma violação entre eles. Rodada B: rede indisponível; 12 violações seguidas; depois religar a rede|Serial|A: `FILA_CHEIA`; descarta o evento não crítico mais antigo; a violação continua na fila. B: as 8 primeiras violações ficam na fila e as 4 seguintes entram no resumo (`[FILA] 8 pendentes + resumo de 4 violacao(oes)`); ao religar, saem as 8 na ordem e depois um `VIOLACAO_LACRE_RESUMO` com `quantidade=4`, `dia_primeira` e `dia_ultima`; o resumo é zerado|Esperado: Com a fila no limite, só eventos não críticos são descartados e nenhuma violação é perdida; as excedentes são agregadas num único registro (RF010; RN09).|
|CT11|Ruído de botão|BTN_AUTORIZAR|Pressões rápidas (ruído de contato) e depois o botão segurado por 3 s|Serial|Um único `AUTORIZACAO_REGISTRADA` por pressão real (debounce de 50 ms, detecção por borda); segurar não repete o evento|Esperado: Leituras falsas do botão não geram autorizações em duplicidade (RF007).|
|CT12|Bateria|POT_BATERIA|Nível abaixo de 20%, depois acima de 25%|Serial|`BATERIA_BAIXA` uma única vez; só rearma depois de superar 25%, conforme RN10 (histerese)|Esperado: O alerta de bateria não fica oscilando perto do limite (RF019, RF027; RN10).|
|CT13|Falha do sensor de bateria|POT_BATERIA|Potenciômetro no mínimo (ADC = 0, representa divisor desconectado)|Serial|`SENSOR_BATERIA_FALHA`; nenhum `BATERIA_BAIXA` falso|Esperado: Leitura impossível é tratada como falha do sensor, não como bateria vazia. No dispositivo físico, o diagnóstico elétrico fica para o CT30 (RF019, RF027; RN10).|
|CT14|Encerramento|BTN_CICLO, SW_LACRE e comando `NF`|NF de retorno lançada (`NF` no Serial); lacre fechado; botão segurado por 2 s|Serial|`CICLO_ENCERRADO` com `ciclos_encerrados` somando um; `dias_fora` zera; um toque curto depois abre o `CICLO_INICIADO` com o `ciclo_id` seguinte|Esperado: Encerrar o ciclo libera o dispositivo sem apagar o histórico (RF016, RF017; RN11).|
|CT15|Vínculo simultâneo|BTN_CICLO|Ciclo ativo; toque curto (tentativa de novo vínculo)|Serial|`VINCULO_RECUSADO`; o ciclo atual continua igual|Esperado: Impede vínculos simultâneos indevidos, como pede o TAPI (RF004; RN01).|
|CT16|Encerramento recusado|BTN_CICLO, SW_LACRE e comando `NF`|Rodada A: lacre aberto e NF lançada. Rodada B: lacre fechado e NF ausente; segurar o botão por 2 s em ambas|Serial|`ENCERRAMENTO_RECUSADO` com `motivo` `lacre_aberto` (A) ou `nf_retorno_ausente` (B); o ciclo continua ativo|Esperado: O ciclo só pode ser encerrado com o lacre fechado e a NF de retorno lançada (RF016; RN11).|
|CT35|Desligamento|SW_ENERGIA|Chave ON/OFF para a direita e depois de volta para a esquerda|LEDs; Serial|Ao desligar, `DISPOSITIVO_DESLIGADO` e todos os LEDs apagados; ao religar, novo `STATUS` e LEDs de volta ao estado do ciclo|Esperado: O desligamento fica registrado, porque pode indicar tentativa de burlar o rastreio. É um desligamento lógico: o ESP32 continua alimentado, para de ler entradas e de publicar, e o relógio do ciclo continua contando; o corte real de energia fica para o protótipo físico. Numerado como CT35 porque os CT24 a CT34 estão reservados no planejamento de energia e GNSS.|

#### Resultados da execução da Sprint 1 — 22 e 23/10/2026

**Ambiente e configuração.** Wokwi for VS Code 4.0.1; circuito `src/firmware/prototipo_wokwi/diagram.json`; firmware `demo`; 1 s simulado = 1 dia; prazo de retorno 5 dias; autorização 3 dias; fila de 8 eventos; limiares de permanência 150 e 10 em ciclos distintos. Rede simulada independente do Wi-Fi. Operação e captura automatizadas por Codex, a pedido do grupo; entradas acionadas na interface real e comandos `NF`/`LIMIAR` enviados pela Serial local, sem ajustar variáveis internas para aprovar os testes.

| Rodada | Versão do firmware | SHA-256 do binário demo |
|---|---|---|
| CT01–CT16 e CT35, 22/10/2026 | `7317dd9`, sem alterações locais de firmware durante os ensaios | `E3A60385B943E5B5230535E85D1F9F7426F529F241CB59B9B8A5FCA669AB6F49` |
| Reteste CT14/CT16, 23/10/2026 | `7777594`, com estado do ciclo atualizado antes de publicar o encerramento | `103531CD7BE0D802035A2A6687D21DC33297BBB741177B91772ECCEC6AA6E601` |

**Critério e evidências.** Os resultados abaixo usam os critérios dos CTs, RFs e RNs desta seção. As aprovações se limitam ao comportamento funcional observado no simulador. As capturas mostram estados da execução; contagens, ordem e não repetição foram conferidas nos logs vinculados.

| CT | Situação | Resultado observado | Evidência e log |
|---|---|---|---|
| CT01 | APROVADO no simulador | `STATUS` com lacre íntegro e dias zero; somente verde aceso. Wi-Fi conectou depois do primeiro JSON; não comprova publicação MQTT. | [Print](../assets/wokwi/CT01.png) · [Log A][serial-a] |
| CT02 | APROVADO no simulador | Um `STATUS_SEMANAL` no dia 7 e outro no dia 14, sem emissão intermediária. | [Dia 7](../assets/wokwi/CT02.png) · [Dia 14](../assets/wokwi/CT02-dia14.png) · [Log B][serial-b] |
| CT03 | APROVADO no alerta funcional | Um único `ALERTA_ATRASO`, no dia 6. Amarelo observado piscando; frequência exata de 1 Hz não medida. | [Print](../assets/wokwi/CT03.png) · [Log B][serial-b] |
| CT04 | APROVADO no simulador | Um `ALERTA_PERMANENCIA` no dia 150 e outro no dia 10, em ciclos distintos. | [150 dias](../assets/wokwi/CT04.png) · [10 dias](../assets/wokwi/CT04-limiar10.png) · [Log 150][serial-150] · [Log ciclo 2][serial-ciclos] |
| CT05 | APROVADO na detecção local | Abertura sem autorização válida gerou violação crítica fora da rotina semanal; vermelho observado piscando. | [Print](../assets/wokwi/CT05-evento.png) · [Log B][serial-b] |
| CT06 | APROVADO no simulador | Retirada autorizada 337 ms após autorização, sem violação nessa retirada; fechamento gerou `LACRE_REAPLICADO`. | [Retirada](../assets/wokwi/CT06-retirada.png) · [Reaplicação](../assets/wokwi/CT06.png) · [Log B][serial-b] |
| CT07 | APROVADO no simulador | Abertura depois de vencer a janela gerou `VIOLACAO_LACRE`, sem `RETIRADA_AUTORIZADA`. | [Print](../assets/wokwi/CT07.png) · [Log A][serial-a] |
| CT08 | APROVADO no armazenamento simulado | Sem rede simulada, violação guardada; fila com um pendente e LEDs vermelho/azul acesos. | [Print](../assets/wokwi/CT08.png) · [Log B][serial-b] |
| CT09 | **BLOQUEADO** para aprovação integral | Dois eventos reenviados pela Serial, na ordem e com dias originais; fila zero. Falta confirmação do servidor exigida pelo CT. | [Antes](../assets/wokwi/CT09-antes.png) · [Depois](../assets/wokwi/CT09.png) · [Log B][serial-b] |
| CT10 | APROVADO na fila/reenvio simulado | A: violação preservada e não críticos mais antigos descartados. B: oito violações individuais e resumo de quatro, depois fila zero. | [Rodada A](../assets/wokwi/CT10-A-violacao-preservada.png) · [Resumo B](../assets/wokwi/CT10.png) · [Log da fila][serial-fila] |
| CT11 | APROVADO nos estímulos aplicados | Três pressões curtas geraram três autorizações; uma pressão mantida gerou somente uma. Fronteira exata de 50 ms não medida. | [Evento](../assets/wokwi/CT11.png) · [Pressão mantida](../assets/wokwi/CT11-pressao-mantida.png) · [Log A][serial-a] |
| CT12 | APROVADO no simulador | Sequência 17% → 22% → 17% → 31% → 17% gerou somente dois alertas, confirmando rearme acima de 25%. | [Print](../assets/wokwi/CT12.png) · [Log A][serial-a] |
| CT13 | APROVADO no diagnóstico simulado | Partindo de 78%, entrada zero gerou uma falha do sensor e nenhum `BATERIA_BAIXA`. | [Print isolado](../assets/wokwi/CT13-isolado.png) · [Log C][serial-c] |
| CT14 | **APROVADO no reteste de 23/10/2026** | Dois encerramentos com `ciclo_ativo=false`, `dias_fora=0` e contadores 1/2. Toque curto após o primeiro iniciou o ciclo 2 com dias zero; o lacre permaneceu fechado durante todo o primeiro ciclo. | [Encerramento corrigido](../assets/wokwi/CT14-corrigido.png) · [Reutilização](../assets/wokwi/CT14-corrigido-reutilizado.png) · [Segundo encerramento](../assets/wokwi/CT14-corrigido-segundo-ciclo.png) · [Log do reteste][serial-reteste] |
| CT15 | APROVADO no simulador | Novo vínculo recusado com `motivo=ciclo_ativo`; ciclo 1 permaneceu ativo. | [Print](../assets/wokwi/CT15.png) · [Log B][serial-b] |
| CT16 | APROVADO nas duas recusas | Recusou lacre aberto com NF e lacre fechado sem NF, mantendo o ciclo ativo; ambas as condições foram confirmadas novamente na versão corrigida. | [Lacre aberto](../assets/wokwi/CT16-corrigido-lacre-aberto.png) · [NF ausente](../assets/wokwi/CT16-corrigido-nf-ausente.png) · [Log do reteste][serial-reteste] |
| CT35 | APROVADO no desligamento lógico observado | Desligamento registrado, LEDs apagados e 26,535 s simulados sem novas mensagens; novo `STATUS` e LEDs restaurados ao religar. | [Desligado](../assets/wokwi/CT35-desligado.png) · [Religado](../assets/wokwi/CT35.png) · [Log A][serial-a] |

**Correção e validação do CT14.** A falha original era publicar `CICLO_ENCERRADO` antes de desativar o ciclo e zerar os dias. A ordem foi corrigida, sem alterar o critério esperado. No reteste de 23/10/2026, os encerramentos em `uptime_ms=91417` e `215814` trouxeram ciclo inativo, dias zero e contadores 1 e 2; a reutilização em `118236` iniciou o ciclo 2 com dias zero. As recusas do CT16 por NF ausente (`132328`) e lacre aberto com NF (`176792`) mantiveram o ciclo ativo. Os ambientes `demo`/`weekly` compilaram e `scripts/verify.ps1` passou. Capturas e logs anteriores foram preservados como histórico.

**Limitações e pendências.** CT09 depende de transporte e confirmação do servidor: a saída na Serial e o esvaziamento da fila não comprovam entrega. Não foram validados precisão de 1 Hz/debounce, retenção após corte de energia, autonomia, diagnóstico elétrico da bateria ou contagem de dias por NF real. CT04 tem logs por intervalo: a rodada de 150 dias foi gravada aproximadamente do dia 84 até depois do limiar; o log do ciclo 2 cobre o limiar 10. No primeiro `STATUS` após religar no CT35, os dias ainda eram os anteriores ao desligamento; os registros seguintes atualizaram a contagem para incluir o intervalo desligado.

Os limites exatos de bateria em 20% e 25% e os demais novos cenários de falha combinada continuam pendentes, com numeração a partir do CT36; CT24–CT34 permanecem reservados. Abrir e fechar o lacre enquanto o dispositivo está desligado não foi ensaiado nesta rodada e continua como discussão de regra com o grupo.

Os logs ficam em `assets/wokwi/`, acessíveis pelos links de cada CT. O Log A contém dois boots; as contagens usam o segundo, a partir do segundo `ets Jul 29 2019`. O log do reteste de 23/10 começa na confirmação de NF do ciclo 1 e segue até depois do segundo encerramento. Os blocos a seguir exibem as capturas sem reproduzir a Serial inteira no documento; o CT01 já aparece na Figura 1.

[serial-a]: ../assets/wokwi/CT01-CT07-CT11-CT12-CT35-execucao.serial.txt
[serial-b]: ../assets/wokwi/CT02-CT03-CT05-CT06-CT08-CT09-CT15-execucao.serial.txt
[serial-c]: ../assets/wokwi/CT13-isolado.serial.txt
[serial-150]: ../assets/wokwi/CT04-limiar150.serial.txt
[serial-ciclos]: ../assets/wokwi/CT04-CT14-CT16-execucao.serial.txt
[serial-fila]: ../assets/wokwi/CT10-execucao.serial.txt
[serial-reteste]: ../assets/wokwi/CT14-2026-10-23.serial.txt

<details>
<summary>CT02 — Status periódico</summary>

![CT02: Status semanal do dia 7; o dia 14 está na captura complementar.](../assets/wokwi/CT02.png)

*Figura 3.4.1-CT02 — Status semanal do dia 7; o dia 14 está na captura complementar. Fonte: captura real do simulador Wokwi for VS Code, 22/10/2026.*

Capturas complementares: [Dia 14](../assets/wokwi/CT02-dia14.png).

</details>

<details>
<summary>CT03 — Prazo e atraso</summary>

![CT03: Alerta de atraso no dia 6, antes do status semanal.](../assets/wokwi/CT03.png)

*Figura 3.4.1-CT03 — Alerta de atraso no dia 6, antes do status semanal. Fonte: captura real do simulador Wokwi for VS Code, 22/10/2026.*

</details>

<details>
<summary>CT04 — Permanência em 150 e 10 dias</summary>

![CT04: Alerta de permanência com limiar de 150 dias.](../assets/wokwi/CT04.png)

*Figura 3.4.1-CT04 — Alerta de permanência com limiar de 150 dias. Fonte: captura real do simulador Wokwi for VS Code, 22/10/2026.*

Capturas complementares: [Limiar 10](../assets/wokwi/CT04-limiar10.png) · [Confirmação de configuração](../assets/wokwi/CT04-configuracao.serial.txt).

</details>

<details>
<summary>CT05 — Violação do lacre</summary>

![CT05: Violação crítica registrada e LED vermelho aceso.](../assets/wokwi/CT05-evento.png)

*Figura 3.4.1-CT05 — Violação crítica registrada e LED vermelho aceso. Fonte: captura real do simulador Wokwi for VS Code, 22/10/2026.*

Capturas complementares: [Estado posterior, em outra fase do piscar](../assets/wokwi/CT05.png).

</details>

<details>
<summary>CT06 — Retirada autorizada e reaplicação</summary>

![CT06: Retirada autorizada dentro da janela de três dias simulados.](../assets/wokwi/CT06-retirada.png)

*Figura 3.4.1-CT06 — Retirada autorizada dentro da janela de três dias simulados. Fonte: captura real do simulador Wokwi for VS Code, 22/10/2026.*

Capturas complementares: [Reaplicação do lacre](../assets/wokwi/CT06.png).

</details>

<details>
<summary>CT07 — Autorização expirada</summary>

![CT07: Violação após a autorização vencer.](../assets/wokwi/CT07.png)

*Figura 3.4.1-CT07 — Violação após a autorização vencer. Fonte: captura real do simulador Wokwi for VS Code, 22/10/2026.*

</details>

<details>
<summary>CT08 — Evento sem conexão</summary>

![CT08: Violação guardada com um evento pendente e LEDs azul/vermelho acesos.](../assets/wokwi/CT08.png)

*Figura 3.4.1-CT08 — Violação guardada com um evento pendente e LEDs azul/vermelho acesos. Fonte: captura real do simulador Wokwi for VS Code, 22/10/2026.*

</details>

<details>
<summary>CT09 — Reconexão — BLOQUEADO para confirmação do servidor</summary>

![CT09: Reenvio local ordenado e fila zero; não comprova confirmação do servidor.](../assets/wokwi/CT09.png)

*Figura 3.4.1-CT09 — Reenvio local ordenado e fila zero; não comprova confirmação do servidor. Fonte: captura real do simulador Wokwi for VS Code, 22/10/2026.*

Capturas complementares: [Dois eventos pendentes antes de reconectar](../assets/wokwi/CT09-antes.png).

</details>

<details>
<summary>CT10 — Fila cheia — rodadas A e B</summary>

Pré-condição das duas rodadas: ciclo inativo, fila inicialmente vazia, lacre fechado e rede indisponível, evitando status periódicos na contagem. Na rodada A, uma violação e dez alterações `LIMIAR 201` a `210` provocaram três descartes dos não críticos mais antigos; a violação e os eventos 204–210 foram reenviados. Na B, doze aberturas sem autorização, separadas por onze fechamentos estáveis, produziram oito violações individuais e resumo de quatro; ambos foram esvaziados ao reconectar. Houve uma interrupção do capturador entre o armazenamento inicial da violação A e os dez comandos; os intervalos ausentes não foram reconstruídos.

![CT10: Resumo de quatro violações enviado após oito individuais, com fila esvaziada.](../assets/wokwi/CT10.png)

*Figura 3.4.1-CT10 — Resumo de quatro violações enviado após oito individuais, com fila esvaziada. Fonte: captura real do simulador Wokwi for VS Code, 22/10/2026.*

Capturas complementares: [A: fila cheia](../assets/wokwi/CT10-A-cheia.png) · [A: violação preservada](../assets/wokwi/CT10-A-violacao-preservada.png) · [A: fila zero](../assets/wokwi/CT10-A-fila-vazia.png) · [B: início do resumo](../assets/wokwi/CT10-B-resumo-iniciado.png) · [B: oito mais quatro pendentes](../assets/wokwi/CT10-B-cheia-resumo4.png).

</details>

<details>
<summary>CT11 — Ruído e pressão mantida do botão</summary>

Foram aplicadas três pressões curtas e uma mantida, com o bounce padrão do Wokwi. A pressão mantida apareceu nos heartbeats de 62 a 88 s sem repetir a autorização. A fronteira de 50 ms não foi medida.

![CT11: Uma autorização registrada na pressão mantida, sem repetição nos heartbeats seguintes.](../assets/wokwi/CT11.png)

*Figura 3.4.1-CT11 — Uma autorização registrada na pressão mantida, sem repetição nos heartbeats seguintes. Fonte: captura real do simulador Wokwi for VS Code, 22/10/2026.*

Capturas complementares: [Botão mantido pressionado](../assets/wokwi/CT11-pressao-mantida.png).

</details>

<details>
<summary>CT12 — Bateria baixa e histerese</summary>

![CT12: Segundo alerta somente após recuperação acima de 25% e nova queda.](../assets/wokwi/CT12.png)

*Figura 3.4.1-CT12 — Segundo alerta somente após recuperação acima de 25% e nova queda. Fonte: captura real do simulador Wokwi for VS Code, 22/10/2026.*

Capturas complementares: [Primeiro alerta](../assets/wokwi/CT12-primeiro-alerta.png) · [22%](../assets/wokwi/CT12-recuperacao-insuficiente.png) · [Nova queda sem repetição](../assets/wokwi/CT12-sem-repeticao.png) · [Recuperação em 31%](../assets/wokwi/CT12-recuperacao.png).

</details>

<details>
<summary>CT13 — Falha do sensor de bateria</summary>

![CT13: Entrada zero depois de bateria saudável gera falha do sensor, sem alerta falso de bateria baixa.](../assets/wokwi/CT13-isolado.png)

*Figura 3.4.1-CT13 — Entrada zero depois de bateria saudável gera falha do sensor, sem alerta falso de bateria baixa. Fonte: captura real do simulador Wokwi for VS Code, 22/10/2026.*

Capturas complementares: [Rodada adicional](../assets/wokwi/CT13.png).

</details>

<details>
<summary>CT14 — Encerramento e reutilização — APROVADO</summary>

![CT14: Encerramento corrigido com ciclo inativo, dias zero e contador incrementado.](../assets/wokwi/CT14-corrigido.png)

*Figura 3.4.1-CT14 — Encerramento corrigido: `ciclo_ativo=false`, `dias_fora=0` e contador 1. Fonte: captura real do simulador Wokwi for VS Code, 23/10/2026.*

Capturas complementares: [Ciclo 2 iniciado com dias zero](../assets/wokwi/CT14-corrigido-reutilizado.png) · [Segundo encerramento com contador 2](../assets/wokwi/CT14-corrigido-segundo-ciclo.png). A [captura anterior à correção](../assets/wokwi/CT14.png) foi preservada como histórico e não representa o resultado do reteste.

</details>

<details>
<summary>CT15 — Recusa de vínculo simultâneo</summary>

![CT15: Vínculo recusado enquanto o ciclo atual está ativo.](../assets/wokwi/CT15.png)

*Figura 3.4.1-CT15 — Vínculo recusado enquanto o ciclo atual está ativo. Fonte: captura real do simulador Wokwi for VS Code, 22/10/2026.*

</details>

<details>
<summary>CT16 — Encerramento recusado — rodadas A e B</summary>

![CT16: Recusa com lacre aberto e NF lançada na versão corrigida.](../assets/wokwi/CT16-corrigido-lacre-aberto.png)

*Figura 3.4.1-CT16 — Recusa por lacre aberto, mesmo com NF de retorno lançada, na versão corrigida. Fonte: captura real do simulador Wokwi for VS Code, 23/10/2026.*

Captura complementar: [Recusa por NF ausente com lacre fechado](../assets/wokwi/CT16-corrigido-nf-ausente.png).

</details>

<details>
<summary>CT35 — Desligamento lógico e religamento</summary>

![CT35: Desligamento registrado e quatro LEDs apagados.](../assets/wokwi/CT35-desligado.png)

*Figura 3.4.1-CT35 — Desligamento registrado e quatro LEDs apagados. Fonte: captura real do simulador Wokwi for VS Code, 22/10/2026.*

Capturas complementares: [Novo STATUS e LEDs restaurados ao religar](../assets/wokwi/CT35.png).

</details>

#### CT17–CT23 — ensaios complementares: execução de 23/10/2026

**Situação:** CT17, CT19, CT20, CT21 e CT23 **APROVADOS no simulador**; CT18 e CT22 **REPROVADOS**, mantendo os critérios esperados da tabela. Execução por automação local no Wokwi for VS Code 4.0.1, em 23/10/2026. O resultado caracteriza comportamento funcional simulado.

**Configuração e reprodução:** base `7777594`, ESP32 DevKit v1, ambiente `demo`, dia de 1 s, coleta de 30 s, espera GNSS de 10 s, validade de fix de 3 s e debounce do lacre de 60 ms. CT17/CT23 e a contraprova do CT18 usam o binário original (SHA-256 `103531CD7BE0D802035A2A6687D21DC33297BBB741177B91772ECCEC6AA6E601`). CT18–CT22 também usam a [instrumentação isolada de ensaio](../tests/wokwi-complementares/README.md), que inclui a lógica de produção sem mudanças e acrescenta leitura NVS, reinício e diagnóstico pelo GPIO32; SHA-256 do binário instrumentado: `2AC42AA1EB4208ECC696E74FBE4D0A3EF2DCFBA6964FEC661D918F2344A0F8AA`. O gerador fornece pulsos digitais e NMEA nos mesmos GPIOs do circuito principal; constantes e esperados não foram ajustados. Os diagramas são gerados separadamente pelo roteiro, preservando o circuito de produção.

Os logs completos ficam em arquivos externos, vinculados nos resultados. A NVS começa vazia em cada rodada nova; o CT22 reinicia a CPU na mesma instância para manter a flash. As sondas leem `q0..q15`, `p0..p7`, `pmeta`, `ctx` e `previous` diretamente com `Preferences`. Mensagens `[ensaio]` identificam a instrumentação e não são eventos de negócio.

| CT | Pré-condição e procedimento | Resultado esperado / medição | Limitação ou preparação |
|---|---|---|---|
| CT17 | Acionar lacre, autorização, ciclo, chave de rede e chave ON/OFF individualmente; repetir com rede simulada indisponível e lacre aberto. | Heartbeat mostra a leitura bruta dos dois botões, `dias_fora`, `fila` e separa `wifi` de `rede_sim`. Os LEDs seguem o estado de negócio: verde fixo com lacre íntegro e piscando na retirada autorizada, vermelho piscando na violação, amarelo no atraso/permanência e azul piscando com a rede simulada indisponível. Os botões geram os eventos de autorização e de ciclo dos CT06–CT16. | Diagnóstico de bancada. Não interpretar LED como confirmação de servidor. |
| CT18 | Com captura temporal controlada, aplicar pulsos de 20 e 40 ms no lacre, depois 10 aberturas/fechamentos estáveis de 200 ms; repetir durante espera por fix e com rede indisponível. | Zero eventos nos pulsos curtos observados; um `seal_changed` por transição estável, sem repetição enquanto o nível é mantido, com `authorized_removal=true` só na retirada autorizada; abertura sem autorização gera também `VIOLACAO_LACRE` (CT05). Registrar `L_det = t_registro_local − t_estabilizacao_contato`, perdas e duplicações; ampliar para 30 estímulos por condição ao medir p95. | Exige injeção temporizada; cliques manuais não comprovam milissegundos. Debounce atual de 60 ms; limite máximo de latência pendente. |
| CT19 | Rodada A: iniciar sem fix. Rodada B: obter posição válida, invalidar o fix e aguardar próxima coleta. Rodada C: interromper NMEA por mais de 3 s antes de solicitar nova coleta. | Sem fix recente, emitir `location_unavailable` após a espera configurada de 10 s, mais latência de processamento medida. Latitude/longitude atuais e `moved` são `null`; histórico é preservado quando disponível. O lacre continua respondendo durante a espera. | Rodada C exige controle da emissão UART no ensaio. Não aceitar só a passagem do tempo com NMEA válido chegando. A referência de comparação é a coleta válida anterior, distinta do último fix observado. |
| CT20 | Com memória de teste sem referência e fix contínuo, coletar primeiro ponto; depois pontos sintéticos separados por aproximadamente 50 m e 150 m da respectiva coleta anterior. Observar ao menos três coletas em `demo`. | Primeiro ponto: `baseline_created=true`, distância e `moved` nulos. Nas seguintes, `moved=false` para 50 m e `true` para 150 m. Com fix disponível, intervalo de 30 s mais latência de processamento; registrar desvio. | Comparar coletas válidas consecutivas. Testar fronteira de 100 m em teste matemático separado, considerando arredondamento; não alegar sete dias medidos com demo de 30 s. |
| CT21 | Iniciar fila vazia em memória de teste; gerar 17 registros, contando também coletas automáticas. Inspecionar chaves `q0` a `q15`; alternar a chave de rede. | Os primeiros 16 ocupam a fila; o 17º aparece na Serial com aviso de limite e não substitui os anteriores. A chave de rede não esvazia a fila. Medir quantos registros ficaram somente na Serial. | Caracteriza a limitação atual; confirmação de entrega, prioridade e reenvio continuam pendentes. Inspeção de NVS exige ferramenta/instrumentação de teste; capacidade final e prioridade ainda pendentes. |
| CT22 | Salvar posição A e registros conhecidos, com ciclo ativo há alguns dias e eventos na fila sem rede; reiniciar preservando flash/NVS. Sem novo fix, observar saída; depois coletar B. | Referência A, registro de diagnóstico, fila de eventos, `ciclo_id`, `dias_fora` e estado do lacre permanecem; `STATUS` com `contexto_restaurado=true`. Histórico após reinício sem idade conhecida apresenta `last_valid_age_ms=null`; novo ponto B compara com A. `uptime_ms` reinicia e não deve ser tratado como horário absoluto. | Confirmar preservação de flash no ambiente usado; se indisponível no simulador, executar em hardware. Sem acesso à NVS, não aprovar retenção da fila apenas pelo JSON da posição. |
| CT23 | Mover potenciômetro ao mínimo, meio e máximo; observar heartbeat/JSON. | Percentual sintético permanece entre 0 e 100 e acompanha a entrada; o mínimo absoluto (ADC = 0) gera `SENSOR_BATERIA_FALHA`, conforme a RN10 e o CT13. Não emitir conclusão de bateria desconectada, autonomia ou tensão física com esses dados. | ADC e percentual simulados não são medição de estado de carga de uma bateria real. |

**Resultados medidos e evidências**

| CT | Situação | Resultado observado | Evidência / log completo |
|---|---|---|---|
| CT17 | APROVADO | Leituras brutas dos dois botões, autorização/retirada, recusa de encerramento, rede indisponível com Wi-Fi conectado, fila e desligamento/religamento observados. LEDs acompanharam os estados; frequência exata não medida. | [Print](../assets/wokwi/CT17.png) · [Serial](../assets/wokwi/CT17-CT23.serial.txt) |
| CT18 | **REPROVADO** | Pulsos de 20/40 ms ignorados. Binário original: 20/20 transições com rede e 16/20 sem rede (4 perdas); sem duplicação observada. Rodada instrumentada: 20/20 e 14/20, respectivamente. | [Original](../assets/wokwi/CT18-original.png) · [Serial original](../assets/wokwi/CT18-original.serial.txt) · [Serial instrumentada](../assets/wokwi/CT18.serial.txt) |
| CT19 | APROVADO | Três saídas sem fix em 11.630/81.630/151.630 ms, com latitude/longitude/`moved` nulos; histórico preservado nas duas últimas. NMEA interrompido de 120 a 160 s, contador parado em 7.788 caracteres. Seis transições do lacre detectadas durante as três esperas. | [Print](../assets/wokwi/CT19.png) · [Serial](../assets/wokwi/CT19.serial.txt) |
| CT20 | APROVADO | Primeiro ponto criou referência. Depois, 50,04 m → `moved=false` e 150,11 m → `true`; coletas em 2.068/32.067/62.067 ms, intervalos de 29.999/30.000 ms (desvios −1/0 ms de 30 s). | [Print](../assets/wokwi/CT20.png) · [Serial](../assets/wokwi/CT20.serial.txt) |
| CT21 | APROVADO | NVS inicialmente vazia; 17 registros gerados (1 coleta + 16 transições). `q0..q15` iguais aos primeiros 16 JSON, preservados nas sondas de 21/28 s após alternar a rede; 1 registro somente na Serial e 1 aviso de limite. | [Print NVS](../assets/wokwi/CT21.png) · [Serial](../assets/wokwi/CT21.serial.txt) |
| CT22 | **REPROVADO no ambiente** | `q0..q2` e `p0..p3` conhecidos antes do reset reapareceram sem alterações; referência A, lacre aberto e ciclo 1 retidos, `STATUS` restaurado com 42 dias. Sem fix: idade histórica nula; B comparou com A (150,11 m). Porém `uptime_ms` continuou crescente após o reinício (novo `STATUS` em 46.419 ms), contrariando o esperado. | [Sem fix após reset](../assets/wokwi/CT22.png) · [B](../assets/wokwi/CT22-ponto-B.png) · [Serial](../assets/wokwi/CT22.serial.txt) |
| CT23 | APROVADO | Entrada mínima/meio/máxima resultou em 0/50/100%; todos os percentuais entre 0 e 100. Zero gerou 1 `SENSOR_BATERIA_FALHA`, sem `BATERIA_BAIXA` falso na rodada. | [Print](../assets/wokwi/CT23.png) · [Serial](../assets/wokwi/CT17-CT23.serial.txt) |

**Medições e pendências:** no CT18 instrumentado, a latência entre a borda e o primeiro byte do JSON foi de 60,523 a 64,926 ms com rede disponível (20 transições). A associação temporal da sonda ficou inválida em parte da rodada sem rede; os valores dessa condição foram descartados, sem editar o log bruto. A contagem foi conferida também no binário original. Não se calculou p95: faltam 30 estímulos por condição e um limite de latência acordado. A retirada autorizada positiva está na execução do CT17; as aberturas do CT18 ocorreram sem autorização.

No CT22, o log registra `SW_CPU_RESET` seguido de `TG1WDT_SYS_RESET`; a retenção NVS foi verificada, mas o relógio de uptime não reiniciou no Wokwi. A causa e o comportamento em hardware permanecem pendentes; não foi alterado o esperado para aprovar o caso. As falhas dos CT18/CT22 exigem investigação e reteste. Corte físico de energia, entrega ao servidor e validação elétrica continuam fora do que estas capturas comprovam.

<details>
<summary>CT17 — Diagnóstico das entradas e estados</summary>

![CT17: Rede simulada indisponível, Wi-Fi conectado e lacre violado.](../assets/wokwi/CT17.png)

*Figura 3.4.1-CT17 — Rede simulada indisponível, Wi-Fi conectado e lacre violado. Fonte: captura real do simulador Wokwi for VS Code, 23/10/2026.*

Capturas complementares: [Autorização](../assets/wokwi/CT17-autorizacao.png) · [Retirada autorizada](../assets/wokwi/CT17-retirada-autorizada.png) · [Botão de ciclo](../assets/wokwi/CT17-ciclo.png) · [Autorização sem rede](../assets/wokwi/CT17-autorizacao-sem-rede.png) · [Desligado](../assets/wokwi/CT17-desligado.png) · [Religado](../assets/wokwi/CT17-religado.png).

</details>

<details>
<summary>CT18 — Transições cronometradas — REPROVADO</summary>

![CT18: Contraprova com o binário original e rede indisponível.](../assets/wokwi/CT18-original.png)

*Figura 3.4.1-CT18 — Contraprova com o binário original e rede indisponível. Fonte: captura real do simulador Wokwi for VS Code, 23/10/2026.*

Capturas complementares: [Rodada instrumentada](../assets/wokwi/CT18.png).

</details>

<details>
<summary>CT19 — Ausência de fix e interrupção NMEA</summary>

![CT19: Coleta sem NMEA com coordenadas atuais nulas e histórico preservado.](../assets/wokwi/CT19.png)

*Figura 3.4.1-CT19 — Coleta sem NMEA com coordenadas atuais nulas e histórico preservado. Fonte: captura real do simulador Wokwi for VS Code, 23/10/2026.*

Capturas complementares: [Fix inválido com NMEA ativo](../assets/wokwi/CT19-fix-invalido.png).

</details>

<details>
<summary>CT20 — Comparação entre coletas válidas</summary>

![CT20: Deslocamento consecutivo de 150,11 m com moved=true.](../assets/wokwi/CT20.png)

*Figura 3.4.1-CT20 — Deslocamento consecutivo de 150,11 m com moved=true. Fonte: captura real do simulador Wokwi for VS Code, 23/10/2026.*

Capturas complementares: [Deslocamento de 50,04 m com moved=false](../assets/wokwi/CT20-50m.png).

</details>

<details>
<summary>CT21 — Limite de diagnóstico na NVS</summary>

![CT21: q15 preservada na inspeção posterior à alternância da rede.](../assets/wokwi/CT21.png)

*Figura 3.4.1-CT21 — q15 preservada na inspeção posterior à alternância da rede. Fonte: captura real do simulador Wokwi for VS Code, 23/10/2026.*

</details>

<details>
<summary>CT22 — Retenção no reinício — REPROVADO no ambiente</summary>

![CT22: Histórico restaurado sem fix novo e com idade desconhecida nula.](../assets/wokwi/CT22.png)

*Figura 3.4.1-CT22 — Histórico restaurado sem fix novo e com idade desconhecida nula. Fonte: captura real do simulador Wokwi for VS Code, 23/10/2026.*

Capturas complementares: [Ponto B comparado com A após reinício](../assets/wokwi/CT22-ponto-B.png).

</details>

<details>
<summary>CT23 — Faixa da entrada de bateria simulada</summary>

![CT23: Potenciômetro no máximo com leitura sintética de 100%.](../assets/wokwi/CT23.png)

*Figura 3.4.1-CT23 — Potenciômetro no máximo com leitura sintética de 100%. Fonte: captura real do simulador Wokwi for VS Code, 23/10/2026.*

Capturas complementares: [Mínimo e falha do sensor](../assets/wokwi/CT23-minimo.png) · [Meio em 50%](../assets/wokwi/CT23-meio.png).

</details>

**Próximas sprints.** As seções seguintes mantêm as orientações e os exemplos originais do template. Seus exemplos de CT não são resultados de execução. A proposta de evolução do circuito e os CT24–CT34 estão no [planejamento de energia e GNSS](outros/planejamento-energia-gnss.md).

### 3.4.2. Protótipo Físico do Projeto (offline) (sprint 2)	

_Aqui você deve registrar diversas situações de teste, indicando exemplos de leitura (entrada) e escrita (saída) apresentadas pelo seu sistema físico. Estes registros serão utilizados para testar seus componentes, portanto, descreva várias situações, incluindo não apenas casos de sucesso, mas também de possíveis falhas nas leituras de entradas e saídas. <br>
Siga as nomenclaturas e convenções já utilizadas anteriormente, e não se esqueça dos alinhamentos de negócios e experiência do usuário para pensar em situações representativas. Inclua figuras do protótipo físico e descrições dos testes realizados para ilustrar o funcionamento do protótipo. Continue a numeração de CT a partir da tabela anterior. A tabela a seguir serve como exemplo._

**CT#** | **Descrição** | **Pré-condição** | **Procedimento de teste** | **Pós-condição/Resultado esperado** | **Observações** (opcional)
|----------|----------|----------|----------|----------|----------|
|CT11|ex. Verificação de conexão de rede bem-sucedida. | ex. O dispositivo está desligado.| ex. Ligue o dispositivo, aguarde alguns segundos e verifique se ele estabelece uma conexão bem-sucedida com a rede Wi-Fi. | ex. O LED verde indicador de conexão acende de forma contínua, sinalizando uma conexão bem-sucedida à rede. |

### 3.4.3. Protótipo do Projeto com MQTT e I2C (sprint 3)	

_Aqui você deve registrar diversas situações de uso de seu sistema como um todo, indicando exemplos de ação do usuário e resposta do sistema, apontando como o ambiente deverá estar configurado para receber a ação e produzir a resposta. Estes registros serão utilizados para testar seu sistema, portanto, descreva várias situações, incluindo não apenas casos de sucesso, mas também de falha nos comportamentos do sistema. <br>
Siga as nomenclaturas e convenções já utilizadas anteriormente, e não se esqueça dos alinhamentos de negócios e experiência do usuário para pensar em situações representativas. Inclua figuras do protótipo físico e dashboards, além de descrições dos testes realizados para ilustrar o funcionamento do protótipo. Continue a numeração de CT a partir da tabela anterior. A tabela a seguir serve como exemplo._

**CT#** | **Configuração do ambiente** | **Ação do usuário** | **Resposta esperada do sistema** 
|----------|----------|----------|----------|
|CT21|ex. precisa de um computador conectado na interface, dois ou mais dispositivos que simulem o posicionamento de um item X no espaço físico etc. | ex. usuário logado busca a localização do item X, que está ativo e operando normalmente | ex. interface do sistema acessa os dados da última localização registrada do item X e apresenta, constando local e horário de última atualização | 

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
2. CERRADINHOBIO. *Nossos Números/Dados de Produção*. Disponível em: https://www.cerradinhobio.com.br/a-companhia/nossos-numeros-dados-de-producao/. Acesso em: 15 out. 2026.
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
