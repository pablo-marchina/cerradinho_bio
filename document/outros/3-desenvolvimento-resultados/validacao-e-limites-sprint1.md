# Plano de medição, rastreabilidade e validação — Sprint 1

> Documento complementar à seção 3.1 e à RTM. Metas preliminares não são resultados atingidos. Nenhum dado institucional, fiscal ou pessoal deve ser considerado homologado sem confirmação documentada do parceiro.

## Indicadores operacionais do piloto

| ID | Definição reproduzível | Numerador / denominador e janela | Amostra e responsável | Condição de validade |
|---|---|---|---|---|
| KPI-01 posição GNSS | % de amostras programadas com posição externa válida | fixes válidos / tentativas programadas, por dispositivo, 4 semanas | Até 5 dispositivos representativos; Firmware/PCM | Excluir apenas indisponibilidade classificada e documentada; capturar HDOP, idade, UTC GNSS |
| KPI-02 consulta | Tempo para encontrar situação atual, prazo, NF e última posição | consultas completas ≤120 s / consultas iniciadas; p50, p90 | ≥20 tarefas por perfil, usuários convidados; UX | Medição cega com cronômetro, protótipo demonstrado e mesmo roteiro |
| KPI-03 precisão alertas | % de alertas sem condição real correspondente | falsos positivos confirmados / alertas analisados, por tipo; mínimo 30 alertas | QA/PCM, com dupla revisão e dataset rotulado | Definir primeiro ground truth de autorização, prazo e violação; não dividir por eventos normais |
| KPI-04 latência aviso | Tempo entre lacre confirmado e mensagem recebida pelo responsável | p95 em minutos de `(recebido - ocorrido)`, com e sem rede | ≥30 eventos por condição, QA | **Não mensurável nesta Sprint**: Serial não comprova recebimento; meta ≤15 min é prospectiva |
| KPI-05 integridade | Ciclos corretos após operação e reinício | ciclos sem duplicidade/perda de metadados / ciclos ensaiados | ≥5 ciclos + falhas injetadas, QA | Relatar separadamente ruptura entre escrita pendente e confirmação NVS |
| KPI-06 custo | Custo por dispositivo e por ciclo, segregando capital e recorrência | total aprovado de materiais + serviços + frete / dispositivos; amortização em ciclos reais | 1 e 5 unidades, Hardware/Compras | Sem falsa precisão: qualquer linha não cotada mantém total como parcial |

## Cenários para CTs futuros (não executados)

| Novo caso proposto | RF/RN | Estímulo, oráculo e tipo de teste | Sprint prevista |
|---|---|---|---|
| CT-F01 | RF001 | Criar e consultar equipamento por identificador e foto; rejeitar duplicidade. Integração. | 2–3 |
| CT-F02 | RF002 / RN02 | Abrir ciclo com/sem NF, fornecedor, responsável e prazo; ausência deve bloquear. Integração. | 2–3 |
| CT-F03 | RF003 / RN01 | Vincular device a ciclo correto e testar tentativa concorrente em dois dispositivos. Integração. | 2–3 |
| CT-F08 | RF008 | Derrubar Wi-Fi real no simulador/placa, medir tentativa e reconexão; não usar SW_REDE como prova. Dispositivo. | 2 |
| CT-F09 | RF009 | Inspecionar tópico, JSON, autenticação e ACK MQTT de receptor independente. Integração. | 2–3 |
| CT-F12 | RF012 / RN12 | Comparar diferença de datas com NF de remessa, horário e fusos; reiniciar sem alterar origem. Integração. | 3 |
| CT-F15 | RF015 / RN08 | Consultar na interface última posição válida, lacre, bateria, prazo, vínculo e atualização do backend. Sistema. | 3 |
| CT-F17 | RF017 / RN11 | Encerrar, reutilizar dispositivo, consultar integralmente os dois históricos após reinício. Sistema. | 3 |
| CT-F18 | RF018 / RN18 | Confirmar envio ao responsável, entrega no canal e trilha de ciência. Integração. | 3–4 |
| CT-F20 | RF020 / RN16 | Cadastrar e revogar um device com segredo exclusivo; rejeitar desconhecido. Segurança. | 3 |
| CT-F21 | RF021 / RN15/17 | UID imutável, autenticação, perfis e acesso negado sem credencial. Segurança. | 3 |
| CT-F22 | RF022 | Gráficos, faixas, mapa, fonte dos dados e filtros por tipo/ciclo. UI. | 4 |
| CT-F23 | RF023 / RN18 | Atribuir alerta, registrar tratamento, trilha sem sobrescrever histórico. Sistema. | 4 |
| CT-F24 | RF024 / RN15 | API com credencial, paginação, IDs, isolamento entre papéis e contrato. Integração. | 4 |
| CT-F25 | RF025 / RN14 | Relógio do servidor identifica lacuna de comunicação e elimina duplicatas. Backend. | 3–4 |
| CT-F26 | RF026 / RN13 | Parâmetros legais validados pelo Fiscal, modalidades distintas e exceções. Negócio/integração. | 3–4 |
| CT-F28 | RF028 / RN19 | Alerta preventivo antes do prazo, sem duplicar atraso, limites configurados e aprovados. Sistema. | 3–4 |
| CT-F29 | RF029 / RN20 | Registrar retorno físico pendente; tentar encerrar sem baixa NF: recusar. Sistema. | 3 |

