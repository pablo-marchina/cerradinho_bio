# Issue #74 — Inventário de prints Wokwi para a RTM (6.10 → 6.8)

**Origem:** evidências já versionadas no repositório, produzidas nas execuções registradas em **22 e 23/10/2026**. Este inventário organiza e confere capturas existentes; **não houve nova execução do Wokwi nem recaptura em 28/10/2026**. Consultar [REGISTRO-COMPLEMENTAR-2026-10-22.md](REGISTRO-COMPLEMENTAR-2026-10-22.md) e a [seção 3.4.1](../../document/documentacao.md).

**Escopo:** CT01–CT23 e CT35 (24 IDs). Print de um caso não equivale a aprovação: CT09 está **BLOQUEADO** para ACK do servidor; CT18 e CT22 estão **REPROVADOS**. A Serial, os sensores e a chave de rede são simulados, e os resultados não validam MQTT/BI, autonomia ou hardware físico.

**Para a issue #79 (6.8):** usar a coluna *RF/RN* como associação já presente na RTM da seção 3.3.4, não como prova de atendimento integral ao RF. Atualizar a RTM após os demais cards de RF/RNF/CT físico; manter os três impedimentos. Casos CT17 e CT35 são **transversais**, sem RF/RN diretamente atribuídos na RTM. Os CT24–CT34 ainda são reservados.

O [arquivo CSV](issue74-mapeamento-rtm.csv) contém a mesma tabela, sem links Markdown, para importação/filtragem. O [inventário SHA-256](issue74-evidencias.sha256) permite detectar modificações nas imagens e logs selecionados; valide com `python scripts/verify_issue74.py` a partir da raiz do projeto.

