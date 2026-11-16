# Especificação de requisitos — Dashboard GuaráTracking

> **Escopo da entrega frontend:** as exigências de MQTT, API, autenticação e persistência abaixo permanecem requisitos de produto/integração futura. As telas e comandos iniciais serão simulados; não comprovam a seção 3.4.3.

**Versão:** 1.0 · **Data:** 11/11/2026 · **Status:** proposta para validação pelo grupo, pela orientação e pelo parceiro.
**Escopo:** interface web, backend, banco de dados, integração MQTT e API do protótipo IoT que acompanha equipamentos remetidos à manutenção externa.
**Base:** TAPI final Cerradinho, `document/documentacao.md` (RF001–RF035, RN01–RN22, RNF01–RNF08, RTM), firmware `src/firmware/main.cpp` e `include/`, planejamento do Artefato 8/10 da Sprint 3, feedbacks das Sprints 1/2, benchmark Trackunit/Fleetio/MaintainX/Traccar/ThingsBoard e guia de estilos GuaráTracking v2.

> **Legenda de priorização:** **P0/S3** = essencial para a demonstração integrada da Sprint 3; **P1/MVP** = mínimo do piloto/escopo do parceiro, mesmo que não caiba integralmente na Sprint 3; **P2** = evolução ou refinamento. **Origem**: `[PROJETO]` já tem previsão ou regra no TAPI/IoTDoc; `[PROPOSTO]` é requisito de solução sugerido para tornar a interface segura, verificável ou melhor; `[RUBRICA]` é critério acadêmico. Não interpretar especificação como implementação concluída.

## 1. Página inicial e navegação

| ID | Prioridade | Requisito e critério de aceite | Rastreabilidade |
|---|---|---|---|
| DASH-VIS-01 | P0/S3 | Exibir o nome GuaráTracking, identificação do ambiente (`DEMONSTRAÇÃO — DADOS FICTÍCIOS` se aplicável), equipamento/dispositivo selecionado e horário da última atualização do painel. Um dado simulado nunca deve parecer telemetria real. | [PROPOSTO]; política de conteúdo restrito |
| DASH-VIS-02 | P0/S3 | No topo do detalhe, mostrar claramente **lacre**, **bateria**, **conexão MQTT**, **última comunicação** e **fila conhecida**; dados ausentes aparecem como `indisponível`, nunca zero inventado. | [PROJETO] RF015, RF019, RF025 |
| DASH-VIS-03 | P1/MVP | Exibir cards de `em manutenção`, `atrasados`, `permanência ≥ limiar`, `violação do lacre não tratada`, `sem comunicação esperada` e `retorno físico/baixa fiscal pendente`, cada um com filtro clicável e contagem derivada de dados reais do sistema. | [PROJETO] RF013–RF015, RF022, RF023, RF025, RF029 |
| DASH-VIS-04 | P1/MVP | Apresentar uma tabela prioritária de equipamentos que exigem ação, com equipamento/ID, fornecedor, responsável, prazo, dias fora, lacre, última posição e horário; cada linha abre o detalhe correto. | [PROJETO] RF015, RF022; persona Geraldo |
| DASH-VIS-05 | P1/MVP | Implementar navegação consistente entre Visão Geral, Equipamentos, Alertas, Mapa, Detalhe do Ciclo, Dispositivos e Histórico. O estado do filtro deve ser preservado ao voltar do detalhe. | [PROPOSTO]; benchmark Fleetio/Trackunit |
| DASH-VIS-06 | P1/MVP | Permitir busca por identificador/nome do ativo, TAG, NF de remessa e fornecedor, com filtros combináveis e ordenação por urgência, prazo e última comunicação. | [PROJETO] RF015/RF022; [PROPOSTO] UX |
| DASH-VIS-07 | P2 | Permitir indicadores gerenciais por fornecedor, faixa de dias fora, tipos de alerta, evolução temporal e taxa de recuperação, sempre distinguindo valores medidos de estimados. | [PROJETO] RF022; [PROPOSTO] métricas |

## 2. Cadastro, ciclo e regras fiscais

