# Evidências dos testes no Wokwi

Capturas reais e logs Serial dos CT01–CT23 e CT35 no protótipo V1 (22 e 23/10/2026) e da rodada do V2 (02 e 03/11/2026), com os CT43–CT45, ao final. Critérios, configuração, resultados e limitações estão na [seção 3.4.1 da documentação](../../../document/documentacao.md). As etapas complementares de cada CT ficam neste diretório e estão vinculadas nessa seção.

| CT | Captura principal / etapas | Log Serial |
|---|---|---|
| CT01 | [Inicialização](ct01.png) | [A](CT01-CT07-CT11-CT12-CT35-execucao.serial.txt) |
| CT02 | [Dia 7](ct02.png), [dia 14](ct02Dia14.png) | [B](CT02-CT03-CT05-CT06-CT08-CT09-CT15-execucao.serial.txt) |
| CT03 | [Atraso](ct03.png) | [B](CT02-CT03-CT05-CT06-CT08-CT09-CT15-execucao.serial.txt) |
| CT04 | [150 dias](ct04.png), [10 dias](ct04Limiar10.png) | [150](CT04-limiar150.serial.txt), [10](CT04-CT14-CT16-execucao.serial.txt), [configuração](CT04-configuracao.serial.txt) |
| CT05 | [Violação](ct05Evento.png) | [B](CT02-CT03-CT05-CT06-CT08-CT09-CT15-execucao.serial.txt) |
| CT06 | [Retirada](ct06Retirada.png), [reaplicação](ct06.png) | [B](CT02-CT03-CT05-CT06-CT08-CT09-CT15-execucao.serial.txt) |
| CT07 | [Autorização expirada](ct07.png) | [A](CT01-CT07-CT11-CT12-CT35-execucao.serial.txt) |
| CT08 | [Sem rede](ct08.png) | [B](CT02-CT03-CT05-CT06-CT08-CT09-CT15-execucao.serial.txt) |
| CT09 | [Fila antes](ct09Antes.png), [reenvio local](ct09.png) | [B](CT02-CT03-CT05-CT06-CT08-CT09-CT15-execucao.serial.txt) |
| CT10 | [Rodada A](ct10AViolacaoPreservada.png), [resumo da B](ct10.png) | [Fila](CT10-execucao.serial.txt) |
| CT11 | [Autorização](ct11.png), [pressão mantida](ct11PressaoMantida.png) | [A](CT01-CT07-CT11-CT12-CT35-execucao.serial.txt) |
| CT12 | [Histerese](ct12.png) | [A](CT01-CT07-CT11-CT12-CT35-execucao.serial.txt) |
| CT13 | [Falha do sensor isolada](ct13Isolado.png) | [C](CT13-isolado.serial.txt) |
| CT14 | [Encerramento corrigido](ct14Corrigido.png), [reutilização](ct14CorrigidoReutilizado.png), [segundo encerramento](ct14CorrigidoSegundoCiclo.png) | [Reteste de 23/10/2026](CT14-2026-10-23.serial.txt) |
| CT15 | [Vínculo recusado](ct15.png) | [B](CT02-CT03-CT05-CT06-CT08-CT09-CT15-execucao.serial.txt) |
| CT16 | [Lacre aberto](ct16CorrigidoLacreAberto.png), [NF ausente](ct16CorrigidoNfAusente.png) | [Reteste de 23/10/2026](CT14-2026-10-23.serial.txt) |
| CT17 | [Diagnóstico das entradas e estados](ct17.png) | [Serial](CT17-CT23.serial.txt) |
| CT18 | [Transições cronometradas — REPROVADO](ct18Original.png), [instrumentada](ct18.png) | [Serial](CT18-original.serial.txt) |
| CT19 | [Ausência de fix e interrupção NMEA](ct19.png), [fix inválido](ct19FixInvalido.png) | [Serial](CT19.serial.txt) |
| CT20 | [Comparação entre coletas válidas](ct20.png), [50 m](ct2050m.png) | [Serial](CT20.serial.txt) |
| CT21 | [Limite de diagnóstico na NVS](ct21.png) | [Serial](CT21.serial.txt) |
| CT22 | [Retenção no reinício — REPROVADO no ambiente](ct22.png), [ponto B](ct22PontoB.png) | [Serial](CT22.serial.txt) |
| CT23 | [Faixa da entrada de bateria simulada](ct23.png), [mínimo](ct23Minimo.png), [meio](ct23Meio.png) | [Serial](CT17-CT23.serial.txt) |
| CT35 | [Desligado](ct35Desligado.png), [religado](ct35.png) | [A](CT01-CT07-CT11-CT12-CT35-execucao.serial.txt) |

