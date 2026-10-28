# Circuito principal Wokwi: GuaráTracking

Esta pasta reúne o circuito principal integrado: ESP32 DevKit v1, GPS/GNSS
customizado, lacre, bateria simulada, quatro LEDs com resistores de 220 Ω,
dois botões, chave de rede simulada e chave ON/OFF. O diagrama anterior da raiz foi incorporado aqui.

O firmware principal preserva a comparação de posições, o debounce do lacre e a
registro NVS de diagnóstico existentes, tem o ciclo (dias fora, status semanal e alertas de
atraso e de permanência, CT02–CT04) e as regras dos CT05–CT16: retirada autorizada, violação do
lacre, fila de eventos sem rede, debounce dos botões, bateria baixa, encerramento e
vínculo de ciclo, além do registro de desligamento pela chave ON/OFF. JSON na serial
e Wi-Fi conectado não representam envio confirmado a servidor ou broker.

## Módulo GNSS de referência

O módulo físico escolhido em 21/10/2026 é o **Quectel LC76G, variante PA**.
O chip customizado desta pasta ainda é um gerador genérico de NMEA: sua adaptação
ao protocolo e aos estados de energia do LC76G-PA está pendente. O modelo deverá
usar o mesmo driver GNSS do dispositivo físico, incluindo os comandos PAIR usados,
entrada/saída de backup e falhas de aquisição. A placa e a versão de firmware
do módulo ainda precisam ser definidas.

O contrato de simulação e os CT26/CT28 estão no
[planejamento de energia e GNSS](../../../document/outros/3-desenvolvimento-resultados/3.4.1-prototipo-wokwi/planejamento-energia-gnss.md).
Os 9600 baud da configuração atual descrevem o simulador; conferir a configuração
da unidade física antes de integrar. Consumo e autonomia exigem ensaios físicos.

## Arquivos

| Arquivo | Conteúdo |
|---|---|
| `diagram.json` | Único diagrama principal, com todos os componentes integrados. |
| `wokwi.toml` | Simulação demo: firmware compilado na raiz, coleta a cada 30 s e serial RFC2217 na porta 4000. |
| `wokwi.weekly.toml` | Mesmo circuito com coleta a cada sete dias reais. |
| `../main.cpp` | Firmware principal compilado pelo PlatformIO. |
| `../../../include/cycle_rules.h` | Regras do ciclo (CT02–CT04), testadas em `tests/model_tests.c`. |
| `../../../include/seal_rules.h`, `event_queue.h`, `battery_rules.h` | Regras de lacre, fila sem rede e bateria (CT05–CT13), testadas em `tests/model_tests.c`. |
| `../../../include/sim_clock.h` | Relógio de 64 bits do ciclo, que atravessa a volta do `millis()`, testado em `tests/model_tests.c`. |
| `../../../include/config.h` | Pinagem e parâmetros centralizados do firmware principal. |
| `sketch.ino` | Teste isolado de ligações, excluído da compilação principal. Conta caracteres do GPS, sem interpretar posição. |

## Mapa de pinos

