---
name: issue-manager
description: Audita, cria e corrige cards (issues) do GitLab do projeto GuaráTracking (graduacao/2026-2b/t24/g01 em git.inteli.edu.br). Confere cada card contra o template em português do repositório e contra as regras de labels, tamanho, estimativa, datas, milestone e revisor, propõe as correções exatas e aplica uma por vez depois do OK do dono do card. Use sempre que alguém pedir para revisar, melhorar, padronizar ou atualizar uma issue ou card, auditar os cards antes de uma sprint review, listar labels ou membros, ou reescrever a descrição de uma issue.
---

# Issue Manager (GitLab)

Projeto `graduacao/2026-2b/t24/g01` em `https://git.inteli.edu.br`. Tudo o que a skill precisa está nesta pasta.

## Requisitos

- **Python 3**, só biblioteca padrão.
- **`GITLAB_TOKEN`**: token pessoal com escopo `api` numa variável de ambiente de *usuário* do Windows. Os scripts leem o registro (`HKCU\Environment`) primeiro e depois o ambiente do processo. Nunca imprima, cole ou commite o token. Se ele não existir, pare e avise o dono.
- **Rede** até `git.inteli.edu.br` (funciona no notebook do grupo; ambientes em nuvem em geral não alcançam).
- Rode de dentro desta pasta: `python scripts/<nome>.py`.

## Arquivos

```
SKILL.md
scripts/
├── audit_cards.py    audit (só leitura) e apply (escreve em um card por vez)
├── get_labels.py     lista as labels do projeto
└── get_members.py    lista os membros do projeto
references/
├── gitlab-api.md     endpoints, labels, pessoas e armadilhas
└── model.md          cópia do template de card (.gitlab/issue_templates/Tempalte01.md)
```

## Fluxo: melhorar a descrição de uma issue

1. Leia a issue atual (`python scripts/audit_cards.py audit --iid N`, ou `GET /issues/N`) e leia a descrição bruta inteira antes de reescrever.
2. Reescreva seguindo `references/model.md` na ordem: Título, Descrição (detalhada), DoR, DoD, Critérios de Aceitação, Links, Observações, Datas, Desenvolvedor(a), Revisor(a).
   - **Descrição** suficiente para outra pessoa executar sem perguntar; cite requisito ou regra de negócio de `document/documentacao.md` (seções 3.3.1 e 3.3.2) quando houver.
   - **DoR** e **DoD** como listas verificáveis; **Critérios de Aceitação** como itens que alguém consegue marcar como feito ou não feito.
   - Mantenha os `@username` de quem desenvolve e de quem revisa, e mantenha qualquer informação real que o autor escreveu. Não invente dados do parceiro.
   - Apague as linhas de instrução do template (as que começam com "Inclua tags", "Não se esqueça", "Em datas") depois de cumpridas.
3. Salve o rascunho num arquivo fora do repositório e mostre ao dono. Aplique só depois do OK:
   `python scripts/audit_cards.py apply --iid N --description-file rascunho.md`
4. Rode `audit --iid N` de novo e confira o resultado.

## Fluxo: auditoria antes da sprint review

1. `python scripts/audit_cards.py audit --milestone "Sprint 1" --assignee <username>` (ou `--iid 39 40`, `--json` para saída de máquina). Cada integrante é avaliado pelos próprios cards.
2. O script confere: responsável, milestone e data de entrega; uma label de prioridade e uma de tamanho; label de tipo; label de coluna do kanban (card fechado com `Done`); estimativa mínima por tamanho (P 30m, M 1h, G 2h); `Data de início: AAAA-MM-DD` sob `## Datas`; em card fechado, tempo gasto igual à estimativa; seções do template presentes, preenchidas e em ordem; Desenvolvedor(a) e Revisor(a) como `@username`, pessoas diferentes; sem placeholder e sem linha de atribuição de IA.
3. As heurísticas erram às vezes: leia a descrição bruta antes de reportar um problema de seção ou placeholder.
4. Vá além do script: compare a coluna do card com o estado real (branches, merge requests) e a data com a sprint.
5. Proponha correções exatas e aplique uma por vez, com OK do dono:
   `python scripts/audit_cards.py apply --iid N [--add-labels "Prioridade Média,M"] [--remove-labels ...] [--estimate 1h] [--due AAAA-MM-DD] [--start AAAA-MM-DD] [--milestone "Sprint 1"] [--set-assignee arthur.loyola] [--description-file rascunho.md]`

## Regras

1. **Nunca invente label.** Rode `get_labels.py`. Prioridade: `Prioridade Alta`, `Prioridade Média` ou `Prioridade Baixa`. Tamanho: `PP`, `P`, `M`, `G` ou `GG`. As listas no topo de `audit_cards.py` seguem `references/gitlab-api.md`; se o grupo criar ou renomear labels, atualize as duas.
2. **Nunca remova uma label que você não colocou** sem perguntar.
3. **Cards em português**, seguindo `references/model.md`, sem linha de atribuição de IA.
4. **Desenvolvedor(a) e Revisor(a) ficam na descrição** com `@username`; vários colegas podem não ser membros diretos, então `assignee_ids` pode falhar.
5. **Ponto de alinhamento não é dependência.** Escreva "Não é bloqueante; alinhar com @fulano antes de decidir X".
6. **Escrita só com OK explícito do dono**, um card por vez. A auditoria só lê.
