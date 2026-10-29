# Relatório de preparação de entrega — GuaráTracking, Sprint 1

**Escopo:** relatório histórico e roteiro de validação da Sprint 1. O status final depende do commit que será avaliado no GitLab.

A documentação da disciplina está em [documentacao.md](../../../documentacao.md), e o [fechamento com critérios objetivos](FECHAMENTO-S1.md) contém o checklist de pendências. Capturas históricas estão em `assets/wokwi/`, sem substituição por novos resultados simulados.

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
| Sintaxe C++ do firmware nos modos demo/weekly | Executável no host | Build oficial do commit avaliado |
| CI ESP32 anterior (`915a744`) | Aprovado historicamente | Compilação do commit final |
| Publicação histórica no Wokwi | Há URL antiga | Protótipo integrado atualizado |
| CT18/CT22/CT09 originais | REPROVADO / REPROVADO / BLOQUEADO | Aprovação funcional integral |
| LGPD / BOM | Minutas, estimativas e fontes | Homologação jurídica ou aquisição final |

Não há declaração de nota máxima até a execução e documentação das pendências. A planilha atual apresenta valores referenciais para 16 materiais; isso não comprova aquisição, estoque ou homologação.

## Matriz de evidências por entregável

| Entregável | Evidência prevista | Critério de revisão |
|---|---|---|
| Negócios | Seções 1, 3.1.1–3.1.7; figuras e anexos | Subtópicos <= 300 palavras, templates e fontes conferidos |
| Personas | Seção 3.2 e três cards | Origem das hipóteses e uso de IA declarados |
| RF/RN/RTM | Seções 3.3.1, 3.3.2 e 3.3.4 | 29 RF e 20 RN com vínculos, pendências identificadas |
| Wokwi | Seção 3.4.1, firmware, imagens, logs | Classe própria, funções e CTs executáveis no mesmo commit |

## Procedimento de validação final

1. Executar testes host e auditor documental; preservar logs da execução.
2. Compilar ESP32 nos ambientes `demo` e `weekly`; registrar SHA, ferramentas e resultados.
3. Reexecutar CT18 e CT22 com estímulos originais; registrar bloqueio/aceite formal do CT09.
4. Confirmar canal LGPD e aprovar BOM institucionalmente antes de usar dados ou realizar compra.
5. Conferir pipeline, comentários e ausência de conflitos antes do merge.

**Resultados antigos reprovados continuam visíveis** até novas evidências.
