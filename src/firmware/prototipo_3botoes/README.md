# Diagrama de 3 botões

Circuito do Wokwi com só três botões, na pinagem alternada da bancada: **ON/OFF**, **AVANÇAR** e **VOLTAR**. Ele parte do `../prototipo_bancada` e não substitui nenhum outro diagrama (`prototipo_wokwi` continua com chaves, e os CTs do Wokwi seguem nele).

## Mapa de pinos

| Item | GPIO | Observação |
|---|---|---|
| ON/OFF | 19 | botão para o GND; segurar 3 s |
| AVANÇAR | 25 | botão para o GND; próxima tela do LCD |
| VOLTAR | 32 | botão para o GND; tela anterior |
| Bateria (potenciômetro) | 34 | ADC1 |
| Buzzer | 12 | |
| LCD I2C | SDA 21, SCL 23 | VCC no VIN |
| GNSS (chip `gps`) | RX do ESP no 36 (VP), TX do ESP no 17 | |

Não há LED. Compile o ambiente `bancada` (`pio run -e bancada`); o `wokwi.toml` aponta para esse firmware.

## Limitação

Sem lacre, rede, autorização e ciclo físicos, o firmware atual não consegue gerar esses eventos: eles só existirão quando o `cmd` do MQTT passar a executar (hoje ele apenas registra o comando no Serial). A retirada desses itens do enunciado precisa ser justificada na seção 3.4.3 (nota "Exceções ao enunciado"); isso não foi feito neste commit.
