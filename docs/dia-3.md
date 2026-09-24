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
