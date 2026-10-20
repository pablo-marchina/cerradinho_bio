# Resultado da preparação e validação

Este relatório preserva os resultados da preparação original. A validação da
integração ao `g01` aparece ao final; não confundir as duas execuções.

Trabalho iniciado em **19/10/2026** e concluído em **20/10/2026** (America/Sao_Paulo),
em uma pasta de preparação separada, antes da integração ao repositório `g01`.

## Arquivos e ferramentas

A implementação existente foi reaproveitada. Foram conferidos `AGENT.md`, README,
modelagem, instruções e roteiro de validação antes das alterações. `tmp/` foi
preservado. Nenhum acelerômetro, backend, modem físico ou sensor IR foi acrescentado.

- Mantidos e conferidos: `platformio.ini`, `include/config.h`,
  `include/tracking_math.h`, `diagram.json`, `chips/gps.chip.c`,
  `chips/gps.chip.json`, `chips/nmea.h` e `wokwi.weekly.toml`.
- GPS recompilado em `chips/gps.chip.wasm`: **140.250 bytes**.
- `src/firmware/main.cpp`: saídas `printf` com CRLF para alinhar o terminal Wokwi;
  `storage.isKey("previous")` antes da leitura inicial para evitar o diagnóstico
  `NOT_FOUND` em uma NVS ainda sem referência. As chaves NVS, a fila e a regra de
  comparação foram preservadas.
- `scripts/common.ps1`: reutiliza o Python do PlatformIO sem depender do PATH.
- `scripts/validate-project.py`: aceita JSONC gerado pelo VS Code/PlatformIO;
  confere componentes, alimentação, UARTs, pull-up, baud rates, monitor expandido,
  extensões, ambientes e cabeçalhos dos binários.
- `scripts/verify.ps1`: passou a incluir o linter oficial com warnings como erro.
- `.vscode/tasks.json`: tarefas para semanal e para validação completa sequencial.
- `wokwi.toml` e `scripts/capture-serial.{ps1,py}`: captura opcional da serial pela
  porta local 4000. Nenhum comando de aplicação é enviado ao ESP32 pela captura.
- README e roteiro atualizados. Cópias dos arquivos alterados estão em
  `build/setup-before-20261019/` na pasta original, incluindo o relatório anterior.

O setup foi executado novamente e reaproveitou as dependências locais, sem novos
downloads necessários nesta sessão. Ferramentas: Wokwi CLI **0.29.0**, WASI SDK
**25.0**, Espressif32 **6.12.0**, Arduino ESP32 **2.0.17**, TinyGPSPlus **1.0.3** e
Python **3.11.7** do PlatformIO. Extensões já instaladas: PlatformIO IDE **3.3.4** e
Wokwi Simulator **4.0.1**. Não foram gravados tokens, credenciais ou licenças no projeto.

## Compilação e verificações locais

| Verificação | Resultado real |
|---|---|
| `scripts/setup.ps1` | Concluído; dependências já atualizadas |
| `scripts/build.ps1` | GPS e ESP32 demo compilados com sucesso |
| `scripts/build.ps1 -Environment weekly` | GPS e ESP32 semanal compilados com sucesso |
| `tests/model_tests.c` em WASI/Node.js | PASS: coordenadas, distâncias, tolerância, fronteiras NMEA, checksum e fix inválido |
| Consistência do projeto | PASS, inclusive depois de abrir o projeto no PlatformIO |
| Wokwi CLI `lint --warnings-as-errors` | Saída 0, sem erros ou warnings; uma mensagem informativa sobre a placa legada |

A primeira execução do verificador após abrir o VS Code revelou que os JSONs
gerados pelo PlatformIO contêm comentários. A leitura foi corrigida para JSONC;
o teste foi repetido e aprovado. A primeira tentativa de captura encontrou a
porta fechada; executar novamente **Wokwi: Start Simulator**, após parar a
simulação, recarregou a configuração e habilitou a captura.

