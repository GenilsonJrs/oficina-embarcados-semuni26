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
