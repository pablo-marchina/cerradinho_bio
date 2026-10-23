---
name: mr-review
description: Revisa merge requests (MRs) do GitLab do projeto GuaráTracking (graduacao/2026-2b/t24/g01 em git.inteli.edu.br). Confere o MR contra as regras do grupo (branch, título e commits da seção 7 do AGENT.md, issue certa, revisor, conflitos, card na coluna certa, report usado como evidência, IDs de RF/RN/CT inexistentes, figura sem legenda, seção apagada), revisa o conteúdo contra os critérios do card e a avaliação (exigência do Adalove, template do drive, limite de tamanho da seção, lógica do framework, clareza para quem lê) e redige o parecer no formato do grupo. Publica ou edita o comentário só depois do OK de quem revisa e nunca aprova nem faz merge. Use sempre que alguém pedir para revisar, ver, conferir, comentar ou dar parecer num MR (inclusive só com o link ou "!20"), fazer a segunda rodada de revisão ("revisão atualizada"), reescrever comentários já postados ou checar se um MR está pronto para merge.
---

# Revisão de MR (GitLab)

Projeto `graduacao/2026-2b/t24/g01` em `https://git.inteli.edu.br`. Os MRs vão para a `develop`; só a branch de snapshot (skill `repo-snapshot`) vai para a `main`.

## Requisitos

- Python 3, só biblioteca padrão.
- A skill `issue-manager` na mesma pasta `skills/skills/`: o script usa a classe `Api` dela.
- `GITLAB_TOKEN` como variável de ambiente de usuário do Windows (veja a `issue-manager`). Nunca imprima nem cole o token.
- Rode de dentro desta pasta: `python scripts/check_mr.py ...`.

## Fluxo

1. **Leia o card.** Abra a issue do MR (`python ../issue-manager/scripts/audit_cards.py audit --iid N`) e anote o que ela pede: descrição, DoD e critérios de aceitação. A revisão é contra o card e a avaliação, não contra o que você acha que a seção deveria ter. Se a revisão é de uma seção da documentação, peça a quem pediu a revisão o texto do Adalove da seção, caso ele não tenha vindo no pedido.
2. **Rode o script** (só leitura):
   `python scripts/check_mr.py check --iid M` (ou `--json`). Ele lista ERRO e AVISO e sai com código 1 se houver ERRO.
3. **Leia o diff inteiro, a descrição e todos os comentários já feitos.** Para documentação, leia a seção como fica na branch e como está na `develop`. O script aponta, mas não entende o texto: confirme cada ERRO e AVISO antes de repetir no parecer, porque as heurísticas erram. Comentário que já foi feito não se repete.
4. **Revise o conteúdo** com o checklist do tipo de entrega em [references/checklist.md](references/checklist.md). Para seção da documentação, siga também [references/conteudo-documentacao.md](references/conteudo-documentacao.md). Se o card, o Adalove ou o `AGENT.md` definirem limite de tamanho para a seção, conte as palavras:
   `python scripts/count_words.py ../../../document/documentacao.md --section 3.1.4 --ref <source_branch> --limit <N>`
5. **Quando o MR afirmar fatos sobre o parceiro, confira na fonte antes de comentar.** Se o texto diz quem faz cada etapa, valores, prazos ou sistemas, confira nos relatórios do kick-off, no TAPI ou nas personas (3.2.1) antes de apontar erro ou acerto. MR de código, de formatação ou de processo normalmente não precisa disso. Se você não achar a informação, escreva como pergunta e não como erro. Na revisão do MR !20, uma tarefa foi atribuída à logística por intuição, mas o kick-off dizia que ela era do PCM, e o comentário teve de ser corrigido.
6. **Redija o parecer** no formato de [references/formato-comentario.md](references/formato-comentario.md) e salve fora do repositório. Quando o problema for de forma ou de tamanho, inclua a seção reescrita como proposta, já com a nova contagem de palavras.
7. **Mostre o rascunho a quem pediu a revisão** e publique só com OK:
   - comentário geral: `python scripts/check_mr.py note --iid M --body-file parecer.md`
   - comentário numa linha: `python scripts/check_mr.py note --iid M --body-file c1.md --path document/documentacao.md --line 120` (linha do arquivo novo)
   - reescrever um comentário já postado: `python scripts/check_mr.py note --iid M --body-file parecer-v2.md --note-id 493338`
8. **Card:** com o MR aberto em revisão, o card fica em `Waiting Review`, com tempo gasto e estimativa. Se o script apontar outra coluna, avise o dono do card ou, com o OK dele, corrija pela `issue-manager`.

## Segunda rodada ("Revisão atualizada")

Leia os comentários anteriores (`GET /merge_requests/:iid/notes`) e os commits novos desde eles. Responda item por item: o que foi resolvido, o que continua pendente e se surgiu algo novo por causa da correção. Rode o `check` de novo, porque um merge da `develop` no meio pode ter desfeito trabalho de outra pessoa.

## Regras

1. **Nunca aprove, faça merge nem feche o MR.** Isso é de quem revisa, no GitLab.
2. **Escrita só com OK explícito**, um comentário por vez. `check` só lê. O modo automático do Claude Code pode bloquear a escrita na API; nesse caso, peça a aprovação no app em vez de procurar outro caminho.
3. **Não commite na branch do autor.** A correção vai como sugestão, com o texto pronto para ele aplicar. Quem revisa e também reescreve a seção vira autor e revisor do mesmo trabalho.
4. **Para mudar um comentário, edite a nota** (`--note-id`) em vez de postar outra, para o MR não ficar com duas versões da mesma crítica.
5. **Português do Brasil, sem coautoria nem link de conversa de IA** (seção 7 do `AGENT.md`). O `note` recusa texto com isso.
6. **Separe o que bloqueia do que é sugestão.** Bloqueia o que fere critério do card, a avaliação, o `AGENT.md` (reports, dados restritos, Git) ou a coerência com o resto da `develop`. Estilo e preferência são sugestão.
7. **Não invente dados do parceiro** nem peça para o autor inventar. Dado sem fonte vira hipótese marcada ou pendente de validação.
8. **Escopo:** commit ou arquivo de outra issue no MR é apontamento. Peça para separar ou para integrar antes o MR da outra issue.
9. **Diga onde e por quê.** Cada apontamento leva a seção ou o arquivo e a linha, por que é um problema e o que fazer.
10. **Arquivo de apoio grande vai como anexo**, não colado no comentário. Se ele for a fonte de uma figura (por exemplo, o JSON do Framekit), sugira que o autor commite o arquivo em `assets/` junto com a imagem.
