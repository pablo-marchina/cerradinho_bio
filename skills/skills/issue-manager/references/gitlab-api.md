# Referência da API do GitLab (g01)

## Projeto

- Host: `https://git.inteli.edu.br`
- Caminho: `graduacao/2026-2b/t24/g01`
- Base da API: `https://git.inteli.edu.br/api/v4/projects/graduacao%2F2026-2b%2Ft24%2Fg01`. Os scripts usam o caminho codificado, então não é preciso saber o ID numérico. Se precisar dele, `GET /projects/graduacao%2F2026-2b%2Ft24%2Fg01` devolve o campo `id`.

## Autenticação

Cabeçalho em toda requisição: `PRIVATE-TOKEN: <token>`. O token fica na variável de ambiente de usuário `GITLAB_TOKEN` (escopo `api`). Nunca imprima, cole em chat ou commite o token.

## Endpoints usados

| Ação | Método e caminho | Observação |
|---|---|---|
| Ler uma issue | `GET /issues/:iid` | |
| Listar issues | `GET /issues?scope=all&state=all&per_page=20&page=N` | pagine; páginas grandes podem falhar com IncompleteRead |
| Criar issue | `POST /issues` | JSON: title, description, labels (string separada por vírgulas) |
| Atualizar issue | `PUT /issues/:iid` | JSON: description, add_labels, remove_labels, due_date, milestone_id, assignee_ids |
| Estimativa | `POST /issues/:iid/time_estimate?duration=1h` | |
| Tempo gasto | `POST /issues/:iid/add_spent_time?duration=1h` | `reset_spent_time` antes, para substituir |
| Criar label | `POST /labels` | JSON: name, color, description; nomes em português |
| Histórico de labels | `GET /issues/:iid/resource_label_events` | quem pôs ou tirou cada label |
| Datas do card (início e término) | `POST /api/graphql` | veja "Datas do card" abaixo |
| Labels | `GET /labels` | rode `scripts/get_labels.py` |
| Membros | `GET /members/all` | rode `scripts/get_members.py` |
| Milestones | `GET /milestones` | as sprints são milestones |

## Labels

Labels do projeto (levantadas com `python scripts/get_labels.py` em 2026-10-15). Cada card leva uma de prioridade, uma de tamanho, uma de tipo e uma de coluna do kanban.

| Grupo | Labels |
|---|---|
| Prioridade | `Prioridade Alta`, `Prioridade Média`, `Prioridade Baixa` |
| Tamanho (estimativa mínima) | `PP` (menos de 30min), `P` (30min), `M` (1h), `G` (2h), `GG` (mais de 2h) |
| Tipo | `Infraestrutura`, `CODE`, `CODE_REVIEW`, `DESIGN`, `DOCUMENTATION`, `DEPLOY`, `REQUIREMENTS`, `TEST`, `BUG` |
| Kanban | `Backlog`, `Doing`, `Review`, `Waiting Review` |
| Artefato | `Artefato 1`, `Artefato 2`, `Artefato 3`, `Artefato 4` |
| Tema | `Skils` (cards da estrutura de skills) |
| Problemas | `BLOCKED`, `DOR_INCOMPLETE`, `DOD_INCOMPLETE` |
| Turma (padrão do Inteli) | `Ano: ...`, `Curso: ...`, `Semestre: ...`, `Trimestre: ...`, `Status: Em Andamento` |

As listas `PRIORITY`, `SIZE`, `TYPE` e `KANBAN` no topo de `scripts/audit_cards.py` seguem esta tabela. Se o grupo criar ou renomear labels, atualize as duas.

## Datas do card

O campo Datas do card (data de início e data de término no quadro) é o widget `START_AND_DUE_DATE` do work item. A API REST só mostra e grava `due_date`; o início só existe por GraphQL (GitLab 18.6 aqui). `audit_cards.py` usa os dois:

```graphql
query { project(fullPath: "graduacao/2026-2b/t24/g01") { workItems(iids: ["1"]) {
  nodes { id widgets { type ... on WorkItemWidgetStartAndDueDate { startDate dueDate } } } } } }

mutation { workItemUpdate(input: { id: "gid://gitlab/WorkItem/<id>",
  startAndDueDateWidget: { startDate: "2026-10-15", dueDate: "2026-10-15" } }) { errors } }
```

Mande as duas datas na mutation: o script repete a que não mudou.

## Pessoas

Conhecidos até agora: `@arthur.loyola` (desenvolvimento) e `@guilherme.silva` (revisão). Os demais usernames saem de `scripts/get_members.py`.

## Armadilhas

1. No PowerShell, `curl` é alias de `Invoke-WebRequest` e `--header` quebra. Use os scripts Python (só biblioteca padrão).
2. Labels vão como string separada por vírgulas, não como lista JSON.
3. Acentos podem aparecer quebrados no console do Windows, mas são gravados corretamente.
