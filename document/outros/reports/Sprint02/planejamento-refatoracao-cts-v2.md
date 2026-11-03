# Planejamento — refatoração dos CTs do protótipo V2 (Sprint 2)

Documento interno. Divide entre duas pessoas, que trabalham ao mesmo tempo, a atualização dos CTs da seção 3.4.1 ao estado atual do protótipo. Os CTs seguem o formato da última sprint: tabela `CT# | Bloco | Componente de entrada | Leitura da entrada | Componente de saída | Leitura da saída | Descrição`, tabela de resultados `CT | Situação | Resultado observado | Evidência e log` e um bloco `<details>` por CT com a figura e a legenda.

## 1. O que mudou no protótipo desde a rodada V2 de 30/10/2026

| Mudança | Efeito nos CTs |
|---|---|
| O dispositivo liga **sem ciclo**. O ciclo só começa ao segurar CICLO por 3 s (vínculo, `CICLO_INICIADO` com `ciclo_id=1`), e os dias passam a contar a partir do vínculo | Todo CT que precisa de ciclo ativo ganha um passo de vínculo no início. O `STATUS` de boot sai com `ciclo_ativo=false`, e a tela de prazo mostra "sem ciclo ativo" |
| LED vermelho de violação no GPIO13 (220 Ω), com pulso de 150 ms a cada 1 s enquanto o lacre está violado | CT05, CT07, CT08 e CT35 voltam a ter LED como saída. A foto do LED fica no CT38 |
| Eventos `GNSS_SEM_FIX` (`motivo`, `satelites`) e `GNSS_RECUPERADO` (`tempo_sem_posicao_s`, `coletas_sem_fix`), não críticos e enviados pela fila | Ganham um CT próprio, o CT45 |
| Bordas do lacre capturadas em ISR | O CT18 continua REPROVADO até um novo ensaio cronometrado |
| Circuito montado em protoboard dupla | As capturas novas mostram a montagem nova |
| Sem mudança: regras de ciclo, lacre, fila e bateria, as telas do LCD, o buzzer de 30 s e os botões de 3 s | Basta rodar de novo com o binário novo |

## 2. Impacto em cada CT que já existe

| CT | O que muda | Cenário (`tests/wokwi-ci/`) | Quem |
|---|---|---|---|
| CT01 | `STATUS` com `ciclo_ativo=false`. Depois do vínculo, `CICLO_INICIADO` com `ciclo_id=1` | `ct01-03-ciclo.yaml`: segurar `btn_ciclo` por 3 s no início | A |
| CT02, CT03 | Os dias 7/14 e o atraso no dia 6 passam a contar do vínculo | mesmo arquivo | A |
| CT04 (10 e 150) | Precisa de ciclo ativo para emitir `ALERTA_PERMANENCIA` | `ct04-limiar10.yaml` e `ct04-limiar150.yaml`: vínculo no início. A rodada de 150 dias vai para o Wokwi for VS Code, porque passa do limite de 5 min do CLI | A |
| CT05, CT07 | LED vermelho volta como saída | `ct05-07-lacre.yaml` não muda; basta rodar de novo | A |
| CT08, CT09 | Com ciclo, `dia_ocorrencia` deixa de ser sempre 0 | `ct08-09-fila.yaml`: vínculo no início | A |
| CT10 | A pré-condição "ciclo inativo" passa a ser o estado de boot | `ct10a-fila-cheia.yaml` e `ct10b-resumo.yaml` não mexem no ciclo; basta rodar de novo | A |
| CT11, CT12, CT13, CT23 | Nada | rodar de novo | A |
| CT14, CT16 | O primeiro gesto em CICLO agora abre o ciclo 1. Os cenários precisam do vínculo antes do encerramento | `ct14-16-encerramento.yaml` e `run_cts.py`: vínculo no início; conferir `ciclo_id` e `ciclos_encerrados` | A |
| CT15 | Continua inalcançável pelo botão (com ciclo ativo o gesto encerra). Fica o critério do V2 | mesmo arquivo | A |
| CT17, CT35 | A tela de prazo precisa de ciclo para ter conteúdo. No CT35 o LED também apaga | `ct17-35-interface.yaml`: vínculo no início | A |
| CT18 | Reteste cronometrado com a ISR | `tests/wokwi-complementares/`: conferir se a instrumentação usa o `main.cpp` atual e incluir o vínculo | A |
| CT19–CT22 | A lógica de posição e de NVS não mudou. Mantêm o resultado de 23/10; os eventos novos de GNSS ficam no CT45 | — | — |

