# Issue #74 — Inventário de evidências Wokwi para a RTM (6.10 → 6.8)

**Proveniência:** V1 executado em 22–23/10/2026 e V2 executado em 02–03/11/2026, conforme os registros existentes no repositório. Este documento **não reexecuta testes** nem fabrica capturas. Os caminhos acompanham a reorganização da issue #63, em `assets/3-desenvolvimento-resultados/3.4.1-prototipo-wokwi/`.

**Escopo:** CT01–CT23 e CT35, exatamente 24 casos. A coluna de situação V1 se refere somente à rodada original; a coluna V2 registra a repetição e suas limitações. **CT09 segue bloqueado**, **CT18 segue reprovado em ambas as rodadas** e **CT22 permanece reprovado no V1, sem reteste V2**. CT08 V2 não executou a tela "Sem sinal" e CT35 V2 não comprovou silêncio durante o desligamento.

**Handoff para a issue #79 (6.8):** as associações RF/RN reproduzem a RTM da [seção 3.3.4](../../../document/documentacao.md); não atestam atendimento completo dos requisitos. CT17 e CT35 são transversais, sem RF/RN atribuído. CT24–CT34 continuam fora deste levantamento. A [seção 3.4.1](../../../document/documentacao.md#rodada-v2) registra critérios e resultados V2.

**Arquivos estruturados:** [CSV para a RTM](issue74-mapeamento-rtm.csv) e [manifesto SHA-256 de todas as evidências vinculadas](issue74-evidencias.sha256). Execute `python scripts/verify_issue74.py` a partir da raiz, inclusive no Windows. A conferência valida arquivos, PNG, hashes, rastreabilidade e status conhecidos; não substitui execução de hardware.

## Rodada V1 — 22 e 23/10/2026