| Componente | Pino | Modo | Função atual de bancada |
|---|---|---|---|
| GPS TX → ESP RX2 | GPIO 16 | UART2 RX, 9600 baud | Recepção das sentenças GNSS simuladas |
| GPS RX ← ESP TX2 | GPIO 17 | UART2 TX, 9600 baud | Reservado; o modelo não interpreta comandos |
| Lacre (`seal`) | GPIO 27 | `INPUT_PULLUP` | Esquerda = íntegro/LOW; direita = violado/HIGH |
| Bateria (`battery`) | GPIO 34 (ADC1) | analógico | Nível sintético de 0 a 100%, sem alimentação real |
| LED verde | GPIO 25 | saída | Operando, lacre íntegro; piscando = retirada autorizada em curso |
| LED amarelo | GPIO 26 | saída | Piscando a 1 Hz = atraso; fixo = permanência acima do limiar |
| LED vermelho | GPIO 23 | saída | Piscando = lacre violado; GPIO27 permanece exclusivo do lacre |
| LED azul | GPIO 14 | saída | Piscando = rede simulada indisponível |
| SW_REDE (`sw_rede`) | GPIO 18 | `INPUT_PULLUP` | Esquerda = disponível/LOW; direita = indisponível/HIGH |
| SW_ENERGIA (`sw_energia`) | GPIO 19 | `INPUT_PULLUP` | ON/OFF: esquerda = ligado/LOW; direita = desligado/HIGH |
| BTN_AUTORIZAR (`btn_autorizar`) | GPIO 33 | `INPUT_PULLUP` | Registra a autorização de retirada, válida por 3 dias |
| BTN_CICLO (`btn_ciclo`) | GPIO 13 | `INPUT_PULLUP` | Toque curto = novo vínculo; segurar 2 s = encerrar o ciclo |

GPS e potenciômetro têm VCC em 3V3 e GND comum. Cada saída de LED passa por um
resistor de 220 Ω até o ânodo; o cátodo vai ao GND. Botões ligam contatos de grupos
opostos ao GPIO e ao GND. As chaves usam o terminal 2 no GPIO, o terminal 1 no GND
e o terminal 3 aberto. Não há segundo lacre nem segundo potenciômetro.

`network_sim_available` informa a chave; `wifi_connected` informa a conexão real
do Wi-Fi virtual. A chave não desliga o rádio nem implementa transporte ou reenvio.

## Como rodar