## 3. CTs novos

Mesmo formato da tabela principal da 3.4.1. Os textos do LCD são os do `include/screen_controller.h`.

**CT#** | **Bloco** | **Componente de entrada** | **Leitura da entrada** | **Componente de saída** | **Leitura da saída** | **Descrição**
|----------|----------|----------|----------|----------|----------|----------|
|CT36|Navegação do LCD|BTN_AVANCAR e BTN_VOLTAR|Toques curtos com o LCD aceso; voltar estando na tela de NF|LCD 16x2|Cinco telas em ordem: NF da remessa, prazo, última posição, bateria e lacre. Avançar na quinta leva à primeira, e voltar na primeira leva à quinta|Esperado: o operador consulta NF, prazo, posição, bateria e lacre no próprio dispositivo, sem computador (RF005, RF006, RF012, RF019).|
|CT37|Prazo no LCD|Relógio simulado (1 s = 1 dia) e BTN_CICLO|Ciclo vinculado, do dia 0 até depois do dia 5; depois ciclo encerrado|LCD (tela de prazo)|"Faltam N dias" antes do dia 4, "Falta 1 dia" no dia 4, "vence hoje" no dia 5, "ATRASO: N dias" depois e "sem ciclo ativo" com o ciclo encerrado|Esperado: o prazo mostrado no dispositivo usa os mesmos dias que geram o `ALERTA_ATRASO` (RF012, RF013; RN04).|
|CT38|Alarme local|SW_LACRE e BTN_AUTORIZAR|Violação encerrada pelo fechamento; violação mantida aberta por mais de 30 s; abertura com autorização válida|Buzzer (GPIO23), LED vermelho (GPIO13), LCD; Serial|Bipes de 500 ms a 1 Hz da violação até fechar o lacre ou completar 30 s; LED vermelho pulsando enquanto o lacre está violado; nenhum bipe e LED apagado na retirada autorizada|Esperado: a violação chama atenção no local sem depender de rede, e a retirada autorizada não dispara alarme (RF006, RF007; RN05, RN06).|
|CT39|NF da remessa|Comando `REMESSA` no Monitor Serial|`NF-000123` com ciclo ativo; 15 caracteres; caractere inválido (`NF@123`); 16 caracteres; sem ciclo ativo|LCD; Serial|Aceitas: "NF registrada" e depois a tela "NF da remessa / NF-000123". Recusas: `[comando] NF invalida…` ou `[comando] sem ciclo ativo…`, com a NF anterior mantida|Esperado: só uma NF de até 15 caracteres válidos fica associada ao ciclo ativo (RF002; RN02).|
|CT40|Economia do LCD|BTN_AVANCAR e BTN_VOLTAR|15 s sem interação; depois um toque e um segundo toque|LCD e luz de fundo|O LCD apaga após 15 s. O primeiro toque só acende o visor, na mesma tela; o segundo navega|Esperado: o visor só gasta energia quando alguém está olhando, sem perder a tela em que estava (RF019).|
|CT41|Botões de 3 s|BTN_AUTORIZAR e BTN_CICLO|Botão segurado até 3 s; botão solto com cerca de 1 s|LCD; Serial|"Autorizar lacre", "Iniciar ciclo" ou "Encerrar ciclo", com "segure mais N s". Ao soltar antes: `[botao] … solto antes de 3 s; nada registrado`|Esperado: ações que mudam o ciclo ou o lacre exigem intenção clara do operador (RF003, RF007, RF016).|
|CT42|Mensagens temporárias|SW_LACRE, BTN_AUTORIZAR e SW_REDE|Fechar após retirada autorizada; fechar após violação; desligar a rede; religar a rede com eventos na fila|LCD; Serial|"Lacre fechado / reaplicado", "Lacre fechado / apos violacao", "Sem sinal / na fila: N" e "Sinal ok / enviados: N", cada uma por 2 s antes de voltar à tela normal|Esperado: o operador vê na hora o resultado do lacre e da rede (RF007, RF010).|
|CT43|Última posição no LCD|GPS (controle `fixValid`) e reinício do ESP32|Sem posição salva; posição válida e depois `fixValid=0` por segundos, minutos, horas e dias; reinício preservando a NVS|LCD (tela "Ultima posicao"); Serial|"sem fix ainda"; "ha N s", "ha N min", "ha N h" e "ha N dias"; depois do reinício, "salva, sem idade" e a indicação de restauração na Serial|Esperado: o dispositivo mostra a idade da última posição válida e não inventa um horário quando ela é desconhecida (RF005; RN08).|
|CT44|Vínculo inicial|ESP32 (primeiro boot, NVS vazia), BTN_CICLO e comando `REMESSA`|Ligar sem ciclo; `REMESSA` antes do vínculo; CICLO segurado por 3 s|LCD; Serial|`STATUS` com `ciclo_ativo=false` e `dias_fora=0`; tela de prazo "sem ciclo ativo"; `REMESSA` recusada; após 3 s, `CICLO_INICIADO` com `ciclo_id=1`, "Ciclo iniciado / aguardando NF" e dias contando a partir do vínculo|Esperado: o dispositivo só entra em ciclo quando é vinculado a uma remessa (RF002, RF003; RN02).|
|CT45|Falha do GNSS|GPS (controle `fixValid`) e SW_REDE|Coleta com fix; `fixValid=0` por duas coletas ou mais; `fixValid=1`; repetir sem rede|Serial|Um `GNSS_SEM_FIX` (`motivo=sem_fix`, `satelites=0`) na primeira coleta sem posição, sem repetir nas seguintes; um `GNSS_RECUPERADO` na primeira coleta válida, com `tempo_sem_posicao_s` e `coletas_sem_fix`; sem rede, os dois entram na fila como não críticos|Esperado: perda e retorno da posição ficam registrados uma vez cada, sem encher a fila (RF005, RF010; RN08, RN09).|

