# Diagrama de 3 botões

Circuito do Wokwi com só três botões (**ON/OFF**, **AVANÇAR** e **VOLTAR**), na mesma montagem da bancada: duas protoboards de 830 furos empilhadas (`bb1` em cima e `bb2` embaixo), com a placa **ESP32 DevKitC de 38 pinos** (ESP32-WROOM-32, `board-esp32-devkit-c-v4` no Wokwi) sobre o trilho do meio, USB à esquerda. Não substitui nenhum outro diagrama: `prototipo_wokwi` segue com chaves e os CTs do Wokwi continuam nele.

## Mapa de pinos

| Item | GPIO | Observação |
|---|---|---|
| ON/OFF | 19 | botão entre o GPIO e o GND; segurar 3 s |
| AVANÇAR | 25 | botão entre o GPIO e o GND; próxima tela do LCD |
| VOLTAR | 4 | botão entre o GPIO e o GND; tela anterior |
| Bateria (potenciômetro) | 34 | ADC1; extremos em 3V3 e GND |
| Buzzer | 13 | positivo no GPIO, negativo no GND |
| LCD I2C | SDA 21, SCL 22 | VCC no 5V |
| GNSS (chip `gps`) | RX do ESP no 36 (VP), TX do ESP no 17 | VCC no 5V |

Não há LED. O ambiente é `bancada-3botoes` (`pio run -e bancada-3botoes`); o `wokwi.toml` aponta para esse firmware. Só VOLTAR, buzzer e SCL diferem do padrão do `config.h` (por `-D` no `platformio.ini`); ON/OFF, AVANÇAR e SDA não são configuráveis por `-D`.

## Posição dos pinos na placa (DevKitC de 38 pinos, USB à esquerda)

Contagem a partir do USB, da esquerda para a direita, de 1 a 19 em cada fileira.

| Fileira | Pinos usados (posição) |
|---|---|
| de cima | 5V = 1, GPIO13 = 5, GND = 6, GPIO25 = 11, GPIO34 = 15, VP (GPIO36) = 17, 3V3 = 19 |
| de baixo | GPIO4 = 7, GPIO17 = 9, GPIO19 = 12, GND = 13, GPIO21 = 14, GPIO22 = 17, GND = 19 |

Fileira de cima completa: 5V, CMD, D3, D2, 13, GND, 12, 14, 27, 26, 25, 33, 32, 35, 34, VN, VP, EN, 3V3. Fileira de baixo completa: CLK, D0, D1, 15, 2, 0, 4, 16, 17, 5, 18, 19, GND, 21, RX, TX, 22, 23, GND. No diagrama, a fileira de cima fica na linha j de `bb1` e a de baixo na linha d de `bb2`, nas colunas 20 a 38. O firmware não muda: o ambiente continua `esp32dev`.

## Alimentação (3 pares de trilhos)

| Trilho | + | − |
|---|---|---|
| de cima (`bb1`) | 3V3 | GND |
| do meio (`bb2`) | sem uso | GND |
| de baixo (`bb2`) | 5 V | GND |

Os GND dos três pares são unidos por jumpers. Na placa real os trilhos são divididos ao meio (há uma interrupção na coluna 30). Por isso cada um dos 5 trilhos usados tem uma ponte curta (furos 23 e 28) ligando a metade esquerda à direita; sem elas, a metade esquerda e boa parte da direita ficariam sem GND e sem alimentação do ESP32.

## Limitação

Sem lacre, rede, autorização e ciclo físicos, o firmware atual não consegue gerar esses eventos: eles só existirão quando o `cmd` do MQTT passar a executar (hoje ele apenas registra o comando no Serial). A retirada desses itens do enunciado precisa ser justificada na seção 3.4.3 (nota "Exceções ao enunciado"); isso não foi feito neste commit.
