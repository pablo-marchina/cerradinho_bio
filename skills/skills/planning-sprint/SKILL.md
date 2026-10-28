---
name: planning-sprint
description: Monta a planning de uma sprint de projeto em grupo como uma página HTML no estilo "runbook" — linha do tempo por dia útil, um build e no máximo uma revisão por pessoa por dia, duplas fixas de revisores por artefato, tabela de cobertura da rubrica, rascunhos de partida para os artefatos mais difíceis e carga por pessoa. Use sempre que alguém pedir para planejar, dividir ou cronogramar uma sprint, artefatos, entregas ou tarefas entre integrantes de um grupo ("planning", "runbook", "cronograma da sprint", "divisão de tarefas", "quem faz o quê"), ou para incorporar o plano de um colega numa planning existente, mesmo que a pessoa não fale em HTML nem em runbook.
---

# Planning de sprint no formato runbook

O resultado é uma página HTML de coluna única que responde, para cada integrante e cada dia, três perguntas: o que eu construo hoje, o que eu reviso hoje e onde isso conta na nota. Funciona porque:

- cada entrega é revisada por outra pessoa no dia seguinte, então nada chega ao prazo sem segunda leitura;
- as tarefas são específicas o bastante (arquivos, nomes, critério de pronto) para ninguém precisar perguntar "o que exatamente é para fazer?";
- a rubrica aparece mapeada para dono e dia, então dá para ver de cara se algum critério ficou sem responsável.

Siga as etapas na ordem. O CSS e os componentes HTML estão nos apêndices; copie-os em vez de reinventar o visual.

## 1. Juntar as entradas

Antes de planejar, tenha em mãos:

| Entrada | Onde costuma estar |
|---|---|
| Enunciado e rubrica de cada artefato da sprint | Colado pelo usuário, Adalove, PDF |
| Documento do parceiro (TAPI, briefing) | PDF anexado |
| Template da documentação do repositório | `document/documentacao.md` ou similar |
| Integrantes e quem é autor de cada artefato | O usuário informa |
| Dupla de revisores de cada artefato | O usuário informa ou pede para você sugerir |
| Prazo real e data do merge na `main` | Perguntar |
| Se fim de semana conta como dia de trabalho | Perguntar |
| Plano de algum colega para incorporar | Arquivo anexado, se houver |

Descubra sozinho o que dá para descobrir: data de hoje, dia da semana, data de início do módulo (`git log --format='%ad %s' --date=format:'%a %Y-%m-%d'`), branches existentes, estrutura de pastas. Pergunte só o que não está em lugar nenhum, numa pergunta só, com opções e uma recomendada. Prazo e fim de semana mudam o plano inteiro, então valem a pergunta.

Se pedirem para você sugerir revisores, siga a seção 4.

## 2. Ler tudo antes de planejar

- **Rubrica, linha por linha.** Cada critério vira uma linha na tabela de cobertura (seção 7). Anote pesos: eles decidem quem pega mais seções.
- **Documento do parceiro.** Extraia o MVP, as restrições ("não contempla"), o que é conteúdo restrito e os números de referência (prazos, limiares). Contradições entre o parceiro e a rubrica do curso (por exemplo, o curso exige Wi‑Fi e o parceiro diz para não contar com Wi‑Fi) viram um item em "Assumido antes do Dia 1", com a forma de conciliar escrita.
- **Template do repositório.** Use a numeração de seções e os formatos de ID do template (por exemplo `RF001`, `RN01`, `CT01`), não formatos inventados. O texto-guia em itálico do template sai quando a seção é preenchida.
- **Plano de colega, se houver.** Leia inteiro antes de começar; a seção 8 diz como incorporar.

## 3. Regras do cronograma

Escreva estas regras no painel "Regras que este plano segue", com o porquê de cada uma, porque é o que impede o plano de virar caos no meio da semana:

