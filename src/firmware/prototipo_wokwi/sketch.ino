// Teste isolado de ligacao. O PlatformIO compila ../main.cpp como firmware principal.
// Autorizacao e ciclo aparecem apenas no diagnostico; sem regras de negocio aqui.
const int LEDS[] = {25, 26, 23, 14};           // verde, amarelo, vermelho, azul
const int SW_LACRE = 27, SW_REDE = 18;
const int BTN_AUTORIZAR = 33, BTN_CICLO = 13;
const int POT_BATERIA = 34;                    // ADC1
unsigned long ultimaLeitura = 0;
unsigned long caracteresGps = 0;

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, 16, 17);
  for (int pino : LEDS) pinMode(pino, OUTPUT);
  pinMode(SW_LACRE, INPUT_PULLUP);
  pinMode(SW_REDE, INPUT_PULLUP);
  pinMode(BTN_AUTORIZAR, INPUT_PULLUP);
  pinMode(BTN_CICLO, INPUT_PULLUP);
}

void loop() {
  while (Serial2.available()) { Serial2.read(); ++caracteresGps; }
  // Com INPUT_PULLUP, LOW significa chave ligada ao GND ou botão pressionado.
  digitalWrite(LEDS[0], digitalRead(SW_LACRE) == LOW);
  digitalWrite(LEDS[1], digitalRead(BTN_AUTORIZAR) == LOW);
  digitalWrite(LEDS[2], digitalRead(SW_LACRE) == HIGH);
  digitalWrite(LEDS[3], digitalRead(SW_REDE) == HIGH);
  if (millis() - ultimaLeitura < 500) return;
  ultimaLeitura = millis();
  Serial.printf("lacre=%d rede=%d autorizar=%d ciclo=%d bateria_adc=%d gps_chars=%lu\n",
                digitalRead(SW_LACRE), digitalRead(SW_REDE),
                digitalRead(BTN_AUTORIZAR), digitalRead(BTN_CICLO),
                analogRead(POT_BATERIA), caracteresGps);
}
