#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define LARGURA 128
#define ALTURA 64     
#define OLED_RESET -1 
#define ENDERECO 0x3C 

#define PINO_SDA 21  // dados
#define PINO_SCL 22  // clock

Adafruit_SSD1306 display(LARGURA, ALTURA, &Wire, OLED_RESET);

String mensagem = "Aguardando mensagem...";

void mostrarMensagem() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print(mensagem);  // quebra de linha automatica quando o texto nao cabe
  display.display();
}

void setup() {
  Serial.begin(115200);
  Wire.begin(PINO_SDA, PINO_SCL);

  if (!display.begin(SSD1306_SWITCHCAPVCC, ENDERECO, true, false)) {
    Serial.println("OLED nao encontrado!");
    while (true)
      ;
  }

  mostrarMensagem();
  Serial.println("Pronto! Digite uma mensagem:");
}

void loop() {
  if (Serial.available()) {                   // chegou algo pela serial?
    mensagem = Serial.readStringUntil('\n');  // le ate o Enter
    mensagem.trim();                          // tira espacos e \r do final

    Serial.print("Recebido: ");
    Serial.println(mensagem);

    mostrarMensagem();
  }
}
