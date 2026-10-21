// Protótipo GuaráTracking no Wokwi (Sprint 1).
// Issue #13: relógio de dias simulados, status semanal, prazos e LEDs.
// A issue #14 acrescenta lacre com debounce, autorização, fila e rede.

// ---------- Pinos ----------
const int LED_VERDE = 25, LED_AMARELO = 26, LED_VERMELHO = 27, LED_AZUL = 14;
const int SW_LACRE = 32, SW_REDE = 18;
const int BTN_AUTORIZAR = 33, BTN_CICLO = 13;
const int POT_BATERIA = 34;                     // ADC1

// ---------- Tempo simulado (valores de teste, ver 3.4.1) ----------
const unsigned long SEGUNDOS_POR_DIA = 1;       // 1 s real = 1 dia
const unsigned long MS_POR_DIA = SEGUNDOS_POR_DIA * 1000UL;
const unsigned int INTERVALO_STATUS_DIAS = 7;
const unsigned int PRAZO_PREVISTO_DIAS = 5;
const unsigned int LIMIAR_PERMANENCIA_DIAS = 150;   // CT04 roda com 10 e com 150
const unsigned long MEIO_PERIODO_PISCA_MS = 500;    // 1 Hz

class Rastreador {
 public:
  unsigned int ciclo_id = 1;
  unsigned int dias_fora = 0;
  unsigned long inicio_ciclo_ms = 0;
  bool alerta_atraso_emitido = false;
  bool alerta_permanencia_emitido = false;

  // Estados que a issue #14 passa a controlar.
  bool lacre_violado = false;
  bool retirada_em_curso = false;
  bool sem_rede_com_fila = false;

  bool emAtraso() const { return dias_fora > PRAZO_PREVISTO_DIAS; }
  bool emPermanencia() const { return dias_fora >= LIMIAR_PERMANENCIA_DIAS; }

  void iniciarCiclo(unsigned long agora_ms) {
    dias_fora = 0;
    inicio_ciclo_ms = agora_ms;
    alerta_atraso_emitido = false;
    alerta_permanencia_emitido = false;
  }

  // Monta a mensagem JSON que, no produto, vai para o broker MQTT.
  String montarJson(const char *tipo) const {
    String json = "{\"tipo\":\"";
    json += tipo;
    json += "\",\"ciclo_id\":";
    json += ciclo_id;
    json += ",\"dias_fora\":";
    json += dias_fora;
    json += ",\"lacre\":\"";
    json += lacre_violado ? "aberto" : "fechado";
    json += "\",\"atraso\":";
    json += emAtraso() ? "true" : "false";
    json += ",\"permanencia\":";
    json += emPermanencia() ? "true" : "false";
    json += "}";
    return json;
  }
};

Rastreador rastreador;

void publicarEvento(const char *tipo) {
  Serial.println(rastreador.montarJson(tipo));
}

// Atraso e permanência são alertas distintos: cada um sai uma única vez por ciclo
// e os dois podem coexistir.
void verificarPrazos() {
  if (rastreador.emAtraso() && !rastreador.alerta_atraso_emitido) {
    rastreador.alerta_atraso_emitido = true;
    publicarEvento("ALERTA_ATRASO");
  }
  if (rastreador.emPermanencia() && !rastreador.alerta_permanencia_emitido) {
    rastreador.alerta_permanencia_emitido = true;
    publicarEvento("ALERTA_PERMANENCIA");
  }
}

// Avança o relógio simulado. Processa um dia por vez para não pular o status
// nem um alerta se o loop atrasar.
void atualizarRelogio(unsigned long agora_ms) {
  unsigned long dias = (agora_ms - rastreador.inicio_ciclo_ms) / MS_POR_DIA;
  while (rastreador.dias_fora < dias) {
    rastreador.dias_fora++;
    if (rastreador.dias_fora % INTERVALO_STATUS_DIAS == 0) {
      publicarEvento("STATUS_SEMANAL");
    }
    verificarPrazos();
  }
}

// Verde: operando, lacre íntegro (piscando = retirada autorizada em curso).
// Amarelo: piscando = atraso; fixo = permanência acima do limiar.
// Vermelho: violação do lacre. Azul: sem rede, com eventos na fila.
void atualizarLeds(unsigned long agora_ms) {
  bool pisca = (agora_ms / MEIO_PERIODO_PISCA_MS) % 2 == 0;

  bool verde = !rastreador.lacre_violado && (!rastreador.retirada_em_curso || pisca);
  bool amarelo = rastreador.emPermanencia() || (rastreador.emAtraso() && pisca);
  bool vermelho = rastreador.lacre_violado && pisca;
  bool azul = rastreador.sem_rede_com_fila && pisca;

  digitalWrite(LED_VERDE, verde);
  digitalWrite(LED_AMARELO, amarelo);
  digitalWrite(LED_VERMELHO, vermelho);
  digitalWrite(LED_AZUL, azul);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_AMARELO, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  pinMode(LED_AZUL, OUTPUT);
  pinMode(SW_LACRE, INPUT_PULLUP);
  pinMode(SW_REDE, INPUT_PULLUP);
  pinMode(BTN_AUTORIZAR, INPUT_PULLUP);
  pinMode(BTN_CICLO, INPUT_PULLUP);

  rastreador.iniciarCiclo(millis());
  publicarEvento("STATUS");
}

void loop() {
  unsigned long agora = millis();
  atualizarRelogio(agora);
  atualizarLeds(agora);
}
