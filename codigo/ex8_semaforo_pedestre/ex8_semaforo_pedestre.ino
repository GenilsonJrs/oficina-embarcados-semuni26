#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const int PINO_VERMELHO = 15;
const int PINO_AMARELO = 2;
const int PINO_VERDE = 4;
const int PINO_BOTAO = 16;
const int PINO_BUZZER = 17;
const int PINO_SDA = 21;
const int PINO_SCL = 22;

const int BUZZER_LIGADO = HIGH;
const int BUZZER_DESLIGADO = LOW;

const int CONTAGEM = 3;
const int TEMPO_TRAVESSIA = 5;

Adafruit_SSD1306 tela(128, 64, &Wire, -1);

void acender(int vermelho, int amarelo, int verde) {
  digitalWrite(PINO_VERMELHO, vermelho);
  digitalWrite(PINO_AMARELO, amarelo);
  digitalWrite(PINO_VERDE, verde);
}

void bipar(int duracao, int pausa) {
  digitalWrite(PINO_BUZZER, BUZZER_LIGADO);
  delay(duracao);
  digitalWrite(PINO_BUZZER, BUZZER_DESLIGADO);
  delay(pausa);
}

void escreverCentralizado(String texto, int tamanho, int y) {
  int largura = texto.length() * 6 * tamanho;
  tela.setTextSize(tamanho);
  tela.setCursor((128 - largura) / 2, y);
  tela.print(texto);
}

void mostrar(String linha1, String linha2) {
  tela.clearDisplay();
  escreverCentralizado(linha1, 3, 8);
  escreverCentralizado(linha2, 2, 44);
  tela.display();
}

void mostrarNumero(int numero) {
  tela.clearDisplay();
  escreverCentralizado(String(numero), 6, 8);
  tela.display();
}

void contagemRegressiva() {
  for (int numero = CONTAGEM; numero > 0; numero--) {
    mostrarNumero(numero);
    bipar(100, 900);
  }
}

bool botaoApertado() {
  if (digitalRead(PINO_BOTAO) == LOW) {
    delay(50);
    return digitalRead(PINO_BOTAO) == LOW;
  }
  return false;
}

void esperarSoltarBotao() {
  while (digitalRead(PINO_BOTAO) == LOW) {
    delay(10);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(PINO_VERMELHO, OUTPUT);
  pinMode(PINO_AMARELO, OUTPUT);
  pinMode(PINO_VERDE, OUTPUT);
  pinMode(PINO_BUZZER, OUTPUT);
  pinMode(PINO_BOTAO, INPUT_PULLUP);
  digitalWrite(PINO_BUZZER, BUZZER_DESLIGADO);

  Wire.begin(PINO_SDA, PINO_SCL);
  if (!tela.begin(SSD1306_SWITCHCAPVCC, 0x3C, true, false)) {
    Serial.println("OLED nao encontrado!");
    while (true) {
      delay(1000);
    }
  }
  tela.setTextColor(SSD1306_WHITE);
  tela.setTextWrap(false);

  acender(LOW, LOW, HIGH);
  mostrar("PARE", "PEDESTRE");
}

void loop() {
  if (botaoApertado()) {
    acender(LOW, HIGH, LOW);
    contagemRegressiva();

    acender(HIGH, LOW, LOW);
    mostrar("PODE", "ATRAVESSAR");
    for (int i = 0; i < TEMPO_TRAVESSIA; i++) {
      bipar(200, 800);
    }

    contagemRegressiva();

    acender(LOW, LOW, HIGH);
    mostrar("PARE", "PEDESTRE");
    esperarSoltarBotao();
  }
}