As capturas originais são de 22/10/2026. O reteste do CT14/CT16, em 23/10/2026, confirmou dois encerramentos com ciclo inativo, dias zero, contadores 1/2, reutilização e as duas recusas. Os arquivos anteriores do CT14 foram preservados como histórico; a evidência atual usa os arquivos `CT14-corrigido*.png`. A aprovação integral do CT09 continua bloqueada pela ausência de confirmação de servidor.

CT17–CT23 executados em 23/10/2026: CT17, CT19, CT20, CT21 e CT23 aprovados no simulador; CT18 e CT22 reprovados. CT18 perdeu transições de 200 ms sem rede; CT22 reteve os dados, mas não reiniciou o uptime no ambiente. Os registros dos CT36–CT42 e dos CT43–CT45 estão nas seções do V2, abaixo. CT24–CT34 permanecem reservados para o planejamento de energia e GNSS.

## Rodada do protótipo V2 — 02 e 03/11/2026

Capturas do LCD e logs Serial da repetição dos CTs no V2, com LCD, botões de navegação, buzzer, LED de violação e boot sem ciclo. As execuções foram duas:

- **CT01–CT10 e CT18, exceto o CT04 com limiar 150:** pelos cenários de `tests/wokwi-ci/` no Wokwi CLI, em 02/11/2026.
- **Demais CTs e CT04 com limiar 150:** à mão no Wokwi for VS Code, em 03/11/2026, com a Serial gravada por `tests/wokwi-vscode/capturar.py`.