| ID | Prioridade | Requisito e critério de aceite | Rastreabilidade |
|---|---|---|---|
| DASH-CIC-01 | P1/MVP | Cadastrar e consultar ao menos **5 equipamentos reais ou representativos**, com ID exclusivo, descrição, foto ilustrativa/opcional e informações de identificação; usar dados anonimizados quando a implantação for acadêmica/pública. | [PROJETO] RF001; TAPI MVP 1 |
| DASH-CIC-02 | P1/MVP | Cadastrar fornecedores e responsáveis por ciclo, identificáveis na lista e no detalhe. Exigir referência válida sem exibir dados corporativos restritos em deploy público. | [PROJETO] RF002; TAPI |
| DASH-CIC-03 | P0/S3 | Mostrar ID do ciclo, NF de remessa, dias fora, prazo previsto, estado `no prazo/vencendo/atrasado`, limiar de permanência e estado `ativo/encerrado` do ciclo simulado; diferenciar o dado de demonstração do dado real. | [PROJETO] RF002/RF012–RF015; RN03/RN04 |
| DASH-CIC-04 | P1/MVP | A abertura completa de um ciclo só é aceita após NF de remessa (número **e data de emissão**), fornecedor, responsável, prazo previsto e equipamento; também definir TAG vinculada. Validar campos antes de alterar o estado. | [PROJETO] RN02, RF002/RF003 |
| DASH-CIC-05 | P1/MVP | Impedir **mais de um ciclo ativo por equipamento e por dispositivo** inclusive sob duas requisições concorrentes. Em recusa, manter o vínculo anterior sem mudança. | [PROJETO] RN01, RF003/RF004 |
| DASH-CIC-06 | P1/MVP | Calcular `dias fora` com base na data de emissão da NF de remessa conforme regra documentada; usar datas/fusos consistentes, não `uptime` do ESP32 como relógio fiscal. | [PROJETO] RF012; RN12; RNF03.1 |
| DASH-CIC-07 | P1/MVP | Exibir **atraso do prazo previsto** e **permanência acima do limiar** como condições independentes e simultâneas; limiar inicial de 150 dias, configurável por ciclo com inteiro >0. | [PROJETO] RN03/RN04; RF013/RF014 |
| DASH-CIC-08 | P1/MVP | Registrar `retorno físico confirmado`, `NF de retorno lançada/pendente`, `lacre conferido`, `baixa fiscal pendente` e `encerrado` sem confundi-los. Se faltar NF de retorno ou lacre estiver aberto, a solicitação de encerramento é recusada e mostra o motivo. | [PROJETO] RN11/RN20, RF016/RF029; RNF04.4 |
| DASH-CIC-09 | P1/MVP | Após encerrar, liberar TAG para um novo ciclo sem excluir os registros antigos. A página do ativo deve conservar todo o histórico anterior e o dispositivo deve mostrar vínculos anteriores corretamente. | [PROJETO] RF017, RN01/RN11 |
| DASH-CIC-10 | P2 | Oferecer prorrogação com nova remessa simbólica e novo ciclo associado, preservando o anterior e reiniciando a contagem somente a partir da nova NF. | [PROJETO] RN12 |
| DASH-CIC-11 | P2 | Mostrar prazos fiscais especiais **apenas quando parametrizados e aprovados pelo Fiscal**, por UF/tipo de operação/prorrogação; não tratar 270/360 dias citados no caso de GO como prazo universal. | [PROJETO] RN13, RF026 |

## 3. Lacre, bateria, localização e conectividade

