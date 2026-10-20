// Teste de ligação do circuito (issue #6). A lógica do protótipo vem na #13.
const int LEDS[] = {25, 26, 27, 14};           // verde, amarelo, vermelho, azul
const int SW_LACRE = 32, SW_REDE = 18;
const int BTN_AUTORIZAR = 33, BTN_CICLO = 13;
const int POT_BATERIA = 34;                    // ADC1

void setup() {
  Serial.begin(115200);
  for (int pino : LEDS) pinMode(pino, OUTPUT);
  pinMode(SW_LACRE, INPUT_PULLUP);
  pinMode(SW_REDE, INPUT_PULLUP);
  pinMode(BTN_AUTORIZAR, INPUT_PULLUP);
  pinMode(BTN_CICLO, INPUT_PULLUP);
}

void loop() {
  // Com INPUT_PULLUP, LOW significa chave ligada ao GND ou botão pressionado.
  digitalWrite(LEDS[0], digitalRead(SW_LACRE) == LOW);
  digitalWrite(LEDS[1], digitalRead(BTN_AUTORIZAR) == LOW);
  digitalWrite(LEDS[2], digitalRead(BTN_CICLO) == LOW);
  digitalWrite(LEDS[3], digitalRead(SW_REDE) == LOW);
  Serial.printf("lacre=%d rede=%d autorizar=%d ciclo=%d bateria_adc=%d\n",
                digitalRead(SW_LACRE), digitalRead(SW_REDE),
                digitalRead(BTN_AUTORIZAR), digitalRead(BTN_CICLO),
                analogRead(POT_BATERIA));
  delay(500);
}
