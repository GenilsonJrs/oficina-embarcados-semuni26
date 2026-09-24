// PINOS
#define PIN_LED  21
#define PIN_CLK  17
#define PIN_DT   16

// PWM
#define PWM_FREQ 5000                  // 5 kHz: sem cintilação visível
#define PWM_RES 8                      // 8 bits de resolução
#define DUTY_MAX ((1 << PWM_RES) - 1)  // 255
#define PWM_CANAL 0                    // só usado no core 2.x

// ENCODER
#define PASSOS_POR_CLIQUE 4  // transições por "clique" (KY-040 = 4; se pular de 2 em 2, use 2)
#define PASSO_BRILHO 5       // quanto o duty muda a cada clique

volatile int32_t contagemBruta = 0;
volatile uint8_t estadoEncoder = 0;
int32_t posicaoAnterior = 0;
int brilho = 0;

// Decodificador de quadratura: compara o estado anterior com o atual e
// descarta transições impossíveis (ruído e bouncing dos contatos)
void IRAM_ATTR isrEncoder() {
  static const int8_t tabela[16] = { 0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0 };
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

void setup() {
  Serial.begin(115200);

  pinMode(PIN_CLK, INPUT_PULLUP);
  pinMode(PIN_DT, INPUT_PULLUP);
  estadoEncoder = (digitalRead(PIN_CLK) << 1) | digitalRead(PIN_DT);
  attachInterrupt(digitalPinToInterrupt(PIN_CLK), isrEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_DT), isrEncoder, CHANGE);

  configurarPWM(PIN_LED, PWM_CANAL);
  escreverPWM(PIN_LED, PWM_CANAL, brilho);
}

void loop() {
  int32_t atual = contagemBruta;  // leitura de 32 bits é atômica na ESP32
  int cliques = (atual - posicaoAnterior) / PASSOS_POR_CLIQUE;

  if (cliques != 0) {
    posicaoAnterior += cliques * PASSOS_POR_CLIQUE;
    brilho = constrain(brilho + cliques * PASSO_BRILHO, 0, DUTY_MAX);
    escreverPWM(PIN_LED, PWM_CANAL, brilho);
  }
}
