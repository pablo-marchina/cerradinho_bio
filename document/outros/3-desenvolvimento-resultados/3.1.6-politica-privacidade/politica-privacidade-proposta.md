# Minuta de política de privacidade — GuaráTracking (versão de trabalho)

> **Não homologada:** este documento descreve um desenho de tratamento para eventual piloto; não é uma política publicada nem prova de conformidade. Até aprovação institucional, executar a demonstração somente com dados fictícios. Atualizar com o DPO/Jurídico antes de coletar dados reais.

## Informações gerais

A solução acompanha equipamentos da CerradinhoBio enviados à manutenção externa. **Papel proposto, não confirmado:** CerradinhoBio como controladora; eventual equipe/fornecedor como operador somente se houver instrumento e instruções documentados.

#### 3.1.6.2 Dados coletados

Dos usuários: nome, e-mail corporativo, cargo, perfil de acesso, `usuario_id`, credenciais, endereço IP e registros de acesso. Dos responsáveis por fornecedores: nome e contato profissional. Do dispositivo: posição GNSS semanal, estado do lacre, bateria e horário dos eventos. A posição do equipamento pode identificar indiretamente quem o transporta, por isso não há rastreamento contínuo.

#### 3.1.6.3 Fontes

Cadastro feito pelos usuários na plataforma, dados do ciclo informados pelo PCM e pelo Fiscal, mensagens dos dispositivos e registros técnicos gerados pelo sistema.

#### 3.1.6.4 Finalidades

Autenticar usuários, registrar ciclos de manutenção, emitir alertas aos responsáveis, manter o histórico para auditoria e fornecer dados ao BI. Os dados não serão usados para avaliar o desempenho individual de pessoas.

#### 3.1.6.5 Bases legais

As bases devem ser definidas por finalidade. **Execução de contrato** (art. 7º, V) só se aplica aos dados do titular que seja parte de contrato ou de procedimento preliminar solicitado por ele; não deve justificar genericamente dados de empregados, motoristas ou contatos de fornecedores. Para autenticação, segurança e rastreabilidade operacional, avaliar **legítimo interesse** (art. 7º, IX) com teste de balanceamento, necessidade, transparência e canal de oposição. Para documentos fiscais, avaliar obrigação legal (art. 7º, II). Nenhuma base está homologada.

#### 3.1.6.6 Armazenamento

No protótipo, os dados ficam apenas na memória do dispositivo e na saída Serial. No piloto, provedor, região, transferências e papéis de acesso dependem de escolha e contrato; não há localização de armazenamento homologada.

#### 3.1.6.7 Retenção

**Prazos propostos (não aprovados):** histórico de ciclos até 5 anos, logs de acesso até 6 meses e contas até 90 dias depois do desligamento. Esses prazos não são automaticamente obrigações legais para todas as categorias; ajustar por finalidade, necessidade, dispositivo legal, backup e exclusão.

#### 3.1.6.8 Cookies

No protótipo não há painel web; para a plataforma futura, está previsto cookie de sessão. Outros cookies, se introduzidos, exigirão inventário e transparência.

#### 3.1.6.9 Compartilhamento

Somente com os operadores necessários ao funcionamento (provedor de nuvem, broker MQTT e operadora de conectividade), sob contrato, e com autoridades quando houver obrigação legal. Não há venda nem cessão de dados.

#### 3.1.6.10 Segurança

**No simulador:** entradas sintéticas, JSON Serial e estado local; o protótipo não prova TLS nem autenticação em servidor. **Previsto para produto:** autenticação individual, perfis, TLS, segredo fora do código, trilha de auditoria e plano de resposta a incidentes, sujeitos a testes.

#### 3.1.6.11 Direitos

O titular pode pedir confirmação do tratamento, acesso, correção, anonimização, bloqueio ou eliminação de dados desnecessários, informação sobre compartilhamento e revisão de decisões automatizadas (art. 18) [40].

#### 3.1.6.12 Solicitações

A CerradinhoBio publica uma política institucional de privacidade em seu [site oficial](https://www.cerradinhobio.com.br/politica-de-privacidade/). O canal só será indicado no GuaráTracking após confirmação formal de que recebe demandas desse tratamento. O procedimento deverá autenticar o solicitante, registrar o pedido, encaminhá-lo ao controlador e acompanhar o prazo aplicável. Nenhum contato pessoal ou e-mail será divulgado sem homologação.

#### 3.1.6.13 Encarregado

**Encarregado e contato indicados pela CerradinhoBio (a confirmar).** A gerência do projeto deverá obter confirmação formal do Jurídico/encarregado até **10 dias úteis antes do piloto com dados reais**. Se identidade e canal não forem aprovados até esse marco, o piloto permanecerá restrito a dados fictícios ou anonimizados.


## Aprovações obrigatórias antes do piloto

| Decisão | Evidência necessária | Responsável | Prazo-limite | Status |
|---|---|---|---|---|
| Identidade e papéis controlador/operador | Jurídico/encarregado aprova a cadeia de tratamento | Projeto + Jurídico do parceiro | 10 dias úteis antes do piloto real | Não confirmada |
| Finalidades, dados e bases legais | Inventário e parecer para cada finalidade | Jurídico/encarregado | 10 dias úteis antes do piloto real | Não confirmada |
| Fornecedores, infraestrutura, transferências | Provedor, contratos, região e controles | TI + Jurídico | Antes da contratação | Não confirmada |
| Retenção, backup, eliminação | Tabela de prazos e responsáveis | Dados + Jurídico | 10 dias úteis antes do piloto real | Não confirmada |
| Segurança implementada | Testes de autenticação, TLS, perfis e incidentes | TI/Software | Antes de inserir dado real | Não implementada neste protótipo |
| Canal do titular e encarregado | Confirmação institucional atualizada | Projeto + parceiro | 10 dias úteis antes do piloto real | Não confirmada |

**Regra de bloqueio:** sem aprovação dos seis itens, nenhuma pessoa da equipe deve inserir dados pessoais reais no dispositivo, plataforma, repositório ou demonstração.

Referências normativas: [LGPD](https://www.planalto.gov.br/ccivil_03/_ato2015-2018/2018/lei/l13709compilado.htm), arts. 7º, 9º, 18, 41, e [orientações da ANPD](https://www.gov.br/anpd/pt-br).