| CT | Situação V1 | RF / RN na RTM | Print principal | Complementares / logs | Ressalva |
|---|---|---|---|---|---|
| CT01 | APROVADO (inicialização) | RF009 / — | [ct01.png](ct01.png) | —; [CT01-CT07-CT11-CT12-CT35-execucao.serial.txt](CT01-CT07-CT11-CT12-CT35-execucao.serial.txt) | STATUS local; publicação MQTT não comprovada. |
| CT02 | APROVADO (simulador) | RF009, RF011 / RN07 | [ct02.png](ct02.png) | [ct02Dia14.png](ct02Dia14.png); [CT02-CT03-CT05-CT06-CT08-CT09-CT15-execucao.serial.txt](CT02-CT03-CT05-CT06-CT08-CT09-CT15-execucao.serial.txt) | Status nos dias 7 e 14; relógio acelerado, não calendário fiscal. |
| CT03 | APROVADO (alerta funcional) | RF012, RF013 / RN04, RN12, RN13 | [ct03.png](ct03.png) | —; [CT02-CT03-CT05-CT06-CT08-CT09-CT15-execucao.serial.txt](CT02-CT03-CT05-CT06-CT08-CT09-CT15-execucao.serial.txt) | ALERTA_ATRASO no dia 6; frequência exata do LED não medida. |
| CT04 | APROVADO (simulador) | RF012, RF014 / RN03, RN04, RN12, RN13 | [ct04.png](ct04.png) | [ct04Limiar10.png](ct04Limiar10.png); [CT04-limiar150.serial.txt](CT04-limiar150.serial.txt), [CT04-CT14-CT16-execucao.serial.txt](CT04-CT14-CT16-execucao.serial.txt) | Permanência em 150 e 10 dias simulados; registro de 150 dias é parcial. |
| CT05 | APROVADO (detecção local) | RF006 / RN05 | [ct05Evento.png](ct05Evento.png) | [ct05.png](ct05.png); [CT02-CT03-CT05-CT06-CT08-CT09-CT15-execucao.serial.txt](CT02-CT03-CT05-CT06-CT08-CT09-CT15-execucao.serial.txt) | Violação local do lacre; entrega no servidor não comprovada. |
| CT06 | APROVADO (simulador) | RF007 / RN06 | [ct06Retirada.png](ct06Retirada.png) | [ct06.png](ct06.png); [CT02-CT03-CT05-CT06-CT08-CT09-CT15-execucao.serial.txt](CT02-CT03-CT05-CT06-CT08-CT09-CT15-execucao.serial.txt) | Retirada autorizada e reaplicação do lacre. |
| CT07 | APROVADO (simulador) | RF006, RF007 / RN05, RN06 | [ct07.png](ct07.png) | —; [CT01-CT07-CT11-CT12-CT35-execucao.serial.txt](CT01-CT07-CT11-CT12-CT35-execucao.serial.txt) | Autorização expirada gera violação local. |
| CT08 | APROVADO (fila local) | RF008, RF010 / RN09, RN22 | [ct08.png](ct08.png) | —; [CT02-CT03-CT05-CT06-CT08-CT09-CT15-execucao.serial.txt](CT02-CT03-CT05-CT06-CT08-CT09-CT15-execucao.serial.txt) | Rede simulada indisponível; não é teste da infraestrutura Wi-Fi. |
| CT09 | BLOQUEADO (ACK do servidor) | RF008, RF010 / RN09, RN22 | [ct09.png](ct09.png) | [ct09Antes.png](ct09Antes.png); [CT02-CT03-CT05-CT06-CT08-CT09-CT15-execucao.serial.txt](CT02-CT03-CT05-CT06-CT08-CT09-CT15-execucao.serial.txt) | Reenvio na Serial e fila zero observados; ACK do servidor NÃO testado. |
| CT10 | APROVADO (fila simulada) | RF010 / RN09, RN22 | [ct10.png](ct10.png) | [ct10ACheia.png](ct10ACheia.png), [ct10AViolacaoPreservada.png](ct10AViolacaoPreservada.png), [ct10AFilaVazia.png](ct10AFilaVazia.png), [ct10BResumoIniciado.png](ct10BResumoIniciado.png), [ct10BCheiaResumo4.png](ct10BCheiaResumo4.png); [CT10-execucao.serial.txt](CT10-execucao.serial.txt) | Prioridade de críticos e resumo de quatro; sem entrega confirmada. |
| CT11 | APROVADO (estímulos aplicados) | RF007 / RN06 | [ct11.png](ct11.png) | [ct11PressaoMantida.png](ct11PressaoMantida.png); [CT01-CT07-CT11-CT12-CT35-execucao.serial.txt](CT01-CT07-CT11-CT12-CT35-execucao.serial.txt) | Pressão mantida sem repetição; limite exato de debounce não medido. |
| CT12 | APROVADO (simulador) | RF019, RF027 / RN10 | [ct12.png](ct12.png) | [ct12PrimeiroAlerta.png](ct12PrimeiroAlerta.png), [ct12RecuperacaoInsuficiente.png](ct12RecuperacaoInsuficiente.png), [ct12SemRepeticao.png](ct12SemRepeticao.png), [ct12Recuperacao.png](ct12Recuperacao.png); [CT01-CT07-CT11-CT12-CT35-execucao.serial.txt](CT01-CT07-CT11-CT12-CT35-execucao.serial.txt) | Histerese de bateria simulada; não comprova tensão/autonomia reais. |
| CT13 | APROVADO (simulador) | RF019, RF027 / RN10 | [ct13Isolado.png](ct13Isolado.png) | [ct13.png](ct13.png); [CT13-isolado.serial.txt](CT13-isolado.serial.txt) | ADC zero gera falha do sensor; sem diagnóstico físico do divisor. |
| CT14 | APROVADO (reteste 23/10) | RF003, RF016, RF017 / RN01, RN11, RN12, RN21 | [ct14Corrigido.png](ct14Corrigido.png) | [ct14CorrigidoReutilizado.png](ct14CorrigidoReutilizado.png), [ct14CorrigidoSegundoCiclo.png](ct14CorrigidoSegundoCiclo.png); [CT14-2026-10-23.serial.txt](CT14-2026-10-23.serial.txt) | Usar os arquivos corrigidos; CT14.png é evidência histórica de falha anterior. |
| CT15 | APROVADO (simulador) | RF004 / RN01 | [ct15.png](ct15.png) | —; [CT02-CT03-CT05-CT06-CT08-CT09-CT15-execucao.serial.txt](CT02-CT03-CT05-CT06-CT08-CT09-CT15-execucao.serial.txt) | Recusa de vínculo simultâneo. |
| CT16 | APROVADO (reteste 23/10) | RF016 / RN11 | [ct16CorrigidoLacreAberto.png](ct16CorrigidoLacreAberto.png) | [ct16CorrigidoNfAusente.png](ct16CorrigidoNfAusente.png); [CT14-2026-10-23.serial.txt](CT14-2026-10-23.serial.txt) | Duas recusas: lacre aberto e NF de retorno ausente. |
| CT17 | APROVADO (diagnóstico simulado) | Transversal / Transversal | [ct17.png](ct17.png) | [ct17Ciclo.png](ct17Ciclo.png), [ct17Autorizacao.png](ct17Autorizacao.png), [ct17RetiradaAutorizada.png](ct17RetiradaAutorizada.png), [ct17AutorizacaoSemRede.png](ct17AutorizacaoSemRede.png), [ct17Desligado.png](ct17Desligado.png), [ct17Religado.png](ct17Religado.png); [CT17-CT23.serial.txt](CT17-CT23.serial.txt) | Diagnóstico transversal; sem associação direta a RF/RN na RTM. |
| CT18 | REPROVADO (perda de transições) | RF006 / RN05 | [ct18Original.png](ct18Original.png) | [ct18.png](ct18.png); [CT18-original.serial.txt](CT18-original.serial.txt) | 4 de 20 transições estáveis de 200 ms perdidas sem rede; preservar reprovação. |
| CT19 | APROVADO (simulador) | RF005, RF015 / RN08, RN22 | [ct19.png](ct19.png) | [ct19FixInvalido.png](ct19FixInvalido.png); [CT19.serial.txt](CT19.serial.txt) | Sem GNSS válido, sinalização local; não valida painel ou satélite físico. |
| CT20 | APROVADO (simulador) | RF005, RF015 / RN08, RN22 | [ct20.png](ct20.png) | [ct2050m.png](ct2050m.png); [CT20.serial.txt](CT20.serial.txt) | Coletas sintéticas de deslocamento; não implica entrega ao dashboard. |
| CT21 | APROVADO (NVS no simulador) | RF010 / RN09, RN22 | [ct21.png](ct21.png) | —; [CT21.serial.txt](CT21.serial.txt) | Limite de 16 diagnósticos persistidos na NVS instrumentada. |
| CT22 | REPROVADO (uptime do ambiente) | RF005, RF010, RF015 / RN08, RN09, RN22 | [ct22.png](ct22.png) | [ct22PontoB.png](ct22PontoB.png); [CT22.serial.txt](CT22.serial.txt) | Dados retidos, mas uptime_ms não reiniciou; não aprovar o CT integralmente. |
| CT23 | APROVADO (entrada sintética) | RF019, RF027 / RN10 | [ct23.png](ct23.png) | [ct23Minimo.png](ct23Minimo.png), [ct23Meio.png](ct23Meio.png); [CT17-CT23.serial.txt](CT17-CT23.serial.txt) | Percentuais sintéticos 0/50/100%; sem calibração física. |
| CT35 | APROVADO (desligamento lógico) | Transversal / Transversal | [ct35Desligado.png](ct35Desligado.png) | [ct35.png](ct35.png); [CT01-CT07-CT11-CT12-CT35-execucao.serial.txt](CT01-CT07-CT11-CT12-CT35-execucao.serial.txt) | Desligamento lógico, não corte físico de alimentação. |

