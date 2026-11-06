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

<a name="cenarios-planejados"></a>

## Cenários de teste planejados (não executados)

| Novo caso proposto | RF/RN | Estímulo, oráculo e tipo de teste | Sprint prevista |
|---|---|---|---|
| CT-F01 | RF001 | Criar e consultar equipamento por identificador e foto; rejeitar duplicidade. Integração. | 2–3 |
| CT-F02 | RF002 / RN02 | Abrir ciclo com/sem NF, fornecedor, responsável e prazo; ausência deve bloquear. Integração. | 2–3 |
| CT-F03 | RF003, RF004 / RN01 | Vincular um dispositivo a um ciclo e tentar vinculá-lo a outro ciclo ativo: recusar e preservar o vínculo original (RF004). Em cenário separado, tentar abrir simultaneamente dois ciclos para o mesmo equipamento usando dispositivos distintos: aceitar no máximo um (RN01). Integração. Depende de entrada de vínculo acessível com ciclo ativo: hoje o botão chama encerramento, sem alcançar `VINCULO_RECUSADO` em `requestLink()`. | 2–3 |
| CT-F08 | RF008 | Derrubar Wi-Fi real no simulador/placa, medir tentativa e reconexão; não usar SW_REDE como prova. Dispositivo. | 2 |
| CT-F09 | RF009 | Inspecionar tópico, JSON, autenticação e ACK MQTT de receptor independente. Integração. | 2–3 |
| CT-F12 | RF012 / RN12 | Comparar diferença de datas com NF de remessa, horário e fusos; reiniciar sem alterar origem. Integração. | 3 |
| CT-F15 | RF015 / RN08, RN15 | Consultar na interface última posição válida, lacre, bateria, prazo, vínculo e atualização do backend. Sistema. | 3 |
| CT-F17 | RF017 / RN11 | Encerrar, reutilizar dispositivo, consultar integralmente os dois históricos após reinício. Sistema. | 3 |
| CT-F18 | RF018 / RN18 | Confirmar envio ao responsável, entrega no canal e trilha de ciência. Integração. | 3–4 |
| CT-F20 | RF020 / RN16 | Cadastrar e revogar um device com segredo exclusivo; rejeitar desconhecido. Segurança. | 3 |
| CT-F21 | RF021 / RN15/17 | UID imutável, autenticação, perfis e acesso negado sem credencial. Segurança. | 3 |
| CT-F22 | RF022 / RN15 | Gráficos, faixas, mapa, fonte dos dados e filtros por tipo/ciclo. UI. | 4 |
| CT-F23 | RF023 / RN15, RN18 | Atribuir alerta, registrar tratamento, trilha sem sobrescrever histórico. Sistema. | 4 |
| CT-F24 | RF024 / RN15 | API com credencial, paginação, IDs, isolamento entre papéis e contrato. Integração. | 4 |
| CT-F25 | RF025 / RN14 | Relógio do servidor identifica lacuna de comunicação e elimina duplicatas. Backend. | 3–4 |
| CT-F26 | RF026 / RN13 | Parâmetros legais validados pelo Fiscal, modalidades distintas e exceções. Negócio/integração. | 3–4 |
| CT-F28 | RF028 / RN19 | Alerta preventivo antes do prazo, sem duplicar atraso, limites configurados e aprovados. Sistema. | 3–4 |
| CT-F29 | RF029 / RN20 | Registrar retorno físico pendente; tentar encerrar sem baixa NF: recusar. Sistema. | 3 |
| CT-F30 | RF030 / RN18 (tratamento) | Com prazos e destinatários fictícios previamente acordados, avançar o relógio pelos limites de escalonamento; verificar no receptor independente a entrega ao requisitante, coordenador e diretor na etapa correta, sem destinatários indevidos nem repetição para a mesma etapa. Registrar ciclo, etapa e horários. Integração. Depende de canal de e-mail, configuração e aprovação das faixas; não executado. | 3–4 (proposta) |
| CT-U01 | RNF04.1, RNF04.3, RNF04.4; RF015, RF017, RF022, RF023, RF029 / RN08, RN20 | Teste moderado, sem ajuda, com 9 participantes representativos (3 por perfil) e 2 tarefas por participante: PCM localiza alerta e interpreta posição/horário; Fiscal identifica retorno físico e pendência fiscal; Logística consulta histórico e filtra pendências. Registrar acertos, tempos e erros por perfil. Exigir ao menos 5/6 acertos e mediana de até 3 minutos por perfil, sem interpretar retorno sem baixa fiscal como encerramento. Metas propostas, a validar; depende do painel e dos participantes; não executado. | 3–4 (proposta) |
| CT-Q01 | RNF07.3; RF003, RF016, RF017; RN01, RN11 | Reproduzir **duas vezes de modo independente**, em host/simulador e a partir de log anonimizado, um defeito já observado. Fixar versão, configuração e estímulos; a mesma condição defeituosa e a sequência lógica devem reaparecer. Salvar entradas, dois logs e comparação. Qualidade/manutenibilidade; **planejado, não executado**. | 3 (proposta) |
| CT-Q02 | RNF07.4; RF003, RF004, RF016, RF017; RN01, RN11 | Repetir **duas execuções** do mesmo roteiro versionado, coletar estados e eventos, comparar diff e classificar qualquer divergência temporal/física justificada. Guardar logs completos, parâmetros, SHA e resultado. Qualidade/manutenibilidade; **planejado, não executado**. | 3 (proposta) |

