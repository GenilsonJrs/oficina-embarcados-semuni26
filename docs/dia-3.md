# Dia 3 — Colocando em movimento

<p class="resumo-inicial">Até aqui um pino só sabia duas coisas: ligado ou desligado. Hoje
ele aprende a fazer <strong>meia luz</strong>. Ligando e desligando rápido demais para o olho
perceber, a ESP32 controla o brilho de um LED, mistura as cores de um LED RGB e decide a
velocidade de um motor. No fim do dia, tudo isso vira um <strong>carrinho 4WD controlado
pelo celular</strong> — o desafio final da oficina.</p>

## :material-presentation: Slides da aula

<div class="moldura-slide" markdown="0">
<iframe src="https://www.canva.com/design/DAHVrQZxlXs/jZwaFOkhVmzazRfph9f9rA/view?embed"
        loading="lazy" allowfullscreen="allowfullscreen" allow="fullscreen"></iframe>
</div>

[Abrir em tela cheia](https://www.canva.com/design/DAHVrQZxlXs/jZwaFOkhVmzazRfph9f9rA/view){ .baixar target="_blank" }

<small>*SEMUNI — Aula 03*, por Luiz Guilherme Faria.</small>

## :material-clock-outline: Como o dia se organiza

<div class="info-cartoes" markdown="0"><div class="info"><div class="rotulo">14h00 – 14h40</div><div class="valor">PWM</div><div class="detalhe">intensidade de LEDs e velocidade de motores</div></div><div class="info"><div class="rotulo">14h40 – 15h30</div><div class="valor">Ponte H</div><div class="detalhe">sentido de giro e velocidade</div></div><div class="info"><div class="rotulo">15h30 – 15h45</div><div class="valor">Intervalo</div></div><div class="info"><div class="rotulo">15h45 – 16h40</div><div class="valor">Desafio final</div><div class="detalhe">integração em equipes</div></div><div class="info"><div class="rotulo">16h40 – 17h00</div><div class="valor">Apresentação</div><div class="detalhe">resultados e encerramento</div></div></div>

## :material-lightbulb: O que você vai aprender

<ul class="lista-icones">
<li>O que é <strong>PWM</strong> e como um pino digital produz “meia luz”</li>
<li><strong>Duty cycle</strong> e resolução: por que 8 bits dão 256 níveis</li>
<li>Ler um <strong>encoder rotativo</strong> com interrupções</li>
<li>Misturar cores num <strong>LED RGB</strong></li>
<li>Controlar sentido e velocidade de motores com a <strong>ponte H L298N</strong></li>
<li>Pilotar um carrinho pelo celular, via <strong>Bluetooth</strong></li>
</ul>

## :material-sine-wave: Meia luz com um pino digital

Um pino da ESP32 continua só sabendo ligar e desligar. O truque do **PWM** é fazer isso
**5 mil vezes por segundo**. Se o pino passa metade do tempo ligado, o LED parece aceso com
metade do brilho; se passa 10% do tempo, parece quase apagado. Essa fração de tempo ligado
é o **duty cycle**.

Com 8 bits de resolução, o duty vai de **0** (sempre desligado) a **255** (sempre ligado).
É o mesmo princípio que controla a velocidade de um motor: mais tempo ligado, mais força.

## :material-numeric-1-circle: Missão 1 — Brilho de um LED com encoder

Girando o encoder, o brilho do LED sobe ou desce de 5 em 5. O encoder avisa a placa por
**interrupção**: cada vez que um dos seus pinos muda, a ESP32 para o que está fazendo e
anota o movimento, sem perder nenhum clique.

<div class="video-demo horizontal" markdown="0">
<video controls preload="metadata" muted playsinline poster="../assets/dia3-missao1-capa.jpg">
<source src="../assets/dia3-missao1.mp4" type="video/mp4">
Seu navegador não reproduz vídeo. <a href="../assets/dia3-missao1.mp4">Baixe o arquivo</a>.
</video>
<div class="legenda-video">
<span class="rotulo">Demonstração</span>
<h3>O brilho acompanhando o giro</h3>
<p>Para um lado o LED acende aos poucos; para o outro, vai apagando. Não existe “ligado” e
“desligado” aqui: são 256 níveis de brilho no meio do caminho.</p>
</div>
</div>

| Componente | Pino da ESP32 |
|---|---|
| LED (com resistor) | **GPIO 21** |
| Encoder CLK | **GPIO 17** |
| Encoder DT | **GPIO 16** |
| Encoder + e GND | 3V3 e GND |

```cpp title="ex9_pwm_encoder.ino"
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
```

[Baixar ex9_pwm_encoder.ino](codigo/ex9_pwm_encoder/ex9_pwm_encoder.ino){ .baixar download }

??? question "Por que usar interrupção para ler o encoder?"
    O encoder muda de estado muito rápido quando você gira. Se o programa só olhasse os
    pinos de vez em quando, perderia passos. Com a interrupção, **cada mudança** chama a
    função `isrEncoder()` na hora, e o `loop()` só precisa conferir quanto a contagem andou.

??? question "O que faz a tabela de 16 posições?"
    Ela compara o estado anterior dos dois pinos com o atual. Das 16 combinações possíveis,
    só algumas são movimentos de verdade (para um lado ou para o outro); as demais são
    ruído ou trepidação do contato, e valem zero. É isso que deixa a leitura estável.

## :material-numeric-2-circle: Missão 2 — Cores de um LED RGB

Agora são três LEDs num só: vermelho, verde e azul. Cada um recebe o seu PWM, e a mistura
dos três forma as outras cores. O **botão do encoder** escolhe qual cor você está
ajustando; girar muda a intensidade dela.

<div class="video-demo horizontal" markdown="0">
<video controls preload="metadata" muted playsinline poster="../assets/dia3-missao2-capa.jpg">
<source src="../assets/dia3-missao2.mp4" type="video/mp4">
Seu navegador não reproduz vídeo. <a href="../assets/dia3-missao2.mp4">Baixe o arquivo</a>.
</video>
<div class="legenda-video">
<span class="rotulo">Demonstração</span>
<h3>Misturando as cores</h3>
<p>A cada clique no botão, o LED pisca na cor que passou a ser ajustada. Girando o encoder,
ela ganha ou perde força — e a cor final muda junto.</p>
</div>
</div>

| Componente | Pino da ESP32 |
|---|---|
| LED RGB — vermelho | **GPIO 21** |
| LED RGB — verde | **GPIO 19** |
| LED RGB — azul | **GPIO 18** |
| LED RGB — pino comum | GND (catodo comum) ou 3V3 (anodo comum) |
| Encoder CLK · DT | **GPIO 17** · **GPIO 16** |
| Encoder SW (botão) | **GPIO 4** |
| Encoder + e GND | 3V3 e GND |

```cpp title="ex10_rgb_encoder.ino"
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
```

[Baixar ex10_rgb_encoder.ino](codigo/ex10_rgb_encoder/ex10_rgb_encoder.ino){ .baixar download }

??? question "Meu LED faz o contrário: gira para mais e ele apaga"
    O seu LED é de **anodo comum**: o pino compartilhado vai no 3V3, e a lógica fica
    invertida. Troque `#define ANODO_COMUM false` para `true` e ligue o pino comum no 3V3.

??? question "Como faço amarelo, roxo ou branco?"
    Mistura de luz, não de tinta. Vermelho + verde dá **amarelo**; vermelho + azul dá
    **roxo**; os três no máximo dão **branco**. Ajuste uma cor, aperte o botão, ajuste a
    próxima.

## :material-flag-checkered: Desafio final — Carrinho 4WD

Hora de juntar tudo. O carrinho tem quatro motores, dois de cada lado, comandados por uma
**ponte H L298N**. A ESP32 manda PWM para a ponte, e a ponte entrega aos motores a força
que a placa sozinha não teria.

| ESP32 | L298N |
|---|---|
| **D27** | IN1 — motores esquerdos |
| **D26** | IN2 — motores esquerdos |
| **D25** | IN3 — motores direitos |
| **D33** | IN4 — motores direitos |
| **GND** | GND — comum entre ESP32, ponte H e bateria |

Os dois motores da esquerda vão em paralelo em **OUT1/OUT2**, e os da direita em
**OUT3/OUT4**. Todos os pinos ficam do lado do VIN da ESP32.

!!! warning "Deixe os jumpers do ENA e do ENB colocados"
    Eles não são ligados à ESP32. A velocidade é controlada por PWM direto nos pinos IN, e
    sem os jumpers os motores simplesmente não giram.

### :material-car: Código 1 — Controle básico dos motores

Cria as funções de movimento — frente, ré, girar, curva e parar — e roda uma sequência de
demonstração. É o jeito de conferir se a fiação está certa antes de pensar em controle
remoto.

```cpp title="ex11_carrinho_motores.ino"
/*
 * ================================================================
 *  CARRINHO 4WD - ESP32 + PONTE H (L298N)
 *  CÓDIGO 1: CONTROLE BÁSICO DOS MOTORES
 * ================================================================
 *  Oficina de Sistemas Embarcados - Desafio Final
 *
 *  O que este código faz:
 *   - Cria funções para mover o carrinho (frente, ré, girar, curva, parar)
 *   - Executa uma sequência de demonstração repetidamente
 *
 *  Usa apenas pinos do lado do VIN da ESP32 (DevKit V1 de 30 pinos).
 *
 *  Ligações (ESP32 -> L298N):
 *   D27 -> IN1  (motores ESQUERDOS)
 *   D26 -> IN2  (motores ESQUERDOS)
 *   D25 -> IN3  (motores DIREITOS)
 *   D33 -> IN4  (motores DIREITOS)
 *   GND -> GND  (OBRIGATÓRIO: GND comum entre ESP32, ponte H e bateria)
 *
 *  ENA e ENB NÃO são usados: DEIXE OS JUMPERS do ENA e do ENB colocados.
 *  A velocidade é controlada por PWM direto nos pinos IN.
 *
 *  Motores: os 2 motores da esquerda em paralelo em OUT1/OUT2,
 *           os 2 motores da direita em paralelo em OUT3/OUT4.
 *
 *  Placa no Arduino IDE: "ESP32 Dev Module"
 * ================================================================
 */

// ---------------- PINOS ----------------
const int IN1 = 27;   // motores esquerdos
const int IN2 = 26;   // motores esquerdos
const int IN3 = 25;   // motores direitos
const int IN4 = 33;   // motores direitos

// Se um LADO inteiro girar ao contrário, troque para true
// (evita ter que refazer a fiação!)
const bool INVERTER_MOTOR_ESQ = false;
const bool INVERTER_MOTOR_DIR = false;

// Velocidade padrão: 0 (parado) até 255 (máxima)
// Obs: em carrinhos 4WD, abaixo de ~110 os motores podem não ter força.
int velocidade = 200;

// ------------------------------------------------------------
// Controla UM lado do carrinho.
// vel: -255 a 255  (positivo = frente, negativo = ré, 0 = parado)
// Um pino recebe o PWM (velocidade) e o outro fica em 0.
// ------------------------------------------------------------
void controlarMotor(int pinoA, int pinoB, int vel, bool inverter) {
  if (inverter) vel = -vel;
  vel = constrain(vel, -255, 255);

  if (vel > 0) {            // gira para frente
    analogWrite(pinoA, vel);
    analogWrite(pinoB, 0);
  } else if (vel < 0) {     // gira para trás
    analogWrite(pinoA, 0);
    analogWrite(pinoB, -vel);
  } else {                  // parado
    analogWrite(pinoA, 0);
    analogWrite(pinoB, 0);
  }
}

// Controla os DOIS lados de uma vez
void moverMotores(int velEsq, int velDir) {
  controlarMotor(IN1, IN2, velEsq, INVERTER_MOTOR_ESQ);
  controlarMotor(IN3, IN4, velDir, INVERTER_MOTOR_DIR);
}

// ---------------- MOVIMENTOS ----------------
void frente(int v)        { moverMotores( v,  v); }
void re(int v)            { moverMotores(-v, -v); }
void girarEsquerda(int v) { moverMotores(-v,  v); }  // gira no próprio eixo
void girarDireita(int v)  { moverMotores( v, -v); }  // gira no próprio eixo
void parar()              { moverMotores( 0,  0); }

// ---------------- SETUP ----------------
void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  parar();
  delay(3000);  // tempo para colocar o carrinho no chão
}

// ---------------- LOOP (sequência de demonstração) ----------------
// Sempre paramos um pouco entre movimentos: inverter o motor de uma vez
// gera um pico de corrente que pode reiniciar a ESP32.
void loop() {
  frente(velocidade);                        // frente
  delay(2000);
  parar();
  delay(500);

  re(velocidade);                            // ré
  delay(2000);
  parar();
  delay(500);

  girarEsquerda(velocidade);                 // gira para a esquerda
  delay(1000);
  parar();
  delay(500);

  girarDireita(velocidade);                  // gira para a direita
  delay(1000);
  parar();
  delay(500);

  moverMotores(velocidade, velocidade / 3);  // curva suave para a direita
  delay(2000);
  parar();

  delay(3000);                               // pausa antes de repetir
}
```

[Baixar ex11_carrinho_motores.ino](codigo/ex11_carrinho_motores/ex11_carrinho_motores.ino){ .baixar download }

??? question "Um lado inteiro gira ao contrário. Refaço a fiação?"
    Não precisa. Troque `INVERTER_MOTOR_ESQ` ou `INVERTER_MOTOR_DIR` para `true`, conforme
    o lado, e envie o código de novo.

??? question "Por que o carrinho para entre um movimento e outro?"
    Inverter o sentido de um motor de uma vez gera um pico de corrente, que pode derrubar a
    alimentação e **reiniciar a ESP32**. Meio segundo parado entre os movimentos evita isso.

### :material-bluetooth: Código 2 — Controle pelo celular (Dabble)

O carrinho passa a obedecer ao app **Dabble**, pelo Bluetooth da própria ESP32. As setas
movem o carrinho; o joystick dá controle proporcional — quanto mais longe do centro, mais
rápido.

!!! warning "Antes de compilar: combine as versões"
    Instale a biblioteca **DabbleESP32** pelo Gerenciador de Bibliotecas. O que importa é a
    **dupla** biblioteca + pacote de placas *esp32 by Espressif Systems*:

    | DabbleESP32 | Pacote ESP32 | Resultado |
    |---|---|---|
    | **1.6.1** (a atual) | **3.x** | compila |
    | 1.5.x | 2.0.17 | compila |
    | 1.6.1 | 2.0.17 | **não compila** |

    O comentário no topo do código manda instalar o pacote 2.0.17. Isso valia para as
    versões antigas da biblioteca. Com a DabbleESP32 atual, **mantenha o pacote 3.x**.

<div class="info-cartoes" markdown="0"><div class="info"><div class="rotulo">Triângulo</div><div class="valor">Rápido</div><div class="detalhe">velocidade 255</div></div><div class="info"><div class="rotulo">Círculo</div><div class="valor">Médio</div><div class="detalhe">velocidade 200</div></div><div class="info"><div class="rotulo">X</div><div class="valor">Devagar</div><div class="detalhe">velocidade 150</div></div><div class="info"><div class="rotulo">Quadrado</div><div class="valor">Livre</div><div class="detalhe">buzina, farol: é com a equipe</div></div></div>

```cpp title="ex12_carrinho_dabble.ino"
/*
 * ================================================================
 *  CARRINHO 4WD - ESP32 + PONTE H (L298N)
 *  CÓDIGO 2: CONTROLE PELO APP DABBLE (BLUETOOTH)
 * ================================================================
 *  Oficina de Sistemas Embarcados - Desafio Final
 *
 *  Biblioteca necessária: "DabbleESP32" (Gerenciador de Bibliotecas)
 *
 *  ATENÇÃO: a DabbleESP32 NÃO compila com o pacote de placas ESP32 3.x.
 *  Em Ferramentas > Placa > Gerenciador de Placas, instale a versão
 *  2.0.17 do pacote "esp32 by Espressif Systems".
 *
 *  No celular:
 *   1. Abra o Dabble e conecte pelo ícone de conexão DO PRÓPRIO APP
 *      (não pareie pelas configurações de Bluetooth do celular).
 *   2. Escolha o nome do seu carrinho e abra o módulo "Gamepad".
 *
 *  Controles:
 *   Modo Digital  : setas movem o carrinho (cima + lado = curva)
 *   Modo Joystick : controle proporcional (quanto mais longe, mais rápido)
 *   Triângulo = rápido | Círculo = médio | X = devagar
 *   Quadrado  = livre para o desafio (buzina, farol, etc.)
 *
 *  Ligações: iguais às do Código 1 (só pinos do lado do VIN).
 *   D27 -> IN1 | D26 -> IN2 | D25 -> IN3 | D33 -> IN4 | GND -> GND
 *   DEIXE OS JUMPERS do ENA e do ENB colocados.
 * ================================================================
 */

#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <DabbleESP32.h>

// Cada equipe deve usar um nome DIFERENTE, senão os carrinhos se confundem!
const char* NOME_BLUETOOTH = "Carrinho_Equipe1";

// ---------------- PINOS ----------------
const int IN1 = 27;   // motores esquerdos
const int IN2 = 26;   // motores esquerdos
const int IN3 = 25;   // motores direitos
const int IN4 = 33;   // motores direitos

const bool INVERTER_MOTOR_ESQ = false;  // true se o lado girar ao contrário
const bool INVERTER_MOTOR_DIR = false;

// ---------------- VELOCIDADES (ajustadas para 4WD) ----------------
const int VEL_RAPIDA = 255;
const int VEL_MEDIA  = 200;
const int VEL_LENTA  = 150;
const int VEL_MINIMA = 110;   // abaixo disso os motores costumam não girar
int velocidade = VEL_MEDIA;

// ---------------- FUNÇÕES DOS MOTORES ----------------
// vel: -255 a 255. Um pino recebe o PWM e o outro fica em 0.
void controlarMotor(int pinoA, int pinoB, int vel, bool inverter) {
  if (inverter) vel = -vel;
  vel = constrain(vel, -255, 255);

  if (vel > 0)      { analogWrite(pinoA, vel); analogWrite(pinoB, 0);    }
  else if (vel < 0) { analogWrite(pinoA, 0);   analogWrite(pinoB, -vel); }
  else              { analogWrite(pinoA, 0);   analogWrite(pinoB, 0);    }
}

void moverMotores(int velEsq, int velDir) {
  controlarMotor(IN1, IN2, velEsq, INVERTER_MOTOR_ESQ);
  controlarMotor(IN3, IN4, velDir, INVERTER_MOTOR_DIR);
}

void parar() { moverMotores(0, 0); }

// Converte um fator de -1.0 a 1.0 em PWM, respeitando a velocidade mínima
int converterPWM(float fator) {
  if (fabs(fator) < 0.1) return 0;  // zona morta do joystick
  int pwm = VEL_MINIMA + (velocidade - VEL_MINIMA) * fabs(fator);
  return (fator > 0) ? pwm : -pwm;
}

// ---------------- MODO DIGITAL (setas) ----------------
void controleDigital() {
  bool cima     = GamePad.isUpPressed();
  bool baixo    = GamePad.isDownPressed();
  bool esquerda = GamePad.isLeftPressed();
  bool direita  = GamePad.isRightPressed();

  int curva = velocidade * 0.4;  // lado interno mais lento na curva

  if (cima && esquerda)       moverMotores(curva, velocidade);         // curva frente-esquerda
  else if (cima && direita)   moverMotores(velocidade, curva);         // curva frente-direita
  else if (baixo && esquerda) moverMotores(-curva, -velocidade);       // curva ré-esquerda
  else if (baixo && direita)  moverMotores(-velocidade, -curva);       // curva ré-direita
  else if (cima)              moverMotores(velocidade, velocidade);    // frente
  else if (baixo)             moverMotores(-velocidade, -velocidade);  // ré
  else if (esquerda)          moverMotores(-velocidade, velocidade);   // gira esquerda
  else if (direita)           moverMotores(velocidade, -velocidade);   // gira direita
  else                        parar();
}

// ---------------- MODO JOYSTICK ----------------
// "Mistura diferencial": o eixo Y dá a velocidade para frente/trás
// e o eixo X faz um lado girar mais que o outro para virar.
void controleJoystick() {
  float x = GamePad.getXaxisData();  // -7 (esquerda) a +7 (direita)
  float y = GamePad.getYaxisData();  // -7 (baixo)    a +7 (cima)

  float esq = constrain((y + x) / 7.0, -1.0, 1.0);
  float dir = constrain((y - x) / 7.0, -1.0, 1.0);

  moverMotores(converterPWM(esq), converterPWM(dir));
}

// ---------------- SETUP ----------------
void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  parar();

  Dabble.begin(NOME_BLUETOOTH);
}

// ---------------- LOOP ----------------
void loop() {
  Dabble.processInput();  // lê os comandos do app (sempre no início do loop)

  // Troca de velocidade
  if (GamePad.isTrianglePressed()) velocidade = VEL_RAPIDA;
  if (GamePad.isCirclePressed())   velocidade = VEL_MEDIA;
  if (GamePad.isCrossPressed())    velocidade = VEL_LENTA;

  // Botão livre para o desafio (ex: LED ou buzzer no D13, D14 ou D32)
  // if (GamePad.isSquarePressed()) { ... }

  // Se o joystick estiver sendo usado, ele tem prioridade; senão, usa as setas
  if (GamePad.getRadius() > 0) {
    controleJoystick();
  } else {
    controleDigital();
  }
}
```

[Baixar ex12_carrinho_dabble.ino](codigo/ex12_carrinho_dabble/ex12_carrinho_dabble.ino){ .baixar download }

??? question "Como conecto o celular?"
    Abra o Dabble e conecte pelo **ícone de conexão do próprio app** — não pareie pelas
    configurações de Bluetooth do celular. Escolha o nome do seu carrinho na lista e abra o
    módulo **Gamepad**.

??? question "Por que cada equipe precisa de um nome diferente?"
    Com vários carrinhos ligados na mesma sala, todos aparecem na lista do app. Se dois
    tiverem o mesmo nome, não dá para saber qual é o seu. Troque o `NOME_BLUETOOTH` antes
    de enviar o código.

## :material-star-circle: Extra — Carrinho pelo navegador, sem aplicativo

Outra forma de dirigir o mesmo carrinho: em vez do Dabble, a ESP32 **cria a própria rede
Wi-Fi** e serve uma página de controle. Qualquer celular ou computador conecta, abre o
navegador, escolhe o nome na lista e dirige — pelos botões na tela ou pelas setas do teclado.
Soltou, o carrinho para.

A fiação é a **mesma do desafio final**: D27, D26, D25 e D33 na ponte H.

<div class="info-cartoes" markdown="0"><div class="info"><div class="rotulo">Rede Wi-Fi</div><div class="valor">CARRINHO-01</div><div class="detalhe">muda com o número do carrinho</div></div><div class="info"><div class="rotulo">Senha</div><div class="valor">semuni2026</div></div><div class="info"><div class="rotulo">Endereço</div><div class="valor">192.168.4.1</div><div class="detalhe">abra no navegador</div></div></div>

!!! warning "Vários carrinhos na mesma sala"
    Antes de enviar o código, cada equipe troca o `NUMERO_DO_CARRINHO` no topo por um
    número diferente (1, 2, 3...). É ele que dá o nome da rede — `CARRINHO-01`,
    `CARRINHO-02` — e evita que um celular conecte no carrinho de outra equipe. Os pontos
    que podem ser mudados estão marcados com **MUDE AQUI** no código.

??? example "Ver o código completo"
    ```cpp title="carrinho_wifi_arduino.ino"
    #include <WiFi.h>
    #include <WebServer.h>

    // MUDE AQUI: cada carrinho com um número diferente (1, 2, 3...).
    // Ele vira o nome da rede: CARRINHO-01, CARRINHO-02, CARRINHO-03...
    const int NUMERO_DO_CARRINHO = 1;

    // MUDE AQUI se quiser: senha da rede do carrinho (mínimo de 8 caracteres).
    const char SENHA_DA_REDE[] = "semuni2026";

    // MUDE AQUI se quiser: endereço da página do carrinho.
    // Pode ser igual em todos, porque cada carrinho cria a própria rede.
    const IPAddress ENDERECO(192, 168, 4, 1);

    const int MAXIMO_DE_APARELHOS = 4;

    char nomeDaRede[16];

    int POTENCIA = 85;
    int POTENCIA_DA_CURVA = 85;

    const bool INVERTER_LADO_ESQUERDO = false;
    const bool INVERTER_LADO_DIREITO = true;

    const int PINO_ESQUERDA_FRENTE = 27;
    const int PINO_ESQUERDA_TRAS = 26;
    const int PINO_DIREITA_FRENTE = 25;
    const int PINO_DIREITA_TRAS = 33;

    const unsigned long PARADA_AUTOMATICA_MS = 1500;
    const unsigned long TEMPO_SEM_USO_MS = 20000;

    struct Usuario {
      const char *nome;
      const char *usuario;
    };

    // MUDE AQUI se precisar: quem aparece na lista de motoristas da página.
    // Primeiro o nome que aparece na tela, depois um apelido sem espaço e sem acento.
    const Usuario USUARIOS[] = {
      {"Ana Beatriz", "ana-beatriz"},
      {"Arthur Fernandes", "arthur-fernandes"},
      {"Arthur Noia", "arthur-noia"},
      {"Beatryz Pina", "beatryz-pina"},
      {"Daniel Abreu", "daniel-abreu"},
      {"Dominic Batista", "dominic-batista"},
      {"Eduardo Teles", "eduardo-teles"},
      {"Gabriel Mesquita", "gabriel-mesquita"},
      {"Gabriela Lemos", "gabriela-lemos"},
      {"Genilson Junior", "genilson-junior"},
      {"Graziele Alves", "graziele-alves"},
      {"Isadora Oliveira", "isadora-oliveira"},
      {"Joao Felipe Rosa", "joao-felipe-rosa"},
      {"Joao Gabryel", "joao-gabryel"},
      {"Joao Lucas Ramos", "joao-lucas-ramos"},
      {"Joao Vithor C.", "joao-vithor-c"},
      {"Joao Vitor B.", "joao-vitor-b"},
      {"Julia Amanda", "julia-amanda"},
      {"Laura Feitosa", "laura-feitosa"},
      {"Lorena Alves", "lorena-alves"},
      {"Lucas Leal", "lucas-leal"},
      {"Ludmila Lima", "ludmila-lima"},
      {"Luiz Faria", "luiz-faria"},
      {"Marco Antonio", "marco-antonio"},
      {"Marcos Queiroz", "marcos-queiroz"},
      {"Matheus Costa", "matheus-costa"},
      {"Pedro Urani", "pedro-urani"},
      {"Pietro Lima", "pietro-lima"},
      {"Rafael Trindade", "rafael-trindade"},
      {"Renato Sampaio", "renato-sampaio"},
      {"Rodrigo Ferreira", "rodrigo-ferreira"},
      {"Sheila Araujo", "sheila-araujo"},
      {"Sofia Lima", "sofia-lima"},
    };

    const int TOTAL_USUARIOS = sizeof(USUARIOS) / sizeof(USUARIOS[0]);

    WebServer servidor(80);

    int dutyEsquerdaFrente = 0;
    int dutyEsquerdaTras = 0;
    int dutyDireitaFrente = 0;
    int dutyDireitaTras = 0;

    void prepararMotores() {
      pinMode(PINO_ESQUERDA_FRENTE, OUTPUT);
      pinMode(PINO_ESQUERDA_TRAS, OUTPUT);
      pinMode(PINO_DIREITA_FRENTE, OUTPUT);
      pinMode(PINO_DIREITA_TRAS, OUTPUT);
    }

    int forca(int potencia) {
      if (potencia < 0) {
        potencia = 0;
      }
      if (potencia > 100) {
        potencia = 100;
      }
      return map(potencia, 0, 100, 0, 255);
    }

    void ladoEsquerdo(bool paraFrente, bool paraTras, int potencia) {
      if (INVERTER_LADO_ESQUERDO) {
        bool guarda = paraFrente;
        paraFrente = paraTras;
        paraTras = guarda;
      }
      dutyEsquerdaFrente = paraFrente ? forca(potencia) : 0;
      dutyEsquerdaTras = paraTras ? forca(potencia) : 0;
      analogWrite(PINO_ESQUERDA_FRENTE, dutyEsquerdaFrente);
      analogWrite(PINO_ESQUERDA_TRAS, dutyEsquerdaTras);
    }

    void ladoDireito(bool paraFrente, bool paraTras, int potencia) {
      if (INVERTER_LADO_DIREITO) {
        bool guarda = paraFrente;
        paraFrente = paraTras;
        paraTras = guarda;
      }
      dutyDireitaFrente = paraFrente ? forca(potencia) : 0;
      dutyDireitaTras = paraTras ? forca(potencia) : 0;
      analogWrite(PINO_DIREITA_FRENTE, dutyDireitaFrente);
      analogWrite(PINO_DIREITA_TRAS, dutyDireitaTras);
    }

    void frente() {
      ladoEsquerdo(true, false, POTENCIA);
      ladoDireito(true, false, POTENCIA);
    }

    void tras() {
      ladoEsquerdo(false, true, POTENCIA);
      ladoDireito(false, true, POTENCIA);
    }

    void esquerda() {
      ladoEsquerdo(false, true, POTENCIA_DA_CURVA);
      ladoDireito(true, false, POTENCIA_DA_CURVA);
    }

    void direita() {
      ladoEsquerdo(true, false, POTENCIA_DA_CURVA);
      ladoDireito(false, true, POTENCIA_DA_CURVA);
    }

    void parar() {
      ladoEsquerdo(false, false, 0);
      ladoDireito(false, false, 0);
    }

    bool executar(String acao) {
      if (acao == "frente") {
        frente();
        return true;
      }
      if (acao == "tras") {
        tras();
        return true;
      }
      if (acao == "esquerda") {
        esquerda();
        return true;
      }
      if (acao == "direita") {
        direita();
        return true;
      }
      if (acao == "parar") {
        parar();
        return true;
      }
      return false;
    }

    const char ESTILO[] = R"PAGINA(<!DOCTYPE html><html lang="pt-br"><head>
    <meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
    <title>RoboControl</title><style>
    *{box-sizing:border-box}
    body{margin:0;padding:22px 18px 30px;min-height:100vh;background:#0a1420;
    background-image:
    radial-gradient(ellipse 75% 55% at 6% 92%,rgba(232,133,44,.40),transparent 68%),
    radial-gradient(ellipse 55% 45% at 95% 4%,rgba(70,120,180,.34),transparent 70%),
    linear-gradient(175deg,#0b1a2c 0%,#0a1420 52%,#150f0e 100%);
    background-attachment:fixed;color:#e6eef8;
    font-family:ui-monospace,SFMono-Regular,Consolas,Menlo,monospace}
    .topo{display:flex;justify-content:space-between;align-items:flex-start;gap:14px;
    max-width:940px;margin:0 auto 26px}
    .marca{font-family:system-ui,Segoe UI,Arial,sans-serif;font-size:27px;font-weight:700;
    letter-spacing:-.02em;line-height:1}
    .marca i{font-style:normal;color:#f0a94a}
    .marca small{display:block;margin-top:7px;font-family:inherit;font-size:10px;
    font-weight:500;letter-spacing:.24em;color:#7f97b4}
    .lema{text-align:right;font-size:9.5px;letter-spacing:.2em;color:#6f89a8;
    line-height:1.9;padding-top:5px}
    .painel{position:relative;max-width:430px;margin:0 auto;padding:32px 22px 30px;
    border-radius:22px;border:1px solid rgba(126,182,226,.28);
    background:linear-gradient(158deg,rgba(20,42,66,.66),rgba(9,21,35,.5));
    box-shadow:inset 0 1px 0 rgba(255,255,255,.05),0 24px 70px rgba(0,0,0,.5);
    text-align:center}
    .painel::before,.painel::after{content:"";position:absolute;width:28px;height:28px;
    border-color:rgba(150,205,240,.65);border-style:solid}
    .painel::before{top:-1px;left:-1px;border-width:2px 0 0 2px;border-radius:22px 0 0 0}
    .painel::after{bottom:-1px;right:-1px;border-width:0 2px 2px 0;border-radius:0 0 22px 0}
    .chip{display:inline-block;padding:5px 13px;margin-bottom:18px;border-radius:5px;
    border:1px solid rgba(240,169,74,.45);background:rgba(240,169,74,.08);
    color:#f0b975;font-size:10.5px;letter-spacing:.2em}
    h1{margin:0 0 9px;font-size:19px;font-weight:600;letter-spacing:.2em;
    text-transform:uppercase;color:#eaf2fb}
    .ajuda{margin:0 0 24px;font-family:system-ui,Segoe UI,Arial,sans-serif;font-size:14.5px;
    color:#93a9c2;line-height:1.55}
    select{font-family:inherit;font-size:15px;padding:14px 12px;width:100%;
    border-radius:11px;background:rgba(8,20,33,.85);color:#e6eef8;
    border:1px solid rgba(126,182,226,.34);margin-bottom:14px;appearance:none;
    text-align:center}
    select:focus{outline:none;border-color:rgba(240,169,74,.8)}
    .entrar{font-family:inherit;font-size:14px;font-weight:600;letter-spacing:.16em;
    text-transform:uppercase;padding:15px 20px;width:100%;border-radius:11px;cursor:pointer;
    border:2px solid rgba(240,169,74,.8);background:rgba(240,169,74,.12);color:#ffca85;
    box-shadow:0 0 20px rgba(240,169,74,.16)}
    .entrar:active{background:rgba(240,169,74,.32);box-shadow:0 0 28px rgba(240,169,74,.45)}
    .grade{display:grid;grid-template-columns:repeat(3,1fr);gap:12px;
    max-width:310px;margin:0 auto}
    .grade span{display:block}
    .grade button{aspect-ratio:1;width:100%;border-radius:19px;cursor:pointer;
    display:flex;flex-direction:column;align-items:center;justify-content:center;gap:4px;
    border:2px solid rgba(240,169,74,.75);background:rgba(240,169,74,.07);color:#ffca85;
    box-shadow:0 0 18px rgba(240,169,74,.16),inset 0 0 22px rgba(240,169,74,.07);
    font-family:inherit;font-size:9.5px;letter-spacing:.13em;text-transform:uppercase;
    -webkit-user-select:none;user-select:none;touch-action:none;
    transition:background .09s,box-shadow .09s,transform .09s}
    .grade button:active{background:rgba(240,169,74,.34);
    box-shadow:0 0 34px rgba(240,169,74,.55);transform:scale(.94)}
    .grade svg{width:30px;height:30px;stroke:currentColor;stroke-width:2.6;fill:none;
    stroke-linecap:round;stroke-linejoin:round}
    .baixo svg{transform:rotate(180deg)}
    .esq svg{transform:rotate(-90deg)}
    .dir svg{transform:rotate(90deg)}
    .parar{border-color:rgba(214,96,86,.75)!important;background:rgba(214,96,86,.09)!important;
    color:#ffb0a6!important;box-shadow:0 0 16px rgba(214,96,86,.16)!important}
    .parar:active{background:rgba(214,96,86,.38)!important;
    box-shadow:0 0 32px rgba(214,96,86,.5)!important}
    .parar b{display:block;width:13px;height:13px;border-radius:3px;background:currentColor}
    #aviso{margin-top:22px;font-family:system-ui,Segoe UI,Arial,sans-serif;font-size:14.5px;
    color:#93a9c2;min-height:21px}
    #aviso.ocupado{color:#f0b975}
    #assumir{margin-top:14px;font-family:inherit;font-size:11.5px;font-weight:600;
    letter-spacing:.14em;text-transform:uppercase;padding:12px 22px;cursor:pointer;
    border-radius:9px;border:1px solid rgba(240,169,74,.6);background:rgba(240,169,74,.12);
    color:#f0b975}
    #assumir:active{background:rgba(240,169,74,.34)}
    .ajustes{margin-top:26px;padding-top:20px;border-top:1px solid rgba(126,182,226,.16)}
    .ajustes .titulo{font-size:9.5px;letter-spacing:.22em;text-transform:uppercase;
    color:#6f89a8;margin-bottom:11px}
    .duplo{display:flex;gap:10px;justify-content:center}
    .duplo button{flex:1;font-family:inherit;font-size:10px;font-weight:600;
    letter-spacing:.12em;text-transform:uppercase;padding:11px 8px;cursor:pointer;
    border-radius:9px;border:1px solid rgba(126,182,226,.34);background:rgba(8,20,33,.6);
    color:#93a9c2}
    .duplo button.ligado{border-color:rgba(240,169,74,.8);background:rgba(240,169,74,.16);
    color:#ffca85}
    .duplo button.ligado::after{content:" invertido";opacity:.75}
    .duplo button:active{transform:scale(.97)}
    .dica{margin-top:11px;font-family:system-ui,Segoe UI,Arial,sans-serif;font-size:12px;
    color:#6f89a8;line-height:1.5}
    .dica b{color:#93a9c2}
    .broto{width:26px;height:26px;vertical-align:-5px;margin-right:7px}
    .robo{position:fixed;left:16px;bottom:8px;width:172px;height:172px;
    pointer-events:none;opacity:.97;z-index:-1}
    @media(max-width:900px){.robo{width:128px;height:128px;opacity:.5;left:4px;bottom:0}}
    @media(max-width:560px){.robo{display:none}}
    .sair{display:inline-block;margin-top:22px;color:#6f8fb8;font-size:10.5px;
    letter-spacing:.16em;text-transform:uppercase;text-decoration:none;
    border-bottom:1px solid rgba(111,143,184,.4);padding-bottom:3px}
    .rodape{max-width:940px;margin:28px auto 0;text-align:right;font-size:9.5px;
    letter-spacing:.2em;color:#6f89a8;line-height:1.9}
    @media(max-width:620px){
    .topo{flex-direction:column;align-items:center;text-align:center}
    .lema,.rodape{text-align:center}
    .marca{font-size:23px}}
    </style></head><body>
    <div class="topo">
    <div class="marca"><svg class="broto" viewBox="0 0 24 24"><path d="M12 21 v-8" stroke="#8fd07a" stroke-width="2" stroke-linecap="round" fill="none"/><path d="M12 14 q-7 -1 -8 -8 q8 0 8 8 z" fill="#8fd07a"/><path d="M12 15 q6 -2 7 -9 q-8 1 -7 9 z" fill="#6fb85c"/></svg>Robo<i>Control</i><small>CONTROLE SEU CARRINHO</small></div>
    <div class="lema">PEQUENOS MOVIMENTOS<br>GRANDES DESCOBERTAS</div>
    </div>
    <div class="painel">
    )PAGINA";

    const char CONTROLE[] = R"PAGINA(<div class="grade">
    <span></span>
    <button data-acao="frente" class="cima"><svg viewBox="0 0 24 24"><polyline points="6 15 12 9 18 15"></polyline></svg>frente</button>
    <span></span>
    <button data-acao="esquerda" class="esq"><svg viewBox="0 0 24 24"><polyline points="6 15 12 9 18 15"></polyline></svg>esquerda</button>
    <button data-acao="parar" class="parar"><b></b>parar</button>
    <button data-acao="direita" class="dir"><svg viewBox="0 0 24 24"><polyline points="6 15 12 9 18 15"></polyline></svg>direita</button>
    <span></span>
    <button data-acao="tras" class="baixo"><svg viewBox="0 0 24 24"><polyline points="6 15 12 9 18 15"></polyline></svg>r&eacute;</button>
    <span></span></div>
    <div id="aviso"></div>
    <button id="assumir" hidden>assumir o controle</button>
    <a class="sair" href="/">trocar de motorista</a>
    </div>
    <div class="rodape">UM FUTURO<br>MAIS BRILHANTE</div>
    <svg class="robo" viewBox="0 0 200 200"><defs><linearGradient id="gcorpo" x1="0" y1="0" x2="0" y2="1"><stop offset="0" stop-color="#e3b65c"/><stop offset="1" stop-color="#94701f"/></linearGradient><linearGradient id="golho" x1="0" y1="0" x2="0" y2="1"><stop offset="0" stop-color="#b3bcc9"/><stop offset="1" stop-color="#606b7c"/></linearGradient><radialGradient id="gbrilho" cx="50%" cy="55%" r="50%"><stop offset="0" stop-color="#f0a94a" stop-opacity=".30"/><stop offset="1" stop-color="#f0a94a" stop-opacity="0"/></radialGradient></defs><ellipse cx="100" cy="150" rx="96" ry="52" fill="url(#gbrilho)"/><rect x="18" y="140" width="56" height="46" rx="23" fill="#49525f" stroke="#2b323c" stroke-width="2"/><rect x="126" y="140" width="56" height="46" rx="23" fill="#49525f" stroke="#2b323c" stroke-width="2"/><circle cx="36" cy="163" r="8" fill="#333a45"/><circle cx="56" cy="163" r="8" fill="#333a45"/><circle cx="144" cy="163" r="8" fill="#333a45"/><circle cx="164" cy="163" r="8" fill="#333a45"/><rect x="30" y="104" width="16" height="44" rx="7" fill="#7b8697"/><rect x="154" y="104" width="16" height="44" rx="7" fill="#7b8697"/><rect x="52" y="88" width="96" height="66" rx="10" fill="url(#gcorpo)" stroke="#6d5215" stroke-width="2"/><rect x="64" y="102" width="72" height="36" rx="6" fill="#00000022" stroke="#7d5f1c" stroke-width="2"/><line x1="64" y1="120" x2="136" y2="120" stroke="#7d5f1c" stroke-width="2"/><circle cx="76" cy="111" r="3.5" fill="#ffd98a"/><circle cx="88" cy="111" r="3.5" fill="#c0873a"/><rect x="94" y="70" width="12" height="24" rx="5" fill="#7b8697"/><rect x="44" y="34" width="52" height="46" rx="23" fill="url(#golho)" stroke="#39414e" stroke-width="2"/><rect x="104" y="34" width="52" height="46" rx="23" fill="url(#golho)" stroke="#39414e" stroke-width="2"/><circle cx="70" cy="57" r="15" fill="#141c26"/><circle cx="130" cy="57" r="15" fill="#141c26"/><circle cx="70" cy="57" r="8" fill="#2a6ea8"/><circle cx="130" cy="57" r="8" fill="#2a6ea8"/><circle cx="65" cy="51" r="4" fill="#d8ebff" opacity=".92"/><circle cx="125" cy="51" r="4" fill="#d8ebff" opacity=".92"/><path d="M100 88 q-3 -16 -14 -22 q13 -1 16 12 q4 -14 17 -13 q-12 7 -15 23 z" fill="#5fae4e"/><rect x="97" y="84" width="4" height="10" rx="2" fill="#4a8c3c"/></svg>
    <script>
    var atual='';
    var repetidor=null;
    var aviso=document.getElementById('aviso');
    var botaoAssumir=document.getElementById('assumir');
    var teclas={ArrowUp:'frente',ArrowDown:'tras',ArrowLeft:'esquerda',ArrowRight:'direita'};
    function mostrar(t){
    var partes=t.split('|');
    aviso.textContent=partes.length>1?partes[1]:t;
    var ocupado=partes[0]=='ocupado';
    aviso.className=ocupado?'ocupado':'';
    botaoAssumir.hidden=!ocupado;}
    function enviar(acao){
    return fetch('/comando?u='+eu+'&a='+acao).then(function(r){return r.text()})
    .then(mostrar)
    .catch(function(){aviso.textContent='sem resposta do carrinho';});}
    function manda(acao){
    if(acao==atual)return;
    atual=acao;
    if(repetidor){clearInterval(repetidor);repetidor=null;}
    enviar(acao);
    if(acao!='parar'){repetidor=setInterval(function(){enviar(acao)},500);}}
    function solta(){manda('parar')}
    botaoAssumir.addEventListener('click',function(){atual='';manda('assumir')});
    var botoes=document.querySelectorAll('.grade button');
    for(var i=0;i<botoes.length;i++){(function(b){
    var acao=b.getAttribute('data-acao');
    b.addEventListener('mousedown',function(){manda(acao)});
    b.addEventListener('touchstart',function(e){e.preventDefault();manda(acao)});
    b.addEventListener('mouseup',solta);b.addEventListener('mouseleave',solta);
    b.addEventListener('touchend',function(e){e.preventDefault();solta()});
    })(botoes[i])}
    document.addEventListener('keydown',function(e){
    if(teclas[e.key]&&!e.repeat){e.preventDefault();manda(teclas[e.key])}});
    document.addEventListener('keyup',function(e){
    if(teclas[e.key]){e.preventDefault();solta()}});
    window.addEventListener('blur',solta);
    </script></body></html>
    )PAGINA";

    const char FIM_LOGIN[] = R"PAGINA(</div>
    <div class="rodape">UM FUTURO<br>MAIS BRILHANTE</div>
    <svg class="robo" viewBox="0 0 200 200"><defs><linearGradient id="gcorpo" x1="0" y1="0" x2="0" y2="1"><stop offset="0" stop-color="#e3b65c"/><stop offset="1" stop-color="#94701f"/></linearGradient><linearGradient id="golho" x1="0" y1="0" x2="0" y2="1"><stop offset="0" stop-color="#b3bcc9"/><stop offset="1" stop-color="#606b7c"/></linearGradient><radialGradient id="gbrilho" cx="50%" cy="55%" r="50%"><stop offset="0" stop-color="#f0a94a" stop-opacity=".30"/><stop offset="1" stop-color="#f0a94a" stop-opacity="0"/></radialGradient></defs><ellipse cx="100" cy="150" rx="96" ry="52" fill="url(#gbrilho)"/><rect x="18" y="140" width="56" height="46" rx="23" fill="#49525f" stroke="#2b323c" stroke-width="2"/><rect x="126" y="140" width="56" height="46" rx="23" fill="#49525f" stroke="#2b323c" stroke-width="2"/><circle cx="36" cy="163" r="8" fill="#333a45"/><circle cx="56" cy="163" r="8" fill="#333a45"/><circle cx="144" cy="163" r="8" fill="#333a45"/><circle cx="164" cy="163" r="8" fill="#333a45"/><rect x="30" y="104" width="16" height="44" rx="7" fill="#7b8697"/><rect x="154" y="104" width="16" height="44" rx="7" fill="#7b8697"/><rect x="52" y="88" width="96" height="66" rx="10" fill="url(#gcorpo)" stroke="#6d5215" stroke-width="2"/><rect x="64" y="102" width="72" height="36" rx="6" fill="#00000022" stroke="#7d5f1c" stroke-width="2"/><line x1="64" y1="120" x2="136" y2="120" stroke="#7d5f1c" stroke-width="2"/><circle cx="76" cy="111" r="3.5" fill="#ffd98a"/><circle cx="88" cy="111" r="3.5" fill="#c0873a"/><rect x="94" y="70" width="12" height="24" rx="5" fill="#7b8697"/><rect x="44" y="34" width="52" height="46" rx="23" fill="url(#golho)" stroke="#39414e" stroke-width="2"/><rect x="104" y="34" width="52" height="46" rx="23" fill="url(#golho)" stroke="#39414e" stroke-width="2"/><circle cx="70" cy="57" r="15" fill="#141c26"/><circle cx="130" cy="57" r="15" fill="#141c26"/><circle cx="70" cy="57" r="8" fill="#2a6ea8"/><circle cx="130" cy="57" r="8" fill="#2a6ea8"/><circle cx="65" cy="51" r="4" fill="#d8ebff" opacity=".92"/><circle cx="125" cy="51" r="4" fill="#d8ebff" opacity=".92"/><path d="M100 88 q-3 -16 -14 -22 q13 -1 16 12 q4 -14 17 -13 q-12 7 -15 23 z" fill="#5fae4e"/><rect x="97" y="84" width="4" height="10" rx="2" fill="#4a8c3c"/></svg>
    </body></html>
    )PAGINA";

    void abrirPagina() {
      servidor.setContentLength(CONTENT_LENGTH_UNKNOWN);
      servidor.send(200, "text/html", "");
      servidor.sendContent(ESTILO);
      servidor.sendContent(String("<div class=\"chip\">") + nomeDaRede + "</div>");
    }

    void fecharPagina() {
      servidor.sendContent("");
    }

    void enviarLogin() {
      abrirPagina();
      servidor.sendContent("<h1>Quem vai dirigir?</h1>");
      servidor.sendContent("<p class=\"ajuda\">Escolha o seu nome na lista e entre.</p>");
      servidor.sendContent("<select id=\"quem\">");
      for (int i = 0; i < TOTAL_USUARIOS; i++) {
        servidor.sendContent(String("<option value=\"") + USUARIOS[i].usuario + "\">" +
                             USUARIOS[i].nome + "</option>");
      }
      servidor.sendContent("</select>");
      servidor.sendContent("<button class=\"entrar\" onclick=\"entrar()\">Entrar</button>");
      servidor.sendContent("<script>function entrar(){location.href='/carro/'"
                           "+document.getElementById('quem').value}</script>");
      servidor.sendContent(FIM_LOGIN);
      fecharPagina();
    }

    void enviarControle(String nome, String usuario) {
      abrirPagina();
      servidor.sendContent("<h1>Controle direto</h1>");
      servidor.sendContent("<p class=\"ajuda\">" + nome +
                           ", segure o bot&atilde;o ou use as setas do teclado."
                           " Soltou, o carrinho para.</p>");
      servidor.sendContent("<script>var eu='" + usuario + "';</script>");
      servidor.sendContent(CONTROLE);
      fecharPagina();
    }

    void enviarRecado(String titulo, String texto) {
      abrirPagina();
      servidor.sendContent("<h1>" + titulo + "</h1>");
      servidor.sendContent("<p class=\"ajuda\">" + texto + "</p>");
      servidor.sendContent("<a class=\"sair\" href=\"/\">voltar</a>");
      servidor.sendContent(FIM_LOGIN);
      fecharPagina();
    }

    String motorista = "";
    String ultimaAcaoRegistrada = "";
    unsigned long ultimoComando = 0;

    void registrar(String quem, String acao, String resposta) {
      if (acao == ultimaAcaoRegistrada) {
        return;
      }
      ultimaAcaoRegistrada = acao;
      Serial.print("[");
      Serial.print(millis() / 1000);
      Serial.print("s] ");
      Serial.print(quem);
      Serial.print(" pediu ");
      Serial.print(acao);
      Serial.print(" -> ");
      Serial.print(resposta);
      Serial.print("  | duty esq ");
      Serial.print(dutyEsquerdaFrente);
      Serial.print("/");
      Serial.print(dutyEsquerdaTras);
      Serial.print("  dir ");
      Serial.print(dutyDireitaFrente);
      Serial.print("/");
      Serial.println(dutyDireitaTras);
    }

    void montarNomeDaRede() {
      sprintf(nomeDaRede, "CARRINHO-%02d", NUMERO_DO_CARRINHO);
    }

    int canalDoCarrinho() {
      const int CANAIS[3] = {1, 6, 11};
      return CANAIS[(NUMERO_DO_CARRINHO - 1) % 3];
    }

    int procurarUsuario(String usuario) {
      for (int i = 0; i < TOTAL_USUARIOS; i++) {
        if (usuario == USUARIOS[i].usuario) {
          return i;
        }
      }
      return -1;
    }

    String nomeDe(String usuario) {
      int indice = procurarUsuario(usuario);
      if (indice < 0) {
        return "";
      }
      return String(USUARIOS[indice].nome);
    }

    String comandar(String usuario, String acao) {
      if (procurarUsuario(usuario) < 0) {
        return "erro|faca login de novo";
      }
      if (acao == "assumir") {
        parar();
        motorista = usuario;
        ultimoComando = millis();
        return "ok|agora o controle e seu";
      }
      bool livre = (motorista == "") || (millis() - ultimoComando > TEMPO_SEM_USO_MS);
      if (motorista != usuario && !livre) {
        return "ocupado|" + nomeDe(motorista) + " esta dirigindo";
      }
      motorista = usuario;
      ultimoComando = millis();
      if (!executar(acao)) {
        return "erro|nao entendi o comando";
      }
      if (acao == "parar") {
        return "ok|parado";
      }
      return "ok|andando para " + acao;
    }

    void vigiar() {
      if (motorista == "") {
        return;
      }
      unsigned long paradoHa = millis() - ultimoComando;
      if (paradoHa > PARADA_AUTOMATICA_MS) {
        if (ultimaAcaoRegistrada != "" && ultimaAcaoRegistrada != "parar") {
          Serial.println("  ...parada automatica: ficou 1,5s sem receber comando");
          ultimaAcaoRegistrada = "parar";
        }
        parar();
      }
      if (paradoHa > TEMPO_SEM_USO_MS) {
        motorista = "";
      }
    }

    void atenderComando() {
      String usuario = servidor.arg("u");
      String acao = servidor.arg("a");
      String resposta = comandar(usuario, acao);
      registrar(nomeDe(usuario), acao, resposta);
      servidor.send(200, "text/plain", resposta);
    }

    void atenderOutros() {
      String caminho = servidor.uri();
      if (caminho.startsWith("/carro/")) {
        String usuario = caminho.substring(7);
        int indice = procurarUsuario(usuario);
        if (indice < 0) {
          enviarRecado("Nao achei esse nome", "Volte e escolha na lista.");
        } else {
          enviarControle(USUARIOS[indice].nome, usuario);
        }
        return;
      }
      servidor.send(404, "text/plain", "nao existe");
    }

    void setup() {
      Serial.begin(115200);
      prepararMotores();
      parar();

      montarNomeDaRede();
      bool redeNoAr = WiFi.softAP(nomeDaRede, SENHA_DA_REDE, canalDoCarrinho(), 0, MAXIMO_DE_APARELHOS);
      WiFi.softAPConfig(ENDERECO, ENDERECO, IPAddress(255, 255, 255, 0));

      Serial.println();
      if (redeNoAr) {
        Serial.print("Rede Wi-Fi no ar: ");
        Serial.println(nomeDaRede);
        Serial.print("Senha: ");
        Serial.println(SENHA_DA_REDE);
        Serial.print("Canal: ");
        Serial.println(canalDoCarrinho());
        Serial.print("Aparelhos ao mesmo tempo: ");
        Serial.println(MAXIMO_DE_APARELHOS);
        Serial.print("Abra no navegador: http://");
        Serial.println(WiFi.softAPIP());
        Serial.print("Inverter lado esquerdo: ");
        Serial.println(INVERTER_LADO_ESQUERDO ? "sim" : "nao");
        Serial.print("Inverter lado direito: ");
        Serial.println(INVERTER_LADO_DIREITO ? "sim" : "nao");
        Serial.println("Pronto. Cada comando recebido aparece abaixo.");
      } else {
        Serial.println("FALHOU ao criar a rede Wi-Fi");
      }

      servidor.on("/", enviarLogin);
      servidor.on("/comando", atenderComando);
      servidor.onNotFound(atenderOutros);
      servidor.begin();
    }

    void loop() {
      servidor.handleClient();
      vigiar();
    }
    ```

[Baixar carrinho_wifi_arduino.ino](codigo/carrinho_wifi_arduino/carrinho_wifi_arduino.ino){ .baixar download }

??? question "Uma roda para de girar só na curva. É fio solto?"
    Provavelmente não. Girar no próprio eixo é o movimento mais pesado para um 4WD, porque
    as rodas arrastam de lado no chão. Os dois motores de um lado recebem a mesma tensão, e
    o que tiver mais atrito trava primeiro. Se na ré a mesma roda gira normalmente, o fio
    está bom: aumente o `POTENCIA_DA_CURVA`.

??? question "Quem dirige quando duas pessoas entram ao mesmo tempo?"
    Quem apertou primeiro. A outra pessoa vê o aviso de que o carrinho está ocupado e um
    botão para assumir o controle. Depois de 20 segundos sem comandos, o carrinho fica
    livre de novo.

## :material-help-circle: Se algo não funcionar

| Sintoma | Causa mais comum |
|---|---|
| Encoder muda de 2 em 2 cliques | troque `PASSOS_POR_CLIQUE` de 4 para 2 |
| Girar para a direita diminui o brilho | fios CLK e DT invertidos: troque os dois de lugar |
| LED RGB faz tudo ao contrário | LED de anodo comum: use `ANODO_COMUM true` e o pino comum no 3V3 |
| Um lado do carrinho gira ao contrário | `INVERTER_MOTOR_ESQ` ou `INVERTER_MOTOR_DIR` como `true` |
| Motores não giram | jumpers do ENA e ENB retirados, ou GND não é comum com a bateria |
| Motores zumbem mas não andam | velocidade abaixo de ~110: falta força no 4WD |
| ESP32 reinicia ao mudar de direção | pico de corrente: pare antes de inverter o sentido |
| Código do Dabble não compila | versões desencontradas: DabbleESP32 1.6.1 pede o pacote ESP32 3.x; a 1.5.x pede o 2.0.17 |
| Carrinho não aparece no app | pareado pelo celular em vez de pelo app, ou nome repetido |
