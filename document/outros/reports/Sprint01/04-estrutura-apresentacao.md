# Arthur - Estrutura da apresentação da Sprint 1

**Autor:** Arthur Loyola (@arthur.loyola)

> **Pergunta investigada:** como apresentar, em 5 minutos exatos e com 2 pessoas, os quatro artefatos da Sprint 1 de modo que o parceiro confirme ou corrija o que assumimos, em especial quem são as nossas personas.
> **O que foi feito:** cruzamento de `ArtefatosSolicitados.md` (itens e critérios dos Artefatos 1 a 4), do planning (`SP1Planning.html`), dos reports do kick-off e dos slides da professora (arquitetura da narrativa, timeboxing de 5 minutos, matriz de qualidade); a estrutura virou um roteiro de alinhamento, em que cada bloco termina com uma pergunta de confirmação.
> **Evidência:** `ArtefatosSolicitados.md`, `SP1Planning.html`, report de personas (`03-personas-do-fluxo.md`), reports do kick-off e slides "Sprint 1 - Encontro 2" (Camila Naves Arantes).
> **Em aberto:** status real de cada artefato no dia da review; prints do Wokwi (CT03, CT04 e CT05); numeração final de RF, RN e CT (hoje só existe no planning; a `documentacao.md` ainda está com o texto do template); trecho do Bruno a confirmar com ele; roteiro falado (card #43).
> **Alimenta:** roteiro da apresentação da Sprint 1 (card #43); critérios de Didática, Organização, Envolvimento e Contribuição individual da review.
> **Implicação:** a apresentação mostra os quatro artefatos em uma linha de raciocínio só (negócio, personas, requisitos, protótipo), com as personas em destaque, e termina na pergunta da professora: "O que precisamos validar com o parceiro antes de avançar?". Cada bloco gera um item para a lista de validação.

## 1. Regras da professora que moldam a apresentação

| Regra | Como a estrutura responde |
|---|---|
| Arquitetura da narrativa: Problema, Decisão, Evidência, Resultado, Limitações, Próximo passo | É a ordem dos blocos da seção 3. |
| Pergunta de fechamento: "O que precisamos validar com o parceiro antes de avançar?" | Último bloco; as respostas alimentam o registro de alinhamento (seção 5). |
| Público: a sala inteira e o parceiro, sem falar para uma só pessoa | Os números do parceiro entram como fato, sem cobrar ninguém. |
| 5 minutos para a Sprint 1, "planejada cirurgicamente" | Roteiro cronometrado, 5 segundos de folga. |
| Os 2 que apresentam respondem as perguntas | Arthur e Bruno ensaiam as respostas de todos os blocos. |
| Slides apoiam, não são roteiro; evitar excesso cognitivo | Seis slides, uma ideia por slide. |
| Matriz de qualidade: didática, organização (divisão de falas), envolvimento, contribuição individual | Divisão de falas na seção 2; contribuição na seção 6. |
| Roteiro no Drive na segunda até as 16h; slides na quarta até as 16h | Seção 7. |

## 2. Quem fala

- **Arthur:** blocos 1, 2 e 6 (cerca de 2min55s).
- **Bruno:** blocos 3, 4 e 5 (cerca de 2min).

Cada um cita, em uma frase, o próprio trabalho (seção 6) e passa a fala ao colega.

## 3. Estrutura

Padrão de todo bloco: o slide mostra **o que assumimos** e termina com uma linha **"Confirmar:"**, a pergunta de uma frase para o parceiro. Assim a sala entende o que é fato, o que é hipótese e o que o parceiro precisa dizer.

| # | Bloco | Artefato | Quem | Tempo | Slide | Confirmar com o parceiro |
|---|---|---|---|---|---|---|
| 1 | Problema e negócio | Art. 1 | Arthur | 0:00–1:15 | Uma página de negócio: quem é a Cerradinho e o problema (R$ 602.969 de complemento de ICMS na safra 24/25, item que volta "por fora"); objetivos (visibilidade dos ativos fora, vínculo de cada ciclo à NF, atraso separado de permanência); solução em alto nível (dispositivo reutilizável com lacre, alertas e API para o BI); em uma linha cada, o que a análise de negócio trouxe (5 Forças e SWOT, Value Proposition Canvas, 2 ou 3 riscos principais, LGPD e BOM). | "O problema e os objetivos estão como vocês os veem? Os dados da última safra podem ser enviados?" |
| 2 | Personas | Art. 2 | Arthur | 1:15–2:30 | Três cartões lado a lado, nesta ordem: **Geraldo, analista de PCM (principal)**, **Clarice, analista fiscal** e **Izidio, analista de logística**. Cada cartão: papel no fluxo, a dor principal, o que precisa da solução e uma fala do parceiro. Faixa final: como construímos (kick-off, reports, síntese com IA; **sem entrevistas, são hipóteses**). | "Estas são as pessoas do problema de vocês? Falta alguém? O PCM é mesmo quem usa o painel?" |
| 3 | Decisão: requisitos e regras | Art. 4 | Bruno | 2:30–3:30 | Quadro persona, regra de negócio, requisito e caso de teste (RTM em miniatura), com quatro regras: o ciclo só abre com NF, fornecedor, saída e prazo; atraso e permanência são alertas distintos; lacre aberto sem autorização é violação crítica; o ciclo só fecha com conferência e NF de retorno. Abaixo, os seis grupos de requisito da rubrica em uma linha (sensor, formato MQTT, Wi‑Fi, usuário e acesso ao painel, dispositivos na plataforma, gráficos). | "Estas regras refletem o processo real? O limiar de 150 dias e a janela de retirada autorizada (3 dias) estão certos?" |
| 4 | Evidência: protótipo | Art. 3 | Bruno | 3:30–4:10 | Prints do Wokwi: CT05 (lacre aberto, LED vermelho), CT03 e CT04 (atraso e permanência). Uma linha: protótipo próprio com LEDs e botões, classe e funções no código, 15 casos de teste com sucesso e falha, código no repositório. | "Os alertas e as cores fazem sentido para quem vai operar?" |
| 5 | Resultado e limitações | Arts. 1 a 4 | Bruno | 4:10–4:30 | Os quatro artefatos em uma linha cada, com o status real (preencher na véspera). Três limitações: sem dados da última safra; o Wokwi simula, não prova localização em campo (Wi‑Fi no protótipo, canal de produção em aberto); personas e regras ainda não validadas. | — |
| 6 | Próximo passo e pergunta de ouro | — | Arthur | 4:30–4:55 | Próximos passos da Sprint 2 e a pergunta: **"O que precisamos validar com o parceiro antes de avançar?"**, com a lista dos "Confirmar" dos blocos 1 a 4 como resposta sugerida. | O parceiro responde; o grupo anota Sim, Não ou Ajuste. |
| — | Folga | — | — | 4:55–5:00 | — | — |

## 4. Cobertura dos artefatos

Conferência de `ArtefatosSolicitados.md` contra a apresentação e contra o planning. "Citado" quer dizer uma linha no slide; o detalhe fica na `documentacao.md`.

### 4.1 Artefato 1: negócio (bloco 1)

| Item pedido | Aparece na apresentação | Observação do planning |
|---|---|---|
| Introdução e parceiro (seção 1) | Citado: quem é a Cerradinho | Dados de porte e local dependem de fonte pública conferida pelo Pablo. |
| Objetivos (1.1) e proposta (1.2) | Sim: objetivos e solução em alto nível | 1.1 sem tecnologia; 1.2 só em alto nível, sem decidir o canal de comunicação. |
| Justificativa (1.3) | Citado: benefícios como potencial, não garantia | O parceiro disse que o ganho financeiro não é critério de sucesso. |
| Contexto da indústria e 5 Forças (3.1.1) | Citado em uma linha | Setor é bioenergia, não IoT. |
| SWOT (3.1.2) | Citado em uma linha | — |
| Descrição da solução, 6 itens (3.1.3) | Sim: problema, solução, uso e benefício aparecem nos blocos 1 a 3; critério de sucesso fica na documentação | Metas de sucesso são propostas, não acordadas com o parceiro; entram em "Confirmar". |
| Value Proposition Canvas (3.1.4) | Citado: dores das personas ligadas ao que a solução alivia (bloco 2) | O canvas parte das personas P1 e P2. |
| Matriz de riscos (3.1.5) | Citado: 2 ou 3 riscos (por exemplo, GNSS sem sinal em oficina coberta, autonomia de 150 dias, oficina sem Wi‑Fi) | Escolher na matriz final. |
| LGPD (3.1.6) | Citado em uma linha: a localização no transporte é dado pessoal indireto e fica minimizada | DPO ainda sem nome, a ser indicado pelo parceiro. |
| BOM (3.1.7) | Citado em uma linha | Itens sem preço público ficam como estimativa; nada de custo real. |

### 4.2 Artefato 2: personas (bloco 2)

| Item da avaliação | Onde aparece |
|---|---|
| Pesquisa e reunião de informações | Faixa "como construímos": kick-off, reports e síntese |
| Síntese | Os três cartões |
| Template próprio e seu uso | Os cartões seguem o template e o guia de estilos |
| Profundidade e relação com o projeto | Cada cartão liga a dor ao requisito |
| Descrição do processo e uso de IA | Faixa "como construímos", com a nota de que a IA foi usada e o texto foi conferido pelo grupo |

### 4.3 Artefato 3: protótipo no Wokwi (bloco 4)

| Nível "Avançado" da rubrica | Onde aparece |
|---|---|
| Conta no Wokwi e código publicado | Uma linha no slide |
| Projeto próprio com LEDs e botões, `setup` e `loop` | Prints do circuito e dos LEDs |
| Ao menos 2 funções, além de uma classe com instância usada | Uma linha ("classe e funções") |
| Casos de teste com sucesso e falha, e o protótipo passa neles | Prints de CT03, CT04 e CT05; 15 casos no total |

### 4.4 Artefato 4: requisitos, regras e RTM (bloco 3)

| Item da rubrica | Onde aparece |
|---|---|
| Coleta via sensor, formato MQTT, Wi‑Fi, chave de usuário e acesso ao painel, dispositivos na plataforma, gráficos | Uma linha com os seis grupos, ligados aos requisitos |
| Regras de negócio | As quatro regras do quadro |
| Matriz de rastreabilidade | O próprio quadro persona, regra, requisito e caso de teste |

## 5. Alinhamento com o planning

Pontos em que a apresentação e o planning divergem, ou em que o planning impõe um cuidado. Se o grupo concordar, os cards e a documentação seguem esta coluna.

| Tema | Planning | Apresentação | Ação |
|---|---|---|---|
| Personas | P1 Gestão Patrimonial e P2 Manutenção; P3 opcional | Geraldo (PCM), Clarice (fiscal) e Izidio (logística), a partir do kick-off | Cards #11, #12 e #18 já atualizados; a seção 3.2.1 precisa registrar a troca e o motivo. |
| Wi‑Fi e celular | Wi‑Fi só no protótipo; canal de produção fica para uma sprint seguinte | Dito como limitação | Manter nos blocos 4 e 5. |
| Localização no Wokwi | O simulador não é prova de geolocalização; coordenadas fictícias | Dito como limitação | Não mostrar mapa como se fosse campo. |
| Limiar de 150 dias | É limiar de permanência, não autonomia da bateria nem prazo da manutenção | Pergunta de confirmação no bloco 3 | Cuidar da frase falada. |
| Requisitos não funcionais, dashboard e API | Fora desta sprint; dashboard e API entram só como requisito | A API aparece como requisito, não como entrega | Não apresentar tela. |
| Numeração de RF, RN e CT | Rascunho RF001 a RF019, RN01 a RN13, CT01 a CT15 | Slides citam só regras pelo texto, sem número, até a numeração final | Confirmar com @luiz.cazelatto. |
| Dados restritos | Nada de NF, fornecedor, rota ou custo real no material | Só dados fictícios nos slides e prints | Conferir os prints. |
| Dados da última safra | Pendentes do parceiro; sem dado, a dor é qualitativa | Entra em "Confirmar" do bloco 1 | Não citar números da safra. |
| Aviso na portaria | Ideia do grupo, não discutida com o parceiro | Aparece como ideia no cartão da Clarice | Marcar como hipótese. |

## 6. Contribuição individual

| Pessoa | Trabalho a citar | Ligação com o todo | Fonte |
|---|---|---|---|
| Arthur | Reports do kick-off, separação das três personas, guia de estilos, estrutura e roteiro da apresentação | Personas e fluxo alimentam os requisitos e as regras de negócio | Cards #40, #42, #43 e #44 |
| Bruno | A confirmar com ele: pesquisa e proto-personas, introdução das personas, texto do processo de criação com IA | Mostra como as personas foram construídas | Cards #5, #12 e #19 |

## 7. Regras para caber nos 5 minutos

- Arthur fala cerca de 380 palavras e Bruno cerca de 260 (130 palavras por minuto).
- Seis slides, um por bloco. Cada slide: no máximo uma imagem ou um quadro, sem parágrafo.
- Wokwi só em print; vídeo, se houver, testado offline antes.
- Ensaiar com cronômetro pelo menos duas vezes. Se passar de 4:55, o primeiro corte é o bloco 5, que vira uma frase no começo do bloco 6.
- Os dois respondem perguntas depois dos 5 minutos, inclusive as dos blocos do colega.

## 8. Prazos

- Roteiro no Drive: segunda-feira até as 16h, na pasta da Sprint 1, com nome padronizado.
- Slides no Drive: quarta-feira até as 16h; só a versão final, identificada.
- Review: quinta-feira.
