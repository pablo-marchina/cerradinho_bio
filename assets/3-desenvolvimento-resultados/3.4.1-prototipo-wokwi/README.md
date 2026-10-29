# Evidências dos testes no Wokwi — Sprint 1

Capturas reais e logs Serial dos CT01–CT23 e CT35. Critérios, configuração, resultados e limitações estão na [seção 3.4.1 da documentação](../../../document/documentacao.md). As etapas complementares de cada CT ficam neste diretório e estão vinculadas nessa seção.

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

CT17–CT23 executados em 23/10/2026: CT17, CT19, CT20, CT21 e CT23 aprovados no simulador; CT18 e CT22 reprovados. CT18 perdeu transições de 200 ms sem rede; CT22 reteve os dados, mas não reiniciou o uptime no ambiente. Os novos cenários a partir de CT36 ainda não têm execução registrada neste conjunto. CT24–CT34 permanecem reservados para o planejamento de energia e GNSS.
