#define VERMELHO 15
#define AMARELO  2
#define VERDE    4
#define BOTAO    16
#define BUZZER   17

TaskHandle_t semaforo;

volatile bool podeAtravessar = false;

// TASK 1 - CONTROLA O SEMÁFORO
void tarefaSemaforo(void *parametro) {

  int estado = 0;

  while (true) {

    // Apaga todas as cores
    digitalWrite(VERMELHO, LOW);
    digitalWrite(AMARELO, LOW);
    digitalWrite(VERDE, LOW);

    // Acende a cor atual
    if (estado == 0) {

      digitalWrite(VERDE, HIGH);
      podeAtravessar = false;

    } else if (estado == 1) {

      digitalWrite(AMARELO, HIGH);
      podeAtravessar = false;

    } else {

      digitalWrite(VERMELHO, HIGH);
      podeAtravessar = true;
    }

    // Espera no máximo 5 segundos OU até receber uma notificação do botão
    ulTaskNotifyTake(
      pdTRUE,
      pdMS_TO_TICKS(5000)
    );

    estado++;  // Passa para a próxima cor

    if (estado > 2) {
      estado = 0;
    }
  }
}


// TASK 2 - MONITORA O BOTÃO
void tarefaBotao(void *parametro) {

  while (true) {

    if (digitalRead(BOTAO) == LOW) {

      xTaskNotifyGive(semaforo); // Avisa a Task do semáforo

      // Espera o botão ser solto para não registrar vários cliques
      while (digitalRead(BOTAO) == LOW) {

        vTaskDelay(pdMS_TO_TICKS(10));
      }
    }

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}


// TASK 3 - CONTROLA O BUZZER
void tarefaBuzzer(void *parametro) {

  while (true) {

    if (podeAtravessar) {

      // BIP
      digitalWrite(BUZZER, HIGH);
      vTaskDelay(pdMS_TO_TICKS(200));

      // PAUSA
      digitalWrite(BUZZER, LOW);
      vTaskDelay(pdMS_TO_TICKS(200));

    } else {

      digitalWrite(BUZZER, LOW); // Fora do vermelho, fica desligado

      vTaskDelay(pdMS_TO_TICKS(50));
    }
  }
}


void setup() {

  pinMode(VERMELHO, OUTPUT);
  pinMode(AMARELO, OUTPUT);
  pinMode(VERDE, OUTPUT);

  pinMode(BOTAO, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);

  xTaskCreate(tarefaSemaforo,"Semaforo", 2048, NULL, 1, &semaforo);   // Cria a Task do semáforo
  xTaskCreate(tarefaBotao, "Botao", 2048, NULL, 1, NULL);  // Cria a Task do botão
  xTaskCreate(tarefaBuzzer, "Buzzer", 2048, NULL, 1, NULL); // Cria a Task do buzzer
}


// nao usamos mas precisamos pra compilar no arduino ide
void loop() {

}