1. **Cada tarefa tem um dono e um revisor, nunca a mesma pessoa.** Os revisores de um artefato saem sempre da dupla fixa daquele artefato, para que conheçam o contexto.
2. **Um build e no máximo uma revisão por pessoa por dia.** O Dia 1 é só build (não há nada para revisar). O último dia é só revisão e merge.
3. **Nenhuma dependência atravessa o dia.** O que se revisa hoje foi construído ontem, e cada build usa só o que existe no fim do dia anterior. Exceções inevitáveis (mapa de pinos combinado entre duas pessoas, numeração de IDs) são combinadas na primeira hora do Dia 1 e escritas na nota do dia.
4. **O tempo segue a tarefa.** Estimativas dizem quanto o trabalho custa, não uma meta. Se o grupo tiver um teto de horas, registre a decisão.
5. **Uma branch por tarefa, MR com o revisor marcado.** Nome da branch e dos commits conforme a seção 7 do `AGENT.md` (`tipo/NN-descricao`, `tipo(#NN): descrição`). O MR vai para a branch de integração do grupo (a `dev`, se existir; senão a `main`). Ninguém faz commit direto na `dev` nem na `main`.
6. **Arquivo compartilhado (ex.: `documentacao.md`): cada um edita só a sua seção**, MRs pequenos, rebase antes do merge.
7. **Evidência só do que rodou.** Print e "passou" só depois de executar; falha fica registrada como falha; teste não executado aparece como pendente.
8. **Fato com fonte, hipótese com rótulo.** Dado público leva URL e data de consulta. O que depende do parceiro e não chegou aparece como `PENDENTE DE VALIDAÇÃO`, nunca inventado.
9. **Nada restrito no repositório.** Se o parceiro classifica dados como restritos, só dados fictícios.

## 4. Montar a grade de revisões

Primeiro liste os builds de cada pessoa em cada dia. Depois, para cada dia `d` a partir do 2, atribua um revisor a cada build do dia `d−1`, respeitando:

- o revisor vem da dupla do artefato daquele build;
- cada pessoa revisa no máximo uma coisa por dia;
- ninguém revisa o próprio trabalho.

Com N pessoas e N builds por dia, cada dia vira uma permutação: todo mundo revisa exatamente uma coisa. Monte a tabela autor → revisor dia a dia e confira as três condições antes de escrever as tarefas.

**Equilíbrio, quando você escolhe os revisores:**

- Conte as vagas: artefatos × 2 revisores. Se houver mais vagas que pessoas, alguém revisa dois artefatos; dê isso a quem tem o build mais leve.
- Quem é autor de dois artefatos revisa o mínimo possível.
- Prefira revisores que precisam do conteúdo depois: quem faz personas revisa negócios; quem faz o protótipo revisa requisitos (o RF precisa casar com o caso de teste). Escreva esse motivo na tabela "Quem revisa o quê".
- Respeite as duplas que o grupo já definiu, mesmo que você faria diferente.

## 5. Ordenar por dependência

Para cada artefato, pergunte "do que este depende?" e coloque o consumidor pelo menos um dia depois do produtor. Exemplos que se repetem em projetos de IoT/negócios:

- Matriz de rastreabilidade (RTM) depende de RF, RN, personas e casos de teste: vai no penúltimo ou antepenúltimo dia de build.
- BOM depende da lista de componentes do protótipo.
- Value Proposition Canvas depende das personas e da descrição da solução.
- Política de privacidade (LGPD) depende do rascunho de requisitos (que dados existem).
- SWOT depende do contexto da indústria.
- Documentar casos de teste depende de tê-los executado; corrigir falhas vem no dia seguinte à execução, e a regressão completa no último dia de build.

Correções de revisão entram no próximo build do dono, citando a origem ("aplicar a revisão do Dia 3"). Se a revisão acontece no mesmo dia do build que a usaria, a correção vai para o dia seguinte; confira isso tarefa por tarefa, porque é o erro mais comum.

O último dia de build é de consolidação (seção final, regressão, README, remoção do texto-guia). O último dia é só revisão e merge; o prazo real, se for depois, fica como contingência.

Quando um artefato tem dois autores e um deles também está em outro artefato, divida as seções pelo peso na rubrica e pela carga, e diga a divisão no painel "Assumido".

## 6. Escrever as tarefas

Cada linha de pessoa num dia tem, nesta ordem, a revisão e depois o build.

**Build:** título curto, minutos, e uma instrução que diga:

- os arquivos e caminhos exatos (`assets/3-desenvolvimento-resultados/3.1.2-swot/swot.png`, `src/firmware/prototipo_wokwi/`);
- o conteúdo concreto, com os nomes que o grupo vai usar (classes, constantes, IDs, eventos);
- o critério de pronto ("compila no Wokwi e já acende um LED ao apertar um botão");
- as armadilhas que a rubrica ou o parceiro apontam ("o setor é o do parceiro, não IoT"; "não invente o contato do DPO").