| Ambiente final | RAM estática | Flash da aplicação |
|---|---:|---:|
| demo | 43.948 / 327.680 bytes | 750.245 / 1.310.720 bytes |
| weekly | 43.948 / 327.680 bytes | 750.253 / 1.310.720 bytes |

Esses números não medem heap em execução, consumo ou autonomia. O aviso de
`implicit-fallthrough` da TinyGPSPlus registrado na preparação anterior pertence
à dependência; ela não foi modificada. A compilação incremental final não o
reproduziu. O DevKit v1 solicitado foi mantido apesar da informação de tipo legado
no catálogo do Wokwi.

## Execução no simulador

Foi utilizada a licença **Community já disponível** no perfil do VS Code. Nenhuma
licença foi ativada ou adquirida. O token `WOKWI_CLI_TOKEN` não estava disponível;
a execução funcional foi feita pela extensão, com captura serial local.

O roteiro abaixo foi executado **na mesma sessão**, sem reiniciar entre mudança
de posição, perda de fix e recuperação. Coordenadas fictícias de demonstração.
Os controles do Custom Chip estavam disponíveis e foram utilizados ao vivo.

| Caso | Evidência observada |
|---|---|
| Inicialização | `[boot]`, modo demo, intervalo 30 s, tolerância 100 m, NVS ok |
| Heartbeat | Linhas a cada 2 s, inclusive inicialmente sem fix/Wi-Fi e durante a perda de fix |
| Primeiro ponto | Em 2.070 ms: `baseline_created=true`, `moved=null`, `distance_m=null` |
| Mudança de local | Latitude alterada de -23,55 para -23,54; em 32.072 ms: **1.111,76 m**, `moved=true` |
| Próxima referência | Em 62.072 ms, mesma posição: distância 0, `moved=false` |
| Perda de fix | `fixValid=0`; em 102.071 ms: `location_unavailable`, latitude/longitude/moved `null`; última posição preservada |
| Lacre violado | Em 171.242 ms: `seal_changed`, `seal=violated`, independente da coleta |
| Lacre íntegro | Em 182.025 ms: novo `seal_changed`, `seal=intact`; retirada autorizada continua `null` |
| Recuperação | `fixValid=1`; em 212.086 ms: distância 0 e `baseline_created=false`, usando a referência anterior à perda |
| Bateria sintética | Valores 78%, 0% e 100% observados ao alterar o potenciômetro |
| Wi-Fi | Transição inicial de desconectado para conectado ao Wokwi-GUEST; não houve transporte ao servidor |
| Fila cheia | 21 registros emitidos; 16 confirmações de gravação local e 5 avisos de limite; novos registros continuaram na serial |

A posição lida em -23,54 aparece como -23,540002 devido à precisão de ponto
flutuante do controle. Isso não alterou a classificação com tolerância de 100 m.
As consultas ocorreram aproximadamente a cada 30 s quando havia fix; sem fix,
o timeout de 10 s é somado à espera, pois o próximo intervalo começa ao concluir
a tentativa anterior. O comportamento semanal tem a mesma regra com sete dias.

O roteiro funcional ocorreu antes dos dois ajustes finais de diagnóstico
(CRLF e verificação da existência da chave NVS). Ambos os ambientes foram
recompilados depois. Foram capturados 45 segundos da inicialização do firmware
final: console alinhado, sem `NOT_FOUND`, primeira referência em 2.072 ms e nova
coleta sem movimento em 32.074 ms. O relatório e os logs distinguem essas
execuções pelo marcador `[boot]`.

Logs da preparação (copiados localmente para `build/pre-integracao/`,
gerados e ignorados pelo Git):

- `build/pre-integracao/firmware-demo-final.log` e `firmware-weekly-final.log`;
- `build/pre-integracao/verification-final.log`;
- `build/pre-integracao/simulation-serial.log`: roteiro e conferência final;
- `build/pre-integracao/simulation-summary.json`: primeira captura (21 registros);
- `build/pre-integracao/simulation-final-check.txt`: conferência da inicialização.

## Limites e pendências