**Evidências não equivalentes:** CT08/09 não provam reconexão Wi-Fi; CT01/02 não provam MQTT; CT19/20 não provam listagem RF015; CT03/04 não provam NF real como marco temporal; CT14 não prova histórico consultável. Atualizar a RTM ao executar os novos CTs, mantendo IDs, data, firmware SHA, ambiente, estímulos e logs brutos.

## Correções e análise dos testes históricos

- **CT18 — REPROVAÇÃO HISTÓRICA:** ensaio legado reportou 16/20 transições sem rede; a captura por interrupção (ISR) foi proposta na revisão de 23/10 e adiada para a Sprint 2, por exigir novo reteste completo. Reexecutar exatamente os 20 pulsos e ampliá-los para ≥30, com contagem, timestamp, latências e métrica de overflow. Inclua situação de NVS indisponível. Atualização de 05/11/2026: o [reteste no Wokwi](../../../assets/3-desenvolvimento-resultados/3.4.1-prototipo-wokwi/v2-ct18-reteste.serial.txt) passou nas contagens, assim como o [CT65 físico com jumper](../../../assets/3-desenvolvimento-resultados/3.4.2-prototipo-fisico-offline/ct65-pulsos-lacre.serial.txt), com 20 transições por condição e nenhum evento nos pulsos de 20/40 ms. Permanecem planejados a ampliação do lote, a latência externa, overflow e falhas NVS; os retestes não apagam as reprovações anteriores.
- **CT22 — REPROVADO (parcial: uptime):** CT22.a reprovou porque o uptime não reiniciou como esperado e houve watchdog; CT22.b observou retenção das filas `q0..q2` e `p0..p3`; CT22.c observou referência A retida e comparação de B com A. As três asserções pertencem à rodada de 23/10/2026, no [log CT22](../../../assets/3-desenvolvimento-resultados/3.4.1-prototipo-wokwi/CT22.serial.txt). Retenção local não comprova o painel nem aprova o caso global. A investigação do watchdog e dos snapshots duplos ficou para a Sprint 2. Repetir reset manual, watchdog e interrupções em cada etapa da gravação, preservando o oráculo original. O [CT62 físico de 05/11/2026](../../../assets/3-desenvolvimento-resultados/3.4.2-prototipo-fisico-offline/ct62-reinicio-real.serial.txt) comprovou reinício do uptime e restauração de ciclo/lacre; não comprovou toda a fila, a última posição ou o histórico integral. CT68 permanece planejado para corte durante gravação.
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
