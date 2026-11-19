# Visões por persona e fluxo do processo

**Status:** proposta implementada no frontend com dados fictícios, para validar com o grupo e o parceiro.
**Base:** personas da seção 3.2.1 (Geraldo, Clarice e Izidio), relatórios do kick-off sobre o fluxo de remessa para conserto e sobre as personas, RF001 a RF030 e guia de estilos v2.

## 1. Problema da versão anterior

A tela inicial partia do dispositivo: lacre, bateria, fila e comandos de 6 equipamentos. Geraldo cuida de dezenas de ativos ao mesmo tempo e precisa ver primeiro **em que etapa cada ativo está** e **o que pede ação dele**. Clarice precisa saber o que voltou sem baixa fiscal. Izidio precisa ver onde o processo trava. Nenhuma dessas perguntas tinha resposta direta.

## 2. Fluxo ideal

| # | Etapa | Quem age | O que a plataforma faz |
|---|---|---|---|
| 1 | Planejamento | PCM | Cadastra o ativo com foto, OM, área e criticidade; vincula uma TAG livre e confirma o lacre; registra oficina, orçamento e prazo combinado. |
| 2 | Aguardando NF | Fiscal | Recebe o pedido com os dados já preenchidos. Emite e assina a NF-e no sistema fiscal e vincula o número ao ativo. CFOP 5.915 ou 6.915 conforme a UF da oficina. |
| 3 | Aguardando coleta | PCM | Recebe o aviso da NF emitida e aciona a logística. A DANFE segue com o ativo. |
| 4 | Indo para a oficina | TAG e logística | A TAG registra a saída e a posição. O prazo legal corre desde a emissão da NF (RF012). |
| 5 | Em reparo | PCM | Acompanha o prazo combinado, recebe alerta antes de vencer, cobra a oficina por e-mail ou telefone e registra a resposta. |
| 6 | Voltando | TAG e logística | A TAG registra a saída da oficina. |
| 7 | Na usina sem baixa | Fiscal | A chegada na portaria avisa o Fiscal, que lança a NF de retorno (5.916 ou 6.916). O ciclo não fecha antes disso (RF016, RF029). |
| 8 | Encerrado | Sistema | Libera a TAG para outro ativo e preserva o histórico (RF017). |

## 3. As três visões

- **PCM (Geraldo), rota `/`:** esteira das 7 etapas com contagem e dono de cada uma; blocos de situação (críticos, acima do prazo, urgentes, precisam de ligação, já cobrados, sem sinal); mapa esquemático com usina, oficinas e ativos em rota; fila de ação ordenada por urgência; lista completa com busca. Botão **Novo envio** abre o cadastro em 4 passos.
- **Fiscal (Clarice), rota `/fiscal`:** NFs de remessa a emitir; ativos que voltaram sem NF de retorno; os 10 ativos mais perto do prazo legal (270 dias em GO, 360 fora); busca "esse ativo já voltou?" por NF, TAG ou OM; lista do que a NF de remessa para conserto leva, para validar com o Fiscal.
- **Logística (Izidio), rota `/logistica`:** maior gargalo do momento; tempo médio em cada etapa; saídas e retornos por mês; desempenho de cada oficina (reparo médio e taxa no prazo); passagens paradas entre áreas e quem cobrar; exportação dos ciclos em CSV para o BI.
- **Ficha do ativo, rota `/ativos/:id`:** foto, etapas com datas, cartão de próxima ação com o dono da etapa, oficina e combinado com e-mail pronto e telefone, dados fiscais com barra do prazo legal, TAG e lacre, e histórico.

As telas da TAG (telemetria, comandos MQTT, alertas, mapa e dispositivos) continuam no grupo **TAG e dispositivos** do menu. A antiga visão geral passou para `/telemetria`.

## 4. Benchmark

| Referência | O que aproveitamos |
|---|---|
| Fleetio e MaintainX (manutenção de frota e ativos) | Cartões de situação clicáveis que filtram a lista e "próxima ação" por item. |
| project44 e FourKites (rastreamento de cargas) | Esteira de etapas com contagem por etapa e alerta de exceção, não só mapa. |
| Trackunit e Samsara (telemetria de equipamentos) | Mapa como contexto secundário e "último sinal" com idade, sem fingir posição atual. |
| Asset Panda e Sortly (cadastro de ativos) | Cadastro com foto tirada no celular e etiqueta vinculada no mesmo passo. |

## 5. Regras de exibição

- **Crítico:** lacre violado, atraso em ativo de criticidade alta ou prazo legal com 15 dias ou menos.
- **Acima do prazo:** a caminho ou na oficina e depois da data prometida.
- **Urgente:** vence em até 7 dias ou prazo legal em até 30 dias.
- **Precisa de ligação:** atrasado ou vencendo em até 5 dias, sem cobrança registrada nos últimos 3 dias.
- **Sem sinal:** TAG fora da usina sem comunicação há mais de 8 dias (intervalo semanal mais folga).
- **NF:** maiúsculas automáticas e a mesma regra do firmware (até 15 caracteres entre letras, dígitos, hífen, ponto e barra).

## 6. Limites e pendências

- A carteira de 70 ativos, as oficinas, os contatos e as NFs são fictícios e gerados de forma determinística (`src/portfolio/seed.ts`). As alterações ficam no navegador.
- O ICMS em risco é estimado em 12% do valor da NF. É ilustrativo e não é cálculo fiscal.
- O GuaráTracking não emite NF-e: só guarda o vínculo. Os campos da seção do Fiscal precisam ser validados com a Clarice real.
- A troca de visão não é controle de acesso: autenticação e perfis (RF021) continuam pendentes.
- O e-mail usa `mailto:` com texto pronto. O envio automático e o escalonamento (RF018 e RF030) dependem de backend.
