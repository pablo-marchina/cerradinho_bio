# Registro complementar de execução — CT01–CT16 e CT35

## Identificação e alcance

- Data: 22/10/2026, fuso America/Sao_Paulo.
- Responsável pela operação e captura: Codex, na sessão solicitada pelo usuário.
- Versão: commit `7317dd9`. Firmware demo SHA-256 `E3A60385B943E5B5230535E85D1F9F7426F529F241CB59B9B8A5FCA669AB6F49`.
- Ambiente: Wokwi for VS Code 4.0.1, `src/firmware/prototipo_wokwi/diagram.json`, `wokwi.toml`, `.pio/build/demo/firmware.bin`.
- Configuração: 1 segundo simulado = 1 dia do ciclo; prazo 5 dias; permanência 150 dias; autorização 3 dias; fila 8; debounce configurado de botão 50 ms e lacre 60 ms. Rede simulada independente da conexão Wi-Fi. As rodadas anteriores de CT04 também utilizaram limiar 10.
- Durante as execuções não houve alteração de firmware, circuito, seção 3.4.1 ou RTM. Após os testes, a documentação da Sprint 1 foi atualizada com os resultados, as capturas e os links para estes registros e logs. Os critérios existentes e os IDs foram preservados; a redação do rearme da bateria foi alinhada à RN10 (superar 25%). Os prints enviados pelo usuário foram preservados; estas são capturas adicionais da execução real.
- Chaves e botões operados pela interface do simulador. Pressões temporizadas usam o recurso de botão mantido do Wokwi: Ctrl+Espaço para pressionar, Espaço para liberar. O potenciômetro foi ajustado pelo controle acessível da interface, sem alterar variáveis internas do firmware.
- Serial recebida diretamente pela porta RFC2217 local 4000, a 115200 baud. Os logs são bytes recebidos do simulador; não foram reconstruídos. Capturas nativas foram convertidas para o formato PNG, sem modificar seu conteúdo.
- Escopo desta conclusão: os casos existentes CT01–CT16 e CT35. CT17–CT23, CT24–CT34 e os novos cenários propostos a partir de CT36 não são declarados executados por este registro.

O [registro anterior](REGISTRO-2026-10-22.md) contém os procedimentos, resultados e logs de CT04, CT10, CT14 e CT16. Este documento o complementa e fornece o índice consolidado. A situação de aprovação abaixo se refere ao comportamento funcional observado no simulador; limites de medição estão indicados, sem declarar validação física, entrega ao servidor ou precisão temporal instrumental.

## Índice consolidado

