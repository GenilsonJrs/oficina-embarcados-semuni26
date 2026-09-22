#include <ESP32Servo.h>

#define SERVO 13
#define TRIG 5
#define ECHO 18
#define BUZZER 19

#define ANGULO_MINIMO 20
#define ANGULO_MAXIMO 160
#define PASSO 2

#define DISTANCIA_ALERTA 40
#define DISTANCIA_PERTO 5
#define PAUSA_PERTO 70
#define PAUSA_LONGE 480

Servo servo;
int angulo = ANGULO_MINIMO;
int passo = PASSO;

long medirDistancia() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duracao = pulseIn(ECHO, HIGH, 25000);
  if (duracao == 0) {
    return 999;
  }
  return duracao / 58;
}

void apitar(long distancia) {
  int pausa = map(distancia, DISTANCIA_PERTO, DISTANCIA_ALERTA, PAUSA_PERTO, PAUSA_LONGE);
  if (pausa < PAUSA_PERTO) {
    pausa = PAUSA_PERTO;
  }
  tone(BUZZER, 3000, 45);
  delay(pausa);
}

void setup() {
  Serial.begin(115200);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(BUZZER, OUTPUT);
  servo.attach(SERVO);
  servo.write(angulo);
  delay(500);
}

void loop() {
  servo.write(angulo);
  delay(60);

  long distancia = medirDistancia();

  Serial.print(angulo);
  Serial.print(" graus  ");
  Serial.print(distancia);
  Serial.println(" cm");

  if (distancia <= DISTANCIA_ALERTA) {
    apitar(distancia);
    return;
  }

  angulo = angulo + passo;
  if (angulo >= ANGULO_MAXIMO || angulo <= ANGULO_MINIMO) {
    passo = -passo;
  }
}