| ID | Prioridade | Requisito e critério de aceite | Rastreabilidade |
|---|---|---|---|
| DASH-TEL-01 | P0/S3 | Exibir lacre como `Íntegro`, `Retirada autorizada/em andamento`, `Violado`, `Aguardando reaplicação` ou `Indeterminado` quando o dado não permitir conclusão; violação aberta tem destaque textual e visual. | [PROJETO] RN05/RN06, RF006/RF007; [PROPOSTO] indeterminado |
| DASH-TEL-02 | P0/S3 | Ao receber `VIOLACAO_LACRE`, apresentar notificação destacada **fora da cadência semanal**, preservando horário da ocorrência; não apagar o registro histórico ao rearmar/fechar o lacre. | [PROJETO] RN05, RF006/RF032 |
| DASH-TEL-03 | P0/S3 | Exibir bateria em `%` e rótulo `Simulado` quando originada de potenciômetro; abaixo de 20% destacar alerta; zero identificado como falha do sensor, não 0% normal. A nova emissão de alerta só rearma após passar de 25%. | [PROJETO] RN10, RF019/RF027; limiares pendentes de validação |
| DASH-TEL-04 | P0/S3 | Exibir `Conectado ao broker`, `Conexão perdida (LWT)` e `Desconhecido`, com último horário do estado. LWT só representa **desconexão anormal**; indisponibilidade intencional ou modo de economia precisam de estado separado. | [PROPOSTO] MQTT LWT + RF008/RF025 |
| DASH-TEL-05 | P1/MVP | Exibir última comunicação recebida, periodicidade configurada e alerta `sem atualização esperada` após **dois intervalos de status** (14 dias quando semanal); não confundir com conexão momentânea ao broker. | [PROJETO] RN07/RN14, RF011/RF025; parâmetro a validar |
| DASH-TEL-06 | P0/S3 | Exibir `fila local: N pendentes` e `última informação da fila em [hora]`; se offline, marcar **valor possivelmente desatualizado**. Tratar limite atual de 8 eventos e mostrar contador/resumo de excedentes se reportado. | [PROJETO] RN09/RF010; [PROPOSTO] frescor de metadados |
| DASH-TEL-07 | P1/MVP | Ao reconectar, reconciliar reenvio de eventos em ordem de ocorrência; mostrar quantos eventos foram confirmados no backend e quantos ainda estão pendentes. Não afirmar `sincronizado` apenas por `Serial.println` ou QoS do broker. | [PROJETO] RF009/RF010; RN09; [PROPOSTO] ACK aplicação |
| DASH-TEL-08 | P0/S3 | Exibir última localização com latitude/longitude, horário de coleta, **idade da posição** e rótulo `Última posição válida`. Não rotular uma localização anterior como `posição atual`; sem fix, mostrar ausência e preservar a última válida historicamente. | [PROJETO] RN08, RF005/RF031, RNF04.2 |
| DASH-TEL-09 | P1/MVP | Informar `última comunicação` separadamente de `última localização`; mostrar qualidade/fonte da posição quando disponível (fix, HDOP, satélites) e razão de `GNSS_SEM_FIX` ou `GNSS_RECUPERADO`. Não afirmar conformidade RN08 sem data GNSS absoluta e HDOP validados. | [PROJETO] RN08/RN22, RF031 |
| DASH-TEL-10 | P1/MVP | Exibir timestamp da ocorrência, timestamp de recepção no servidor e fuso horário nos dados. Em demonstração com `dia_ocorrencia` ou `uptime`, avisar `tempo simulado/relativo`; não converter para UTC inventado. | [PROJETO] RN09/RNF03.1; [PROPOSTO] integridade temporal |
| DASH-TEL-11 | P2 | Exibir parâmetros de consumo, autonomia **estimada a partir de medição**, versão do firmware, diagnóstico da rede/sinal e manutenção do dispositivo apenas quando houver dados disponíveis. | [PROJETO] TAPI testes consumo; [PROPOSTO] diagnóstico |

## 4. Eventos, alertas e gestão de tarefas

| ID | Prioridade | Requisito e critério de aceite | Rastreabilidade |
|---|---|---|---|
| DASH-EVT-01 | P0/S3 | Feed de eventos em **ordem decrescente de ocorrência**, com tipo, severidade, equipamento, TAG, data/hora e origem; para eventos atrasados exibir também a data de recepção. | [PROJETO] RF010/RF015, RN09 |
| DASH-EVT-02 | P0/S3 | Registrar e exibir `VIOLACAO_LACRE`, `RETIRADA_AUTORIZADA`, `LACRE_REAPLICADO`, `LACRE_FECHADO`, `BATERIA_BAIXA`, `ALERTA_ATRASO`, `ALERTA_PERMANENCIA`, `GNSS_SEM_FIX`, `GNSS_RECUPERADO`, `CICLO_INICIADO` e `CICLO_ENCERRADO` quando emitidos. Tipos extras do firmware são mapeados com fallback legível. | [PROJETO] firmware V2/RN/RF |
| DASH-EVT-03 | P1/MVP | Exibir alertas distintos por `violação crítica`, `atraso`, `permanência`, `bateria`, `silêncio de comunicação`, `pré-vencimento` e `prazo fiscal` quando habilitados e validados. **Prevenção e regra fiscal** não aparecem como prontas antes de configuração aprovada. | [PROJETO] RF013/14/23/25/26/27/28; RN19 |
| DASH-EVT-04 | P1/MVP | Cada alerta deve ter estados `Aberto`, `Ciente`, `Em tratamento` e `Resolvido`; registrar responsável, decisão, justificativa, autor e horários de cada transição, sem sobrescrever ações antigas. | [PROJETO] RF023, RN17/RN18, RNF06.2 |
| DASH-EVT-05 | P1/MVP | Destacar pendência de ciência/tratamento que ultrapassar a meta de 1 dia útil **somente quando o prazo tiver sido validado** pelo parceiro. Escalonamento por e-mail é fluxo separado. | [PROJETO] RN18, RF018/RF030; parâmetros a validar |
| DASH-EVT-06 | P1/MVP | Oferecer filtros por tipo, gravidade, data, fornecedor, responsável, equipamento e estado do alerta, com opção de abrir o ciclo correspondente. | [PROPOSTO] benchmark Trackunit |
| DASH-EVT-07 | P1/MVP | Em mensagens duplicadas (por reconexão/reenvio), manter uma ocorrência por `event_id` e registrar recepções/retries sem duplicar alerta; contagens agregadas de overflow não viram eventos individuais fictícios. | [PROJETO] RN09; [PROPOSTO] idempotência |
| DASH-EVT-08 | P2 | Gerar notificações por e-mail aos responsáveis e escalonamento requisitante → coordenador → diretor apenas após validação da regra e cadastro dos destinatários. Cada tentativa de entrega é auditada. | [PROJETO] RF018/RF030; TAPI |