| CT | Situação | Resultado e evidências |
|---|---|---|
| CT01 | APROVADO no simulador | Inicialização com lacre íntegro, dias zero, verde aceso e demais LEDs apagados. [Print](CT01.png); log A. Wi-Fi ainda desconectado no primeiro JSON, conforme ressalva abaixo. |
| CT02 | APROVADO no simulador | Um status no dia 7 e outro no dia 14, sem status semanal intermediário. [Dia 7](CT02.png), [dia 14](CT02-dia14.png); log B. |
| CT03 | APROVADO no alerta funcional | Um único `ALERTA_ATRASO`, no dia 6; amarelo observado em fases acesa/apagada. [Print](CT03.png); logs A/B. Frequência exata de 1 Hz não medida. |
| CT04 | APROVADO no simulador | Alertas únicos nas rodadas de limiar 150 e 10. [150](CT04.png), [10](CT04-limiar10.png); [registro anterior](REGISTRO-2026-10-22.md). |
| CT05 | APROVADO na detecção local | Abertura não autorizada gerou `VIOLACAO_LACRE` crítico fora do status semanal. [Evento e vermelho aceso](CT05-evento.png), [estado posterior](CT05.png); log B. |
| CT06 | APROVADO no simulador | `RETIRADA_AUTORIZADA` dentro da janela, sem violação nessa retirada, seguida de `LACRE_REAPLICADO`. [Retirada](CT06-retirada.png), [reaplicação](CT06.png); log B. |
| CT07 | APROVADO no simulador | Abertura depois da autorização vencer gerou violação crítica, sem retirada autorizada. [Print](CT07.png); log A. |
| CT08 | APROVADO no armazenamento simulado | Sem rede simulada, violação guardada e exatamente um pendente no primeiro registro. Vermelho e azul acesos na captura. [Print](CT08.png); log B. |
| CT09 | BLOQUEADO para aprovação integral | Reenvio local de dois eventos na ordem, com dias originais preservados, fila zero e azul apagado. [Antes](CT09-antes.png), [depois](CT09.png); log B. Falta transporte e confirmação do servidor exigida pelo CT. |
| CT10 | APROVADO na fila/reenvio simulado | Rodadas A e B registradas, incluindo resumo de quatro violações após oito individuais. [Resumo](CT10.png); [registro anterior](REGISTRO-2026-10-22.md). Sem confirmação de servidor. |
| CT11 | APROVADO nos estímulos aplicados | Três pressões curtas: três autorizações; uma pressão mantida: uma autorização, sem repetição. [Evento](CT11.png), [pressão mantida](CT11-pressao-mantida.png); log A. Não foi medida a fronteira exata de 50 ms. |
| CT12 | APROVADO no simulador | Sequência 17% → 22% → 17% → 31% → 17% confirmou histerese: somente dois alertas, um em cada queda com rearme válido. [Segundo alerta](CT12.png) e sequência detalhada abaixo; log A. |
| CT13 | APROVADO no diagnóstico simulado | Partindo de 78%, entrada zero gerou uma falha do sensor e nenhum alerta de bateria baixa. [Rodada isolada](CT13-isolado.png), [rodada adicional](CT13.png); log C e log A. |
| CT14 | REPROVADO na consistência do JSON | Fluxo encerrou e permitiu reutilizar, mas o JSON de encerramento manteve `ciclo_ativo=true` e dias anteriores. [Encerramento](CT14.png), [reutilização](CT14-reutilizado.png); [registro anterior](REGISTRO-2026-10-22.md). Não corrigido nem reclassificado como aprovado. |
| CT15 | APROVADO no simulador | Toque curto com ciclo 1 ativo gerou `VINCULO_RECUSADO`, motivo `ciclo_ativo`; nenhum novo ciclo foi iniciado. [Print](CT15.png); log B. |
| CT16 | APROVADO nas duas recusas | Lacre aberto com NF e lacre fechado sem NF recusaram encerramento, mantendo ciclo ativo. [A](CT16.png), [B](CT16-B-nf-ausente.png); [registro anterior](REGISTRO-2026-10-22.md). |
| CT35 | APROVADO no desligamento lógico observado | Desligamento registrado, LEDs apagados, silêncio na Serial por 26.535 ms simulados; religamento publicou `STATUS` e restaurou LEDs do ciclo. [Desligado](CT35-desligado.png), [religado](CT35.png); log A. |

## Logs desta complementação

- **Log A:** [CT01, CT07, CT11, CT12, CT13 adicional e CT35](CT01-CT07-CT11-CT12-CT35-execucao.serial.txt). Contém dois boots: o primeiro ensaio foi reiniciado após aproximadamente 12 segundos para enquadrar a inicialização. Para contagens dos testes desta rodada, usar o segundo boot, a partir do segundo `ets Jul 29 2019`. Os dois boots originais estão preservados no arquivo.
- **Log B:** [CT02, CT03, CT05, CT06, CT08, CT09 e CT15](CT02-CT03-CT05-CT06-CT08-CT09-CT15-execucao.serial.txt). Nova rodada, gravada desde o boot até o reenvio dos dois eventos. Ciclo 1 e contexto não restaurado na inicialização.
- **Log C:** [CT13 isolado](CT13-isolado.serial.txt). Continuação da rodada B, depois da reconexão, iniciada com bateria saudável. Não contém novo boot.

Todos os tempos abaixo são `uptime_ms` do respectivo boot, não horário civil. Os capturadores foram encerrados depois de pausar a simulação; os arquivos foram conferidos até a última linha recebida.

