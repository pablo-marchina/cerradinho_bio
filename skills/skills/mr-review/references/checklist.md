# Checklist de revisão de MR

Use junto com a saída do `check_mr.py`. Os itens vêm dos problemas que mais se repetiram nos MRs !1 a !24 da Sprint 1.

## Todo MR

- [ ] O MR resolve a issue certa e só ela. Commits de outra issue, ou arquivos fora do escopo do card, saem do MR ou esperam o MR da outra issue entrar antes.
- [ ] Cada critério de aceitação e cada item do DoD do card estão atendidos. Liste os que faltam.
- [ ] Nada que já estava na `develop` foi apagado ou revertido sem motivo. Isso costuma acontecer quando a branch recebe um merge da `develop` e o conflito é resolvido às pressas.
- [ ] Branch `tipo/NN-descricao-sprintN`, título e commits `tipo(#NN): descrição`, autor com e-mail `@sou.inteli.edu.br` (seção 7 do `AGENT.md`). Isso é aviso, não bloqueia sozinho, mas registre no parecer.
- [ ] A descrição do MR diz o que muda, por quê e como conferir.
- [ ] Revisor do MR é o mesmo do card e é outra pessoa que não o autor.
- [ ] Sem dados restritos da Cerradinho: nomes reais de fornecedores, NFs, rotas, localizações, cadastros (seção 3 do `AGENT.md`).
- [ ] Sem coautoria nem link de conversa de IA em commit, descrição ou arquivo.

## Documentação (`document/documentacao.md`)

- [ ] Atende à avaliação da seção (rubrica do Adalove ou orientação da professora) e usa o template oficial quando ele for pedido. Diga no texto qual template foi usado.
- [ ] Nenhum report de `document/outros/reports/` usado como justificativa ou evidência (seção 4 do `AGENT.md`). A fonte real (reunião com data, TAPI, artigo, norma, teste) está escrita no próprio texto.
- [ ] Toda figura tem legenda `*Figura — ...*` e `Fonte: ...` logo abaixo. O texto alternativo descreve o que a imagem mostra.
- [ ] Referências citadas no padrão do grupo (citação numérica ou autor-data), com as URLs na lista de referências e não soltas no corpo.
- [ ] Dado sem fonte aparece como hipótese ou "pendente de validação". Medido e estimado estão separados.
- [ ] IDs citados (RF, RN, CT, personas P1 a P3, cenários) existem na `develop` ou são criados neste MR. Se dependem de outro MR, ele entra antes.
- [ ] Texto e imagem dizem a mesma coisa (níveis, nomes, números).
- [ ] O texto apresenta o que a seção mostra e como ler antes de entrar nos detalhes. Siglas e termos técnicos são explicados na primeira vez.
- [ ] Coerência com as seções vizinhas: personas, requisitos e regras de negócio batem com o que já está na `develop`.

## Código, firmware e protótipo

- [ ] Compila e roda como a descrição do MR diz. Rode, se puder, e diga no parecer o que rodou.
- [ ] Casos de borda do domínio: estouro de `millis()` (cerca de 49,7 dias) em contagem de prazos longos, reinício sem perder fila e ciclo, estado inicial do lacre, sem conexão.
- [ ] Os comportamentos batem com os RFs e RNs citados.
- [ ] Testes ou casos de teste (CTs) atualizados, com as evidências pedidas no card (capturas do Wokwi em `assets/`, saída serial).
- [ ] Sem credencial, token ou senha de Wi-Fi no código.
