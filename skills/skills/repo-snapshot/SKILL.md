---
name: repo-snapshot
description: Prepara uma branch Snapshot limpa do repositório GuaráTracking (g01) para virar a main entregue à Cerradinho e aos professores. Corta uma branch a partir da branch de trabalho, para de rastrear o material de apoio a IA (AGENT.md, skills/ e afins) num único commit, ignora esse material só nessa branch e verifica que o diff tem apenas remoções. Use quando uma sprint fechar e a main precisar de uma versão limpa, ou quando pedirem para cortar, refazer ou conferir uma branch de snapshot. Não faz merge na main.
---

# Snapshot do repositório

A `main` é o que a Cerradinho e os professores veem, e é protegida contra force push. A branch de trabalho (hoje a própria `main` ou uma `develop`, se o grupo criar) pode guardar material de apoio a agentes de IA. A **branch Snapshot** faz a ponte: sai da branch de trabalho e a única mudança dela é parar de rastrear esse material, para que o merge na `main` entregue um repositório limpo sem apagar nada na branch de trabalho.

> **Decisão do grupo:** hoje o `AGENT.md` está na `main` por escolha do grupo. Só use esta skill quando o grupo decidir que a entrega da sprint deve sair sem o material de IA. Na dúvida, pergunte ao Arthur (SM) antes de cortar.

Esta skill **só prepara a branch**. O merge na `main` é um passo separado, feito por uma pessoa, via merge request.

## 1. Pré-condições (pare se alguma falhar)

```bash
git fetch origin --prune
SRC=origin/main                     # ou origin/develop, se o grupo trabalhar numa develop
git rev-parse "$SRC"                # anote o SHA: toda verificação abaixo usa ele, porque a branch pode andar
git status --porcelain              # deve sair vazio
# Nenhum dado restrito pode estar rastreado (planilhas, dumps, credenciais): deve sair vazio.
git ls-tree -r --name-only "$SRC" | grep -iE '\.(csv|tsv|xlsx?|env|pem|key)$|secret|token'
```

Se o último comando listar algo, pare e avise: dado restrito no histórico é problema maior que a snapshot (ver `AGENT.md`, seção 3).

## 2. Levantar a lista de remoção na branch de origem

Não confie numa lista fixa; ela cresce a cada sprint.

```bash
for p in AGENT.md AGENTS.md CLAUDE.md skills .claude .opencode .cursor; do
  printf '%-12s %s\n' "$p" "$(git ls-tree -r --name-only "$SRC" -- "$p" | wc -l | tr -d ' ')"
done
# Arquivos com cara de IA fora dessas raízes: cada um precisa de decisão.
git ls-tree -r --name-only "$SRC" | grep -vE '^(skills/|AGENT\.md$)' \
  | grep -iE 'agent|skill|claude|opencode|codex|cursor|copilot|prompt|\.mdc$|llm'
```

**O que conta como material de IA:** arquivos cujo propósito é guiar sessões de IA ou que uma ferramenta gerou para elas (instruções de agente, skills, planos de sessão).
**O que fica:** `.gitlab/issue_templates/` (configuração do repositório), `document/`, `src/`, `assets/` e qualquer relatório que registre um teste ou decisão, mesmo que cite IA. Menções em texto a `AGENT.md` ou `skills/` dentro de arquivos que ficam não são editadas: a snapshot muda rastreamento, nunca conteúdo.

## 3. Cortar a branch e fazer o único commit

Use uma worktree separada, porque fazer checkout da snapshot no seu clone normal tira `AGENT.md` e `skills/` da sua pasta de trabalho.

```bash
IID=<número da issue>
SRC_SHA=<SHA anotado na seção 1>
SNAP=chore/$IID-snapshot-limpo          # segue a convenção de branch do AGENT.md
WT=../snapshot-$IID                     # fora do repositório
git worktree add -b "$SNAP" "$WT" "$SRC_SHA"
cd "$WT"
git rm --cached -r --quiet AGENT.md
git rm --cached -r --quiet skills
# um git rm --cached por raiz encontrada na seção 2
```

Acrescente o bloco ao `.gitignore` com `printf >>`, nunca com editor, para não mexer em outras linhas por causa de encoding ou fim de linha:

```bash
tail -c 1 .gitignore | xxd        # deve ser 0a; se não for, o primeiro printf junta duas linhas
{ printf '\n'; printf '%s\n' \
"# Somente na branch de snapshot (issue #$IID): material de apoio a IA mantido na branch de" \
"# trabalho e excluído aqui porque esta branch vai para a main. Não traga estas linhas de volta." \
"AGENT.md" "AGENTS.md" "CLAUDE.md" "skills/" ".claude/" ".opencode/"; } >> .gitignore
git add .gitignore
git diff --cached -- .gitignore   # uma linha em branco e depois o bloco
git commit -m "chore(#$IID): remove material de apoio a IA da branch de snapshot"
```

Mantenha os dois `printf` separados: um único `printf '\n%s\n'` repete o formato para cada argumento e coloca uma linha em branco antes de cada entrada.

## 4. Verificar antes do push

```bash
git diff "$SRC_SHA...$SNAP" --stat                                  # tabela para postar na issue
git rev-list --count "$SRC_SHA..$SNAP"                              # deve ser 1
git diff --name-status "$SRC_SHA...$SNAP" | awk '$1!="D"'           # deve ser só: M .gitignore
git diff "$SRC_SHA...$SNAP" -- .gitignore | grep '^-' | grep -v '^---'   # não deve imprimir nada
git status --porcelain                                              # não deve imprimir nada
git push -u origin "$SNAP"
```

O `git status` vazio também prova que as linhas novas do `.gitignore` cobrem tudo o que foi removido, já que os arquivos continuam no disco da worktree.

## 5. Entregar e parar

Comente na issue: o SHA de origem, a saída do `--stat` e a lista do que foi removido. Abra o merge request para a `main` e avise quem vai fazer o merge. Antecipe conflitos sem mexer em branch nenhuma:

```bash
git merge-tree --write-tree --name-only origin/main "$SNAP"   # só leitura; lista arquivos em conflito
```

Remova a worktree no fim: `git worktree remove ../snapshot-$IID`.
