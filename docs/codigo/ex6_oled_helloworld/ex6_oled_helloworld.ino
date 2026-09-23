#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define LARGURA 128    // largura do display em pixels
#define ALTURA 64      // altura do display em pixels
#define OLED_RESET -1  // nosso display nao tem pino de reset
#define ENDERECO 0x3C  // endereço I2C do nosso display

#define PINO_SDA 19  // dados
#define PINO_SCL 18  // clock

Adafruit_SSD1306 display(LARGURA, ALTURA, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);
  Wire.begin(PINO_SDA, PINO_SCL);  // SDA, SCL

  if (!display.begin(SSD1306_SWITCHCAPVCC, ENDERECO, true, false)) {
    Serial.println("OLED nao encontrado!");
    while (true);
  }
}

void loop() {
  display.clearDisplay();
  display.setTextSize(1);  // 1 = caracteres de 6x8 pixels
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Hello, world!");
  display.display(); 
}