**Evidências não equivalentes:** CT08/09 não provam reconexão Wi-Fi; CT01/02 não provam MQTT; CT19/20 não provam listagem RF015; CT03/04 não provam NF real como marco temporal; CT14 não prova histórico consultável. Atualizar a RTM ao executar os novos CTs, mantendo IDs, data, firmware SHA, ambiente, estímulos e logs brutos.

## Correções e análise dos testes históricos

- **CT18 — REPROVADO:** ensaio legado reportou 16/20 transições sem rede; a captura por interrupção (ISR) foi proposta na revisão de 23/10 e adiada para a Sprint 2, por exigir novo reteste completo. Reexecutar exatamente os 20 pulsos e ampliá-los para ≥30, com contagem, timestamp, latências e métrica de overflow. Inclua situação de NVS indisponível.
- **CT22 — REPROVADO NO AMBIENTE:** NVS previamente validada, mas uptime não reiniciou como esperado e houve watchdog. A investigação do watchdog e dos snapshots duplos ficou para a Sprint 2. Repetir reset manual, watchdog e interrupções em cada etapa da gravação, preservando o oráculo original.
- **CT09 — BLOQUEADO:** Manter bloqueio até implementar o envio MQTT com confirmação do servidor (Sprint 3). Nunca modificar relatório histórico para fazê-lo passar.

## Matriz de consistência das personas

| Persona | Informações de pesquisa (TAPI/kick-off) | Hipóteses de pesquisa | Exclusões explícitas |
|---|---|---|---|
| Geraldo/PCM | Acompanhamento, lacre, fornecedor, prazo, cobranças | Demografia, letramento 3/5, metas de interface e preferência de aviso | Rastreio contínuo de pessoal não previsto |
| Clarice/Fiscal | Nota de remessa/retorno e pendência após retorno físico | Idade/formação, letramento 4/5, alerta de portaria, prazo específico 270/360 dias | Emissão NF-e é tarefa do sistema fiscal, não função do GuaráTracking |
| Izidio/Logística | Consolidação de dados e BI, ciclos fora, acompanhamento ICMS | Demografia, letramento 4/5, formato exato de API | Cálculo/tributação definitiva não será automatizado sem validação Fiscal |

**Plano de validação:** entrevistar ao menos um representante por perfil, observar tarefa real (com dados minimizados), coletar comentários anônimos, ajustar cenários e documentar consentimento/justificativa institucional da pesquisa.

## LGPD — decisões ainda obrigatórias antes do piloto real

| Item | Responsável sugerido | Evidência de aceite |
|---|---|---|
| Controlador, operador, CNPJ, encarregado | Jurídico/DPO + parceiro | Confirmação assinada ou e-mail institucional |
| Base legal por finalidade e mapeamento de dados | Jurídico/DPO | Inventário de tratamento aprovado |
| Fornecedores, região, transferências internacionais | TI/Contratos | Lista de operadores/suboperadores, contratos e fluxo de dados |
| Retenção por categoria, backup e descarte | Segurança/Jurídico | Tabela de prazos e descarte validado |
| Política de cookies e canais de titulares | DPO/Produto | Política publicada com contato aplicável ao produto |
| Segurança implementada vs planejada | Segurança/Engenharia | Evidências de teste de acesso, TLS, proteção de segredos e resposta a incidente |

## BOM — hipóteses e critérios de compra

Orçar pelo menos duas alternativas em itens com risco de estoque; levantar data, fornecedor, moeda, prazo de entrega e comprovante. Confirmar antena compatível com módulo GNSS escolhido, proteção de Li-ion, autonomia estimada com duty cycle, fixação do gabinete e requisitos da operadora (SIM e mensalidade). O CSV/planilha anexos separam hardware do circuito Wokwi de componentes físicos e contratos de dados. Total parcial não é preço final.

## Porter / riscos — fundamentos de classificação

A intensidade da rivalidade considera fabricantes de telemetria, prestadores de rastreamento de ativos e softwares de manutenção, mas diferencia **substitutos organizacionais** (planilha, SAP e contatos telefônicos) da concorrência direta de solução integrada. Poder de fornecedores cresce na conectividade rural, módulos GNSS e serviços de oficina; poder de compradores é elevado em grandes clientes corporativos que demandam integração, confiabilidade e ROI; entrantes digitais têm baixo custo de software, porém validação industrial e hardware criam barreiras; substitutos operacionais persistem enquanto não houver valor comprovado.

Escalas de probabilidade/impacto na matriz de riscos são **estimativas qualitativas**, não frequência observada. Responsáveis devem revisar cada pontuação após medição e registrar gatilho, contingência e impacto no teste piloto, por exemplo: R02 cobertura — gatilho HDOP/ausência de fix no galpão; R04 rede — >2 ciclos de envio offline; R05 lacre — taxa de falsos positivos >5%; R10 privacidade — acesso indevido ou dado pessoal sem base. Sem validação em campo, não publicar essas estimativas como estatísticas de ocorrência.

## Checkpoint de entrega

Executar no hardware/simulador oficial; registrar SHA de commit, binário (hash), log, captura, data, pessoa e resultado; atualizar o link Wokwi integrado; definir se GitLab institucional satisfaz instrução que diz GitHub; rever links e os limites de 300 palavras das seções de negócios; só então fazer merge request `develop → main`. O patch não substitui publicação, homologação, cotação ou prova de aceitação.

## Novos ensaios locais para transporte

Um protocolo de confirmação manual pela Serial (ACK de bancada) foi proposto na revisão de 23/10 e adiado para a Sprint 2, junto com o reteste completo que ele exige. Nenhum CT histórico foi reclassificado.