**Revisão:** uma ou duas frases sobre o que conferir de forma verificável: rodar o código, recalcular uma linha da planilha, abrir o template oficial, clicar os links. Revisão que diz só "revisar o texto" não serve.

**Rascunhos de partida.** Para os artefatos mais difíceis (requisitos, casos de teste), inclua uma tabela de rascunho na página e diga que é ponto de partida para o dono refinar. Mapeie cada item da rubrica para pelo menos uma linha do rascunho. Isso economiza o dia mais caro do artefato.

**Referência comum.** Tudo que mais de uma tarefa usa vai num bloco único (caminhos, formatos de ID, pinos, constantes, tipos de evento, formato do payload, links diretos dos templates, vocabulário, contatos do parceiro). As tarefas citam esses nomes em vez de redefini-los.

**Minutos e carga.** Some build + revisão por pessoa e dia. Na tabela de carga, mostre horas com uma casa decimal e calcule o total a partir dos minutos, não da soma das horas arredondadas.

**Gates.** No fim de alguns dias (por exemplo o 2º, o penúltimo de build e o último de build), escreva na nota do dia o que precisa existir. O último gate de build diz o que congela se algo falhar.

## 7. Estrutura da página

Nesta ordem. Cada seção existe porque responde a uma pergunta que alguém do grupo vai fazer:

1. **Cabeçalho:** eyebrow (sprint · artefatos · grupo · parceiro), um título-tese em uma frase, um parágrafo dizendo quem faz o quê e onde as frentes se encontram, e a faixa de fatos (Dia 1, merge, prazo, dias úteis, mais um número característico do projeto).
2. **Regras que este plano segue** + fichas dos grupos (autores e revisores, uma cor por artefato).
3. **Quem revisa o quê, e por quê:** tabela artefato · seções · autores · revisores · motivo.
4. **Assumido antes do Dia 1** (painel de alerta): decisões que você tomou e que mudam o trabalho.
5. **A confirmar com o parceiro ou a orientadora** (alerta): pendências externas, com o que fazer se não chegarem.
6. **Onde cada critério avaliado é atendido:** rubrica → dono, dia e revisor. Para rubricas por nível (Insuficiente → Avançado), uma linha por exigência do nível mais alto.
7. **Do documento do parceiro para esta sprint:** cada item do MVP com status (`simulado`, `requisito`, `sprint N`, `respeitado`).
8. **Plano do colega: o que entra** (só se houver; seção 8).
9. **Checklist de toda revisão.**
10. **Rascunhos de partida** (tabelas de RF, casos de teste etc.).
11. **Referência comum.**
12. **Linha do tempo:** um bloco por dia útil, com nota/gate, as pessoas e as tags do dia; fins de semana como linha "off"; dia de contingência como linha vermelha.
13. **Carga por pessoa, em horas.**
14. **Se faltar tempo: o que corta primeiro** (P0 não se corta, P1 qualidade, P2 extras) e o que fazer se alguém faltar.
15. **Fora desta sprint, de propósito.**
16. **Rodapé:** grupo, turma, parceiro, data em que foi escrito.

## 8. Incorporar o plano de um colega

O grupo já decidiu algumas coisas (autores, duplas, prazo); essas decisões ganham do plano do colega. De resto, avalie cada ideia pelo retorno:

- **Entra** o que melhora a nota ou evita retrabalho com pouco custo: links de templates, regras de integridade, critérios de pronto mais precisos, casos de teste que faltavam, rótulos de pendência.
- **Fica fora** o que conflita com decisões do grupo, duplica o que já existe ou cria trabalho sem ganho na rubrica (scripts de montagem, rodízios que desfazem as duplas, formatos de ID diferentes do template).
- **A decidir:** restrições que mudam o plano inteiro (por exemplo um teto de horas por dia). Não ajuste em silêncio; pergunte e registre a resposta com data.

Mostre o resultado numa tabela "o que entra" com status (`in`, `parcial`, `fora`, `a decidir`) e o motivo de cada linha, e aplique as ideias aceitas nas tarefas e painéis correspondentes. Não reescreva o plano do zero.

## 9. Conferir antes de entregar

