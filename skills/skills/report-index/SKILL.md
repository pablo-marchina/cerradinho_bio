---
name: report-index
description: Mantém os índices de relatórios de cada sprint (document/outros/reports/SprintNN/README.md) do GuaráTracking em sincronia com os relatórios em disco. Adiciona linhas para relatórios novos, corrige linhas que apontam para arquivos movidos ou ausentes, cria o índice de uma sprint nova e exige o cabeçalho padrão com o campo Implicação. Use sempre que um relatório de pesquisa, teste ou verificação for criado, renomeado ou movido, quando começar a pasta de relatórios de uma sprint, ou quando pedirem para conferir ou atualizar um índice de relatórios.
---

# Índice de relatórios

Cada sprint guarda seus relatórios (pesquisas de hardware e conectividade, testes de comunicação, consumo, fixação, exposição à água etc.) em `document/outros/reports/SprintNN/`. O `README.md` dessa pasta é a porta de entrada para quem escreve a `document/documentacao.md`, colega ou IA. Relatório fora do índice é, na prática, invisível.

## 1. Conferir (só leitura)

Da raiz do repositório:

```bash
python skills/skills/report-index/scripts/check_report_index.py                          # todas as sprints
python skills/skills/report-index/scripts/check_report_index.py document/outros/reports/Sprint01
```

ERROS: relatório sem linha, linha apontando para arquivo inexistente, linha duplicada, número errado de células (um `|` dentro de código precisa ser escrito `\|`), célula vazia, relatório sem o cabeçalho padrão ou com campo vazio. Avisos: caracteres danificados (`�`, `Ten?rio`). Sai com código 1 se houver ERRO.

O script não lê o corpo dos relatórios procurando caminhos velhos; depois de mover arquivos, procure o caminho antigo no repositório inteiro.

## 2. Adicionar ou corrigir uma linha

Leia o relatório antes de escrever a linha. Nunca escreva a linha a partir do nome do arquivo.

| Coluna | Regra |
|---|---|
| Relatório | Link relativo ao índice: `[Título legível](arquivo.md)` |
| Autor | Como está no relatório. Se não houver, `Não informado` |
| Issue de origem | `#NN` como está no relatório ou no commit. Senão, `Não informado`. Nunca chute |
| Status | Uma ou duas frases com os números reais do relatório e o que ficou em aberto. Copie os números; não recalcule |
| Alimenta | Seção(ões) da `documentacao.md` que ele apoia, em negrito (`**3.4.2 Protótipo Físico (offline)**`), ou `**Nenhuma diretamente.**` com o motivo |
| Implicação | O que o achado muda para o projeto e para a Cerradinho, tirado da linha `**Implicação:**` do relatório |

- Célula vazia não é permitida. "Não informado" é uma resposta honesta; um chute é um mapeamento errado.
- Se um número foi substituído por um relatório posterior, escreva `novo (era antigo)`; não sobrescreva em silêncio.
- Mantenha a ordem e o formato da tabela. Só corrija linhas de outras pessoas se estiverem quebradas, e só a parte quebrada.
- Em resultados de teste, diferencie **medido** de **estimado** (por exemplo, autonomia de 150 dias é estimada a partir do consumo medido), como pede o `AGENT.md`.
- Nada de dados restritos no índice: nomes reais de fornecedores, NFs, rotas ou localizações reais ficam fora (ver `AGENT.md`, seção 3).

## 3. Cabeçalho padrão dos relatórios

Todo relatório abre com:

```markdown
> **Pergunta investigada:** a única coisa que o relatório se propôs a responder.
> **O que foi feito:** os passos concretos (montagem, testes, scripts, leituras).
> **Evidência:** números reais e o arquivo ou script que os gerou.
> **Em aberto:** o que o relatório não resolveu, dito com clareza.
> **Alimenta:** qual seção da `documentacao.md` ele apoia.
> **Implicação:** o que isso muda para o projeto e para a Cerradinho, e por que importa.
```

Uma boa Implicação liga o achado a uma consequência: "o sinal celular cai dentro do galpão do fornecedor, então o dispositivo precisa guardar eventos e enviar depois, e o requisito de operação sem conectividade sobe de prioridade". Repetir o resultado não é Implicação.

Se faltar o cabeçalho ou a Implicação estiver fraca, não invente. Avise quem pediu e sugira que o autor escreva; rascunhe só se pedirem.

## 4. Pasta de uma sprint nova

Quando surgir o primeiro relatório de uma sprint, crie `document/outros/reports/SprintNN/README.md`:

```markdown
# Relatórios da Sprint N: Índice

> **Para que serve:** porta de entrada para os relatórios de pesquisa, teste e verificação da Sprint N.
>
> **Como usar:** ache a seção que você está escrevendo na coluna Alimenta e leia esses relatórios.
>
> **Lacunas de propósito:** "Não informado" significa que o relatório não traz autor ou issue; chutar criaria um mapeamento errado.

## Índice

| Relatório | Autor | Issue de origem | Status | Alimenta | Implicação |
|---|---|---|---|---|---|
```

Ligue o índice novo ao da sprint anterior e, se existir, ao `README.md` de `document/outros/`. Apague o `placeholder.md` da pasta quando ela tiver conteúdo real.

## 5. Concluir

Rode o checker até sair com código 0 e commite ligado à issue do relatório, seguindo a convenção do `AGENT.md` (ex.: `docs(#NN): indexa relatório de teste de conectividade`).
