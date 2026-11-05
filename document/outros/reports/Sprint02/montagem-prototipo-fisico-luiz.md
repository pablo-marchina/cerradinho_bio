# Montagem do protótipo físico, versão do Luiz (Sprint 2)

Documento interno. Registra como a versão do Luiz do protótipo físico foi montada entre 02 e 04/11/2026: o layout final, a adaptação do firmware, os problemas encontrados e como cada um foi resolvido. Serve de base para a comparação das versões e para a seção 3.4.2, mas não é evidência de entrega: fotos, vídeos e CTs físicos ainda não foram feitos (ver a seção 5).

Os valores deste relatório seguem três origens, indicadas no texto: **medido** (multímetro ou serial da placa), **calculado** (lei de Ohm) e **relatado** (observado na bancada, sem registro).

## 1. Peças e organização

| Peça | Modelo usado | Observação |
|---|---|---|
| Placa | ESP32-DevKitC V4, 19 pinos por lado | Conversor USB CP2102 |
| Protoboards | 2 de 830 furos, encaixadas pelo lado comprido | Um trilho só entre elas; trilhos divididos no meio |
| LCD | 16x2 com módulo I2C | Endereço 0x27, medido com busca I2C |
| Botões | 7 táteis de 4 pernas | VOLTAR, CICLO, LACRE, AUTORIZAR, AVANÇAR, ON/OFF e REDE |
| Potenciômetro | Linear B10K L20 | Simula o nível de bateria |
| Buzzer | Passivo 12085 42R | Com resistor de 100 Ω em série |
| LED | Vermelho, com resistor de 220 Ω | Violação do lacre |
| GNSS | GY-GPS6MV2 | Envia dados; ainda sem fix |

As três chaves deslizantes previstas no Wokwi (lacre, rede e ON/OFF) foram trocadas por botões. Não havia jumpers nas cores do padrão, então a montagem foi feita por coordenadas, sem seguir o código de cores.

### Como ler os furos

`cima 24f` é a placa de cima, coluna 24, linha f. Nas duas placas a linha **a** fica em cima e a **j** embaixo, na mesma ordem do Wokwi. A coluna 1 é o primeiro furo da esquerda: a numeração impressa nesta placa começa no segundo furo e não deve ser usada para contar. Na mesma coluna, os furos a–e são um ponto só, e os furos f–j também.

## 2. Layout final

O ESP32 fica deitado sobre o trilho do meio, com o USB à esquerda. A fileira de 5V a 3V3 entra em `cima 20j` a `cima 38j`, e a de CLK a GND entra em `baixo 20d` a `baixo 38d`. O corpo da placa cobre as linhas a, b e c da placa de baixo, então os fios dessa fileira saem pela linha e.

### Trilhos

| Trilho | + | − | De onde vem |
|---|---|---|---|
| De cima | 3V3 | GND | + pelo jumper perto da coluna 46; − pelo jumper perto da coluna 61 |
| Do meio | 3V3 | GND | `cima 38f` (3V3), `cima 25f` e `baixo 32e` (GND) |
| De baixo | 5 V | GND | `cima 20f` (5V, só para o LCD); − pelo jumper perto da coluna 61 |

Os trilhos são divididos no meio, por volta da coluna 30. Um fio curto liga as duas metades do − de cima. O + de baixo não recebe jumper de borda, porque é 5 V e não pode encostar nos trilhos de 3V3.

### Botões

Cada botão ocupa 4 casas na vertical e 3 na horizontal. A ligação interna é vertical, então as duas pernas de cada lado ficam na mesma coluna.

| Botão | GPIO | Colunas | Pernas | Fio do GND | Fio do sinal | Regra |
|---|---|---|---|---|---|---|
| VOLTAR | 26 | 4 e 6 | b e e | `cima 4a` → − de cima | `cima 29f` → `cima 6a` | toque |
| CICLO | 14 | 11 e 13 | b e e | `cima 11a` → − de cima | `cima 27f` → `cima 13a` | segurar 3 s |
| LACRE | 27 | 48 e 50 | b e e | `cima 48a` → − de cima | `cima 28f` → `cima 50a` | toque abre ou fecha |
| AUTORIZAR | 33 | 53 e 55 | b e e | `cima 53a` → − de cima | `cima 31f` → `cima 55a` | segurar 3 s |
| AVANÇAR | 25 | 59 e 61 | b e e | `cima 59a` → − de cima | `cima 30f` → `cima 61a` | toque |
| ON/OFF | 19 | 47 e 49 | f e i | `baixo 47j` → − de baixo | `baixo 31e` → `baixo 49j` | segurar 3 s |
| REDE | 18 | 57 e 59 | f e i | `baixo 57j` → − de baixo | `baixo 30e` → `baixo 59j` | toque alterna |

### Demais peças

| Peça | Terminais | Fios |
|---|---|---|
| LED (GPIO13) | Resistor de 220 Ω em `cima 24f` e `cima 24d`; anodo em `cima 24c`; catodo em `cima 23c` | `cima 23b` → − de cima |
| Potenciômetro (GPIO34) | GND em `cima 40c`, sinal em `cima 42c`, VCC em `cima 44c` | `cima 40e` → − de cima; `cima 34f` → `cima 42e`; `cima 44e` → + de cima |
| Buzzer (GPIO23) | − em `baixo 40f`, + em `baixo 43f`; resistor de 100 Ω em `baixo 43j` e `baixo 45j` | `baixo 40j` → − de baixo; `baixo 37e` → `baixo 45g` |
| LCD (fora das placas) | GND, VCC, SDA e SCL | − de baixo; + de baixo; `baixo 33e`; `baixo 36e` |
| GNSS (fora das placas) | TX, RX, VCC e GND | `baixo 27e`; `baixo 28e`; + do meio; − do meio |

Este é o layout de referência. Alguns furos mudaram durante os ajustes, principalmente os do LCD, então a bancada pode diferir em furos do mesmo grupo elétrico.
