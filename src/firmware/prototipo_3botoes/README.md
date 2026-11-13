# Diagrama de 3 botões

Circuito do Wokwi com só três botões (**ON/OFF**, **AVANÇAR** e **VOLTAR**), na mesma montagem da bancada: duas protoboards de 830 furos empilhadas (`bb1` em cima e `bb2` embaixo), com o ESP32 sobre o trilho do meio. Não substitui nenhum outro diagrama: `prototipo_wokwi` segue com chaves e os CTs do Wokwi continuam nele.

## Mapa de pinos

| Item | GPIO | Observação |
|---|---|---|
| ON/OFF | 19 | botão entre o GPIO e o GND; segurar 3 s |
| AVANÇAR | 25 | botão entre o GPIO e o GND; próxima tela do LCD |
| VOLTAR | 4 | botão entre o GPIO e o GND; tela anterior |
| Bateria (potenciômetro) | 34 | ADC1; extremos em 3V3 e GND |
| Buzzer | 13 | positivo no GPIO, negativo no GND |
| LCD I2C | SDA 21, SCL 22 | VCC no VIN |
| GNSS (chip `gps`) | RX do ESP no 36 (VP), TX do ESP no 17 | VCC no VIN |

Não há LED. O ambiente é `bancada-3botoes` (`pio run -e bancada-3botoes`); o `wokwi.toml` aponta para esse firmware. Só VOLTAR, buzzer e SCL diferem do padrão do `config.h` (por `-D` no `platformio.ini`); ON/OFF, AVANÇAR e SDA não são configuráveis por `-D`.

## Alimentação (3 pares de trilhos)

| Trilho | + | − |
|---|---|---|
| de cima (`bb1`) | 3V3 | GND |
| do meio (`bb2`) | 3V3 | GND |
| de baixo (`bb2`) | 5 V (VIN) | GND |

Os GND dos três pares são unidos por jumpers. Na placa real os trilhos são divididos ao meio (há uma interrupção na coluna 30): se for usar os dois lados de um trilho, ligue-os com um jumper.

## Limitação

Sem lacre, rede, autorização e ciclo físicos, o firmware atual não consegue gerar esses eventos: eles só existirão quando o `cmd` do MQTT passar a executar (hoje ele apenas registra o comando no Serial). A retirada desses itens do enunciado precisa ser justificada na seção 3.4.3 (nota "Exceções ao enunciado"); isso não foi feito neste commit.
