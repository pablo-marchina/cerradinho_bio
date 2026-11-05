// Teste isolado de ligacao. O PlatformIO compila ../main.cpp como firmware principal.
// Autorizacao e ciclo aparecem apenas no diagnostico; sem regras de negocio aqui.
// O LCD I2C (GPIO21/22) e testado pelo firmware principal.
const int BUZZER = 23;
const int SW_LACRE = 27, SW_REDE = 18;
const int BTN_AUTORIZAR = 33, BTN_CICLO = 14;
const int BTN_AVANCAR = 25, BTN_VOLTAR = 26;
const int POT_BATERIA = 34;                    // ADC1
unsigned long ultimaLeitura = 0;
unsigned long caracteresGps = 0;
bool buzzerLigado = false;

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, 16, 17);
  pinMode(BUZZER, OUTPUT);
  pinMode(SW_LACRE, INPUT_PULLUP);
  pinMode(SW_REDE, INPUT_PULLUP);
  pinMode(BTN_AUTORIZAR, INPUT_PULLUP);
  pinMode(BTN_CICLO, INPUT_PULLUP);
  pinMode(BTN_AVANCAR, INPUT_PULLUP);
  pinMode(BTN_VOLTAR, INPUT_PULLUP);
}

void loop() {
  while (Serial2.available()) { Serial2.read(); ++caracteresGps; }
  // Com INPUT_PULLUP, LOW significa chave ligada ao GND ou botão pressionado.
  const bool lacreAberto = digitalRead(SW_LACRE) == HIGH;
  if (lacreAberto != buzzerLigado) {
    buzzerLigado = lacreAberto;
    if (buzzerLigado) tone(BUZZER, 2000);
    else noTone(BUZZER);
  }
  if (millis() - ultimaLeitura < 500) return;
  ultimaLeitura = millis();
  Serial.printf("lacre=%d rede=%d autorizar=%d ciclo=%d avancar=%d voltar=%d bateria_adc=%d gps_chars=%lu\n",
                digitalRead(SW_LACRE), digitalRead(SW_REDE),
                digitalRead(BTN_AUTORIZAR), digitalRead(BTN_CICLO),
                digitalRead(BTN_AVANCAR), digitalRead(BTN_VOLTAR),
                analogRead(POT_BATERIA), caracteresGps);
}
