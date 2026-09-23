#define VERMELHO 15
#define AMARELO  2
#define VERDE    4
#define BOTAO    16
TaskHandle_t semaforo;
// TASK 1 - CONTROLA O SEMÁFORO
void tarefaSemaforo(void *parametro) {

  int estado = 0;

  while (true) {
    // Apaga todas as cores
    digitalWrite(VERMELHO, LOW);
    digitalWrite(AMARELO, LOW);
    digitalWrite(VERDE, LOW);

    if (estado == 0) digitalWrite(VERDE, HIGH); // Acende a cor atual
    else if (estado == 1) digitalWrite(AMARELO, HIGH);
    else digitalWrite(VERMELHO, HIGH);

    // Espera: no máximo 5 segundos OU até receber uma notificação do botão
    ulTaskNotifyTake(
      pdTRUE,
      pdMS_TO_TICKS(5000)
    );

    estado++; // Passa para a próxima cor

    if (estado > 2) estado = 0;
  }
}


// TASK 2 - MONITORA O BOTÃO
void tarefaBotao(void *parametro) {

  while (true) {

    if (digitalRead(BOTAO) == LOW) {

      xTaskNotifyGive(semaforo); // Avisa a tarefa do semáforo

      // Evita vários cliques enquanto o botão continua pressionado
      while (digitalRead(BOTAO) == LOW) {
        vTaskDelay(pdMS_TO_TICKS(10));
      }
    }

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void setup() {

  pinMode(VERMELHO, OUTPUT);
  pinMode(AMARELO, OUTPUT);
  pinMode(VERDE, OUTPUT);

  pinMode(BOTAO, INPUT_PULLUP);

  xTaskCreate(tarefaSemaforo,"Semaforo", 2048, NULL, 1, &semaforo);   // Cria a Task do semáforo
  xTaskCreate(tarefaBotao, "Botao", 2048, NULL, 1,NULL);   // Cria a Task do botão
}

// nao utiliza mas precisamos da estrutura para compilar no arduino ide!!
void loop() {
}