- [ ] Datas e dias da semana corretos; nenhum dia de trabalho num fim de semana excluído.
- [ ] Ninguém tem mais de um build e uma revisão no mesmo dia; ninguém revisa o próprio trabalho.
- [ ] Todo build de `d−1` tem revisão em `d`, feita por alguém da dupla certa.
- [ ] Nenhuma tarefa usa algo construído ou revisado no mesmo dia.
- [ ] Os dias e nomes da tabela de cobertura batem com a linha do tempo.
- [ ] Todo critério da rubrica tem dono.
- [ ] Minutos de cada linha batem com o total da pessoa e com a tabela de carga.
- [ ] Nenhum dado real restrito, nenhum fato do parceiro inventado.
- [ ] O script de "Hoje" tem `data-date` no formato `AAAA-MM-DD` em cada dia.

## 10. Entregar

- Escreva a página em português, frases curtas, voz ativa, sem jargão desnecessário.
- Se houver ferramenta de publicação de página (Artifact), publique e entregue o link; avise que a página é privada até ser compartilhada. Atualizações vão para o mesmo link.
- Sem essa ferramenta, salve um `.html` autônomo: comece com `<!doctype html><html lang="pt-BR"><meta charset="utf-8">` antes do `<title>`.
- No resumo para o usuário: o que tem na página, as decisões que você tomou sozinho (para ele poder mudar) e as pendências externas.

---

## Apêndice A: CSS

Copie o bloco inteiro para o topo da página. Uma cor por artefato: `warm` (negócios), `dash` (UX), `accent` (protótipo), `plum` (requisitos). Renomeie as classes de grupo se os artefatos forem outros, mas mantenha os tokens.