## 4. Divisão do trabalho

| | Pessoa A — ciclo, automação e posição | Pessoa B — interface local do V2 |
|---|---|---|
| CTs | Rodada V2 refeita (CT01–CT17, CT23, CT35), CT04 de 150 dias, CT18, CT43, CT44 e CT45 | CT36 a CT42 |
| Ferramenta principal | Wokwi CLI (`run_cts.py`) e Wokwi for VS Code para CT04-150, CT18 e CT43 | Wokwi for VS Code com gravação de tela e áudio |
| Tamanho | G | G |

### Quem mexe em quê

Cada arquivo tem um único dono. É isso que evita conflito na hora do merge.

| Arquivo ou trecho | Dono | A outra pessoa |
|---|---|---|
| `tests/wokwi-ci/*.yaml` que já existem e `run_cts.py` | A | não edita. Se B quiser automatizar alguma captura, cria `tests/wokwi-ci/ct3x-*.yaml` novo e roda direto com o `wokwi-cli` |
| `tests/wokwi-complementares/` | A | não edita |
| `documentacao.md`, parágrafos do circuito, "Estrutura do código" e "Situação dos CT01–CT16" | A | não edita |
| `documentacao.md`, subseção "Rodada do protótipo V2" | A | não edita |
| `documentacao.md`, subseção "CT36–CT42 — interface local do V2" | B | não edita |
| `documentacao.md`, subseção "CT43–CT45 — posição, vínculo e GNSS no V2" | A | não edita |
| `assets/…/3.4.1-prototipo-wokwi/README.md`, seções com os mesmos nomes acima | dono da subseção | não edita a seção do outro |
| Evidências `v2-*.serial.txt`, `ct*V2*.png`, `ct18V2*`, `ct43*`, `ct44*` e `ct45*` | A | — |
| Evidências `ct36*` a `ct42*` (fotos, vídeos e logs) | B | — |
| Firmware (`src/`, `include/`) | ninguém durante a rodada | um CT que falhar fica REPROVADO, com o motivo, e vira tarefa nova |

### Etapas