| CT | Estado da evidência/teste | RF / RN na RTM | Print principal | Complementares / logs | Limitação |
|---|---|---|---|---|---|
| CT01 | APROVADO (inicialização) | RF009 / — | [PNG](CT01.png) | —; [Log](CT01-CT07-CT11-CT12-CT35-execucao.serial.txt) | STATUS local; publicação MQTT não comprovada. |
| CT02 | APROVADO (simulador) | RF009, RF011 / RN07 | [PNG](CT02.png) | [CT02-dia14.png](CT02-dia14.png); [Log](CT02-CT03-CT05-CT06-CT08-CT09-CT15-execucao.serial.txt) | Status nos dias 7 e 14; relógio acelerado, não calendário fiscal. |
| CT03 | APROVADO (alerta funcional) | RF012, RF013 / RN04, RN12, RN13 | [PNG](CT03.png) | —; [Log](CT02-CT03-CT05-CT06-CT08-CT09-CT15-execucao.serial.txt) | ALERTA_ATRASO no dia 6; frequência exata do LED não medida. |
| CT04 | APROVADO (simulador) | RF012, RF014 / RN03, RN04, RN12, RN13 | [PNG](CT04.png) | [CT04-limiar10.png](CT04-limiar10.png); [Log](CT04-limiar150.serial.txt), [Log](CT04-CT14-CT16-execucao.serial.txt) | Permanência em 150 e 10 dias simulados; registro de 150 dias é parcial. |
| CT05 | APROVADO (detecção local) | RF006 / RN05 | [PNG](CT05-evento.png) | [CT05.png](CT05.png); [Log](CT02-CT03-CT05-CT06-CT08-CT09-CT15-execucao.serial.txt) | Violação local do lacre; entrega no servidor não comprovada. |
| CT06 | APROVADO (simulador) | RF007 / RN06 | [PNG](CT06-retirada.png) | [CT06.png](CT06.png); [Log](CT02-CT03-CT05-CT06-CT08-CT09-CT15-execucao.serial.txt) | Retirada autorizada e reaplicação do lacre. |
| CT07 | APROVADO (simulador) | RF006 / RN05 | [PNG](CT07.png) | —; [Log](CT01-CT07-CT11-CT12-CT35-execucao.serial.txt) | Autorização expirada gera violação local. |
| CT08 | APROVADO (fila local) | RF008, RF010 / RN09 | [PNG](CT08.png) | —; [Log](CT02-CT03-CT05-CT06-CT08-CT09-CT15-execucao.serial.txt) | Rede simulada indisponível; não é teste da infraestrutura Wi-Fi. |
| CT09 | BLOQUEADO (ACK do servidor) | RF008, RF010 / RN09 | [PNG](CT09.png) | [CT09-antes.png](CT09-antes.png); [Log](CT02-CT03-CT05-CT06-CT08-CT09-CT15-execucao.serial.txt) | Reenvio na Serial e fila zero observados; ACK do servidor NÃO testado. |
| CT10 | APROVADO (fila simulada) | RF010 / RN09 | [PNG](CT10.png) | [CT10-A-cheia.png](CT10-A-cheia.png), [CT10-A-violacao-preservada.png](CT10-A-violacao-preservada.png), [CT10-A-fila-vazia.png](CT10-A-fila-vazia.png), [CT10-B-resumo-iniciado.png](CT10-B-resumo-iniciado.png), [CT10-B-cheia-resumo4.png](CT10-B-cheia-resumo4.png); [Log](CT10-execucao.serial.txt) | Prioridade de críticos e resumo de quatro; sem entrega confirmada. |
| CT11 | APROVADO (estímulos aplicados) | RF007 / RN06 | [PNG](CT11.png) | [CT11-pressao-mantida.png](CT11-pressao-mantida.png); [Log](CT01-CT07-CT11-CT12-CT35-execucao.serial.txt) | Pressão mantida sem repetição; limite exato de debounce não medido. |
| CT12 | APROVADO (simulador) | RF019, RF027 / RN10 | [PNG](CT12.png) | [CT12-primeiro-alerta.png](CT12-primeiro-alerta.png), [CT12-recuperacao-insuficiente.png](CT12-recuperacao-insuficiente.png), [CT12-sem-repeticao.png](CT12-sem-repeticao.png), [CT12-recuperacao.png](CT12-recuperacao.png); [Log](CT01-CT07-CT11-CT12-CT35-execucao.serial.txt) | Histerese de bateria simulada; não comprova tensão/autonomia reais. |
| CT13 | APROVADO (simulador) | RF019, RF027 / RN10 | [PNG](CT13-isolado.png) | [CT13.png](CT13.png); [Log](CT13-isolado.serial.txt) | ADC zero gera falha do sensor; sem diagnóstico físico do divisor. |
| CT14 | APROVADO (reteste 23/10) | RF016, RF017 / RN11, RN12 | [PNG](CT14-corrigido.png) | [CT14-corrigido-reutilizado.png](CT14-corrigido-reutilizado.png), [CT14-corrigido-segundo-ciclo.png](CT14-corrigido-segundo-ciclo.png); [Log](CT14-2026-10-23.serial.txt) | Usar os arquivos corrigidos; CT14.png é evidência histórica de falha anterior. |
| CT15 | APROVADO (simulador) | RF004 / RN01 | [PNG](CT15.png) | —; [Log](CT02-CT03-CT05-CT06-CT08-CT09-CT15-execucao.serial.txt) | Recusa de vínculo simultâneo. |
| CT16 | APROVADO (reteste 23/10) | RF016 / RN11 | [PNG](CT16-corrigido-lacre-aberto.png) | [CT16-corrigido-nf-ausente.png](CT16-corrigido-nf-ausente.png); [Log](CT14-2026-10-23.serial.txt) | Duas recusas: lacre aberto e NF de retorno ausente. |
| CT17 | APROVADO (diagnóstico simulado) | Transversal / Transversal | [PNG](CT17.png) | [CT17-ciclo.png](CT17-ciclo.png), [CT17-autorizacao.png](CT17-autorizacao.png), [CT17-retirada-autorizada.png](CT17-retirada-autorizada.png), [CT17-autorizacao-sem-rede.png](CT17-autorizacao-sem-rede.png), [CT17-desligado.png](CT17-desligado.png), [CT17-religado.png](CT17-religado.png); [Log](CT17-CT23.serial.txt) | Diagnóstico transversal; sem associação direta a RF/RN na RTM. |
| CT18 | REPROVADO (perda de transições) | RF006 / RN05 | [PNG](CT18-original.png) | [CT18.png](CT18.png); [Log](CT18-original.serial.txt) | 4 de 20 transições estáveis de 200 ms perdidas sem rede; preservar reprovação. |
| CT19 | APROVADO (simulador) | RF005, RF015 / RN08 | [PNG](CT19.png) | [CT19-fix-invalido.png](CT19-fix-invalido.png); [Log](CT19.serial.txt) | Sem GNSS válido, sinalização local; não valida painel ou satélite físico. |
| CT20 | APROVADO (simulador) | RF005, RF015 / RN08 | [PNG](CT20.png) | [CT20-50m.png](CT20-50m.png); [Log](CT20.serial.txt) | Coletas sintéticas de deslocamento; não implica entrega ao dashboard. |
| CT21 | APROVADO (NVS no simulador) | RF010 / RN09 | [PNG](CT21.png) | —; [Log](CT21.serial.txt) | Limite de 16 diagnósticos persistidos na NVS instrumentada. |
| CT22 | REPROVADO (uptime do ambiente) | RF005, RF010, RF015 / RN08, RN09 | [PNG](CT22.png) | [CT22-ponto-B.png](CT22-ponto-B.png); [Log](CT22.serial.txt) | Dados retidos, mas uptime_ms não reiniciou; não aprovar o CT integralmente. |
| CT23 | APROVADO (entrada sintética) | RF019, RF027 / RN10 | [PNG](CT23.png) | [CT23-minimo.png](CT23-minimo.png), [CT23-meio.png](CT23-meio.png); [Log](CT17-CT23.serial.txt) | Percentuais sintéticos 0/50/100%; sem calibração física. |
| CT35 | APROVADO (desligamento lógico) | Transversal / Transversal | [PNG](CT35-desligado.png) | [CT35.png](CT35.png); [Log](CT01-CT07-CT11-CT12-CT35-execucao.serial.txt) | Desligamento lógico, não corte físico de alimentação. |

## Critério de auditoria e cuidados de interpretação

- Todas as imagens acima são **arquivos pré-existentes**, não imagens geradas ou reconstituídas. Nenhum novo arquivo PNG é necessário para a #74 no estado examinado da `develop`.
- CT02 e CT03 apontam capturas com **conteúdo binário idêntico** (mesmo ponto da saída Serial, como já registrado no relatório complementar). São dois IDs mas não duas tomadas independentes; a distinção está nos eventos/tempos do log.
- CT14/CT16: usar as capturas da versão **corrigida em 23/10**, preservando as imagens originais de 22/10 como histórico. Não promover a falha antiga a sucesso.
- CT18: preferir a captura da execução original **reprovada**; a imagem instrumentada é complementar. CT22: retenção de dados é parcial e não basta para aprovação.
- CT09: reenvio pela Serial não prova recebimento/ACK pelo servidor. CT35: somente desligamento lógico. CT03/CT11: limites temporais exatos não medidos.
- Verificar qualquer evidência substituída novamente contra sua captura no Wokwi; alterar um print exige atualizar o inventário de SHA-256 e revalidar suas conclusões.

## Comando de verificação

```sh
python scripts/verify_issue74.py
```
