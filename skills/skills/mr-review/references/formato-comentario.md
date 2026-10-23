# Formato do parecer

É o formato das revisões que funcionaram melhor na Sprint 1, como as dos MRs !18, !19 e !23. Português, direto, sem elogio vazio.

## Primeira rodada

```markdown
**Revisão — <seção ou tema>**

O card pedia <objetivo em uma ou duas frases>. <Por que isso importa para o que vem depois, se importar.>

**O que está certo:** <o que atende ao card e à avaliação, com o concreto: "os seis itens estão cobertos", "os IDs batem com a 3.3.2">.

**Correções (bloqueiam o merge):**

1. **<Título curto>.** <O que está errado, onde (seção, arquivo e linha) e o que fazer.>
2. ...

**Sugestões (não bloqueiam):**

- <...>

**Parecer:** solicito correções. | **Parecer:** aprovado.
```

- Ligue cada correção a um critério do card, à avaliação ou a uma regra do `AGENT.md`. Se não der para ligar, provavelmente é sugestão.
- Comentários de linha (`note --path --line`) servem para o detalhe. O parecer geral resume e aponta para eles.
- Se não houver correção, diga em uma frase o que conferiu e aprove.

## Segunda rodada

```markdown
**Revisão atualizada — <seção ou tema>**

Resolvido:
- <apontamento anterior> — <como ficou>.

Pendente:
- <apontamento anterior> — <o que ainda falta>.

<Novo, se a correção trouxe algo.>

**Parecer:** aprovado. | **Parecer:** falta <X> para aprovar.
```