## Rodada V2 — 02 e 03/11/2026

A situação V2 não substitui retroativamente a situação V1. Onde não há screenshot V2 individual, a evidência é a **Serial versionada**; onde não houve reteste, ambos os campos ficam vazios. Imagens V2 do LCD são recortes da interface do simulador, não screenshots de toda a janela.

| CT | Situação V2 | Prints V2 | Logs V2 | Ressalva específica V2 |
|---|---|---|---|---|
| CT01 | APROVADO (simulador) | [ct01V2.png](ct01V2.png) | [v2-ct01-ct03.serial.txt](v2-ct01-ct03.serial.txt) | Boot sem ciclo: STATUS e NF não informada; envio à plataforma não verificado. |
| CT02 | APROVADO (simulador) | — | [v2-ct01-ct03.serial.txt](v2-ct01-ct03.serial.txt) | STATUS_SEMANAL nos dias 7 e 14 após vínculo; sem PNG V2 individual. |
| CT03 | APROVADO (simulador) | [ct03V2.png](ct03V2.png) | [v2-ct01-ct03.serial.txt](v2-ct01-ct03.serial.txt) | ALERTA_ATRASO único no dia 6; LCD mostra atraso. |
| CT04 | APROVADO (limiares 10 e 150) | [ct04Limiar10V2.png](ct04Limiar10V2.png), [ct04Limiar150V2.png](ct04Limiar150V2.png) | [v2-ct04-limiar10.serial.txt](v2-ct04-limiar10.serial.txt), [v2-ct04-limiar150.serial.txt](v2-ct04-limiar150.serial.txt) | ALERTA_PERMANENCIA único em cada limiar na Serial; o LCD não indica a permanência. |
| CT05 | APROVADO (detecção local) | [ct05V2.png](ct05V2.png) | [v2-ct05-ct07.serial.txt](v2-ct05-ct07.serial.txt) | Violação e fechamento locais; plataforma não validada. |
| CT06 | APROVADO (simulador) | [ct06V2.png](ct06V2.png) | [v2-ct05-ct07.serial.txt](v2-ct05-ct07.serial.txt) | Retirada autorizada e lacre reaplicado no simulador. |
| CT07 | APROVADO (simulador) | — | [v2-ct05-ct07.serial.txt](v2-ct05-ct07.serial.txt) | Autorização expirada produz violação; sem PNG V2 próprio. |
| CT08 | PARCIAL (fila aprovada; tela não executada) | — | [v2-ct08-ct09.serial.txt](v2-ct08-ct09.serial.txt) | Fila sem rede comprovada; tela "Sem sinal" não executada e sem captura. |
| CT09 | BLOQUEADO (ACK do servidor) | [ct09V2.png](ct09V2.png) | [v2-ct08-ct09.serial.txt](v2-ct08-ct09.serial.txt) | Reenvio e fila zero na Serial; entrega/ACK no servidor não comprovados. |
| CT10 | APROVADO (fila simulada) | — | [v2-ct10a.serial.txt](v2-ct10a.serial.txt), [v2-ct10b.serial.txt](v2-ct10b.serial.txt) | Descarte de não críticos, preservação de violação e resumo de quatro; sem PNG V2. |
| CT11 | APROVADO (estímulos manuais) | [ct11V2Contagem.png](ct11V2Contagem.png) | [v2-ct11.serial.txt](v2-ct11.serial.txt) | Gestos curtos e um longo; não foram aplicados pulsos automáticos de 20 ms. |
| CT12 | APROVADO (simulador) | — | [v2-ct12-ct13-ct23.serial.txt](v2-ct12-ct13-ct23.serial.txt) | Histerese com entrada sintética; sem medição da bateria física. |
| CT13 | APROVADO (diagnóstico simulado) | — | [v2-ct12-ct13-ct23.serial.txt](v2-ct12-ct13-ct23.serial.txt) | ADC zero gera falha; não testa divisor real. |
| CT14 | APROVADO (simulador) | [ct14V2.png](ct14V2.png) | [v2-ct14-ct16.serial.txt](v2-ct14-ct16.serial.txt) | Encerramento e abertura do ciclo 2 no V2; manter reteste V1 de 23/10. |
| CT15 | APROVADO (critério V2 parcial) | — | [v2-ct14-ct16.serial.txt](v2-ct14-ct16.serial.txt) | Sem segundo ciclo; VINCULO_RECUSADO não é alcançável pelo gesto atual (RN01 pendente). |
| CT16 | APROVADO (duas recusas) | — | [v2-ct14-ct16.serial.txt](v2-ct14-ct16.serial.txt) | NF ausente e lacre aberto impedem encerramento; sem PNG V2 próprio. |
| CT17 | APROVADO (interface simulada) | [ct17V2Tela2.png](ct17V2Tela2.png), [ct17V2Tela3.png](ct17V2Tela3.png), [ct17V2Tela4.png](ct17V2Tela4.png), [ct17V2Tela5.png](ct17V2Tela5.png) | [v2-ct17-ct35.serial.txt](v2-ct17-ct35.serial.txt) | Navegação circular no LCD; evidência transversal. |
| CT18 | REPROVADO (abertura falsa sem rede) | — | [v2-ct18.serial.txt](v2-ct18.serial.txt) | Pulsos de 20/40 ms lidos como abertura; falsa VIOLACAO_LACRE, sem latência instrumentada. |
| CT19 | NÃO REEXECUTADO NO V2 | — | — | Manter somente resultado V1 (23/10); sem alegar evidência V2. |
| CT20 | NÃO REEXECUTADO NO V2 | — | — | Manter somente resultado V1 (23/10); sem alegar evidência V2. |
| CT21 | NÃO REEXECUTADO NO V2 | — | — | Manter somente resultado V1 (23/10); sem alegar evidência V2. |
| CT22 | NÃO REEXECUTADO NO V2 | — | — | CT22 segue REPROVADO no V1; não houve reteste V2. |
| CT23 | APROVADO (entrada sintética) | — | [v2-ct12-ct13-ct23.serial.txt](v2-ct12-ct13-ct23.serial.txt) | Entrada sintética 0/50/100%; não calibra bateria física. |
| CT35 | PARCIAL (religamento; silêncio pendente) | [ct35V2Desligado.png](ct35V2Desligado.png), [ct35V2Religado.png](ct35V2Religado.png) | [v2-ct17-ct35.serial.txt](v2-ct17-ct35.serial.txt) | Desligamento/religamento observados; silêncio durante intervalo sem captura Serial. |

