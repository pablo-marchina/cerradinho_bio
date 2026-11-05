# Registro aprofundado e resposta aos riscos — Sprint 1

**Escopo:** riscos capazes de comprometer execução, prazo ou entrega do projeto. Falhas de funcionamento do produto são requisitos e cenários de teste nas seções 3.3 e 3.4. Os níveis 10/30/50/70/90 são faixas ordinais do template, não probabilidades estatísticas. A figura `matrizRiscos.png` ainda representa a versão inicial e deverá ser atualizada conforme esta tabela.

| ID | Fundamentação do nível | Gatilho | Prevenção e contingência | Responsável |
|---|---|---|---|---|
| R01 — dados do parceiro | 70%/alto: volumes e tempos reais ainda dependem de autorização; sem eles, a validação perde representatividade. | Conjunto mínimo não aprovado até a data de preparação do piloto. | Solicitar dados anonimizados; manter massa sintética equivalente e declarar a limitação. | Projeto/Parceiro |
| R02 — acesso ao campo | 50%/muito alto: equipamento, trajeto e oficina dependem de agenda, segurança e terceiro; a ausência impede testes físicos essenciais. | Janela e local não confirmados antes da sprint de ensaio. | Reservar data e preparar percurso externo controlado alternativo. | Projeto/Parceiro |
| R03 — componentes | 70%/alto: o GNSS cotado estava sem estoque e há itens importados, com risco concreto de atraso. | Item crítico indisponível ou prazo maior que a janela de montagem. | Cotar duas alternativas compatíveis e definir data-limite de compra. | Hardware/Compras |
| R04 — expansão de escopo | 50%/alto: firmware, hardware, plataforma, dados e BI disputam prazo acadêmico fixo. | Requisito novo sem retirada ou replanejamento equivalente. | Controle de mudanças; priorizar MVP e registrar itens futuros. | Produto/Projeto |
| R05 — capacidade da equipe | 50%/alto: dependências entre frentes tornam defeitos tardios capazes de bloquear integração e testes. | Marco semanal perdido ou frente crítica sem revisor. | Revisão cruzada, integração semanal e reserva explícita para retrabalho. | Projeto/Equipe |
| R06 — decisão de conectividade | 50%/alto: cobertura e preço por oficina não foram validados; atraso mantém arquitetura e BOM abertas. | Ausência de matriz e decisão na data acordada. | Fixar data de decisão e comparar cobertura, autonomia, confiabilidade e custo. | Produto/Conectividade |
| R07 — custo do piloto | 70%/alto: plano de dados, lacres, reposição, frete e tributos não têm orçamento fechado. | Cenário total superar limite aprovado pelo parceiro. | Orçar cenários de 1 e 5 dispositivos antes de comprar; prever reserva de manutenção. | Produto/Compras |
| R08 — API/BI | 50%/moderado: sistemas, credenciais e contrato corporativo não estão disponíveis ao grupo; o cliente de teste preserva parte do objetivo. | Contrato de dados não aprovado até o teste de integração. | Documentar API e demonstrar em cliente de teste, sem depender de ambiente produtivo. | Software/TI |
| R09 — dados restritos | 30%/muito alto: dados sintéticos reduzem a chance, mas localização, NF ou segredo publicado causariam impacto jurídico e reputacional. | Dado real ou credencial detectado em artefato, log ou demonstração. | Anonimizar, aplicar acesso mínimo e revisar repositório e apresentação. | Dados/LGPD |
| R10 — validação tardia | 50%/alto: papéis, metas e fluxo ainda têm pendências; retorno apenas no fim pode inviabilizar correções. | Decisão do parceiro não registrada no checkpoint. | Reuniões curtas de validação, responsáveis e prazos de resposta. | Projeto/Parceiro |

## Oportunidades

| ID | Fundamentação | Exploração | Responsável |
|---|---|---|---|
| O01 — dados anonimizados antecipados | 50%/alto: dependem de aprovação, mas tornariam cenários e métricas mais realistas sem expor dados. | Preparar modelo mínimo e importar assim que autorizado. | Projeto/Dados |
| O02 — participação de oficina | 30%/alto: depende de terceiro, mas permite validar fixação e fluxo em contexto real. | Agendar roteiro controlado e acordo sem dados sensíveis. | Projeto/Parceiro |

## Rotina de atualização

Revisar a matriz em cada checkpoint. Quando um risco mudar ou se materializar, registrar data, gatilho, ação, evidência, responsável e novo nível. IDs devem permanecer no formato `R01`–`R10` e `O01`–`O02` em texto e figura.
