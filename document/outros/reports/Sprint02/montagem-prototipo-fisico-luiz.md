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

## 3. Adaptação do firmware

Os GPIOs da placa física são os mesmos do Wokwi. As mudanças foram feitas no ambiente PlatformIO `bancada`, que é o `demo` (1 s vale 1 dia) com a opção `SWITCH_BUTTONS=1`. Sem essa opção, as chaves deslizantes do Wokwi continuam funcionando como antes.

| Mudança | Motivo |
|---|---|
| LACRE e REDE alternam a cada toque | Botão não mantém o estado como a chave. O lacre segue pela mesma interrupção e pelo mesmo debounce; só o resultado passou a alternar |
| ON/OFF liga e desliga segurando 3 s | Evita desligar o aparelho com um esbarrão. Ao desligar, o LCD conta 3, 2 e 1 |
| Velocidade do I2C em 50 kHz, e LCD reconfigurado ao acender | Tentativa de deixar o LCD mais tolerante a fios longos, feita antes de a causa dos símbolos aparecer (seção 4.7) |
| Buzzer por canal PWM direto, sem `tone()` | Com `tone()` o buzzer ficava mudo na placa, embora tocasse com uma onda gerada à mão |
| Buzzer em 2700 Hz, antes 2000 Hz | Frequência mais alta entre as testadas, de 1500 a 4000 Hz (relatado, de ouvido) |

Um teste em host novo cobre o modo botões: lacre e rede no toque, ON/OFF em 3 s e a contagem no LCD. Com o modo botões pronto, o validador passou e os ambientes `bancada`, `demo` e `weekly` compilaram. As mudanças seguintes (I2C, buzzer e contagem) passaram nos testes em host, mas só o `bancada` foi compilado e gravado depois delas.

O projeto precisa ser compilado por um caminho curto no Windows. Numa pasta com caminho muito longo, o compilador do ESP32 falhou sem achar `stdint.h`.

Para gravar, na raiz do projeto:

```powershell
pio run -e bancada -t upload --upload-port COM16
pio device monitor --port COM16
```

## 4. Problemas encontrados

### 4.1 A placa não era reconhecida

A gravação falhou com `Write timeout`. O Windows mostrava o `CP2102 USB to UART Bridge Controller` com erro, e o PlatformIO tentou usar uma porta Bluetooth do notebook. A instalação do driver CP210x da Silicon Labs resolveu, e a placa apareceu na COM16. A partir daí a porta passou a ser indicada no comando.

### 4.2 Letras e colunas diferentes do guia

O primeiro guia assumia a linha j no topo das placas. A placa real tem a linha a em cima, como no Wokwi. Todas as coordenadas foram convertidas (a↔j, b↔i, c↔h, d↔g, e↔f), sem mudar a posição física das peças. A numeração impressa também começa no segundo furo, então as colunas passaram a ser contadas pelos furos.

### 4.3 Trilhos divididos no meio

Foi a causa de mais de um sintoma, porque as metades de cada trilho não se ligam.

| Ponto medido (preta no − do meio) | Valor medido |
|---|---|
| `cima 38f` (pino 3V3) | certo |
| + do meio, perto da coluna 3 | 0,18 V |
| + de cima, onde chegava o jumper | 0,13 V |
| + de cima, perto da coluna 44 | 0,15 V |

O 3V3 entra pela metade direita, e o jumper estava na esquerda. Como o trecho central do trilho do meio fica embaixo do ESP32, o jumper foi movido para perto da coluna 46, e o potenciômetro passou a responder. O serial mostrou a bateria indo de 0% até 100%, e cerca de 1930 de 4095 com o eixo perto do meio.

O mesmo defeito deixou VOLTAR e CICLO sem resposta. Em 29 leituras do serial, os dois nunca foram lidos como apertados, enquanto AVANÇAR apareceu em 3. Cada medição de continuidade passava, mas só perto dos botões: a metade esquerda do − de cima não chegava ao GND, que vem pela coluna 61. Um fio curto entre as metades resolveu.

### 4.4 Trilho de baixo sem 5 V

O + de baixo não recebia o fio do pino 5V. Depois de ligado, a tensão oscilava entre 2 e 5 V (medido) em `cima 20f`, enquanto o metal do pino dava cerca de 5 V. Era mau contato do pino com o furo, na ponta do USB. Firmar a placa na protoboard e trocar o fio estabilizou a medida.

### 4.5 Cinco botões sempre apertados

Logo após ligar, LACRE, AUTORIZAR, AVANÇAR, ON/OFF e REDE eram lidos como apertados. O firmware registrou violação e desligou sozinho. Os botões estavam girados 90°: a ligação interna unia as duas colunas, com o GPIO preso ao GND. Eles foram girados e passaram a ocupar 4 casas na vertical e 3 na horizontal. VOLTAR e CICLO foram ajustados da mesma forma depois.

### 4.6 Chaves ON/OFF e rede lidas como desligadas

Antes da troca por botões, o firmware lia a chave ON/OFF como desligada, mesmo com a alavanca à esquerda. Um jumper direto de `baixo 31e` ao − de baixo ligou o aparelho, o que isolou o defeito na chave ou nos fios dela. A chave de rede mostrou o mesmo sintoma. As chaves saíram da montagem, e os botões da seção 2 tomaram o lugar delas.

### 4.7 LCD

Foram três causas em sequência:

1. **Sem resposta no I2C.** A busca não encontrava nenhum endereço. Trocar SDA e SCL não mudou nada. A continuidade mostrou SDA e SCL certos, mas VCC e GND do módulo abertos. Depois de mudar as entradas, a busca achou o endereço **0x27**.
2. **Símbolos aleatórios e pouco contraste.** Um teste só com texto fixo mostrou o texto certo e 0 falhas em 300 tentativas de I2C por rodada, mas com letras quase invisíveis, mesmo com o trimpot no fim do curso.
3. **VCC em 3,28 V (medido).** O fio estava no + do meio, que é 3V3. Passar o VCC para o + de baixo (5 V) resolveu o contraste e os símbolos.

### 4.8 Buzzer

A bobina tem 42 Ω. Ligada direto no GPIO, puxaria cerca de 79 mA (calculado), acima do que o pino aguenta. Com o resistor de 100 Ω em série, a corrente cai para cerca de 23 mA (calculado). O corpo do buzzer também cobria os furos dos fios, então as pernas subiram para a linha f e os fios foram para a linha j.

O LED piscava, mas o buzzer ficava mudo. A continuidade estava certa, e uma onda de 2 kHz gerada à mão tocou baixo. O problema estava no uso do `tone()`, trocado pelo canal PWM direto (seção 3). A causa exata da falha do `tone()` não foi investigada.