## 5. Ações, formulários e MQTT bidirecional

| ID | Prioridade | Requisito e critério de aceite | Rastreabilidade |
|---|---|---|---|
| DASH-CMD-01 | P0/S3 | Mostrar um formulário de comandos destinado à **TAG selecionada**, com ações: `Abrir/vincular ciclo`, `Registrar NF da remessa`, `Registrar NF de retorno`, `Autorizar retirada do lacre` e `Encerrar ciclo`; habilitar somente comandos suportados pelo firmware de demonstração. | [PROPOSTO] UI; [PROJETO] RF002/003/007/016/033 |
| DASH-CMD-02 | P0/S3 | Exigir confirmação explícita para `Autorizar retirada` e `Encerrar ciclo`; indicar dispositivo, ciclo e resumo da ação antes de enviar. Não permitir autorização sobre dispositivo errado. | [PROPOSTO] segurança e UX |
| DASH-CMD-03 | P0/S3 | Cada comando possui `command_id`, `device_id`, `cycle_id`, `type`, payload, `issued_at` e `expires_at`. O backend rejeita formato inválido e ações não autorizadas antes de publicar no MQTT. | [PROPOSTO] contrato de comandos |
| DASH-CMD-04 | P0/S3 | O dispositivo **subscreve um tópico de comandos**, o backend publica o comando e o dispositivo responde no tópico de `responses` com `command_id`, estado `executado/recusado` e motivo. Exibir na UI a resposta efetiva; isto complementa, sem substituir, a publicação de telemetria. | [RUBRICA] Artefato 8: subscribe+publish; [PROPOSTO] resposta correlacionada |
| DASH-CMD-05 | P0/S3 | Diferenciar `Criado`, `Publicado`, `Recebido`, `Executado`, `Recusado`, `Expirado` e `Sem resposta`; publicação no broker, PUBACK e resposta da aplicação **não** são a mesma prova. Em timeout, nunca exibir sucesso. | [PROPOSTO] integridade |
| DASH-CMD-06 | P0/S3 | Ao recusar comando, explicar o motivo: `sem ciclo`, `ciclo já ativo`, `NF ausente`, `NF inválida`, `lacre aberto`, `autorização expirada` etc.; mensagens legíveis sem depender do código bruto. | [PROJETO] RN01/RN02/RN06/RN11; firmware |
| DASH-CMD-07 | P1/MVP | O comando de abertura completo deve respeitar RN02 antes da criação do ciclo, embora o firmware Serial atual permita `VINCULAR` antes de `REMESSA`. **A API não deve expor essa sequência incompleta como fluxo final.** | [PROJETO] RN02; diferença bancada × produção |
| DASH-CMD-08 | P1/MVP | A autorização de retirada deve ter usuário, justificativa, instante e prazo de validade configurável; se dispositivo estiver inacessível, **não** garantir autorização aplicada nem armazenar uma autorização perigosa sem expiração e confirmação de aplicação. O prazo atual de 3 dias é hipótese não homologada. | [PROJETO] RN06/RN17/RN18; [PROPOSTO] segurança |
| DASH-CMD-09 | P1/MVP | O encerramento é transação verificada no servidor: NF de retorno registrada, lacre fechado, histórico persistido e condição fiscal/retorno coerente. A recusa mantém o ciclo aberto. | [PROJETO] RN11/RN20, RF016/RF029 |
| DASH-CMD-10 | P1/MVP | Proteger contra cliques repetidos e resposta atrasada: comando idempotente, expiração, correlação e bloqueio de envio duplicado; respostas de outro dispositivo nunca alteram a ação atual. | [PROPOSTO] robustez |
| DASH-CMD-11 | P2 | Formulários de limiar por ciclo, intervalo de status, responsáveis, revisões/retiradas e revogação de dispositivo; cada configuração exige permissões específicas e exibe a configuração efetivamente aplicada. | [PROJETO] RN03/RN07; [PROPOSTO] administração |

