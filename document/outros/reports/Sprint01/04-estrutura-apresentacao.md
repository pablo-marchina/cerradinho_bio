# Arthur - Estrutura da apresentação da Sprint 1

**Autor:** Arthur Loyola (@arthur.loyola)

> **Pergunta investigada:** como contar, em até 5 minutos e com 2 pessoas, o fluxo do problema pelas personas e destacar os requisitos, principalmente as regras de negócio.
> **O que foi feito:** leitura do planning da Sprint 1 (`SP1Planning.html`: rascunho de RF001–RF019, RN01–RN13 e CT01–CT15) e do report de personas (`03-personas-do-fluxo.md`); montagem de uma estrutura em 6 slides, sem roteiro falado.
> **Evidência:** reports do kick-off (16/10/2026), report de personas e planning da Sprint 1. Formato decidido com o grupo: 2 pessoas, Wokwi só em print.
> **Em aberto:** numeração final das regras de negócio (confirmar com @luiz.cazelatto); prints do Wokwi dos casos CT03, CT04 e CT05; definir quem é a Pessoa A e a Pessoa B; roteiro falado (card #43).
> **Alimenta:** roteiro da apresentação da Sprint 1 (card #43); critério "Organização da apresentação" do Artefato 2.
> **Implicação:** a solução não ganha bloco próprio: ela aparece na volta do mesmo fluxo, parada por parada, junto com a regra de negócio que responde a cada dor. É isso que faz o conteúdo caber em 5 minutos.

## 1. Público e objetivo

Público: a Cerradinho na review e a sala, com outros grupos que tratam o mesmo problema. O parceiro precisa se reconhecer no fluxo (as palavras e os números dele), e a sala precisa ver o que só o nosso grupo enxergou: o analista de PCM como usuário principal e a tese de desenhar para quem opera o ciclo, não para quem paga a conta.

## 2. Fio condutor

**A viagem de um motor.** A apresentação segue um motor enviado para conserto. Na ida, cada persona entra no momento do fluxo em que sofre. Na volta, o mesmo trilho com a TAG mostra, em cada parada, a regra de negócio que resolve aquela dor.

## 3. Estrutura

| # | Bloco | Quem | Tempo | O que mostra | Regras e requisitos |
|---|---|---|---|---|---|
| 1 | Gancho e nossa leitura | Pessoa A | 0:00–0:40 | O número do parceiro (R$ 602.969 de complemento de ICMS na safra 24/25), a pergunta "onde está o motor que saiu há 200 dias?" e a tese: desenhamos para o PCM, "o dono do equipamento" [34:28]. | — |
| 2 | A viagem hoje | Pessoa A | 0:40–2:10 | Linha do tempo do fluxo atual com o contador de 270/360 dias e três paradas: **Geraldo** esquece o item não crítico e o fornecedor atrasa; **Clarice** não vê o físico e o motor volta "por fora"; **Izidio** cobra oficina por senso de dono. Mecânico e portaria aparecem só como pontas. | — |
| 3 | Com a TAG: a saída | Pessoa B | 2:10–2:55 | Geraldo vincula e lacra; o ciclo só abre com NF, fornecedor e prazo; lacre aberto sem autorização vira alerta na hora. Print do Wokwi: LED vermelho e o evento no Serial (CT05). | RN02, RN05 · RF002, RF005 |
| 4 | Com a TAG: fora e de volta | Pessoa B | 2:55–3:50 | Alerta de atraso diferente do de permanência; ciclo que só fecha com conferência e NF de retorno (Clarice); histórico pela API para o BI (Izidio). Print do Wokwi: LED amarelo piscando e fixo (CT03, CT04). | RN04, RN11 · RF012, RF018, RF019 |
| 5 | Quadro de rastreabilidade | Pessoa B | 3:50–4:15 | Uma linha por persona: persona → regra → requisito → caso de teste. É a RTM em miniatura. | — |
| 6 | Pedido ao parceiro | Pessoa A | 4:15–4:40 | O MVP que o parceiro pediu (alerta de prazo, alerta de lacre, localização), o que a Sprint 1 já simula e duas perguntas: o PCM é quem usa o painel? A ideia do aviso na portaria faz sentido? | — |
| — | Folga | — | 4:40–5:00 | Transições e imprevistos. | — |

Os números de RF, RN e CT seguem o rascunho do planning da Sprint 1 e podem mudar na versão final da seção 3.3.

## 4. Regras para caber no tempo

- Uma ideia por slide, sem parágrafo; a fala carrega o texto.
- Cerca de 130 palavras por minuto: a Pessoa A fala por volta de 2,5 minutos (cerca de 330 palavras) e a Pessoa B por volta de 2 minutos (cerca de 270 palavras).
- Quatro regras de negócio no total (RN02, RN04, RN05, RN11). As demais ficam na documentação; o quadro do slide 5 mostra que existem sem lê-las.
- A voz troca uma vez na ida (A para B no slide 3) e uma na volta (B para A no slide 6).
- Wokwi só em print; ao vivo arrisca o tempo.
- Ensaiar com cronômetro. Se passar de 4:40, o primeiro corte é o slide 5, que vira uma frase no slide 4.

## 5. Por que essa ordem

O Artefato 2 avalia a organização da apresentação, e a matriz de rastreabilidade (3.3.4) liga persona, requisito, regra e caso de teste. Contar o fluxo pelas personas e mostrar o quadro persona → regra deixa essa ligação visível sem exibir a matriz inteira. Terminar com perguntas usa a review para validar as personas, que está em aberto no report 03.