## CT01 — inicialização

Critério: CT01 da seção 3.4.1, RF009. Uma captura de inicialização, com lacre fechado e rede simulada disponível.

No segundo boot do log A, o primeiro `STATUS`, em `1633`, informou `dias_fora=0`, `seal="intact"`, `ciclo_id=1`, `ciclo_ativo=true` e `contexto_restaurado=false`. O print foi feito com a simulação pausada em aproximadamente 2,35 s, com somente o verde aceso.

Ressalva: `wifi_connected=false` nesse primeiro JSON; a conexão Wi-Fi aconteceu depois, seguida de heartbeat com `wifi=on`. A aprovação cobre a inicialização e a publicação local por Serial com `network_sim_available=true`, sem comprovar envio do primeiro status a um servidor.

## CT02 e CT03 — periodicidade e atraso

Critérios: CT02, RF011/RF009 e RN07; CT03, RF013/RF012 e RN04.

No log B, antes dos 19 segundos simulados, os únicos `STATUS_SEMANAL` foram em `8627`, dia 7, e `15627`, dia 14. Um único `ALERTA_ATRASO` saiu em `7627`, dia 6. A captura de dia 7 e a captura de atraso mostram a mesma região real da Serial, por isso os arquivos CT02.png e CT03.png têm o mesmo conteúdo.

O segundo boot do log A também permaneceu ativo até depois do dia 287, com exatamente um alerta de atraso e status semanais nos múltiplos de 7 até o desligamento. Isso complementa as fotografias com observação contínua de não duplicação. O amarelo foi visto aceso e apagado, mas a frequência exata de 1 Hz não recebeu medição instrumental; essa verificação temporal permanece pendente para uma aprovação metrológica do critério.

## CT05 e CT06 — abertura e reaplicação

Critérios: CT05, RF006/RN05; CT06, RF007/RN06. Uma retirada autorizada e uma abertura posterior sem autorização válida, ambas com rede disponível.

1. Autorização em `45015` no log B. A abertura foi lida em `45323` com `authorized_removal=true`; `RETIRADA_AUTORIZADA` saiu em `45352`, 337 ms depois da autorização e dentro dos 3.000 ms da janela demo.
2. Fechamento em `51016`; `LACRE_REAPLICADO` em `51052`. Nenhuma `VIOLACAO_LACRE` ocorreu entre a autorização dessa retirada e sua reaplicação.
3. Nova abertura, sem autorização válida, em `56431`, com `authorized_removal=false`; `VIOLACAO_LACRE` crítica em `56472`, dia 54, fora dos dias semanais. O print complementar CT05-evento.png mostra o evento e o vermelho aceso. O CT05.png mostra o estado posterior e uma fase apagada do piscar.

Os 41 ms entre `seal_changed` e `VIOLACAO_LACRE` são intervalo entre dois registros do firmware, não latência desde a estabilização física do contato. Não foi introduzida condição de GNSS sem fix nesta rodada. A detecção durante essa condição pertence ao ensaio específico de lacre do CT18.

## CT07 e CT11 — autorização vencida e botão

Critérios: CT07, RF006 e RN05/RN06; CT11, RF007. Lacre fechado durante as pressões, sem retirada anterior nessa rodada.

No segundo boot do log A, foram aplicadas três pressões curtas reais pelo controle do Wokwi, separadas por liberação: autorizações em `33486`, `33952` e `34453`. Em seguida, uma pressão mantida gerou apenas uma autorização, em `60881`. Heartbeats de 62 a 88 segundos mostraram `autorizar=1`, sem outra autorização nesse intervalo. Depois da liberação, a leitura voltou a zero e a contagem total permaneceu quatro.

O botão do circuito conserva o bounce padrão do Wokwi. Não foram injetados pulsos calibrados para medir a fronteira de 50 ms; o resultado cobre as pressões aplicadas com esse modelo e a ausência de repetição durante a pressão mantida por mais de 3 segundos simulados.

