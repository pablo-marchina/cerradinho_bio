# Ligações do protótipo de 3 botões

Gerado a partir de `diagram.json`. Placa: **ESP32 DevKitC de 38 pinos**, USB à esquerda, sobre as duas protoboards empilhadas. A fileira de **cima** da placa fica na protoboard de **cima** (linha J) e a de **baixo** na protoboard de **baixo** (linha D). Ambiente: `bancada-3botoes`.

Cores: preto GND, vermelho 3V3, laranja 5V, amarelo botões, ciano bateria, verde SDA, azul SCL, roxo buzzer, cinza GNSS.

> Na bancada real a protoboard de baixo está girada 180° (os números das colunas correm ao contrário). Siga cada ligação pelo pino da placa, não pelo número impresso.

## 1. Pinos da placa

Posição = contagem de 1 a 19 a partir do USB. A coluna do furo é a do diagrama.

### Fileira de cima (protoboard de CIMA, linha J)

| Pos. | Pino | Coluna | Uso |
|---|---|---|---|
| 1 | 5V | 20 | alimentação 5 V (LCD e GNSS) |
| 2 | CMD | 21 | (não usado) |
| 3 | D3 | 22 | (não usado) |
| 4 | D2 | 23 | (não usado) |
| 5 | GPIO13 | 24 | buzzer (positivo) |
| 6 | GND.1 | 25 | GND (livre) |
| 7 | GPIO12 | 26 | (não usado) |
| 8 | GPIO14 | 27 | (não usado) |
| 9 | GPIO27 | 28 | (não usado) |
| 10 | GPIO26 | 29 | (não usado) |
| 11 | GPIO25 | 30 | botão AVANÇAR |
| 12 | GPIO33 | 31 | (não usado) |
| 13 | GPIO32 | 32 | (não usado) |
| 14 | GPIO35 | 33 | (não usado) |
| 15 | GPIO34 | 34 | potenciômetro da bateria (sinal) |
| 16 | VN | 35 | (não usado) |
| 17 | VP | 36 | GNSS TX (GPIO36) |
| 18 | EN | 37 | (não usado) |
| 19 | 3V3 | 38 | alimentação 3,3 V (potenciômetro) |

### Fileira de baixo (protoboard de BAIXO, linha D)

| Pos. | Pino | Coluna | Uso |
|---|---|---|---|
| 1 | CLK | 20 | (não usado) |
| 2 | D0 | 21 | (não usado) |
| 3 | D1 | 22 | (não usado) |
| 4 | GPIO15 | 23 | (não usado) |
| 5 | GPIO2 | 24 | (não usado) |
| 6 | GPIO0 | 25 | (não usado) |
| 7 | GPIO4 | 26 | botão VOLTAR |
| 8 | GPIO16 | 27 | (não usado) |
| 9 | GPIO17 | 28 | GNSS RX (GPIO17) |
| 10 | GPIO5 | 29 | (não usado) |
| 11 | GPIO18 | 30 | (não usado) |
| 12 | GPIO19 | 31 | botão ON/OFF |
| 13 | GND.3 | 32 | GND |
| 14 | GPIO21 | 33 | LCD SDA |
| 15 | GPIORX | 34 | serial (RX0, USB) |
| 16 | GPIOTX | 35 | serial (TX0, USB) |
| 17 | GPIO22 | 36 | LCD SCL |
| 18 | GPIO23 | 37 | (não usado) |
| 19 | GND.2 | 38 | GND (livre) |

## 2. Jumpers (fio a fio)

