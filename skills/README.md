# Skills para agentes de IA

Esta pasta guarda as skills do GuaráTracking: pacotes de instruções, scripts e referências que ensinam um agente de IA (Claude, Codex, Copilot etc.) a executar fluxos recorrentes do grupo do jeito que o grupo combinou. Elas complementam o `AGENT.md` da raiz, que continua sendo a primeira leitura obrigatória.

## Estrutura

```text
skills/
├── README.md            este arquivo
├── SKILLS_INDEX.md      mapa: qual skill usar para cada tarefa
└── skills/
    └── <nome-da-skill>/
        ├── SKILL.md     obrigatório: frontmatter (name, description) + instruções
        ├── scripts/     opcional: código executável (Python, Bash)
        ├── references/  opcional: documentação carregada só quando necessário
        └── assets/      opcional: modelos e arquivos usados na saída
```

## Requisitos

- Python 3.
- O validador de skills (`skills/skills/skill-creator/scripts/quick_validate.py`) precisa do PyYAML: `pip install pyyaml`.
- Os scripts da `issue-manager` usam só a biblioteca padrão e precisam da variável de ambiente `GITLAB_TOKEN` (veja a seção Segurança).

## Como usar

1. Leia o `AGENT.md` da raiz.
2. Abra o `SKILLS_INDEX.md` e escolha só as skills ligadas à tarefa. Não carregue todas por padrão: o objetivo é manter o contexto pequeno e preciso.
3. Siga o `SKILL.md` da skill escolhida e abra os arquivos de `references/` apenas quando ele mandar.
4. Rode os scripts da raiz do repositório ou da pasta da skill, como o `SKILL.md` indicar.

## Como criar uma skill nova

Use a skill `skill-creator`:

```bash
python skills/skills/skill-creator/scripts/init_skill.py <nome-da-skill> --path skills/skills --resources scripts,references
# edite o SKILL.md e os recursos
python skills/skills/skill-creator/scripts/quick_validate.py skills/skills/<nome-da-skill>
```

O `quick_validate.py` precisa do PyYAML (veja [Requisitos](#requisitos)).

Depois:

1. adicione a skill numa linha do `SKILLS_INDEX.md`;
2. escreva `description` e corpo em português, com a `description` dizendo **o que** a skill faz e **quando** usar;
3. mantenha o `SKILL.md` curto (idealmente menos de 500 linhas) e mova detalhes para `references/`;
4. commite numa branch própria, ligada a uma issue, seguindo as convenções de Git do `AGENT.md`.

## Quando criar (e quando não)

Crie uma skill quando a orientação for reutilizada em mais de uma tarefa, quando o fluxo for frágil e recorrente, ou quando houver regras do projeto que uma IA costuma esquecer.

Não crie skill para decisão temporária, resumo de uma conversa, conteúdo que pertence à `document/documentacao.md`, ou checklist de uso único. Se uma skill existente quase resolve, ajuste ela em vez de criar outra.

## Segurança

Nenhuma skill pode conter credenciais, tokens ou dados restritos da Cerradinho (cadastros de ativos, NFs, rotas, fornecedores reais). Tokens ficam em variáveis de ambiente, como o `GITLAB_TOKEN` usado pela `issue-manager`.
