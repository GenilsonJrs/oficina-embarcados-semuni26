#define RED 48
#define YELLOW 47
#define GREEN 21

void setup() {
  pinMode(RED, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(GREEN, OUTPUT);
}

void loop() {
  digitalWrite(RED, HIGH);
  delay(1000);
  digitalWrite(RED, LOW);

  digitalWrite(GREEN, HIGH);
  delay(1000);
  digitalWrite(GREEN, LOW);

  digitalWrite(YELLOW, HIGH);
  delay(1000);
  digitalWrite(YELLOW, LOW);
}