Critérios adaptados, resultados e limitações estão na [rodada V2 da seção 3.4.1](../../../document/documentacao.md#rodada-v2). Esta rodada substitui a de 30/10/2026, feita antes do boot sem ciclo.

| CT | Captura do LCD | Log Serial |
|---|---|---|
| CT01–CT03 | [Inicialização](ct01V2.png), [atraso](ct03V2.png) | [Ciclo](v2-ct01-ct03.serial.txt) |
| CT04 | [Limiar 10](ct04Limiar10V2.png), [limiar 150](ct04Limiar150V2.png) | [Limiar 10](v2-ct04-limiar10.serial.txt), [limiar 150](v2-ct04-limiar150.serial.txt) |
| CT05–CT07 | [Violação](ct05V2.png), [retirada autorizada](ct06V2.png) | [Lacre](v2-ct05-ct07.serial.txt) |
| CT08–CT09 | [Reenvio](ct09V2.png); tela "Sem sinal" do CT08 sem captura | [Fila](v2-ct08-ct09.serial.txt) |
| CT10 | — | [A](v2-ct10a.serial.txt), [B](v2-ct10b.serial.txt) |
| CT11 | [Contagem de 3 s](ct11V2Contagem.png) | [Botão](v2-ct11.serial.txt) |
| CT12, CT13, CT23 | — | [Bateria](v2-ct12-ct13-ct23.serial.txt) |
| CT14–CT16 | [Ciclo encerrado](ct14V2.png) | [Encerramento](v2-ct14-ct16.serial.txt) |
| CT17, CT35 | [Prazo](ct17V2Tela2.png), [posição](ct17V2Tela3.png), [bateria](ct17V2Tela4.png), [lacre](ct17V2Tela5.png), [desligado](ct35V2Desligado.png), [religado](ct35V2Religado.png) | [Interface](v2-ct17-ct35.serial.txt) |
| CT18 — REPROVADO | — | [Lacre cronometrado](v2-ct18.serial.txt) |

Os CT19–CT22 não foram repetidos no V2; os resultados da primeira tabela, de 23/10/2026, referem-se ao V1.

## CT36–CT42 — interface local do V2

Capturas e vídeos dos CT36–CT42 no protótipo V2, com LCD, buzzer e LED vermelho no GPIO13. Critérios, resultados e limitações estão na [seção 3.4.1](../../../document/documentacao.md#cts36-42-interface-v2).

| CT | Capturas | Vídeos |
|---|---|---|
| CT36 | [NF](ct36Nf.png) · [Prazo](ct36Prazo.png) · [Posição](ct36Posicao.png) · [Bateria](ct36Bateria.png) · [Lacre](ct36Lacre.png) | — |
| CT37 | [Antes do prazo](ct37AntesPrazo.png) · [Um dia restante](ct37FaltaUmDia.png) · [Vence hoje](ct37VenceHoje.png) · [Atraso](ct37Atraso.png) · [Sem ciclo ativo](ct37SemCiclo.png) | — |
| CT38 | [Violação](ct38Violacao.png) · [Lacre fechado](ct38Fechado.png) · [Limite de 30 s](ct38Limite30s.png) · [Retirada autorizada](ct38RetiradaAutorizada.png) | [ct38-limite-30s](ct38-limite-30s.mkv) · [ct38-retirada-autorizada](ct38-retirada-autorizada.mkv) · [ct38-violacao-fechamento](ct38-violacao-fechamento.mkv) |
| CT39 | [NF registrada](ct39NfRegistrada.png) · [Tela da NF](ct39TelaNf.png) · [Limite de 15 caracteres](ct39Limite15.png) · [Recusa por caractere inválido — entrada do roteiro: REMESSA NF@123](ct39CaractereInvalido.png) · [Recusa por comprimento — entrada do roteiro: REMESSA 1234567890123456](ct39LongaDemais.png) · [Recusa sem ciclo ativo](ct39SemCiclo.png) | — |
| CT40 | [LCD aceso](ct40Antes.png) · [LCD apagado](ct40Apagado.png) · [Primeiro toque em avançar](ct40PrimeiroAvancar.png) · [Segundo toque em avançar](ct40SegundoAvancar.png) · [Primeiro toque em voltar](ct40PrimeiroVoltar.png) · [Segundo toque em voltar](ct40SegundoVoltar.png) | [ct40-apagamento-despertar](ct40-apagamento-despertar.mkv) |
| CT41 | [Contagem para autorizar](ct41ContagemAutorizar.png) · [Contagem para iniciar](ct41ContagemIniciar.png) · [Contagem para encerrar](ct41ContagemEncerrar.png) · [Soltura antes de autorizar](ct41SolturaAutorizar.png) · [Soltura antes de iniciar](ct41SolturaIniciar.png) · [Soltura antes de encerrar](ct41SolturaEncerrar.png) | [ct41-contagem-autorizar](ct41-contagem-autorizar.mkv) · [ct41-contagem-encerrar](ct41-contagem-encerrar.mkv) · [ct41-contagem-iniciar](ct41-contagem-iniciar.mkv) |
| CT42 | [Lacre reaplicado](ct42Reaplicado.png) · [Fechamento após violação](ct42AposViolacao.png) · [Sem sinal](ct42SemSinal.png) · [Sinal recuperado](ct42SinalOk.png) | [ct42-lacre-apos-violacao](ct42-lacre-apos-violacao.mkv) · [ct42-lacre-reaplicado](ct42-lacre-reaplicado.mkv) · [ct42-sem-sinal](ct42-sem-sinal.mkv) · [ct42-sinal-recuperado](ct42-sinal-recuperado.mkv) |

## CT43–CT45 — posição, vínculo e GNSS no V2 (03/11/2026)

Definição, resultados e limitações estão nos [CT43–CT45 da seção 3.4.1](../../../document/documentacao.md#cts43-45).

| CT | Captura | Log Serial |
|---|---|---|
| CT43 | [Sem fix](ct43SemFix.png), [segundos](ct43Segundos.png), [minutos](ct43Minutos.png) | [Posição](ct43-posicao.serial.txt) |
| CT44 | [Boot](ct44Boot.png), [sem ciclo](ct44SemCiclo.png), [ciclo iniciado](ct44Iniciado.png) (execução no Wokwi CLI, 02/11/2026) | [Vínculo](ct44-vinculo.serial.txt) (repetição no Wokwi for VS Code, 03/11/2026) |
| CT45 | [Fila zerada](ct45Fila.png) | [Sem rede](ct45-gnss.serial.txt), [com rede](ct45-gnss-rede.serial.txt) |
