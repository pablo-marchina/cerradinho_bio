# Circuito principal Wokwi: GuaráTracking

Esta pasta reúne o circuito principal integrado: ESP32 DevKit v1, GPS/GNSS
customizado, lacre, bateria simulada, quatro LEDs com resistores de 220 Ω,
dois botões e chave de rede simulada. O diagrama anterior da raiz foi incorporado aqui.

O firmware principal preserva a comparação de posições, o debounce do lacre e a
fila NVS existentes. Os novos controles têm diagnóstico de bancada; as regras de
autorização, ciclo e alertas de prazo ainda precisam ser integradas. JSON na serial
e Wi-Fi conectado não representam envio confirmado a servidor ou broker.

## Módulo GNSS de referência

O módulo físico escolhido em 21/10/2026 é o **Quectel LC76G, variante PA**.
O chip customizado desta pasta ainda é um gerador genérico de NMEA: sua adaptação
ao protocolo e aos estados de energia do LC76G-PA está pendente. O modelo deverá
usar o mesmo driver GNSS do dispositivo físico, incluindo os comandos PAIR usados,
entrada/saída de backup e falhas de aquisição. A placa e a versão de firmware
do módulo ainda precisam ser definidas.

O contrato de simulação e os CT26/CT28 estão na
[documentação do projeto](../../../document/documentacao.md).
Os 9600 baud da configuração atual descrevem o simulador; conferir a configuração
da unidade física antes de integrar. Consumo e autonomia exigem ensaios físicos.

## Arquivos

| Arquivo | Conteúdo |
|---|---|
| `diagram.json` | Único diagrama principal, com todos os componentes integrados. |
| `wokwi.toml` | Simulação demo: firmware compilado na raiz, coleta a cada 30 s e serial RFC2217 na porta 4000. |
| `wokwi.weekly.toml` | Mesmo circuito com coleta a cada sete dias reais. |
| `../main.cpp` | Firmware principal compilado pelo PlatformIO. |
| `../../../include/config.h` | Pinagem e parâmetros centralizados do firmware principal. |
| `sketch.ino` | Teste isolado de ligações, excluído da compilação principal. Conta caracteres do GPS, sem interpretar posição. |

## Mapa de pinos

| Componente | Pino | Modo | Função atual de bancada |
|---|---|---|---|
| GPS TX → ESP RX2 | GPIO 16 | UART2 RX, 9600 baud | Recepção das sentenças GNSS simuladas |
| GPS RX ← ESP TX2 | GPIO 17 | UART2 TX, 9600 baud | Reservado; o modelo não interpreta comandos |
| Lacre (`seal`) | GPIO 27 | `INPUT_PULLUP` | Esquerda = íntegro/LOW; direita = violado/HIGH |
| Bateria (`battery`) | GPIO 34 (ADC1) | analógico | Nível sintético de 0 a 100%, sem alimentação real |
| LED verde | GPIO 25 | saída | Lacre íntegro |
| LED amarelo | GPIO 26 | saída | Botão de autorização pressionado, apenas diagnóstico |
| LED vermelho | GPIO 23 | saída | Lacre violado; GPIO27 permanece exclusivo do lacre |
| LED azul | GPIO 14 | saída | Rede simulada indisponível |
| SW_REDE (`sw_rede`) | GPIO 18 | `INPUT_PULLUP` | Esquerda = disponível/LOW; direita = indisponível/HIGH |
| BTN_AUTORIZAR (`btn_autorizar`) | GPIO 33 | `INPUT_PULLUP` | Leitura no heartbeat; acende o LED amarelo |
| BTN_CICLO (`btn_ciclo`) | GPIO 13 | `INPUT_PULLUP` | Leitura no heartbeat, sem alterar o ciclo |

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
| Iniciar com as chaves à esquerda | Verde aceso; vermelho e azul apagados; heartbeat a cada 2 s |
| Mover o lacre à direita | Verde apaga e vermelho acende após debounce; evento `seal_changed` |
| Pressionar BTN_AUTORIZAR | Amarelo acende; heartbeat mostra `autorizar=1` enquanto pressionado |
| Pressionar BTN_CICLO | Heartbeat mostra `ciclo=1` enquanto pressionado; não modifica NVS |
| Mover SW_REDE à direita | Azul acende e `rede_sim=off`; Wi-Fi real continua com diagnóstico independente |
| Girar o potenciômetro | `bateria_sim` muda no heartbeat |
| GPS com fix válido | `gps_chars` aumenta; posição é coletada e comparada com a última referência válida |
| GPS com `fixValid=0` | Reporta falta de fix e preserva a referência anterior |

Pressione os botões durante um heartbeat para conferir sua leitura periódica.
O amarelo acompanha a leitura bruta do botão; a integração de debounce e eventos
de autorização/ciclo continua pendente. O modo semanal não valida autonomia.

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

- `assets/wokwi/circuito.png`: circuito anterior da issue #6, sem o GPS integrado.
- Projeto público anterior: https://wokwi.com/projects/478276108359405569

O projeto público e a captura anterior não foram atualizados por esta integração.
O roteiro acima descreve resultados esperados; não é evidência de execução visual.

## Teste de ligação (issue #6)

Executado em 19/out/2026 no projeto público. Evidência: `assets/wokwi/circuito.png`.

| Ação | Esperado | Resultado |
|---|---|---|
| Nada acionado | LEDs apagados; `lacre=1 rede=1 autorizar=1 ciclo=1` | Passou |
| SW_LACRE acionada | LED verde aceso; `lacre=0` | Passou |
| SW_REDE acionada | LED azul aceso; `rede=0` | Passou |
| BTN_AUTORIZAR pressionado | LED amarelo aceso; `autorizar=0` | Passou |
| BTN_CICLO pressionado | LED vermelho aceso; `ciclo=0` | Passou |
| POT_BATERIA de um extremo ao outro | `bateria_adc` de 0 a ~4095 | Passou |
