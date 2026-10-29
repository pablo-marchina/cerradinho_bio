# Relatório de preparação de entrega — Sprint 1 GuaráTracking

> **Estado atual após integração documental e técnica:** o firmware V2 contém `ScreenController`, LCD, botões de navegação, buzzer, NF de remessa por `REMESSA`, ISR de lacre e retentativas de NVS. Snapshots alternados/CRC e ACK de servidor **não** fazem parte desta revisão. CT18, CT22 e CT09 conservam seus resultados históricos. Os resultados descritos abaixo foram produzidos em revisões anteriores e não são uma homologação do firmware integrado; consultar a seção 3.4.1 da documentação principal para condições de reteste.


> **Nota (24/10/2026):** este registro descreve a versão `fe2ed7c` da revisão geral. Na entrega, ficou só a classe `IndicatorController`; a captura do lacre por ISR, o ACK manual pela Serial e os snapshots alternados foram adiados para a Sprint 2, porque exigem rodar de novo todos os CTs no Wokwi.


**Referência:** 23/10/2026 · Inteli, Grupo G01 · Parceiro: CerradinhoBio · **base Git:** `a70d1d6` na `develop`.

## Declaração de escopo e de evidências

Esta revisão integra a implementação e a documentação que podiam ser tratadas localmente. **Não é homologação da Sprint 1, não comprova nota máxima e não altera o GitLab/Wokwi públicos**. As evidências históricas permanecem intactas. Novos testes executados em host não substituem a simulação Wokwi nem a compilação oficial ESP32. A equipe deve confirmar a equivalência GitLab/GitHub exigida pela disciplina.

## Atendimento por entrega avaliada

| Critério | Peso quando informado | Entrega verificável no patch | Condição para aceite integral |
|---|---:|---|---|
| Seção 1: introdução, objetivos, solução, justificativa | 1,0 | Parceiro, porte, setor, problema e resultado esperado; quatro seções concisas | Conferir informações do parceiro e referências; nenhuma promessa técnica na parte de objetivos |
| 3.1.1: concorrência e Porter | 1,0 | Cinco empresas concorrentes, modelos, tendências, cinco forças, matriz e justificativas aprofundadas | Revisão final do parceiro e relevância das fontes |
| 3.1.2: SWOT | 1,5 | Quadrantes visuais, fatores internos/externos e vínculo com CerradinhoBio | Validar hipóteses de processo com o parceiro |
| 3.1.3: descrição da solução | 1,0 | Problema, proposta, uso, benefícios, sucesso e indicadores com protocolo complementar | Aprovar metas/amostra com usuário do piloto |
| 3.1.4: VPC | 1,5 | Canvas visual, dores, ganhos, tarefas e relação com proposta | Não tratar hipótese funcional como funcionalidade implementada |
| 3.1.5: riscos | 1,5 | Matriz numerada R01–R11/O01–O02, justificativas adicionais e medidas de mitigação | Validar classificação qualitativa; probabilidades são estimativas de julgamento, não estatística |
| 3.1.6: LGPD | 1,0 | Minuta com inventário detalhado, bases candidatas, retenção e privacidade | **Homologar controlador/operador/contato/DPO/hosting/região/prazo e bases legais**; não afirmar conformidade integral |
| 3.1.7: BOM | 1,5 | Planilha editável baseada no template, fórmulas, separação de protótipo e referências de preço público | **Cotar 8 itens sem preço**, frete, impostos e validar orçamento; valores de mão de obra hipotéticos não são custo real por TAG |
| 3.2: personas | Não informado | Três personas, método e declaração de uso de IA; card fiscal corrigido | Validar dados hipotéticos com usuários; não apresentar amostra sintética como entrevista |
| 3.3.1, 3.3.2, 3.3.4: RF/RN/RTM | Não informado | 29 RF e 20 RN presentes na RTM, evidências/CTs futuros separados | Implementação dos RF de backend/MQTT/BI é futura, não comprovada por Serial |
| 3.4.1: Wokwi | Não informado | Firmware com `setup`/`loop`, classe própria e funções auxiliares; LEDs e botões; roteiro de testes e registros prévios | **Compilar ESP32, executar e registrar nova rodada CT18, CT22, CT09 e publicar o integrado**, preservando reprovações históricas |

As seções 1, 1.1, 1.2, 1.3 e 3.1.1–3.1.7 permanecem abaixo do limite de 300 palavras em contagem conservadora. Os anexos complementares contêm profundidade sem expandir os textos da rubrica.

## Firmware — alterações e limites

