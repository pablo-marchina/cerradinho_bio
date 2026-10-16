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
| Tempo gasto | `POST /issues/:iid/add_spent_time?duration=1h` | |
| Labels | `GET /labels` | rode `scripts/get_labels.py` |
| Membros | `GET /members/all` | rode `scripts/get_members.py` |
| Milestones | `GET /milestones` | as sprints são milestones |

## Labels

O catálogo de labels deste projeto ainda não foi levantado. Rode `python scripts/get_labels.py` e confira se as listas no topo de `scripts/audit_cards.py` (`PRIORITY`, `SIZE`, `TYPE`, `KANBAN`) batem com o que existe. Se o grupo usar outros nomes, ajuste as listas no script em vez de criar labels novas.

## Pessoas

Conhecidos até agora: `@arthur.loyola` (desenvolvimento) e `@guilherme.silva` (revisão). Os demais usernames saem de `scripts/get_members.py`.

## Armadilhas

1. No PowerShell, `curl` é alias de `Invoke-WebRequest` e `--header` quebra. Use os scripts Python (só biblioteca padrão).
2. Labels vão como string separada por vírgulas, não como lista JSON.
3. Acentos podem aparecer quebrados no console do Windows, mas são gravados corretamente.