| Cor | De | Para | Função |
|---|---|---|---|
| laranja | GNSS VCC | trilho de BAIXO, 5V (+), furo 47 | GNSS VCC → trilho de baixo (5V) |
| preto | GNSS GND | trilho de BAIXO, GND (-), furo 46 | GNSS GND → trilho de baixo (GND) |
| amarelo | protoboard de BAIXO, coluna 26, linha E | protoboard de CIMA, coluna 6, linha A | GPIO4 → botão VOLTAR |
| amarelo | protoboard de CIMA, coluna 30, linha F | protoboard de CIMA, coluna 61, linha A | GPIO25 → botão AVANÇAR |
| ciano | protoboard de CIMA, coluna 34, linha F | protoboard de CIMA, coluna 40, linha E | GPIO34 ← sinal do potenciômetro |
| preto | protoboard de CIMA, coluna 4, linha B | trilho de CIMA, GND (-), furo 2 | GND do botão VOLTAR → trilho de cima |
| preto | protoboard de CIMA, coluna 59, linha B | trilho de CIMA, GND (-), furo 48 | GND do botão AVANÇAR → trilho de cima |
| preto | protoboard de CIMA, coluna 39, linha C | trilho de CIMA, GND (-), furo 27 | GND do potenciômetro → trilho de cima |
| vermelho | protoboard de CIMA, coluna 41, linha D | trilho de CIMA, 3V3 (+), furo 36 | 3V3 do potenciômetro ← trilho de cima |
| preto | protoboard de BAIXO, coluna 32, linha E | trilho do MEIO, GND (-), furo 9 | GND da placa → trilho do meio (GND) |
| laranja | protoboard de CIMA, coluna 20, linha I | trilho de BAIXO, 5V (+), furo 8 | pino 5V → trilho de baixo (5V) |
| vermelho | trilho de CIMA, 3V3 (+), furo 1 | trilho do MEIO, 3V3 (+), furo 1 | une o 3V3 do trilho de cima ao do meio |
| preto | trilho de CIMA, GND (-), furo 50 | trilho do MEIO, GND (-), furo 50 | une o GND do trilho de cima ao do meio |
| preto | trilho de BAIXO, GND (-), furo 50 | trilho do MEIO, GND (-), furo 50 | une o GND do trilho de baixo ao do meio |
| roxo | protoboard de CIMA, coluna 24, linha F | protoboard de BAIXO, coluna 54, linha G | GPIO13 → buzzer (positivo) |
| preto | protoboard de BAIXO, coluna 53, linha J | trilho de BAIXO, GND (-), furo 43 | GND do buzzer → trilho de baixo |
| verde | protoboard de BAIXO, coluna 33, linha E | LCD SDA | GPIO21 → LCD SDA |
| azul | protoboard de BAIXO, coluna 36, linha E | LCD SCL | GPIO22 → LCD SCL |
| preto | LCD GND | trilho de BAIXO, GND (-), furo 19 | LCD GND → trilho de baixo (GND) |
| laranja | LCD VCC | trilho de BAIXO, 5V (+), furo 18 | LCD VCC → trilho de baixo (5V) |
| amarelo | protoboard de BAIXO, coluna 31, linha E | protoboard de BAIXO, coluna 48, linha A | GPIO19 → botão ON/OFF |
| preto | protoboard de BAIXO, coluna 46, linha B | trilho do MEIO, GND (-), furo 37 | GND do ON/OFF → trilho do meio |
| vermelho | protoboard de CIMA, coluna 38, linha F | trilho de CIMA, 3V3 (+), furo 40 | pino 3V3 → trilho de cima (3V3) |
| cinza | GNSS TX | pino VP (GPIO36), fileira de cima, posição 17 | GNSS → ESP32 |
| cinza | GNSS RX | pino 17, fileira de baixo, posição 9 | ESP32 → GNSS |

## 3. Peças

| Peça | Terminal | Vai para |
|---|---|---|
| Botão VOLTAR | terminal 1 | coluna 6 da protoboard de cima (jumper amarelo do GPIO4) |
| Botão VOLTAR | terminal 2 | coluna 4 da protoboard de cima (jumper preto ao GND) |
| Botão AVANÇAR | terminal 1 | coluna 61 da protoboard de cima (jumper amarelo do GPIO25) |
| Botão AVANÇAR | terminal 2 | coluna 59 da protoboard de cima (jumper preto ao GND) |
| Botão ON/OFF | terminal 1 | coluna 48 da protoboard de baixo (jumper amarelo do GPIO19) |
| Botão ON/OFF | terminal 2 | coluna 46 da protoboard de baixo (jumper preto ao GND) |
| Potenciômetro | GND, sinal, VCC | colunas 39, 40, 41 da protoboard de cima (GND ao trilho, sinal ao GPIO34, VCC ao 3V3) |
| Buzzer | negativo, positivo | colunas 53 e 54 da protoboard de baixo (negativo ao GND, positivo ao GPIO13) |
| LCD I2C | GND, VCC, SDA, SCL | GND e VCC aos trilhos de baixo, SDA ao GPIO21, SCL ao GPIO22 |
| GNSS | VCC, GND, TX, RX | VCC e GND aos trilhos de baixo, TX ao VP (GPIO36), RX ao pino 17 |

## 4. Trilhos de alimentação

| Trilho | + | − |
|---|---|---|
| de cima | 3V3 | GND |
| do meio | 3V3 | GND |
| de baixo | 5V | GND |

Os GND dos três pares são unidos por jumpers pretos. Na placa real os trilhos são divididos ao meio (interrupção na coluna 30): para usar os dois lados de um trilho, ligue-os com um jumper.