| # | Etapa | Quem | Pronto quando | Depende de | Prazo | Tamanho |
|---|---|---|---|---|---|---|
| 0.1 | Juntar as duas bases | A e B | As MRs dos cenários do wokwi-cli (rodada V2) e do boot sem ciclo/LED/GNSS estão na `develop` | — | Seg 2/11 | P |
| 0.2 | Esqueleto comum | A | Um commit cria na `documentacao.md` os três cabeçalhos ("Rodada do protótipo V2", "CT36–CT42…" e "CT43–CT45…", nessa ordem, antes de "Próximas sprints"), cada um com a tabela de definição da seção 3 e a linha "Resultados: pendente"; cria as mesmas seções no README das evidências; anota o SHA-256 do binário `demo`. B cria a branch a partir desse commit | 0.1 | Seg 2/11 | P |
| A1 | Cenários no boot novo | A | Vínculo (`btn_ciclo` com `pressed=1`, `delay: 3500ms`, `pressed=0`) em `ct01-03`, `ct04-limiar10`, `ct04-limiar150`, `ct08-09`, `ct14-16` e `ct17-35`; `run_cts.py` conferindo `ciclo_ativo=false` no boot, `ciclo_id` e contadores; `python tests/wokwi-ci/run_cts.py` todo PASS ou falha registrada | 0.2 | Ter 3/11 | M |
| A2 | CT44 e CT45 automatizados | A | `ct44-vinculo.yaml` e `ct45-gnss.yaml` com a verificação no `run_cts.py`; logs e capturas do LCD salvos | A1 | Ter 3/11 | M |
| A3 | CT04 de 150 dias | A | Rodada no Wokwi for VS Code com vínculo e log completo do dia 0 até depois do dia 150 | 0.2 | Ter 3/11 | P |
| A4 | CT18 com a ISR | A | Instrumentação de `tests/wokwi-complementares/` com o firmware atual; 20 transições com e sem rede; APROVADO ou REPROVADO com perdas e latência | 0.2 | Qua 4/11 | G |
| A5 | CT43 | A | Fotos de sem fix, segundos e minutos. Horas e dias executados ou registrados como pendentes. Restaurada pela instrumentação que preserva a NVS, ou pendente | A2 | Qua 4/11 | M |
| A6 | Documentação da parte A | A | Rodada V2 reescrita com a data, o SHA e os resultados novos (tabela de adaptação com LED e vínculo); subseção CT43–CT45 com resultados e `<details>`; parágrafos do circuito atualizados (LED 13, protoboard, boot sem ciclo) | A1–A5 | Qua 4/11 | M |
| B1 | CT36 e CT40 (navegação e apagamento) | B | Fotos e vídeos da seção 5 | 0.2 | Ter 3/11 | M |
| B2 | CT37 e CT41 (prazo e contagem) | B | Fotos e vídeo a partir do vínculo | 0.2 | Ter 3/11 | M |
| B3 | CT38 (buzzer e LED) | B | Duas violações separadas e uma retirada autorizada, com vídeo com áudio | 0.2 | Ter 3/11 | M |
| B4 | CT39 e CT42 (comandos e mensagens) | B | Fotos com o Monitor Serial legível nas recusas | 0.2 | Qua 4/11 | M |
| B5 | Documentação da parte B | B | Subseção CT36–CT42 com a tabela de resultados, um `<details>` por CT e a seção no README das evidências | B1–B4 | Qua 4/11 | M |
| F1 | Integração | A e B | As duas MRs na `develop`. A segunda só sincroniza, sem conflito; `scripts/validate-project.py` e links conferidos | A6, B5 | Qua 4/11, fim do dia | P |

O mesmo binário vale para as duas pessoas. Se o firmware da `develop` mudar antes da entrega, A roda a suíte automatizada de novo (minutos), e B só refaz o que a mudança afetar.

## 5. Capturas

Salvar em `assets/3-desenvolvimento-resultados/3.4.1-prototipo-wokwi/`, com imagens em camelCase e logs em kebab-case. As fotos mostram o LCD legível e o componente acionado. Nos comandos e nas recusas, o Monitor Serial também aparece. Legenda: "Fonte: captura real do simulador Wokwi for VS Code, DD/MM/2026", com a data real da execução.

**Comum a B:** antes de cada CT que precise de ciclo, segurar CICLO por 3 s. Esse é o vínculo, e a contagem de dias parte dele.

### Pessoa B

**CT36 — Navegação.** `ct36Nf.png` (NF fictícia ou "nao informada"), `ct36Prazo.png`, `ct36Posicao.png` (idade ou "sem fix ainda"), `ct36Bateria.png` (percentual e estado) e `ct36Lacre.png` ("Lacre INTEGRO / fechado"). Vídeo: as cinco telas avançando e voltando, e voltar da NF levando ao lacre.

**CT37 — Prazo.** Gravar desde o vínculo e extrair os quadros: `ct37AntesPrazo.png` ("Faltam N dias"), `ct37FaltaUmDia.png` (dia 4), `ct37VenceHoje.png` (dia 5; dura cerca de 1 s), `ct37Atraso.png` e `ct37SemCiclo.png`. A última vem depois de lançar `NF` e encerrar o ciclo, quando a mensagem de encerramento já sumiu.