- **Intervalo semanal:** compilado como sete dias; não foi aguardada uma semana
  no simulador. Há coleta imediata no boot; o agendamento não persiste reinícios.
- **NVS:** referência e fila continuam implementadas. O ensaio confirmou avisos
  de gravação e limite; não leu a flash de volta nem comprovou resistência a
  queda de energia. Persistência entre sessões Wokwi continua não garantida.
- **Lacre:** abertura/fechamento testados. Debounce de 60 ms revisado no código;
  rejeição de pulsos menores que 60 ms ainda não ensaiada. A chave não valida IR.
- **Rede:** heartbeat observado sem conexão no início. Queda prolongada de Wi-Fi,
  reconexão em campo, autenticação, transporte, ACK e reenvio continuam pendentes.
  JSON na serial e Wi-Fi conectado não significam entrega ao servidor.
- **Bateria/energia:** potenciômetro sintético. Deep sleep, modem, alimentação,
  proteção, capacidade, corrente de pico, consumo e autonomia são etapas futuras.
- **Validação física:** não realizada. Sensor infravermelho, montagem do lacre,
  GNSS real, níveis elétricos e conectividade externa ainda precisam ser definidos
  e ensaiados. A tolerância de 100 m permanece hipótese a validar em campo.
- **Integração:** relógio UTC, IDs de eventos, associação a ativos/ciclos e remoção
  autorizada permanecem pendentes. A fila comporta 16 registros, sem remoção,
  confirmação ou reenvio implementados.

Compilação, testes matemáticos/NMEA, execução do ESP32 no Wokwi e validação física
são evidências diferentes. Esta entrega conclui o setup da modelagem inicial,
sem afirmar conclusão do MVP integral do TAPI ou validação de autonomia.

## Integração ao repositório g01 — 20/10/2026

O setup foi integrado em `C:\Users\Inteli\Documents\go1denovo\g01`,
na branch existente do card #7. O firmware foi colocado em
`src/firmware/main.cpp`, com conteúdo idêntico ao firmware final da preparação.
O `src_dir` do PlatformIO e o verificador foram adaptados para esse caminho.
Os documentos ficam em `document/modelagem-inicial/`; o README geral mantém
as informações do grupo e aponta para o guia de uso.

As ferramentas instaladas foram reaproveitadas em `.tools/`. Ferramentas,
binários, logs, configurações locais e credenciais não integram os commits.
Os dois guias Wokwi já existentes em `document/` foram preservados, fora desta
entrega. Cópias locais dos arquivos substituídos estão em
`build/integration-before-20261020/`, ignorado pelo Git.

| Verificação executada no g01 | Resultado |
|---|---|
| `scripts/setup.ps1` | Aprovado; dependências dos dois ambientes atualizadas |
| `scripts/build.ps1` | GPS WASM e ESP32 demo compilados; PlatformIO `SUCCESS` |
| `scripts/build.ps1 -Environment weekly` | GPS WASM e ESP32 semanal compilados; PlatformIO `SUCCESS` |
| Testes de lógica em `scripts/verify.ps1` | Aprovados: coordenadas, distância, tolerância, fronteiras NMEA, checksum e fix inválido |
| Verificação estrutural em `scripts/verify.ps1` | Aprovada: componentes, alimentação, UARTs, monitor, pinos, baud rates, ambientes e binários |
| `wokwi-cli lint --warnings-as-errors` | Aprovado; apenas aviso informativo sobre o tipo legado `wokwi-esp32-devkit-v1`, preservado conforme a arquitetura |

Os logs desta execução estão em `build/setup-integracao.log`,
`build/firmware-demo-integracao.log`, `build/firmware-weekly-integracao.log`
e `build/verification-integracao.log`, todos ignorados pelo Git.

A simulação interativa não foi repetida dentro do `g01`. As evidências de
console, movimento, perda de fix, lacre e bateria são as da preparação descrita
acima, usando o mesmo firmware. Não houve validação física nem medição de
consumo ou autonomia nesta integração.
