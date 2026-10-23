# Revisão de conteúdo de seção da documentação

Complementa a parte "Documentação" do `checklist.md` com o que apareceu nas revisões dos MRs !10 (matriz de riscos, 3.1.5) e !20 (Value Proposition Canvas, 3.1.4). As partes "Aderência", "Tamanho", "Leitura" e "Fontes" valem para qualquer seção; em "Lógica do framework", leia só o bloco da seção revisada. Cada item traz o porquê, para você julgar os casos que a lista não prevê.

## Aderência à avaliação

- **Monte uma tabela "exigência do Adalove → Atende / Parcial / Não atende"**, com o motivo. A professora corrige pelo texto do Adalove, então qualquer coisa fora dele é risco de nota.
- **Template do drive.** Seguir a estrutura do template não basta: quem corrige precisa ver que ele foi usado. O padrão do grupo (3.1.1, Cinco Forças) é a figura seguida de `*Figura — ...*` e de `*Fonte: template "<nome do arquivo>", pasta de templates de negócios do Inteli [n]; conteúdo: Grupo G01 (2026).*`, com a referência na seção 5. A pasta de templates está linkada no topo da seção 3.1. Uma figura gerada por outra ferramenta (Framekit etc.) e citada só pela ferramenta não cumpre a exigência.
- **A seção fala da solução que estamos desenvolvendo** (TAG + plataforma), e não de um produto genérico.

## Tamanho

- Só se a avaliação ou a professora tiver dado limite de palavras para a seção. Rode `count_words.py --limit N` e informe o total e o total sem título, legenda, fonte e texto alternativo, porque a regra exata de contagem costuma não estar escrita.
- Se a seção passar do limite, **proponha um corte concreto e mostre a nova contagem**. Por exemplo: tirar colunas de uma tabela quando a figura já mostra a mesma informação, ou trocar um parágrafo de correspondências por uma tabela.
- Antes de pedir acréscimos, some o custo deles. Pedido que estoura o limite precisa vir com o corte correspondente.

## Leitura para quem não participou do trabalho

- **A abertura diz o que a figura mostra e como ler, antes da figura.**
- **Nada de "sopa de códigos".** Um parágrafo como "AD1 e CG1 respondem a D1, atendendo G1" obriga o leitor a procurar cada código na imagem. Código sempre vem com o nome ("D2 – Localização e prazo incertos"), e uma relação item → item (dor → resposta → ganho, risco → mitigação) vira tabela.
- **A mesma informação aparece uma vez só.** Um valor repetido na abertura, no corpo e na figura gasta palavras e cansa quem lê.
- **Ressalvas numa frase só.** "Não presume", "sem garantir" e "sem determinar sozinho" espalhados deixam o texto defensivo. Uma frase como "os ganhos são hipóteses e serão medidos no piloto" cobre todas.
- **Termos técnicos explicados na primeira vez** (GNSS, NB-IoT, LTE-M) e no lugar certo: na causa, não na coluna de justificativa.
- **Nada de metadado de ferramenta**: link da ferramenta no corpo, "30 itens", "exportado diretamente do X".

## Lógica do framework

Pergunte se cada item está no bloco certo e se tem par.

**Value Proposition Canvas (3.1.4)**
- Dor é algo que incomoda o cliente **hoje**. Uma limitação da nossa solução (cobertura de rede, bateria) não é dor do cliente: é risco, e vai para a matriz de riscos. Um aliviador para essa "dor" alivia um problema que o próprio produto cria.
- Produto e serviço é o que entregamos. Criador de ganho é o efeito que o produto gera. "TAG reutilizável" é produto; "reuso da TAG reduz o custo por remessa" é criador de ganho, e precisa do ganho correspondente no perfil.
- Dor escrita como fato ("ICMS pago na safra") não diz qual é o incômodo. A forma certa é algo como "complemento de ICMS por prazo vencido".
- Todo aliviador e todo criador de ganho apontam para uma dor ou um ganho, e vice-versa.
- O perfil cobre as personas da 3.2.1 ou explica quem ficou de fora. Um item do mapa que só serve a uma persona ausente (por exemplo, a API para o BI do Izidio, P3) é sinal disso.

**Matriz de riscos (3.1.5)**
- As duas metades (ameaças e oportunidades), como no template.
- Probabilidade e impacto justificados com a origem (TAPI, kick-off ou hipótese).
- Prazos legais (270 dias em GO e 360 fora de GO, suspensão do ICMS) separados dos alertas internos (150 dias, configurável).
- Os mesmos códigos e posições na figura e na tabela (R10 e não "R010", O01 e não "001").

**Qualquer framework**
- Termos e números iguais aos das outras seções (personas, requisitos, matriz).
- Um item que aparece só ali (uma "API" que nenhuma outra seção cita) precisa de contexto ou de entrar nos requisitos.

## Fontes e figuras

- O TAPI é citado como [6]. O kick-off é citado com a data (16/10/2026), porque vão acontecer outras reuniões com o parceiro.
- Referência nova vai no formato da seção 5.
- Caminho de imagem `../assets/...` com barra normal, porque a barra invertida quebra fora do Windows. Figuras numeradas em sequência no documento, sem duas "Figura 1".
- A descrição do MR bate com os commits. MR que foi consolidado (por exemplo, "dois canvases" que viraram um) costuma ficar com a descrição antiga.