### Tópicos MQTT sugeridos (proposta, não existentes no código atual)

```
guaratracking/v1/devices/{device_id}/telemetry
guaratracking/v1/devices/{device_id}/events
guaratracking/v1/devices/{device_id}/availability
guaratracking/v1/devices/{device_id}/commands
guaratracking/v1/devices/{device_id}/responses
```

**Regras do transporte:** `availability` publica online na conexão e configura LWT `offline` para perda inesperada, sem inferir que desconexão deliberada deve disparar o LWT; a UI usa também `last_seen_at`, período esperado e hora do evento. Telemetria/eventos via QoS apropriado (proposta: QoS 1) e processamento idempotente; persistência e confirmação **de aplicação** exigem ACK e gravação na camada de negócio, não apenas PUBACK do MQTT. Segredos do broker devem ficar no backend e no dispositivo, **nunca no JavaScript público**. Retained messages e comandos com validade precisam de política explícita para evitar executar instrução antiga no retorno da TAG.

**Desvio técnico já conhecido:** `main.cpp` hoje usa Serial para `VINCULAR`, `REMESSA`, `NF` e `LIMIAR`; `publishEvent` imprime JSON e `flushPending` retira eventos sem ACK real do backend. O MQTT bidirecional e sua semântica de confirmação ainda precisam ser implementados e validados.

## 6. Histórico e mapa

| ID | Prioridade | Requisito e critério de aceite | Rastreabilidade |
|---|---|---|---|
| DASH-HIS-01 | P1/MVP | Exibir linha do tempo de um ciclo com envio, vínculo, posição, alertas, ciência, autorização, reaplicação, retorno físico, NF retorno e encerramento; identificar usuários e datas. | [PROJETO] RF017/RF023/RF029 |
| DASH-HIS-02 | P1/MVP | Permitir histórico cruzado por equipamento, TAG e ciclo, distinguindo vínculos anteriores e impedindo que a posição de um ciclo apareça no próximo sem associação explícita. | [PROJETO] RN01, RF017 |
| DASH-HIS-03 | P1/MVP | O mapa mostra **pinos da última posição válida**, com data/idade/qualidade e status de localização, e lista à parte os equipamentos sem posição. Não apresenta itinerário contínuo sem dados que o sustentem. | [PROJETO] RN08, RF005/RF022; TAPI sem rastreamento em tempo real |
| DASH-HIS-04 | P1/MVP | Clicar no pino abre o mesmo detalhe do ciclo da tabela; filtros por ativo/fornecedor/estado de GNSS funcionam sem vazar dados de usuários sem acesso. | [PROPOSTO] UX e segurança |
| DASH-HIS-05 | P2 | Permitir comparar as últimas posições históricas por ciclo, com intervalos reais entre registros e alertas de qualidade; não inferir trajetos exatos entre coletas espaçadas. | [PROPOSTO] rastreabilidade responsável |

## 7. API, dados, segurança e governança

