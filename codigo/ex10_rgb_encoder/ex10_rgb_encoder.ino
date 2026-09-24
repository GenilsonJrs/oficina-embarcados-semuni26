// PINOS
const uint8_t PINOS_RGB[3] = {21, 19, 18};  // R, G, B
#define PIN_CLK 17
#define PIN_DT 16
#define PIN_MS 4

// LED
// #define ANODO_COMUM true // mude para true se o pino comum do LED vai no 3V3
#define ANODO_COMUM false
const char* NOMES[3] = {"Vermelho", "Verde", "Azul"};

// PWM
#define PWM_FREQ 5000               // 5 kHz: sem cintilação visível
#define  PWM_RES 8                  // 8 bits de resolução
#define      DUTY_MAX ((1 << PWM_RES) - 1)  // 255
// Canais PWM (só usados no core 2.x): 0 = R, 1 = G, 2 = B

// ENCODER
#define PASSOS_POR_CLIQUE 4       // KY-040 = 4; se pular de 2 em 2, use 2
#define PASSO_COR          5          // quanto o duty muda a cada clique
#define DEBOUNCE_MS 30

volatile int32_t contagemBruta = 0;
volatile uint8_t estadoEncoder = 0;
int32_t posicaoAnterior = 0;

int cores[3] = {0, 0, 0};  // intensidade de R, G, B (0 a DUTY_MAX)
uint8_t canalAtivo = 0;    // 0 = R, 1 = G, 2 = B

// Decodificador de quadratura: compara o estado anterior com o atual e
// descarta transições impossíveis (ruído e bouncing dos contatos)
void IRAM_ATTR isrEncoder() {
  static const int8_t tabela[16] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};
  estadoEncoder = ((estadoEncoder << 2) | (digitalRead(PIN_CLK) << 1) | digitalRead(PIN_DT)) & 0x0F;
  contagemBruta += tabela[estadoEncoder];
}

// A API do PWM (LEDC) mudou no core 3.x da ESP32; isto compila nas duas versões
void configurarPWM(uint8_t pino, uint8_t canal) {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcAttach(pino, PWM_FREQ, PWM_RES);
#else
  ledcSetup(canal, PWM_FREQ, PWM_RES);
  ledcAttachPin(pino, canal);
#endif
}

void escreverPWM(uint8_t pino, uint8_t canal, uint32_t duty) {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcWrite(pino, duty);
#else
  ledcWrite(canal, duty);
#endif
}

void escreverCor(uint8_t i, int valor) {
  // No anodo comum a lógica é invertida: duty alto = LED mais apagado
  escreverPWM(PINOS_RGB[i], i, ANODO_COMUM ? DUTY_MAX - valor : valor);
}

void atualizarLED() {
  for (uint8_t i = 0; i < 3; i++) escreverCor(i, cores[i]);
}

// Acende só a cor selecionada por um instante, para você saber qual está ajustando
void piscarCanalAtivo() {
  for (uint8_t i = 0; i < 3; i++) escreverCor(i, i == canalAtivo ? DUTY_MAX : 0);
  delay(250);
  atualizarLED();
}

// Retorna true uma única vez a cada clique (com debounce)
bool botaoClicado() {
  static bool estadoEstavel = HIGH;
  static bool ultimaLeitura = HIGH;
  static unsigned long ultimaMudanca = 0;

  bool leitura = digitalRead(PIN_MS);
  if (leitura != ultimaLeitura) {
    ultimaLeitura = leitura;
    ultimaMudanca = millis();
  }
  if (millis() - ultimaMudanca > DEBOUNCE_MS && leitura != estadoEstavel) {
    estadoEstavel = leitura;
    return estadoEstavel == LOW;  // LOW = pressionado (pull-up interno)
  }
  return false;
}

void setup() {
  Serial.begin(115200);

  pinMode(PIN_CLK, INPUT_PULLUP);
  pinMode(PIN_DT, INPUT_PULLUP);
  pinMode(PIN_MS, INPUT_PULLUP);
  estadoEncoder = (digitalRead(PIN_CLK) << 1) | digitalRead(PIN_DT);
  attachInterrupt(digitalPinToInterrupt(PIN_CLK), isrEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_DT), isrEncoder, CHANGE);

  for (uint8_t i = 0; i < 3; i++) configurarPWM(PINOS_RGB[i], i);
  atualizarLED();
  piscarCanalAtivo();
}

void loop() {
  if (botaoClicado()) {
    canalAtivo = (canalAtivo + 1) % 3;
    piscarCanalAtivo();
  }

  int32_t atual = contagemBruta;  // leitura de 32 bits é atômica na ESP32
  int cliques = (atual - posicaoAnterior) / PASSOS_POR_CLIQUE;

  if (cliques != 0) {
    posicaoAnterior += cliques * PASSOS_POR_CLIQUE;
    cores[canalAtivo] = constrain(cores[canalAtivo] + cliques * PASSO_COR, 0, DUTY_MAX);
    atualizarLED();
  }
}