```html
<title>Planning Sprint N Parceiro</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="stylesheet" href="https://fonts.googleapis.com/css2?family=Newsreader:ital,opsz,wght@0,6..72,500;0,6..72,600;1,6..72,500&family=IBM+Plex+Sans:wght@400;500;600&family=IBM+Plex+Mono:wght@400;500;600&display=swap">
<style>
  /* Coluna única, linha do tempo por dia, uma cor por artefato. */
  :root{
    --paper:#F1F3F0; --paper-raised:#FFFFFF; --ink:#182420; --ink-soft:#4B5A54;
    --line:#CBD4CD; --accent:#1F5C52; --accent-soft:#DCE8E3; --warm:#A9691F;
    --warm-soft:#F1E2CC; --dash:#3F5A8A; --dash-soft:#DEE5F1; --plum:#6B4A86; --plum-soft:#EAE1F1;
    --critical:#9B3A34; --critical-soft:#F3DCD8;
    --mono-tint:#E7ECE8; --shadow:0 1px 0 rgba(24,36,32,0.06);
  }
  @media (prefers-color-scheme: dark){
    :root:not([data-theme="light"]){
      color-scheme:dark;
      --paper:#121815; --paper-raised:#182420; --ink:#E7EDE9; --ink-soft:#9FAFA7;
      --line:#2B3B34; --accent:#6BC1AC; --accent-soft:#1D332C; --warm:#E3A458;
      --warm-soft:#3A2E1B; --dash:#93AEDD; --dash-soft:#1E2838; --plum:#C2A3DE; --plum-soft:#2C2236;
      --critical:#E08B82; --critical-soft:#3A211E;
      --mono-tint:#1B2622; --shadow:0 1px 0 rgba(0,0,0,0.3);
    }
  }
  :root[data-theme="dark"]{
    color-scheme:dark;
    --paper:#121815; --paper-raised:#182420; --ink:#E7EDE9; --ink-soft:#9FAFA7;
    --line:#2B3B34; --accent:#6BC1AC; --accent-soft:#1D332C; --warm:#E3A458;
    --warm-soft:#3A2E1B; --dash:#93AEDD; --dash-soft:#1E2838; --plum:#C2A3DE; --plum-soft:#2C2236;
    --critical:#E08B82; --critical-soft:#3A211E;
    --mono-tint:#1B2622; --shadow:0 1px 0 rgba(0,0,0,0.3);
  }
  *{ box-sizing:border-box; }
  @media (prefers-reduced-motion: reduce){ *{ animation-duration:0.001ms !important; transition-duration:0.001ms !important; } }
  body{ background:var(--paper); color:var(--ink); font-family:"IBM Plex Sans","Helvetica Neue",Arial,sans-serif; font-size:16px; line-height:1.5; margin:0; padding-block:3.5rem 6rem; padding-inline:1.25rem; }
  .page{ max-width:900px; margin:0 auto; display:flex; flex-direction:column; gap:3rem; }
  code{ font-family:"IBM Plex Mono",monospace; }

  .masthead{ display:flex; flex-direction:column; gap:0.9rem; }
  .eyebrow{ font-family:"IBM Plex Mono",monospace; font-size:0.72rem; letter-spacing:0.14em; text-transform:uppercase; color:var(--accent); font-weight:600; }
  h1{ font-family:"Newsreader",Georgia,serif; font-weight:600; font-size:clamp(2rem,4.6vw,2.7rem); line-height:1.1; margin:0; text-wrap:balance; }
  .dek{ max-width:68ch; color:var(--ink-soft); font-size:1.02rem; margin:0; }
  .facts{ display:grid; grid-template-columns:repeat(auto-fit,minmax(150px,1fr)); gap:1px; background:var(--line); border:1px solid var(--line); border-radius:10px; overflow:hidden; margin-top:0.4rem; }
  .fact{ background:var(--paper-raised); padding:0.9rem 1rem; display:flex; flex-direction:column; gap:0.25rem; }
  .fact .k{ font-family:"IBM Plex Mono",monospace; font-size:0.66rem; letter-spacing:0.1em; text-transform:uppercase; color:var(--ink-soft); }
  .fact .v{ font-family:"IBM Plex Mono",monospace; font-weight:600; font-size:0.95rem; }
  .fact.merge .v{ color:var(--accent); }
  .fact.deadline .v{ color:var(--critical); }

  .panel{ background:var(--paper-raised); border:1px solid var(--line); border-radius:10px; padding:1.25rem 1.4rem; display:flex; flex-direction:column; gap:0.75rem; box-shadow:var(--shadow); }
  .panel h2{ font-family:"IBM Plex Mono",monospace; font-size:0.72rem; letter-spacing:0.12em; text-transform:uppercase; color:var(--ink-soft); margin:0; }
  .panel p{ margin:0; font-size:0.92rem; max-width:72ch; }
  .panel ul, .panel ol{ margin:0.2rem 0 0; padding-left:1.2rem; font-size:0.92rem; }
  .panel li{ margin-bottom:0.4rem; }
  .panel code, .notes code, .instr code, td code{ background:var(--mono-tint); padding:0 0.3rem; border-radius:3px; font-size:0.9em; overflow-wrap:anywhere; }
  .panel.alert{ border-color:var(--critical); }
  .panel.alert h2{ color:var(--critical); }

  .team{ display:flex; gap:1.6rem; flex-wrap:wrap; margin-top:0.2rem; }
  .grp{ display:flex; flex-direction:column; gap:0.35rem; }
  .grp-label{ font-family:"IBM Plex Mono",monospace; font-size:0.66rem; letter-spacing:0.1em; text-transform:uppercase; }
  .grp.neg .grp-label{ color:var(--warm); }
  .grp.ux .grp-label{ color:var(--dash); }
  .grp.proto .grp-label{ color:var(--accent); }
  .grp.req .grp-label{ color:var(--plum); }
  .grp .names{ display:flex; flex-wrap:wrap; gap:0.35rem; }
  .grp .names span{ font-family:"IBM Plex Mono",monospace; font-size:0.78rem; border-radius:999px; padding:0.14rem 0.6rem; border:1px solid var(--line); }
  .grp.neg .names span{ background:var(--warm-soft); }
  .grp.ux .names span{ background:var(--dash-soft); }
  .grp.proto .names span{ background:var(--accent-soft); }
  .grp.req .names span{ background:var(--plum-soft); }
  .grp .names span.rev{ background:transparent; color:var(--ink-soft); }

  .section-h{ font-family:"Newsreader",Georgia,serif; font-size:1.5rem; font-weight:600; margin:0; text-wrap:balance; }
  .section{ display:flex; flex-direction:column; gap:1rem; }
  .section > p{ margin:0; color:var(--ink-soft); max-width:70ch; font-size:0.95rem; }

  .notes{ background:var(--paper-raised); border:1px solid var(--line); border-radius:10px; padding:1.25rem 1.4rem; display:flex; flex-direction:column; gap:0.6rem; box-shadow:var(--shadow); }
  .notes h2{ font-family:"IBM Plex Mono",monospace; font-size:0.72rem; letter-spacing:0.12em; text-transform:uppercase; color:var(--ink-soft); margin:0; }
  .notes dl{ margin:0; display:grid; grid-template-columns:minmax(0,11rem) minmax(0,1fr); gap:0.45rem 0.9rem; font-size:0.88rem; }
  .notes dt{ font-family:"IBM Plex Mono",monospace; color:var(--ink-soft); font-size:0.8rem; }
  .notes dd{ margin:0; min-width:0; }
  .notes a{ color:var(--dash); text-underline-offset:2px; }
  .notes a:focus-visible{ outline:2px solid var(--dash); outline-offset:2px; border-radius:2px; }
  @media (max-width:560px){ .notes dl{ grid-template-columns:1fr; } .notes dt{ margin-top:0.4rem; } }

  .tbl-wrap{ overflow-x:auto; border:1px solid var(--line); border-radius:10px; background:var(--paper-raised); }
  table.map{ border-collapse:collapse; width:100%; min-width:620px; font-size:0.86rem; }
  table.map th, table.map td{ text-align:left; vertical-align:top; padding:0.55rem 0.75rem; border-bottom:1px solid var(--line); }
  table.map th{ font-family:"IBM Plex Mono",monospace; font-size:0.68rem; letter-spacing:0.08em; text-transform:uppercase; color:var(--ink-soft); font-weight:500; background:var(--mono-tint); }
  table.map tr:last-child td{ border-bottom:none; }
  table.map td.grp-cell{ font-family:"IBM Plex Mono",monospace; font-size:0.74rem; color:var(--accent); white-space:nowrap; }
  table.map td.grp-cell.neg{ color:var(--warm); }
  table.map td.grp-cell.ux{ color:var(--dash); }
  table.map td.grp-cell.req{ color:var(--plum); }
  table.map td.id{ font-family:"IBM Plex Mono",monospace; font-size:0.78rem; white-space:nowrap; font-weight:600; }
  .who{ font-family:"IBM Plex Mono",monospace; font-size:0.76rem; color:var(--ink-soft); white-space:nowrap; }
  .pill{ font-family:"IBM Plex Mono",monospace; font-size:0.66rem; letter-spacing:0.05em; text-transform:uppercase; padding:0.05rem 0.4rem; border-radius:4px; white-space:nowrap; }
  .pill.in{ background:var(--accent-soft); color:var(--accent); }
  .pill.part{ background:var(--warm-soft); color:var(--warm); }
  .pill.out{ background:var(--critical-soft); color:var(--critical); }
  .pill.later{ background:var(--dash-soft); color:var(--dash); }

  .timeline{ list-style:none; margin:0; padding:0; display:flex; flex-direction:column; }
  .timeline > li{ position:relative; }
  .rail{ position:relative; padding-left:1.6rem; }
  .rail::before{ content:""; position:absolute; left:0.4rem; top:0; bottom:0; width:1px; background:var(--line); }
  .off{ display:flex; align-items:center; flex-wrap:wrap; gap:0.75rem; padding:0.9rem 0 0.9rem 1.6rem; color:var(--ink-soft); font-family:"IBM Plex Mono",monospace; font-size:0.78rem; letter-spacing:0.04em; }
  .off::before{ content:""; position:absolute; left:0.28rem; width:7px; height:7px; border-radius:50%; background:var(--paper); border:1px solid var(--line); }
  .off .date{ min-width:6.2rem; }
  .off .tag{ text-transform:uppercase; }
  .buffer{ color:var(--critical); }
  .buffer::before{ border-color:var(--critical); background:var(--critical-soft); }

  .day{ padding:1.3rem 0 1.5rem 1.6rem; display:flex; flex-direction:column; gap:0.9rem; }
  .day::before{ content:""; position:absolute; left:0.16rem; top:1.55rem; width:13px; height:13px; border-radius:50%; background:var(--accent-soft); border:2px solid var(--accent); }
  .day.merge::before{ background:var(--warm-soft); border-color:var(--warm); }
  .day.today{ background:var(--paper-raised); border-radius:12px; box-shadow:var(--shadow); margin-left:-1rem; padding-left:2.6rem; padding-right:1.4rem; border:1px solid var(--accent-soft); }
  .day.today::before{ left:1.16rem; }
  .day-head{ display:flex; align-items:baseline; flex-wrap:wrap; gap:0.6rem 0.9rem; }
  .day-idx{ font-family:"IBM Plex Mono",monospace; font-size:0.74rem; letter-spacing:0.08em; color:var(--accent); font-weight:600; }
  .day-date{ font-family:"IBM Plex Mono",monospace; font-size:0.85rem; color:var(--ink-soft); }
  .today-pill{ font-family:"IBM Plex Mono",monospace; font-size:0.66rem; letter-spacing:0.1em; text-transform:uppercase; background:var(--accent); color:var(--paper-raised); padding:0.15rem 0.5rem; border-radius:999px; display:none; }
  .day.today .today-pill{ display:inline-block; }
  .day h3{ font-family:"Newsreader",Georgia,serif; font-size:1.4rem; font-weight:600; margin:0; text-wrap:balance; }
  .day-note{ font-size:0.86rem; color:var(--ink-soft); margin:0; max-width:72ch; }

  .people{ display:grid; grid-template-columns:1fr; gap:0.6rem; }
  @media (min-width:760px){ .people{ grid-template-columns:1fr 1fr; } }
  .prow{ border-radius:8px; padding:0.6rem 0.75rem; min-width:0; }
  .prow.n{ background:var(--warm-soft); }
  .prow.u{ background:var(--dash-soft); }
  .prow.t{ background:var(--accent-soft); }
  .prow.r{ background:var(--plum-soft); }
  .prow-head{ display:flex; align-items:baseline; gap:0.4rem; font-family:"IBM Plex Mono",monospace; font-size:0.78rem; }
  .prow-name{ font-weight:600; color:var(--ink); }
  .prow-total{ margin-left:auto; color:var(--ink-soft); }
  .prow ul{ list-style:none; margin:0.35rem 0 0; padding:0; display:flex; flex-direction:column; gap:0.55rem; }
  .prow li{ font-size:0.86rem; display:flex; flex-wrap:wrap; gap:0.3rem; align-items:baseline; }
  .kind{ font-family:"IBM Plex Mono",monospace; font-size:0.62rem; letter-spacing:0.05em; text-transform:uppercase; padding:0.01rem 0.35rem; border-radius:4px; flex-shrink:0; background:var(--paper-raised); }
  .kind.rv{ color:var(--ink-soft); }
  .item-min{ font-family:"IBM Plex Mono",monospace; font-size:0.78rem; color:var(--ink-soft); margin-left:auto; white-space:nowrap; }
  .instr{ flex-basis:100%; color:var(--ink-soft); font-size:0.82rem; line-height:1.5; margin-top:0.1rem; }
  .instr code{ background:var(--paper-raised); }
  .tags{ display:flex; flex-wrap:wrap; gap:0.4rem; }
  .tag{ font-family:"IBM Plex Mono",monospace; font-size:0.68rem; color:var(--ink-soft); background:var(--mono-tint); border:1px solid var(--line); padding:0.1rem 0.45rem; border-radius:6px; }

  table.totals{ border-collapse:collapse; font-family:"IBM Plex Mono",monospace; font-size:0.8rem; width:100%; min-width:560px; }
  table.totals th, table.totals td{ text-align:right; padding:0.4rem 0.55rem; border:1px solid var(--line); font-variant-numeric:tabular-nums; }
  table.totals th{ color:var(--ink-soft); font-weight:500; }
  table.totals th.person, table.totals td.person{ text-align:left; color:var(--ink); font-weight:600; }
  table.totals tr.n td.person{ color:var(--warm); }
  table.totals tr.u td.person{ color:var(--dash); }
  table.totals tr.t td.person{ color:var(--accent); }
  table.totals tr.r td.person{ color:var(--plum); }

  footer{ color:var(--ink-soft); font-size:0.82rem; text-align:center; }
</style>
```

