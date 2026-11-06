# Issue #79 — atualização da matriz de rastreabilidade

Atualização: 06/11/2026. Registro interno; não constitui nova execução de ensaio.

Card: [Atualizar a matriz de rastreabilidade](https://git.inteli.edu.br/graduacao/2026-2b/t24/g01/-/issues/79).
Revisor indicado: @arthur.loyola. Trabalho submetido no [MR !62](https://git.inteli.edu.br/graduacao/2026-2b/t24/g01/-/merge_requests/62); revisão humana pendente.

## Base e estado Git

Branch local: `docs/atualizar-rtm-sprint2-#79`, criada a partir de `origin/develop` em `1812fc3`. O commit inicial d07e4c8 foi publicado no MR !62; esta atualização integra a develop em 3893469 para resolver os conflitos da seção de RNFs. A comparação de três versões preservou as atualizações da develop e as alterações anteriores desta tarefa. O backup anterior está na pasta ignorada `.tools/issue79-backup-20261106/`.

## Resultado

- RTM com 35 RFs, 22 RNs, oito categorias RNF, 37 subrequisitos e RNF-REST02.
- Inclusão de todos os 24 casos da seção 3.4.2: 22 executados (CT46–CT66 e CT69) e dois planejados, não executados (CT67/CT68). Cada linha relaciona RF, RN, alcance do RNF, situação, evidência e limite. Status e links de execução conferidos contra a seção de origem.
- Vínculos dos testes físicos nas linhas funcionais e dos ensaios pertinentes nos RNFs. Nenhuma aprovação de cenário foi convertida em validação integral do requisito.
- Evidências V2 da #74 incorporadas, preservando resultados históricos. Acrescentados os retestes de bateria CT12/CT13/CT23, CT40 e CT18. CT18 aprovado nas contagens em 05/11, sem apagar as reprovações anteriores.
- Cenários existentes substituem “CT a definir” na RTM. CT-F03 explicita recusa de vínculo de dispositivo ainda ocupado; CT-F30 descreve escalonamento de e-mail; CT-U01 descreve o ensaio moderado de usabilidade. Os 20 IDs futuros referenciados estão definidos no documento complementar.
- Corrigidos os trechos que ainda declaravam toda a validação física pendente; atualizado o histórico complementar de CT18/CT22 com o alcance dos retestes e de CT62/CT65.
- Mantidas as fichas RNF atualizadas da develop, com os vínculos planejados CT-U01, CT-F20, CT-F21 e CT-F24 e o estado de confiabilidade atualizado pelos novos ensaios.

## Aceitação e limites

| Critério do card | Resultado |
|---|---|
| Incluir CTs da seção 3.4.2 | Realizado: 24 casos relacionados. O impedimento registrado em 05/11 foi resolvido pela atualização da develop. |
| Vincular RNFs | Realizado, com oito categorias, 37 subrequisitos e RNF-REST02; apoio parcial separado de aprovação integral. |
| Revisar “CT a definir” | Realizado na RTM, com cenários futuros definidos e marcados como não executados. |
| Atualizar evidências e verificar caminhos | Realizado; histórico V1/V2 preservado, novos testes físicos e retestes vinculados. |
| Registrar e submeter à revisão indicada | Resultado registrado e publicado no MR !62; revisão humana pendente. |

CT09 continua bloqueado por falta de ACK. CT63/CT64 reenviam na Serial; não comprovam entrega ao servidor. CT62 comprova reinício de uptime e restauração de ciclo/lacre, não toda a fila, última posição ou histórico. CT65 usa jumper, não contato definitivo. CT52 usa NEO-6M, não demonstra equivalência ao LC76G-PA. CT24–CT34, CT67/CT68 e cenários futuros permanecem planejados. Não houve execução de novos testes de firmware, simulador ou bancada nesta tarefa.

## Verificação em 06/11/2026

- `python scripts/verify_issue74.py`: PASS; 24 CTs históricos V1/V2, 96 evidências únicas (71 PNG e 25 logs), caminhos, hashes e associações consistentes. Seu resumo de CT18 refere-se às rodadas históricas, não ao reteste posterior.
- `python scripts/audit-docs.py --strict-local`: 27/31 verificações locais; 555 links sem arquivo ausente. Falhas preexistentes de limite de 300 palavras: 3.1.1 (357), 3.1.3 (389), 3.1.5 (360), 3.1.6 (401). Essas seções foram comparadas e estão idênticas à develop; não foram alteradas pela #79. As duas pendências externas de aceite LGPD e publicação/reteste Wokwi permanecem.
- Conferência estrutural: todos os CT46–CT69 presentes na tabela física, 22 status/links de execução idênticos à fonte e CT67/CT68 marcados não executados; 37 subrequisitos RNF relacionados; 20 IDs futuros definidos; nenhuma linha “CT a definir” na RTM.
- `git diff --check`: sem erros. Nenhum marcador de conflito.
- Seções anteriores a 3.3.1 e posteriores a 3.3.4 idênticas à develop. A descrição da cobertura física dos RFs na 3.3.1 foi ajustada para não contradizer os novos vínculos da RTM. Firmware e evidências recebidos da base não foram editados.
- Antes da preparação do commit, HEAD igual à base `1812fc3` e índice vazio; apenas os três documentos desta issue foram selecionados para o commit.

## Resolução do conflito do MR !62

A atualização da develop pela #111 desdobrou os RNFs em 37 subrequisitos. A resolução preserva as fichas e o vocabulário novos, repõe os vínculos planejados da #79 e atualiza a RTM para RNF03.3–RNF03.5, RNF04.3–RNF04.4, RNF06.7, RNF07.4 e RNF08.3–RNF08.6. RNF-REST02 fica como referência técnica dos novos itens físicos RNF08.4–RNF08.6.

CT-U01 cobre os critérios separados de acerto, tempo e interpretação fiscal. Foram preservadas as ressalvas do CT62 e a agregação de violações no CT64, e diferenciadas as rodadas históricas do CT18. Nenhum teste planejado foi promovido a executado. A auditoria de evidências passa, os 555 links seguem válidos e a auditoria geral mantém somente as quatro falhas de extensão de texto preexistentes.

## Atendimento à revisão do MR !62 em 06/11/2026

Correções do comentário 544037: vocabulário de status padronizado; RF006 com nota do reteste; RN a definir para reconexão, contrato MQTT e escalonamento; CT15 V2 PARCIAL no documento e inventário; CT22 separado em uptime reprovado e retenção observada. CT18 também ganhou asserções separadas. Acrescentados resumo 12/7/16, lacunas das RNs, critério de agregação, visão CT→RF/RN e divergências de firmware.

O ramo VINCULO_RECUSADO existe em requestLink(), mas não é alcançado pelo botão com ciclo ativo, que chama requestClose(). A implementação será acompanhada no [card #113](https://git.inteli.edu.br/graduacao/2026-2b/t24/g01/-/issues/113), com estimativa inicial de 2h; datas de execução ficam pendentes de planejamento. Não houve alteração de firmware nesta revisão.

Verificação atual: verify_issue74.py e verify_issue79.py PASS; quatro testes de regressão do verificador de links PASS. Auditoria geral: 562 links locais válidos, 27/31 verificações locais, mesmas quatro pendências anteriores de extensão e duas externas. Âncoras explícitas conferidas localmente e pela API de renderização Markdown do GitLab. Não foram reexecutados ensaios de simulador ou bancada. Os resultados da seção anterior registram verificações históricas, anteriores a esta revisão.