Depois de a autorização vencer, o lacre foi aberto: `seal_changed` em `111300`, `authorized_removal=false`; `VIOLACAO_LACRE` em `111343`, crítica. A última autorização havia ocorrido 50.462 ms antes do evento, muito além dos 3.000 ms permitidos. Não houve `RETIRADA_AUTORIZADA` nesse boot. O lacre foi fechado em seguida.

## CT08 e CT09 — armazenamento e reconexão

Critérios: CT08, RF010/RF008 e RN05/RN09; CT09, RF010/RF008 e RN09. Fila vazia e lacre fechado antes de indisponibilizar a rede.

1. No log B, a abertura sem rede foi lida em `96725`, `authorized_removal=false`. A Serial registrou `[FILA] guardado: VIOLACAO_LACRE` e `[FILA] 1 pendente`. O print CT08.png mostra o evento pendente e os LEDs vermelho/azul acesos. O Wi-Fi permaneceu conectado, como previsto para SW_REDE.
2. Mantendo a rede indisponível, o status semanal do dia 98 também foi guardado. A fila passou a dois pendentes, sem saturação.
3. Ao restaurar a rede, a Serial anunciou reenvio de dois eventos na ordem. Saíram a violação, com `uptime_ms=96836` e `dia_ocorrencia=95`, e o status, com `uptime_ms=99627` e `dia_ocorrencia=98`. Depois, `[FILA] 0 pendentes`, com azul apagado.

**CT09 permanece BLOQUEADO para aprovação integral:** não existe transporte HTTP/MQTT nem ACK de servidor neste firmware. Esvaziar a fila e imprimir JSON na Serial não comprova entrega confirmada. Os campos `network_sim_available=false` nos JSONs reenviados refletem a condição original armazenada no evento; não indicam que a chave continuava sem rede no momento do reenvio.

## CT12 — bateria baixa e histerese

Critério: CT12, RF019/RF027 e RN10. Bateria inicialmente saudável, depois submetida à sequência abaixo. O ciclo e a rede permaneceram ativos.

| Etapa | Entrada do potenciômetro | Leitura observada | Resultado |
|---|---|---|---|
| Primeira queda | 184 | 17% | Primeiro `BATERIA_BAIXA` em `225386`. [Print](CT12-primeiro-alerta.png). |
| Recuperação insuficiente | 230 | 22% | Nenhum novo alerta. [Print](CT12-recuperacao-insuficiente.png). |
| Nova queda, sem rearme | 184 | 17% | Continuou sem novo alerta. [Print](CT12-sem-repeticao.png). |
| Recuperação acima de 25% | 320 | 31% | Leitura saudável confirmada antes de reduzir novamente. [Print](CT12-recuperacao.png). |
| Segunda queda, após rearme | 184 | 17% | Segundo `BATERIA_BAIXA` em `265814`. [Print](CT12.png). |

Exatamente dois alertas de bateria baixa no segundo boot do log A. A permanência em 17% e a passagem 22% → 17% não duplicaram o primeiro. A preparação anterior à primeira queda incluiu leituras de 78% e 25%; não houve alerta nessas condições.

Esses percentuais são leituras reais do firmware simulado. Não foram arredondados para 18% ou 30% no registro. Os limites exatos de 20% e 25%, propostos como novos casos, não são declarados cobertos por esta sequência.

## CT13 — entrada zero isolada

Critério: CT13, RF019/RF027 e RN10.

Uma primeira aplicação de zero, depois do CT12, gerou `SENSOR_BATERIA_FALHA` em `272814` no log A. Como o alerta de bateria baixa já estava acionado antes desse estímulo, esse resultado isoladamente não bastaria para excluir que a histerese tivesse ocultado um alerta falso.

Por isso foi feita uma rodada independente ao final do log B, com gravação no log C: bateria em 78%, sem qualquer alerta de bateria baixa naquele boot, seguida diretamente de entrada zero. Houve um único `SENSOR_BATERIA_FALHA`, em `110950`, leitura 0%, e nenhum `BATERIA_BAIXA`. Heartbeats seguintes confirmaram 0%; depois a entrada foi restaurada para 800 e a leitura retornou a 78%.