- Uma classe `ScreenController` centraliza as telas LCD (versão integrada); as funções `setup()` e `loop()` utilizam instância da classe, além de outras funções auxiliares.
- Captura de bordas do lacre com buffer temporal na interrupção e debounce isolado testável, para reduzir perda de transições quando o loop fica ocupado. **CT18 não foi repetido no Wokwi**; backlog testado em host com 20/20 transições estáveis, que não comprovam operação de hardware.
- Contexto e fila persistidos em dois snapshots alternados com geração/CRC e verificação de gravação; em caso de corrupção de um slot, o teste de host restaura o anterior. Falhas de flash e reset abrupto de hardware ainda exigem ensaio real. **CT22 permanece historicamente reprovado**.
- Eventos entram na fila persistente antes de uma emissão serial `[TX_SIM]`; somente `ACK XXXXXXXX` **manual de bancada** remove o primeiro item. Sem ACK, há retentativa; token incorreto/antigo não remove. **O token CRC32 não é autenticação**, não existe receptor externo nem MQTT produtivo, e **CT09 permanece bloqueado**.
- Limites: a fila tem oito registros; registros não críticos podem ser descartados; violações excedentes são **agregadas** e perdem os payloads individuais, guardando apenas contagem e extremos temporais. Não prometer ausência absoluta de perda individual.
- Monitor Serial de diagnóstico NVS limitado a 16 registros, com overflow explicitado.

## Testes efetivamente executados nesta revisão

| Verificação | Resultado observado | Alcance |
|---|---|---|
| Testes portáveis C e C++ de regras de ciclo/GNSS, LEDs, snapshots, debounce, token de ACK e integração do `main.cpp` via dublês | **PASS** | Host; não simula ESP32/Wokwi |
| Checagem sintática C++ `main.cpp` com stubs, `DEMO_MODE=1` e `=0` | **PASS**, dois modos | Compilador host; não linka Arduino/ESP32 |
| Verificação estrutural, links locais, seções, RTM, template e casos documentados | **31/33** | 2 pendências externas deliberadas: LGPD institucional e Wokwi publicado |
| Checagem estática do pacote com BOM Excel íntegra | **11/11 PASS** | Estrutura/consistência; não é teste de conteúdo fiscal e funcional |
| Aplicação da alteração na árvore limpa do commit `a70d1d6` | **PASS** | Não significa merge ou push |
| `pio run -e demo`, `pio run -e weekly` da versão atual | **NÃO EXECUTADO** | Toolchain ausente; instalação impedida por DNS/acesso ao índice de pacotes |
| Wokwi integrado atualizado e novos CT18, CT22, CT09 | **NÃO EXECUTADO** | Requer ambiente de simulação, integração do chip custom e evidências |
| LGPD, preço definitivo, identidade institucional, integração `main` | **NÃO HOMOLOGADO** | Dependências da instituição, fornecedores e conta GitLab/Wokwi |

Arquivos: `scripts/verify-host.sh`, `scripts/check-firmware-syntax.sh`, `scripts/audit-docs.py`, `scripts/validate-release.py`, `.gitlab-ci.yml`; evidências históricas `assets/3-desenvolvimento-resultados/3.4.1-prototipo-wokwi/`.

## Como concluir sem mascarar falhas

1. Aplicar o patch **na revisão-base exata** com `git apply --check --binary` e `git apply --binary`; usar branch de revisão, não sobrescrever `main`.
2. Rodar verificações host e pipeline; compilar `pio run -e demo` e `pio run -e weekly` em PlatformIO com dependências; corrigir qualquer erro real.
3. Executar o circuito e os testes da disciplina (inclusive casos malsucedidos) com IDs de caso, estímulos, oráculo, configuração, hash do sketch, prints e logs; não substituir CT09 por ACK manual.
4. Publicar o Wokwi correspondente, conferir URL e acesso; obter aceite sobre GitLab versus GitHub.
5. Completar BOM com cotações datadas, quantitativos/frete e excluir valores hipotéticos do custo real de dispositivo. Confirmar dados LGPD com o parceiro antes de publicá-los como política efetiva.
6. Revisar imagem/hipóteses das personas e indicadores com usuários do piloto; documentar validação com evidência; integrar por MR na `main` e reabrir versão final para auditoria.

**Decisão de liberação:** liberar somente quando critérios avaliados estiverem demonstrados por evidência vinculada ao mesmo commit e os responsáveis homologarem as partes institucionais. O patch não deve ser tratado como conclusão dessas etapas.

## Verificações adicionais (24/10/2026, ambiente de trabalho)

- Foi executado `tests/firmware_integration_tests.cpp` com `main.cpp` real, entradas GPIO, Serial e NVS falsificadas; o caso passou com 20 bordas estáveis, ACK inválido/válido pelo parser, falha de escrita de fila e contexto, recuperação CRC de slot anterior. **Não equivale a Wokwi/ESP32**.
- GitLab CI preparado para `pio run -e demo` / `pio run -e weekly` e Wokwi CLI condicional. **Não há logs de execução desses jobs** no repositório revisto.
- A BOM passou a conter preços referenciais de quatro componentes de bancada adicionais; restam **8 linhas sem preços**. A coluna de cenários foi corrigida para não apresentar o preço de um único GNSS como custo conhecido completo.
- As declarações de aprovação de CT18/CT22/CT09 mantêm o estado histórico e a exigência de nova evidência.
