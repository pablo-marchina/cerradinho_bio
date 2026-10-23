---
name: mr-review
description: Revisa merge requests (MRs) do GitLab do projeto GuaráTracking (graduacao/2026-2b/t24/g01 em git.inteli.edu.br). Confere o MR contra as regras do grupo (branch, título e commits da seção 7 do AGENT.md, issue certa, revisor, conflitos, card na coluna certa, report usado como evidência, IDs de RF/RN/CT inexistentes, figura sem legenda, seção apagada), revisa o conteúdo contra os critérios do card e a avaliação, e redige o parecer no formato do grupo. Publica o comentário só depois do OK de quem revisa e nunca aprova nem faz merge. Use sempre que alguém pedir para revisar, conferir, comentar ou dar parecer num MR, fazer a segunda rodada de revisão ("revisão atualizada"), ou checar se um MR está pronto para merge.
---

# Revisão de MR (GitLab)

Projeto `graduacao/2026-2b/t24/g01` em `https://git.inteli.edu.br`. Os MRs vão para a `develop`; só a branch de snapshot (skill `repo-snapshot`) vai para a `main`.

## Requisitos

- Python 3, só biblioteca padrão.
- A skill `issue-manager` na mesma pasta `skills/skills/`: o script usa a classe `Api` dela.
- `GITLAB_TOKEN` como variável de ambiente de usuário do Windows (veja a `issue-manager`). Nunca imprima nem cole o token.
- Rode de dentro desta pasta: `python scripts/check_mr.py ...`.

## Fluxo

1. **Leia o card.** Abra a issue do MR (`python ../issue-manager/scripts/audit_cards.py audit --iid N`) e anote o que ela pede: descrição, DoD e critérios de aceitação. A revisão é contra o card, não contra o que você acha que a seção deveria ter.
2. **Rode o script** (só leitura):
   `python scripts/check_mr.py check --iid M` (ou `--json`). Ele lista ERRO e AVISO. Sai com código 1 se houver ERRO.
3. **Leia o diff inteiro** e, para documentação, a seção como fica na branch e como está na `develop`. O script aponta, mas não entende o texto. Confirme cada ERRO e AVISO antes de repetir no parecer, porque as heurísticas erram.
4. **Revise o conteúdo** com o checklist do tipo de entrega em [references/checklist.md](references/checklist.md).
5. **Redija o parecer** no formato de [references/formato-comentario.md](references/formato-comentario.md). Salve fora do repositório.
6. **Mostre o rascunho a quem pediu a revisão** e publique só com OK:
   - comentário geral: `python scripts/check_mr.py note --iid M --body-file parecer.md`
   - comentário numa linha: `python scripts/check_mr.py note --iid M --body-file c1.md --path document/documentacao.md --line 120` (linha do arquivo novo)
7. **Card:** com MR aberto em revisão o card fica em `Waiting Review`, com tempo gasto e estimativa. Se o script apontar outra coluna, avise o dono do card ou, com OK dele, corrija pela `issue-manager`.

## Segunda rodada ("Revisão atualizada")

Leia os comentários anteriores (`GET /merge_requests/:iid/notes`) e os commits novos desde eles. Responda item por item: o que foi resolvido, o que continua pendente e se surgiu algo novo por causa da correção. Rode o `check` de novo, porque um merge da `develop` no meio pode ter desfeito trabalho de outra pessoa.

## Regras

1. **Nunca aprove, faça merge nem feche o MR.** Isso é de quem revisa, no GitLab.
2. **Escrita só com OK explícito**, um comentário por vez. `check` só lê.
3. **Português do Brasil, sem coautoria nem link de conversa de IA** (seção 7 do `AGENT.md`). O `note` recusa texto com isso.
4. **Separe o que bloqueia do que é sugestão.** Bloqueia o que fere critério do card, a avaliação, o `AGENT.md` (reports, dados restritos, Git) ou a coerência com o resto da `develop`. Estilo e preferência são sugestão.
5. **Não invente dados do parceiro** nem peça para o autor inventar. Dado sem fonte vira hipótese marcada ou pendente de validação.
6. **Escopo:** commit ou arquivo de outra issue no MR é apontamento. Peça para separar ou para integrar antes o MR da outra issue.
7. **Diga onde.** Cada apontamento leva a seção ou o arquivo e a linha, e o que fazer.