Isso valida o diagnóstico lógico da entrada sintética. Divisor desconectado e diagnóstico elétrico físico permanecem no CT30 reservado.

## CT15 — vínculo simultâneo

Critério: CT15, RF004/RN01. Ciclo 1 ativo, lacre fechado, rede disponível; um toque curto no botão de ciclo.

No log B, `VINCULO_RECUSADO` em `33150`, motivo `ciclo_ativo`, `ciclo_id=1`, `ciclo_ativo=true`. Não houve `CICLO_INICIADO` nem `CICLO_ENCERRADO` nessa rodada. Os dias continuaram avançando no ciclo existente; não se espera congelamento do relógio durante a recusa.

## CT35 — chave de desligamento lógico

Critério: CT35 da seção 3.4.1. Uma sequência desligar/religar, com lacre fechado, rede disponível e bateria em 78%.

No log A, `DISPOSITIVO_DESLIGADO` em `293129`, ciclo 1 ativo, dia 291. A captura feita com o relógio do simulador em aproximadamente 311,8 s mostra os quatro LEDs apagados e a Serial ainda parada nesse registro. Não há linha de heartbeat ou de outro evento entre o desligamento e o próximo `STATUS`, em `319664`: intervalo de 26.535 ms simulados.

Ao religar, verde e amarelo voltaram ao estado do ciclo. O primeiro `STATUS` manteve `dias_fora=291`, com `dia_ocorrencia=318`; em seguida houve atualização dos status semanais pendentes dos dias 294, 301, 308 e 315. O heartbeat em tempo 319 mostrou `dias_fora=318`, confirmando que o relógio continuou durante o desligamento lógico. A ordem dessas mensagens foi preservada no log, sem apresentar o primeiro JSON como se os dias já estivessem atualizados.

Não se cortou a alimentação do ESP32. Não foram movimentados lacre/botões durante o desligamento; a hipótese de abrir e fechar o lacre com o dispositivo desligado continua separada, para discussão da regra com o grupo.

## Conferência e pendências

Foi feita conferência automática dos JSONs reais e das contagens: quatro autorizações no ensaio do CT11; violação após expiração; dois alertas na sequência da bateria; zero alerta falso na rodada isolada de sensor; intervalo sem mensagens durante desligamento; dias 7/14; ordem dos dois eventos reenviados; assinatura PNG dos arquivos. Essa conferência não substitui a execução e não verifica os critérios ainda sem instrumentação. O SHA-256 do firmware permaneceu igual ao registrado antes da execução.

Pendências e acompanhamento após a execução:

1. **CT14:** corrigir a consistência do estado publicado em `CICLO_ENCERRADO` e executar novamente. A falha observada continua REPROVADA.
2. **CT09:** implementar/integrar transporte e confirmação do servidor, então verificar que somente eventos confirmados são removidos. Até lá, aprovação integral BLOQUEADA.
3. **Medições temporais:** medir a frequência de 1 Hz do amarelo e, se exigida aprovação temporal completa, usar estímulos/instrumentação para debounce e latência. Os prints e os registros acima não comprovam essas precisões.
4. **Integração documental — CONCLUÍDA:** registros, prints e logs vinculados na seção 3.4.1 e na RTM da [documentação principal](../../document/documentacao.md), com atualização da compatibilidade da RN11. Somente as seções 3.3.2, 3.3.4 e 3.4.1 da Sprint 1 foram alteradas; IDs preservados e demais sprints intactas.
5. Os novos casos de falha combinada propostos a partir de CT36 e a eventual regra para lacre aberto com equipamento desligado continuam fora desta execução dos CTs existentes.

Estado final da bancada: simulador pausado em aproximadamente 132,3 s da última rodada; ciclo 1 ativo, lacre fechado, rede simulada disponível, bateria 78%, fila zero; capturadores Serial encerrados. O estado final descrito no registro anterior corresponde àquela execução anterior e foi sucedido por esta rodada.

Nenhum commit, push ou MR foi realizado. As evidências estão disponíveis no diretório `assets/wokwi/` para revisão.