**CT38 — Buzzer e LED.** O vídeo com áudio é a evidência principal. Fotos: `ct38Violacao.png` (durante os bipes, com o LED vermelho aceso no quadro e `VIOLACAO_LACRE` na Serial), `ct38Fechado.png`, `ct38Limite30s.png` (com o tempo da simulação) e `ct38RetiradaAutorizada.png` (LED apagado, `RETIRADA_AUTORIZADA` na Serial). São duas violações separadas: uma encerrada pelo fechamento e outra mantida aberta até os 30 s.

**CT39 — REMESSA.** `ct39NfRegistrada.png`, `ct39TelaNf.png`, `ct39Limite15.png` (`123456789012345`), `ct39CaractereInvalido.png` (`NF@123`), `ct39LongaDemais.png` (`1234567890123456`) e `ct39SemCiclo.png` (`NF-000999` com o ciclo encerrado). Nas três recusas, a mensagem da Serial precisa estar legível.

**CT40 — Apagamento.** `ct40Antes.png`, `ct40Apagado.png` (chave de energia ligada), `ct40PrimeiroAvancar.png` (ainda na NF), `ct40SegundoAvancar.png` (prazo), `ct40PrimeiroVoltar.png` (ainda no prazo) e `ct40SegundoVoltar.png` (NF). O vídeo mostra os 15 s.

**CT41 — Contagem.** `ct41ContagemAutorizar.png`, `ct41ContagemCiclo.png` ("Iniciar ciclo" no primeiro vínculo; "Encerrar ciclo" com o ciclo ativo) e `ct41SolturaAntecipada.png` (Serial). O vídeo mostra os dois títulos e a confirmação aos 3 s.

**CT42 — Mensagens temporárias.** `ct42Reaplicado.png`, `ct42AposViolacao.png`, `ct42SemSinal.png` e `ct42SinalOk.png`, com as chaves visíveis. Gravar antes de acionar, porque cada mensagem dura 2 s.

### Pessoa A

**Rodada V2.** Os mesmos nomes da rodada de 30/10 (`ct01V2.png`…, `v2-*.serial.txt`), regravados pelo `run_cts.py`.

**CT43 — Posição.** `ct43SemFix.png`, `ct43Segundos.png`, `ct43Minutos.png`, `ct43Horas.png`, `ct43Dias.png` e `ct43Restaurada.png`. Horas e dias usam tempo real mesmo no `demo`. Se essas esperas não forem feitas, os subcenários ficam pendentes. A chave ON/OFF não serve para a restaurada: é preciso reiniciar o ESP32 preservando a NVS.

**CT44 — Vínculo.** `ct44SemCiclo.png` (tela de prazo "sem ciclo ativo" logo após ligar), `ct44RemessaRecusada.png` (Serial) e `ct44Iniciado.png` ("Ciclo iniciado / aguardando NF"), mais o log `ct44-vinculo.serial.txt`.

**CT45 — GNSS.** `ct45SemFix.png` e `ct45Recuperado.png`, com a Serial, mais o log `ct45-gnss.serial.txt`.

## 6. Pontos que precisam de decisão

1. **Numeração (decidido).** Os CTs do Wokwi V2 usam CT36–CT45. Os CTs do protótipo físico (3.4.2) começam no CT46.
2. **GNSS.** O chip GPS do circuito sempre envia NMEA e só informa 8 ou 0 satélites. Por isso só `motivo=sem_fix` é alcançável no Wokwi. `sem_dados` e `sinal_fraco` exigem novos controles no chip (`chips/gps.chip.*`) ou ficam cobertos só pelos testes em host e registrados como limitação.
3. **Rodada V2 de 30/10.** Ela ainda não chegou à `develop` e descreve o boot antigo. A proposta é substituí-la pela execução nova, sem preservá-la como histórico.
4. **Vídeos.** Onde ficam e como se chamam continua em aberto. A sugestão é camelCase, na mesma pasta das fotos (`ct38Buzzer.mp4`), ou um link externo se o tamanho pesar no repositório.
5. **Ajustes ao roteiro anterior.** O CT37 começa no vínculo, porque não há mais "ciclo novo" automático. O CT38 inclui o LED vermelho. O CT41 mostra "Iniciar ciclo" no primeiro vínculo. O CT44 e o CT45 são novos.