| ID | Prioridade | Requisito e critério de aceite | Rastreabilidade |
|---|---|---|---|
| DASH-API-01 | P1/MVP | Persistir entidades `equipment`, `supplier`, `device`, `maintenance_cycle`, `location`, `event`, `alert`, `alert_action`, `command`, `command_response` e `user` com identificadores estáveis. Não usar o número da NF como identificador único universal. | [PROJETO] TAPI MVP 3/6; [PROPOSTO] modelagem |
| DASH-API-02 | P1/MVP | Oferecer API **autenticada e documentada** de consulta de equipamentos, vínculos, ciclos, últimas localizações, alertas e históricos; cliente BI consegue consumir os dados sem acessar diretamente o banco. | [PROJETO] RF024/RN15; TAPI MVP 6 |
| DASH-API-03 | P1/MVP | Usar JSON/UTF-8 e datas ISO 8601 com fuso, `null` para dados ausentes e identificação dos campos simulado/medido; consultar 12 ciclos fictícios conforme RNF03.1. | [PROJETO] RNF03.1/RNF03.3 |
| DASH-API-04 | P1/MVP | Aplicar autenticação na interface **e na API**, impedindo carregamento dos dados sem login e rejeitando requisição anônima. A interface não é barreira de autorização suficiente. | [PROJETO] RN15/RNF06.1 |
| DASH-API-05 | P1/MVP | Criar `usuario_id` imutável e perfis PCM, Fiscal, Logística e administração com matriz perfil × operação × recurso validada pelo parceiro; recusar leitura ou mutação fora das permissões. | [PROJETO] RF021; RN17; RNF06.7 |
| DASH-API-06 | P1/MVP | Registrar auditoria de cada comando sensível e ação de alerta com usuário, data/hora, alvo, resultado, motivo e identificador de correlação. Nunca incluir senha/token na trilha. | [PROJETO] RN17/RN18, RNF06.2; [PROPOSTO] comandos |
| DASH-API-07 | P1/MVP | Rejeitar mensagens de dispositivo desconhecido ou não autorizado. Cada TAG possui identidade e credencial próprias; o backend verifica assinatura/autenticação e consistência com ciclo antes da mutação. | [PROJETO] RF020; RN16/RNF06 |
| DASH-API-08 | P1/MVP | Usar transporte criptografado, proteção de segredos, validação de entrada e política de acesso por ambiente. Dados reais de NF, fornecedor, localização e rotas não podem ir para demos públicas. | [PROJETO] política de conteúdo restrito, RNF06; detalhes dependem da TI |
| DASH-API-09 | P1/MVP | Oferecer paginação, filtros e ordenação no backend para listas; impedir respostas gigantes, manipulação por IDs de outros usuários e dados entre ciclos trocados em consultas concorrentes. | [PROPOSTO] robustez; RNF06.7 |
| DASH-API-10 | P2 | Oferecer relatórios/exportação CSV anonimizados quando necessário e métricas de SLA de consulta/persistência; usar API como fonte única para dashboard e BI. | [PROJETO] RF022/RF024; [PROPOSTO] relatórios |

## 8. Qualidade, acessibilidade e verificabilidade

| ID | Prioridade | Requisito e critério de aceite | Rastreabilidade |
|---|---|---|---|
| DASH-UX-01 | P0/S3 | Toda tela possui estados de carregamento, vazio, erro de conexão, dado desatualizado e sucesso/recusa de comando; nenhuma falha silenciosa ou sucesso não confirmado. | [PROPOSTO] UX/confiabilidade |
| DASH-UX-02 | P1/MVP | Usar o guia visual GuaráTracking v2: identidade vermelho `#C8232C`, texto `#161414`, fundo `#F7F4F1`, destaques verde `#3F7D4E` e amarelo `#D89B2B`; fontes Fredoka/Atkinson Hyperlegible/JetBrains Mono conforme função. | [PROJETO] guia de estilos |
| DASH-UX-03 | P1/MVP | Estado nunca depende apenas de cor. Textos, ícones, foco visível, contraste, navegação por teclado e mensagens de sucesso/erro acessíveis com HTML semântico, compatíveis com WCAG 2.2 AA como **meta proposta**. | [PROPOSTO] acessibilidade; WCAG 2.2 |
| DASH-UX-04 | P1/MVP | Exibir datas absolutas e relativas (`há 8 dias · 03/11/2026 10:24`), indicar fuso local e ordenar temporalmente pelo evento ocorrido, não confundir recebido com ocorrido. | [PROJETO] RN08/RN09; [PROPOSTO] UI |
| DASH-UX-05 | P1/MVP | Testar os três perfis com CT-U01: 9 participantes (3 por perfil), 2 tarefas cada; mínimo **5 acertos em 6 por perfil**, mediana ≤3 minutos por perfil e zero interpretações de retorno físico pendente como ciclo encerrado. São **metas do IoTDoc, ainda não comprovadas**. | [PROJETO] RNF04.1/3/4 |
| DASH-UX-06 | P1/MVP | Não mostrar posição sem fix como atual e mostrar sempre a idade da última válida, inclusive se faltar rede, ao trocar ciclo ou após reinício. | [PROJETO] RNF04.2 |
| DASH-UX-07 | P1/MVP | Ensaio de alerta de lacre conectado com registro de latência: p95 ≤30 minutos em 30 estímulos (provisório, sujeito a validação); registrar separadamente evento local, chegada à plataforma e ciência do usuário. | [PROJETO] RNF02.1; limite a validar |
| DASH-UX-08 | P1/MVP | Testar consulta API em 12 ciclos fictícios, 30 requisições autenticadas com p95 ≤2 s e alteração persistida refletida em até 5 minutos; são **metas internas do IoTDoc**, não desempenho atual medido. | [PROJETO] RNF03.1/4/5 |
| DASH-UX-09 | P1/MVP | Documentar estados e fluxos com fluxograma e casos de uso, incluindo cenário de sucesso, recusa, desconexão, GNSS ausente, retorno fiscal pendente, violação e recuperação. | [RUBRICA] 3.4.3; [PROJETO] RTM |