## Apêndice B: Componentes HTML

Esqueleto com um exemplo de cada componente. Repita os blocos conforme a necessidade.

```html
<div class="page">

  <header class="masthead">
    <div class="eyebrow">Sprint 1 &middot; Artefatos 1, 2, 3 e 4 &middot; Grupo 01 &middot; Parceiro</div>
    <h1>Uma frase que resume a sprint.</h1>
    <p class="dek">Quem faz o quê, onde as frentes se encontram, quando é o merge.</p>
    <div class="facts">
      <div class="fact"><span class="k">Dia 1</span><span class="v">Qui 15/out</span></div>
      <div class="fact merge"><span class="k">Merge dev &rarr; main</span><span class="v">Qui 22/out</span></div>
      <div class="fact deadline"><span class="k">Prazo</span><span class="v">Sex 23/out</span></div>
    </div>
  </header>

  <section class="panel">
    <h2>Regras que este plano segue</h2>
    <ul><li><strong>Regra.</strong> Por quê.</li></ul>
    <div class="team">
      <div class="grp neg"><span class="grp-label">Art. 1 &middot; Negócios</span>
        <div class="names"><span>Autor A</span><span>Autor B</span><span class="rev">rev. C, D</span></div></div>
    </div>
  </section>

  <section class="panel alert">
    <h2>Assumido antes do Dia 1</h2>
    <ol><li><strong>Decisão.</strong> Motivo e consequência.</li></ol>
  </section>

  <section class="section">
    <h2 class="section-h">Onde cada critério avaliado é atendido</h2>
    <p>Uma frase de contexto.</p>
    <div class="tbl-wrap">
      <table class="map">
        <thead><tr><th>Artefato</th><th>Critério</th><th>Coberto por</th></tr></thead>
        <tbody>
          <tr><td class="grp-cell neg">Art. 1</td><td>Seção X (1,0)</td><td><span class="who">Autor A D1</span> &middot; rev. <span class="who">C D2</span></td></tr>
          <tr><td class="id">RF001</td><td>&hellip;</td><td><span class="pill in">in</span> <span class="pill part">parcial</span> <span class="pill out">fora</span> <span class="pill later">sprint 2</span></td></tr>
        </tbody>
      </table>
    </div>
  </section>

  <section class="notes">
    <h2>Referência comum: os nomes que todas as tarefas usam</h2>
    <dl>
      <dt>Documento</dt><dd><code>document/documentacao.md</code></dd>
    </dl>
  </section>

  <ol class="timeline" id="timeline">
    <li class="rail">
    <div class="day" data-date="2026-10-15">
      <div class="day-head"><span class="day-idx">DIA 1 / 6</span><span class="day-date">Qui 15/out</span><span class="today-pill">Hoje</span></div>
      <h3>Título do dia</h3>
      <p class="day-note">Nota ou gate do fim do dia.</p>
      <div class="people">
        <div class="prow n"><div class="prow-head"><span class="prow-name">Autor A</span><span class="prow-total">220 min</span></div>
          <ul><li><span class="kind rv">revisão</span>O que revisa<span class="item-min">40</span>
            <div class="instr">O que conferir, de forma verificável.</div></li>
            <li><span class="kind">build</span>O que constrói<span class="item-min">180</span>
            <div class="instr">Arquivos, conteúdo, critério de pronto, armadilhas.</div></li></ul></div>
      </div>
      <div class="tags"><span class="tag">Tema do dia</span></div>
    </div>
    </li>
    <li class="off"><span class="date">Sáb 17 &ndash; Dom 18</span><span class="tag">Fim de semana &middot; sem tarefas</span></li>
    <li class="rail">
    <div class="day merge" data-date="2026-10-22"><!-- último dia: só revisão e merge --></div>
    </li>
    <li class="off buffer" data-date="2026-10-23"><span class="date">Sex 23/out</span><span class="tag">Contingência &middot; prazo</span></li>
  </ol>

  <section class="section">
    <h2 class="section-h">Carga por pessoa, em horas</h2>
    <div class="tbl-wrap">
      <table class="totals">
        <thead><tr><th class="person">Pessoa</th><th>D1</th><th>D2</th><th>Total</th></tr></thead>
        <tbody><tr class="n"><td class="person">Autor A</td><td>3.0</td><td>4.7</td><td>7.7</td></tr></tbody>
      </table>
    </div>
  </section>

  <footer>Grupo &middot; Turma &middot; Parceiro &times; Inteli &middot; planning da Sprint N, escrito em &hellip;</footer>
</div>

<script>
  (function(){
    try{
      var d=new Date(), iso=d.getFullYear()+"-"+String(d.getMonth()+1).padStart(2,"0")+"-"+String(d.getDate()).padStart(2,"0");
      var el=document.querySelector('.day[data-date="'+iso+'"]');
      if(el){ el.classList.add("today"); }
    }catch(e){}
  })();
</script>
```

Classes das linhas de pessoa (`prow`) e da tabela de carga (`tr`): `n` negócios, `u` UX, `t` protótipo, `r` requisitos. Pinte a linha da pessoa pela cor do artefato do build daquele dia; quem está em dois artefatos muda de cor conforme o dia.
