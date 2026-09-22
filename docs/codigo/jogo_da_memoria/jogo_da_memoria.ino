#define LED_VERMELHO 2
#define LED_AZUL 4
#define LED_VERDE 5
#define LED_AMARELO 18

#define BOTAO_VERMELHO 19
#define BOTAO_AZUL 21
#define BOTAO_VERDE 22
#define BOTAO_AMARELO 23

#define BUZZER 25

#define TOTAL_FASES 20
#define TEMPO_ACESO 500
#define TEMPO_PAUSA 200
#define TEMPO_LIMITE 5000

int leds[4] = {LED_VERMELHO, LED_AZUL, LED_VERDE, LED_AMARELO};
int botoes[4] = {BOTAO_VERMELHO, BOTAO_AZUL, BOTAO_VERDE, BOTAO_AMARELO};
int tons[4] = {1048, 1320, 1568, 2092};

int sequencia[TOTAL_FASES];
int fase = 0;

void tocarCor(int cor, int duracao) {
  digitalWrite(leds[cor], HIGH);
  tone(BUZZER, tons[cor], duracao);
  delay(duracao);
  digitalWrite(leds[cor], LOW);
}

void piscarTodos(int vezes) {
  for (int i = 0; i < vezes; i++) {
    for (int c = 0; c < 4; c++) {
      digitalWrite(leds[c], HIGH);
    }
    delay(150);
    for (int c = 0; c < 4; c++) {
      digitalWrite(leds[c], LOW);
    }
    delay(150);
  }
}

int esperarBotao() {
  unsigned long inicio = millis();
  while (millis() - inicio < TEMPO_LIMITE) {
    for (int c = 0; c < 4; c++) {
      if (digitalRead(botoes[c]) == LOW) {
        delay(30);
        while (digitalRead(botoes[c]) == LOW) {
        }
        return c;
      }
    }
  }
  return -1;
}

void mostrarSequencia() {
  delay(600);
  for (int i = 0; i <= fase; i++) {
    tocarCor(sequencia[i], TEMPO_ACESO);
    delay(TEMPO_PAUSA);
  }
}

bool vezDoJogador() {
  for (int i = 0; i <= fase; i++) {
    int escolha = esperarBotao();
    if (escolha < 0) {
      return false;
    }
    tocarCor(escolha, 200);
    if (escolha != sequencia[i]) {
      return false;
    }
  }
  return true;
}

void fimDeJogo() {
  Serial.print("Fim de jogo. Voce chegou na fase ");
  Serial.println(fase);
  for (int c = 0; c < 4; c++) {
    digitalWrite(leds[c], HIGH);
  }
  tone(BUZZER, 1480, 200);
  delay(250);
  tone(BUZZER, 1245, 200);
  delay(250);
  tone(BUZZER, 1048, 500);
  delay(800);
  for (int c = 0; c < 4; c++) {
    digitalWrite(leds[c], LOW);
  }
  delay(1500);
  fase = 0;
}

void setup() {
  Serial.begin(115200);
  for (int c = 0; c < 4; c++) {
    pinMode(leds[c], OUTPUT);
    pinMode(botoes[c], INPUT_PULLUP);
  }
  pinMode(BUZZER, OUTPUT);
  randomSeed(analogRead(34));
  Serial.println("Jogo da memoria. Repita a sequencia de cores.");
}

void loop() {
  if (fase == 0) {
    piscarTodos(2);
  }

  sequencia[fase] = random(4);

  Serial.print("Fase ");
  Serial.println(fase + 1);

  mostrarSequencia();

  if (!vezDoJogador()) {
    fimDeJogo();
    return;
  }

  piscarTodos(1);
  fase = fase + 1;

  if (fase >= TOTAL_FASES) {
    Serial.println("Voce venceu todas as fases!");
    piscarTodos(5);
    fase = 0;
  }

  delay(600);
}