## 9. Recorte de implementação por entrega

### Sprint 3 — P0/S3: mínimo demonstrável, não MVP de negócios concluído

1. Página de demonstração com seleção da TAG, estado do lacre, bateria simulada, última posição e idade, estado do MQTT/LWT e última comunicação, fila informada.
2. Feed cronológico de eventos com destaque de `VIOLACAO_LACRE` e carimbo de ocorrência.
3. Formulário funcional ao menos de comandos efetivamente implementados no firmware, com publicação MQTT **real**, subscribe no ESP32, resposta correlacionada e recusa/timeout visíveis; não apresentar formulário fictício como prova técnica.
4. Backend/gateway MQTT próprio com broker, credenciais não expostas no browser, assinatura de telemetria/eventos, comandos e respostas, memória/persistência mínima de demonstração.
5. Capturas reais da aplicação, broker e protótipo físico para **seção 3.4.3**; cada figura com descrição, preparação, entrada, esperado, observado e resultado. Uma imagem Figma é mockup, não prova de pub/sub MQTT. A rubrica exige ainda **LCD I2C, ≥2 LEDs informacionais e botão com debounce por hardware**; o dashboard não substitui essa parte.
6. Casos de teste mínimos: posição sem fix, lacre violado, retirada autorizada, fila offline, reconexão, comando aceito, comando recusado, timeout/sem resposta e evento fora de ordem.

### MVP Cerradinho — P1

O MVP deve executar um **ciclo de ponta a ponta** de cinco cadastros representativos: cadastro/NF/fornecedor/responsável/prazo → vínculo 1:1 → posição externa válida e comunicação → lacre/retirada → alerta/persistência/consulta → retorno físico e NF de retorno → encerramento/reutilização → API autenticada consumida em cliente BI. Os ensaios de bateria, fixação, exposição à água, consumo/cobertura e independência de Wi-Fi corporativo são responsabilidades do projeto, embora não sejam componentes visuais do dashboard.

### Evolução — P2

Gráficos avançados, exportações customizadas, programação extensiva de notificações, rotas históricas parciais, administração de dispositivos/limiares e integração produtiva com sistemas da Cerradinho.

## 10. Casos de aceite propostos para o dashboard

| CT novo (proposto) | Cenário | Resultado obrigatório |
|---|---|---|
| CT-D01 | `VIOLACAO_LACRE` com MQTT ativo | Alerta destacado com instante; feed preserva ocorrência |
| CT-D02 | Autorizar retirada válida e abrir lacre | `RETIRADA_AUTORIZADA`, sem alerta indevido de violação |
| CT-D03 | Abrir lacre com autorização expirada | `VIOLACAO_LACRE` + resposta coerente |
| CT-D04 | Bateria em 19%, 21%, 26%, volta a 19% | Alerta e rearme conforme RN10, sem alerta repetido no intervalo |
| CT-D05 | Falha de bateria (zero inválido) | Diagnóstico de falha, não leitura 0% saudável |
| CT-D06 | GNSS sem fix e última posição com 8 dias | Não atualiza coordenada, mostra `última posição válida há 8 dias` |
| CT-D07 | Queda inesperada MQTT/LWT | Marca perda de conexão quando o broker a detecta; mantém `last_seen` |
| CT-D08 | Sem mensagem por 2 períodos de 7 dias | `sem comunicação esperada` além do estado do LWT |
| CT-D09 | Fila offline cheia e reconexão | Contagem/última leitura honestas; reenvio e overflow visíveis quando reportados |
| CT-D10 | Comando publicado mas sem resposta do ESP32 | `Sem resposta` ou `Expirado`, nunca `Executado` |
| CT-D11 | Comando com `command_id` e resposta correspondente | Só o formulário correto muda para `Executado/Recusado`; audita data/hora |
| CT-D12 | Duas aberturas simultâneas do mesmo ativo | Apenas um vínculo aceito; outro recusado; sem duplicar ciclo |
| CT-D13 | Encerrar sem NF de retorno ou com lacre aberto | Recusa com motivo; ciclo permanece ativo |
| CT-D14 | Retorno físico sem baixa fiscal | Painel mostra `baixa fiscal pendente`, não `encerrado` |
| CT-D15 | Dois ciclos históricos reutilizando TAG | Histórico e posições pertencem ao ciclo correto |
| CT-D16 | Evento ocorrido offline, recebido posteriormente | Ordem por ocorrência; exibe horário de recebimento separado |
| CT-D17 | Usuário anônimo ou sem permissão | API e UI negam acesso/mutação; auditoria sem segredo |
| CT-D18 | API/UX perfis | Executar CT-F24 e CT-U01 com as metas RNF03/RNF04 quando disponíveis |