1. Abra a raiz `g01` no VS Code. Instale as extensões recomendadas e use sua licença Wokwi.
2. Execute os comandos abaixo no terminal da raiz:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\setup.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\build.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\build.ps1 -Environment weekly
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\verify.ps1
```

3. Em **F1 → Wokwi: Select Config File**, selecione o `wokwi.toml` desta pasta.
4. Execute **F1 → Wokwi: Start Simulator**. Para visualizar a montagem, abra o `diagram.json` desta pasta.
5. Para o intervalo semanal real, selecione o `wokwi.weekly.toml` desta pasta.

Os caminhos do firmware e do chip nos TOMLs são relativos a esta pasta e apontam
para `.pio/` e `chips/` na raiz. O PlatformIO seleciona somente `src/firmware/main.cpp`,
evitando compilar o `sketch.ino` de teste junto com o firmware principal.

## Roteiro do circuito integrado

| Ação | Resultado esperado |
|---|---|
| Iniciar com as chaves à esquerda | Verde aceso; vermelho e azul apagados; `STATUS` com `dias_fora=0`; heartbeat a cada 2 s |
| Mover o lacre à direita | Verde apaga e vermelho pisca após debounce; evento `seal_changed` |
| Pressionar BTN_AUTORIZAR | `AUTORIZACAO_REGISTRADA`; abrir o lacre em até 3 s gera `RETIRADA_AUTORIZADA` e verde piscando |
| Pressionar BTN_CICLO | Toque curto com ciclo ativo: `VINCULO_RECUSADO`; segurar 2 s: encerramento (exige lacre fechado e `NF` no Serial) |
| Mover SW_ENERGIA à direita | `DISPOSITIVO_DESLIGADO` e LEDs apagados; de volta à esquerda, novo `STATUS` |
| Mover SW_REDE à direita | Azul pisca e `rede_sim=off`; Wi-Fi real continua com diagnóstico independente |
| Girar o potenciômetro | `bateria_sim` muda no heartbeat |
| GPS com fix válido | `gps_chars` aumenta; posição é coletada e comparada com a última referência válida |
| GPS com `fixValid=0` | Reporta falta de fix e preserva a referência anterior |

Pressione os botões durante um heartbeat para conferir sua leitura periódica.
Comandos no Monitor Serial: `NF` lança a NF de retorno (representa o lançamento na
plataforma) e `LIMIAR n` muda o limiar de permanência do ciclo (RN03). O modo semanal não valida autonomia.

## Validação da integração — 21/10/2026

| Verificação executada | Resultado |
|---|---|
| `scripts/setup.ps1` | Dependências disponíveis e ferramentas verificadas |
| `scripts/build.ps1` | Demo compilada com sucesso |
| `scripts/build.ps1 -Environment weekly` | Semanal compilada com sucesso |
| `scripts/verify.ps1` | Testes de lógica, consistência do circuito/configuração e linter oficial aprovados |

O compilador emitiu um aviso de fallthrough na dependência TinyGPSPlus 1.0.3.
O linter terminou sem erros nem warnings e informou que a placa DevKit v1 usa um
tipo não documentado no catálogo atual. A placa foi preservada para compatibilidade
com o circuito e a configuração anteriores.

A simulação interativa não foi executada nesta integração; não havia token CLI
disponível. O roteiro visual acima e a validação física permanecem pendentes.
Compilação e testes locais não comprovam consumo, autonomia ou entrega ao servidor.

## Evidências anteriores

- `assets/3-desenvolvimento-resultados/3.4.1-prototipo-wokwi/circuito.png`: circuito anterior do teste de ligação, sem o GPS integrado.
- Projeto público anterior: https://wokwi.com/projects/478276108359405569

O projeto público e a captura anterior não foram atualizados por esta integração.
O roteiro acima descreve resultados esperados; não é evidência de execução visual.

## Teste de ligação

Executado em 19/out/2026 no projeto público. Evidência: `assets/3-desenvolvimento-resultados/3.4.1-prototipo-wokwi/circuito.png`.

| Ação | Esperado | Resultado |
|---|---|---|
| Nada acionado | LEDs apagados; `lacre=1 rede=1 autorizar=1 ciclo=1` | Passou |
| SW_LACRE acionada | LED verde aceso; `lacre=0` | Passou |
| SW_REDE acionada | LED azul aceso; `rede=0` | Passou |
| BTN_AUTORIZAR pressionado | LED amarelo aceso; `autorizar=0` | Passou |
| BTN_CICLO pressionado | LED vermelho aceso; `ciclo=0` | Passou |
| POT_BATERIA de um extremo ao outro | `bateria_adc` de 0 a ~4095 | Passou |

## Tempo simulado, prazos e LEDs (CT02–CT04)

O ciclo fica em `include/cycle_rules.h` e é chamado pelo `main.cpp` a cada volta do
`loop()`, sem `delay()`. No ambiente `demo`, 1 segundo real equivale a 1 dia
(`Config::DayMs`); no `weekly`, o dia é real. Os dias vêm de um relógio de 64 bits
(`include/sim_clock.h`) que acumula os intervalos do `millis()`, porque o contador de 32 bits
dá a volta em ~49,7 dias. As constantes ficam no `include/config.h`.

| Constante | Valor | O que controla |
|---|---|---|
| `StatusIntervalDays` | 7 | `STATUS_SEMANAL` a cada 7 dias de ciclo |
| `ExpectedReturnDays` | 5 | Acima disso, `ALERTA_ATRASO` e amarelo piscando a 1 Hz |
| `StayThresholdDays` | 150 | Ao atingir, `ALERTA_PERMANENCIA` e amarelo fixo |

Cada alerta sai uma única vez por ciclo, e os dois podem coexistir; mudar o limiar
pelo comando `LIMIAR n` não rearma um alerta já emitido no ciclo (RN04). Ao ligar pela
primeira vez, o dispositivo publica um `STATUS` com `dias_fora=0`; depois de um reinício,
o `STATUS` traz o ciclo restaurado da NVS e `contexto_restaurado=true`. Em hardware real,
apague a NVS antes de repetir um CT que parte do ciclo 1. Os eventos usam o mesmo JSON dos
demais registros, com `ciclo_id`, `dias_fora`, `atraso` e `permanencia`, e o heartbeat
mostra `dias_fora`.

Para o CT04 com limiar 10, troque `StayThresholdDays` para `10`, recompile, rode e
depois volte para `150`.

## Lacre, autorização, fila, bateria e ON/OFF (CT05–CT16, CT35)

| Regra | Comportamento | Onde |
|---|---|---|
| Violação (RN05) | Lacre aberto sem autorização vigente gera `VIOLACAO_LACRE` na hora, marcado como crítico; vermelho pisca até o lacre fechar (`LACRE_FECHADO`) | `seal_rules.h` |
| Retirada autorizada (RN06) | `BTN_AUTORIZAR` vale por `AuthorizationWindowDays` (3 dias = 3 s no demo), medidos em ms desde o registro, e é consumido na abertura: `RETIRADA_AUTORIZADA`, verde piscando e `LACRE_REAPLICADO` ao fechar | `seal_rules.h` |
| Fila sem rede (RN09) | Com `SW_REDE` à direita, os eventos vão para a fila (8), gravada na NVS (`p0`–`p7`); cheia, sai o não crítico mais antigo. Com as 8 posições ocupadas por violações, as seguintes entram num único resumo (quantidade, primeira e última ocorrência), também na NVS. Ao reconectar, saem na ordem com `dia_ocorrencia` e, por último, o `VIOLACAO_LACRE_RESUMO`, que é zerado após o envio; nenhuma violação é perdida (CT10) | `event_queue.h` |
| Botões | Debounce de 50 ms e evento por borda; segurar não repete | `main.cpp` |
| Bateria (RN10) | `BATERIA_BAIXA` uma vez abaixo de 20%, rearma acima de 25%; ADC = 0 gera `SENSOR_BATERIA_FALHA` | `battery_rules.h` |
| Ciclo (RN01, RN11) | Toque curto com ciclo ativo: `VINCULO_RECUSADO`; segurar 2 s: `CICLO_ENCERRADO` só com lacre fechado e NF de retorno, senão `ENCERRAMENTO_RECUSADO` com `motivo`; com o ciclo encerrado, toque curto: `CICLO_INICIADO` | `main.cpp` |
| ON/OFF (desligamento lógico) | Desligar gera `DISPOSITIVO_DESLIGADO`, apaga os LEDs e suspende leituras e publicações, sem cortar a energia do ESP32; o relógio do ciclo continua. Religar gera `STATUS` | `main.cpp` |
| Reinício | Contexto do ciclo e do lacre (`ctx`) e fila de eventos voltam da NVS; o `STATUS` traz `contexto_restaurado`. Ligar com o lacre aberto gera `VIOLACAO_LACRE` | `main.cpp` |

O `seal_changed` que aparece no Serial é o registro local de diagnóstico (NVS `q0`–`q15`, CT18 e
CT21); os eventos de negócio são os de nome em maiúsculas.

## Publicação no Wokwi web

A URL pública do projeto é uma versão anterior de diagnóstico, diferente do firmware principal (`../main.cpp`). Para atualizá-la:

1. Na raiz, executar `python scripts/export-wokwi.py`, que gera `build/wokwi-publicacao/` com o sketch principal, os headers, o `diagram.json`, o chip GPS customizado e `libraries.txt`. O script não publica nem comprova a compilação no Wokwi web.
2. Abrir o projeto da conta do grupo no Wokwi, substituir os arquivos pelos do pacote e conferir se o chip GPS customizado é aceito. Se a plataforma não aceitar o chip, manter a simulação integrada na extensão do VS Code e registrar isso na documentação.
3. Rodar o projeto e conferir ao menos o CT01 (verde aceso e `STATUS` na Serial) antes de salvar.
4. Registrar na seção 3.4.1 a URL, o commit e a data da publicação.
