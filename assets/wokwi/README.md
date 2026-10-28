# Evidências dos testes no Wokwi — Sprint 1

Capturas reais e logs Serial dos CT01–CT23 e CT35. Critérios, configuração, resultados e limitações estão na [seção 3.4.1 da documentação](../../document/documentacao.md). As etapas complementares de cada CT ficam neste diretório e estão vinculadas nessa seção.

**Issue #74 (6.10):** [inventário auditável para a RTM da issue #79 (6.8)](ISSUE-74-RTM.md) · [mapeamento CSV](issue74-mapeamento-rtm.csv) · [hashes das evidências](issue74-evidencias.sha256). Verificação: `python scripts/verify_issue74.py`, a partir da raiz do repositório. As imagens foram capturadas em 22–23/10/2026; este inventário apenas organiza as evidências já existentes, sem novo ensaio.

| CT | Captura principal / etapas | Log Serial |
|---|---|---|
| CT01 | [Inicialização](CT01.png) | [A](CT01-CT07-CT11-CT12-CT35-execucao.serial.txt) |
| CT02 | [Dia 7](CT02.png), [dia 14](CT02-dia14.png) | [B](CT02-CT03-CT05-CT06-CT08-CT09-CT15-execucao.serial.txt) |
| CT03 | [Atraso](CT03.png) | [B](CT02-CT03-CT05-CT06-CT08-CT09-CT15-execucao.serial.txt) |
| CT04 | [150 dias](CT04.png), [10 dias](CT04-limiar10.png) | [150](CT04-limiar150.serial.txt), [10](CT04-CT14-CT16-execucao.serial.txt), [configuração](CT04-configuracao.serial.txt) |
| CT05 | [Violação](CT05-evento.png) | [B](CT02-CT03-CT05-CT06-CT08-CT09-CT15-execucao.serial.txt) |
| CT06 | [Retirada](CT06-retirada.png), [reaplicação](CT06.png) | [B](CT02-CT03-CT05-CT06-CT08-CT09-CT15-execucao.serial.txt) |
| CT07 | [Autorização expirada](CT07.png) | [A](CT01-CT07-CT11-CT12-CT35-execucao.serial.txt) |
| CT08 | [Sem rede](CT08.png) | [B](CT02-CT03-CT05-CT06-CT08-CT09-CT15-execucao.serial.txt) |
| CT09 | [Fila antes](CT09-antes.png), [reenvio local](CT09.png) | [B](CT02-CT03-CT05-CT06-CT08-CT09-CT15-execucao.serial.txt) |
| CT10 | [Rodada A](CT10-A-violacao-preservada.png), [resumo da B](CT10.png) | [Fila](CT10-execucao.serial.txt) |
| CT11 | [Autorização](CT11.png), [pressão mantida](CT11-pressao-mantida.png) | [A](CT01-CT07-CT11-CT12-CT35-execucao.serial.txt) |
| CT12 | [Histerese](CT12.png) | [A](CT01-CT07-CT11-CT12-CT35-execucao.serial.txt) |
| CT13 | [Falha do sensor isolada](CT13-isolado.png) | [C](CT13-isolado.serial.txt) |
| CT14 | [Encerramento corrigido](CT14-corrigido.png), [reutilização](CT14-corrigido-reutilizado.png), [segundo encerramento](CT14-corrigido-segundo-ciclo.png) | [Reteste de 23/10/2026](CT14-2026-10-23.serial.txt) |
| CT15 | [Vínculo recusado](CT15.png) | [B](CT02-CT03-CT05-CT06-CT08-CT09-CT15-execucao.serial.txt) |
| CT16 | [Lacre aberto](CT16-corrigido-lacre-aberto.png), [NF ausente](CT16-corrigido-nf-ausente.png) | [Reteste de 23/10/2026](CT14-2026-10-23.serial.txt) |
| CT17 | [Diagnóstico das entradas e estados](CT17.png) | [Serial](CT17-CT23.serial.txt) |
| CT18 | [Transições cronometradas — REPROVADO](CT18-original.png), [instrumentada](CT18.png) | [Serial](CT18-original.serial.txt) |
| CT19 | [Ausência de fix e interrupção NMEA](CT19.png), [fix inválido](CT19-fix-invalido.png) | [Serial](CT19.serial.txt) |
| CT20 | [Comparação entre coletas válidas](CT20.png), [50 m](CT20-50m.png) | [Serial](CT20.serial.txt) |
| CT21 | [Limite de diagnóstico na NVS](CT21.png) | [Serial](CT21.serial.txt) |
| CT22 | [Retenção no reinício — REPROVADO no ambiente](CT22.png), [ponto B](CT22-ponto-B.png) | [Serial](CT22.serial.txt) |
| CT23 | [Faixa da entrada de bateria simulada](CT23.png), [mínimo](CT23-minimo.png), [meio](CT23-meio.png) | [Serial](CT17-CT23.serial.txt) |
| CT35 | [Desligado](CT35-desligado.png), [religado](CT35.png) | [A](CT01-CT07-CT11-CT12-CT35-execucao.serial.txt) |

As capturas originais são de 22/10/2026. O reteste do CT14/CT16, em 23/10/2026, confirmou dois encerramentos com ciclo inativo, dias zero, contadores 1/2, reutilização e as duas recusas. Os arquivos anteriores do CT14 foram preservados como histórico; a evidência atual usa os arquivos `CT14-corrigido*.png`. A aprovação integral do CT09 continua bloqueada pela ausência de confirmação de servidor.

CT17–CT23 executados em 23/10/2026: CT17, CT19, CT20, CT21 e CT23 aprovados no simulador; CT18 e CT22 reprovados. CT18 perdeu transições de 200 ms sem rede; CT22 reteve os dados, mas não reiniciou o uptime no ambiente. Os novos cenários a partir de CT36 ainda não têm execução registrada neste conjunto. CT24–CT34 permanecem reservados para o planejamento de energia e GNSS.