## 11. Modelo mínimo de dados (proposta para o contrato)

**Entidades:** `Equipment(id, name, description, image_url?)`; `Device(id, hardware_model, firmware_version, lifecycle_state)`; `MaintenanceCycle(id, equipment_id, device_id, supplier_id, responsible_user_id, remittance_invoice_number, remittance_invoice_issued_at, expected_return_at, stay_threshold_days, physical_return_at?, return_invoice_number?, return_invoice_issued_at?, ended_at?)`; `Event(id, event_id, device_id, cycle_id, type, severity, occurred_at?, received_at, time_basis, payload, is_simulated)`; `Location(id, cycle_id, device_id, latitude, longitude, gps_fix, gps_timestamp?, hdop?, received_at)`; `Alert(id, cycle_id, event_id, type, status, assignee, opened_at)`; `AlertAction(id, alert_id, user_id, type, justification, created_at)`; `Command(id, command_id, device_id, cycle_id, type, payload, actor_id, created_at, expires_at, status, device_ack_at?, result_reason?)`; `User(id, role, ...)`.

**Observação:** este modelo é uma **proposta de implementação**. TAPI/IoTDoc determinam informações e regras, mas não prescrevem estes nomes de tabelas, tópicos e payloads. Adaptar a estrutura ao banco selecionado, sem afirmar que o firmware atual já publica todos esses campos.

### Exemplo ilustrativo de contrato MQTT de comando (não implementado)

```json
{
  "command_id": "cmd-demo-001",
  "device_id": "prototipo-001",
  "cycle_id": "ciclo-demo-005",
  "type": "AUTHORIZE_REMOVAL",
  "payload": {"reason": "Acesso autorizado para reparo"},
  "issued_at": "2026-11-11T13:00:00Z",
  "expires_at": "2026-11-11T13:01:00Z"
}
```

```json
{
  "command_id": "cmd-demo-001",
  "device_id": "prototipo-001",
  "cycle_id": "ciclo-demo-005",
  "status": "EXECUTED",
  "processed_at": "2026-11-11T13:00:01Z",
  "reason": null
}
```

## 12. Referências

**Fontes do projeto:**
- `document/documentacao.md`, principalmente 3.3.1, 3.3.2, 3.3.3, 3.3.4 e 3.4.3.
- `document/outros/Organização de Sprints/SP3/ArtefatosSP3.md` (Artefatos 8–11).
- `src/firmware/main.cpp`, `include/config.h`, `include/cycle_rules.h`, `include/seal_rules.h`, `include/event_queue.h`.
- TAPI final Cerradinho: https://drive.google.com/file/d/1C3qVQO_pfW4NufvX3xocjMEE65nDHmPv/view
- Guia de estilos GuaráTracking v2 (pasta do grupo, Drive).

**Normas externas e referências de UX:**
- MQTT 3.1.1 (OASIS) — Will, Keep Alive, PUBACK e DISCONNECT: https://docs.oasis-open.org/mqtt/mqtt/v3.1.1/mqtt-v3.1.1.html
- WCAG 2.2 — identificação de erros: https://www.w3.org/WAI/WCAG22/Understanding/error-identification
- WCAG — mensagens de status: https://www.w3.org/WAI/WCAG21/Understanding/status-messages
- Trackunit Attention List: https://help.trackunit.com/en/articles/141031-how-do-i-navigate-in-asset-home
- Fleetio: https://www.fleetio.com/blog/custom-dashboard
- MaintainX: https://help.getmaintainx.com/about-reporting
- Traccar: https://github.com/traccar/traccar-web
- ThingsBoard: https://thingsboard.io/docs/user-guide/widgets/

---

**Decisões que ainda precisam de validação:** permissões por perfil; frequência exata de heartbeat versus economia de bateria; transportes/cobertura fora do Wi-Fi; prazos de autorização; limiares de bateria; prazo de ciência dos alertas; metas de latência e autonomia; metas de acessibilidade; campos fiscais e regras legais; escolha do broker/banco; granularidade de histórico e retenção de dados; limites de dados publicáveis. O documento não supõe homologação do parceiro nem sistema já pronto.