## Critérios de interpretação e auditoria

- **CT04:** V2 aprovou os alertas de 10 e 150 dias na Serial; as imagens do LCD **não** comprovam indicação visual de permanência.
- **CT08:** fila simulada foi verificada; a tela "Sem sinal" não foi executada na rodada V2.
- **CT09:** Serial e fila local não equivalem a ACK/recebimento de servidor; manter bloqueado em V1/V2.
- **CT14/CT16:** privilegiar o reteste V1 de 23/10, conservando PNGs antigos como histórico; o V2 tem execução própria.
- **CT15:** o critério V2 não alcança o evento `VINCULO_RECUSADO`; não afirmar que a recusa da RN01 foi exercitada no V2.
- **CT18:** V1 perdeu 4 de 20 transições estáveis sem rede. V2 gerou abertura e violação falsas com pulsos de 20/40 ms sem rede; **ambos reprovados**, por motivos diferentes.
- **CT19–CT22:** não foram repetidos no V2. A reprovação do CT22 é resultado V1 e continua visível.
- **CT35:** o V2 comprovou desligamento e religamento lógicos, mas não o silêncio na Serial durante o intervalo fechado.
- **CT02/CT03 V1:** capturas binariamente idênticas; os eventos distintos são verificáveis no log, não em screenshots independentes.
- Evidências simuladas não validam infraestrutura MQTT/BI, hardware real, autonomia, alimentação ou precisão temporal instrumental.
- Ao substituir uma evidência, atualizar o CSV, o Markdown e o manifesto de SHA-256; o verificador recusa divergências.

## Verificação

```sh
python scripts/verify_issue74.py
```
