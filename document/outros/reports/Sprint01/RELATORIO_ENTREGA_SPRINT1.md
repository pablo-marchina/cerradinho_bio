# Relatório de preparação de entrega — GuaráTracking, Sprint 1

**Base consultada:** export GitLab de 24/10/2026: `main` = `2650e28`; `develop` = `915a744`. As árvores eram equivalentes, mas o GitLab remoto não foi modificado por este trabalho.

A documentação da disciplina está em [documentacao.md](../../../documentacao.md), e o [fechamento com critérios objetivos](FECHAMENTO-S1.md) constitui a referência de status. Capturas históricas estão em `assets/wokwi/`, sem substituição por novos resultados simulados.

## Mudanças propostas na revisão atual

- Contexto da indústria, solução, política de privacidade e BOM condensados até o teto formal de 300 palavras, com anexos preservando o conteúdo detalhado.
- Auditoria automatizada separa verificações locais de evidência externa: `--strict-local` impede regressões documentais no CI; `--strict` exige homologação LGPD e Wokwi documentados, sem deduzir aprovação pela ausência de termos como "pendente".
- Captura do lacre em interrupção com timestamps e debounce separado; **ensaio host 20/20**, pendente de Wokwi/CT18 original. Diagnóstico de motivo de reboot e uptime inicial para investigar CT22.
- Filtro de persistência: ao falhar uma gravação NVS, `contextDirty`/`pendingDirty` continuam sinalizados para retentativa com limitação de frequência; gravações parciais de fila geram diagnóstico. **Não implementa transação nem dupla cópia atômica**.
- Novo teste host injeta falha na gravação do contexto e da fila. Aprovação desse teste não prova persistência após corte real de energia nem resolve CT22.
- Manifesto de evidências externas permanece `pendente` e não deve ser alterado sem documentos e resultados correspondentes.

## Evidências e fronteiras

| Evidência | Status neste pacote | O que não prova |
|---|---|---|
| Testes C de regras + classe de LED C++ | Executáveis no host | Execução Wokwi/ESP32 |
| Falha NVS injetada no `main.cpp` com stubs | Executável no host | Atomicidade sob power loss |
| 20 bordas do lacre por ISR/debounce no `main.cpp` | Executável no host | CT18 aprovado no Wokwi |
| Sintaxe C++ do firmware nos modos demo/weekly | Executável no host | Build oficial do novo patch |
| CI ESP32 anterior (`915a744`) | Aprovado antes deste patch | Compilação de commit posterior |
| Publicação histórica no Wokwi | Há URL antiga | Protótipo integrado atualizado |
| CT18/CT22/CT09 originais | REPROVADO / REPROVADO / BLOQUEADO | Aprovação funcional integral |
| LGPD / BOM | Minutas, estimativas e fontes | Homologação jurídica ou aquisição final |

Não há declaração de nota máxima até a execução e documentação das pendências. A versão anterior deste relatório misturava resultados de patches não mesclados e a BOM com oito preços faltantes: **essas afirmações não se aplicam ao export atual**, que já precifica 16 materiais como referência.
