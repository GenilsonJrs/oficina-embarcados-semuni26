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
