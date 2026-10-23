# Formato do parecer

É o formato das revisões que funcionaram melhor na Sprint 1, como as dos MRs !18, !19 e !23. Português, direto, sem elogio vazio. O autor vai ler sem a sua revisão aberta e, muitas vezes, sem a imagem ao lado, então cada comentário tem de bastar sozinho.

## Primeira rodada

```markdown
**Revisão — <seção ou tema>**

O card pedia <objetivo em uma ou duas frases>. <Por que isso importa para o que vem depois, se importar.>

**O que está certo:** <o que atende ao card e à avaliação, com o concreto: "os seis itens estão cobertos", "os IDs batem com a 3.3.2">.

**Correções (bloqueiam o merge):**

1. **<O problema, numa frase>.** <Por que é um problema, explicando o conceito quando for preciso.> <O que fazer, de forma concreta: texto pronto, troca de palavra, linha de tabela; se houver duas saídas razoáveis, dê as duas.> (<seção, arquivo ou linha>)
2. ...

**Sugestões (não bloqueiam):**

- <...>

**Parecer:** solicito correções. | **Parecer:** aprovado.
```

- Ligue cada correção a um critério do card, à avaliação ou a uma regra do `AGENT.md`. Se não der para ligar, provavelmente é sugestão.
- Código sempre com o nome: "D6 – Cobertura de rede incerta", nunca só "D6".
- Quando a revisão é longa, separe por tema (geral, abertura, figura, personas, legenda) em comentários distintos. O geral resume e aponta para os outros. Comentários de linha (`note --path --line`) servem para o detalhe.
- Proposta de seção reescrita vai dentro de `<details><summary>Proposta da seção X</summary> ... </details>`, para não esconder o parecer.
- Na dúvida sobre um fato, pergunte: "Pelo kick-off, quem prepara a saída é o PCM. É isso que vocês queriam mostrar na T1?".
- Se não houver correção, diga numa frase o que conferiu e aprove.

### Exemplo

Ruim (lista de correções que depende da imagem):

> D6 não é dor. CG4 repete PS1. Tirar D6/AD6.

Bom:

> 1. **"D6 – Cobertura de rede incerta" não é dor do cliente.** Dor é algo que incomoda o PCM ou a Fiscal hoje, e hoje eles não dependem de rede nenhuma: a falta de cobertura é uma limitação da nossa solução. Por isso o "AD6 – Sem comunicação sinalizada" acaba aliviando um problema que o próprio produto cria. Isso é risco e já está no R04 da matriz (3.1.5). Sugestão: tirar D6 e AD6 do canvas. (figura da 3.1.4)

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
