# Dia 2 — Várias coisas ao mesmo tempo

<p class="resumo-inicial">No Dia 1 o <code>delay()</code> resolvia tudo — e travava tudo.
Enquanto a placa contava um segundo, ela não fazia mais nada: não lia botão, não acendia
outro LED, não respondia. Hoje a gente quebra essa limitação. A ESP32 vai executar
<strong>várias tarefas ao mesmo tempo</strong>, cada uma no seu ritmo, e ainda vai aprender
a conversar com um display usando apenas dois fios.</p>

## :material-presentation: Slides da aula

<div class="moldura-slide" markdown="0">
<iframe src="https://www.canva.com/design/DAHVxEQYMtE/fcJ3gOfnEVFt4LqSGBRvYw/view?embed"
        loading="lazy" allowfullscreen="allowfullscreen" allow="fullscreen"></iframe>
</div>

[Abrir em tela cheia](https://www.canva.com/design/DAHVxEQYMtE/fcJ3gOfnEVFt4LqSGBRvYw/view){ .baixar target="_blank" }

<small>*SEMUNI — Aula 02*, por Gabriela de Oliveira Lemos.</small>

## :material-clock-outline: Como o dia se organiza

<div class="info-cartoes" markdown="0"><div class="info"><div class="rotulo">14h00 – 14h45</div><div class="valor">Concorrência</div><div class="detalhe">tarefas independentes e FreeRTOS</div></div><div class="info"><div class="rotulo">14h45 – 15h30</div><div class="valor">Prática</div><div class="detalhe">LED e buzzer em tarefas separadas</div></div><div class="info"><div class="rotulo">15h30 – 15h45</div><div class="valor">Intervalo</div></div><div class="info"><div class="rotulo">15h45 – 17h00</div><div class="valor">I²C e OLED</div><div class="detalhe">display e exibição de informações</div></div></div>

## :material-lightbulb: O que você vai aprender

<ul class="lista-icones">
<li>Por que o <code>delay()</code> trava a placa inteira</li>
<li>O que é uma <strong>tarefa</strong> e como o FreeRTOS executa várias em paralelo</li>
<li>Criar tarefas com <code>xTaskCreate</code> e pausá-las com <code>vTaskDelay</code></li>
<li>Fazer uma tarefa <strong>avisar</strong> a outra, com notificações</li>
<li>O protocolo <strong>I²C</strong>: dois chips conversando com dois fios</li>
<li>Escrever texto num display <strong>OLED</strong></li>
</ul>

## :material-play-circle: Veja funcionando

<div class="video-demo" markdown="0">
<video controls preload="metadata" poster="../assets/dia2-semaforo-capa.jpg">
<source src="../assets/dia2-semaforo.mp4" type="video/mp4">
Seu navegador não reproduz vídeo. <a href="../assets/dia2-semaforo.mp4">Baixe o arquivo</a>.
</video>
<div class="legenda-video">
<span class="rotulo">Demonstração</span>
<h3>O semáforo respondendo ao botão</h3>
<p>Repare no que acontece quando o botão é apertado: o semáforo <strong>não espera</strong>
os cinco segundos terminarem. Ele troca de cor na hora.</p>
<p>Isso é o que a concorrência resolve. No Dia 1, um <code>delay()</code> travava a placa
inteira e nada mais acontecia enquanto ele contava. Aqui, a tarefa do botão continua viva o
tempo todo, em paralelo, e avisa a tarefa do semáforo assim que alguém aperta.</p>
<p>Com o buzzer ligado, uma terceira tarefa entra em cena e apita enquanto o sinal está
vermelho — sem atrapalhar as outras duas.</p>
</div>
</div>

## :material-numeric-1-circle: Semáforo que obedece ao botão

Duas tarefas rodando ao mesmo tempo. A primeira cuida das cores; a segunda fica só olhando
o botão. Quando alguém aperta, a tarefa do botão **avisa** a do semáforo, que troca de cor
imediatamente em vez de esperar os cinco segundos.

O `ulTaskNotifyTake` é o que torna isso possível: ele espera **até 5 segundos, ou até
receber um aviso** — o que vier primeiro.

```cpp title="ex4_semaforo_botao.ino"
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
```

[Baixar ex4_semaforo_botao.ino](../codigo/ex4_semaforo_botao/ex4_semaforo_botao.ino){ .baixar download }

??? question "Por que o `loop()` está vazio?"
    Porque quem faz o trabalho agora são as tarefas, criadas no `setup()`. Elas rodam por
    conta própria, gerenciadas pelo FreeRTOS. O `loop()` continua ali só porque a Arduino
    IDE **exige** essa função para compilar — é estrutura obrigatória, não código morto.

??? question "O que é `vTaskDelay` e por que não usar `delay()`?"
    O `delay()` segura o processador. O `vTaskDelay()` avisa ao sistema *"pode usar esse
    tempo com outra tarefa"*. É essa diferença que faz o botão continuar respondendo
    enquanto o semáforo conta os segundos.

## :material-numeric-2-circle: Um buzzer para quem atravessa

Agora são **três** tarefas. A nova toca o buzzer em bipes curtos enquanto o sinal está
vermelho — o mesmo som dos semáforos com acessibilidade para pessoas cegas.

As tarefas se coordenam por uma variável compartilhada, a `podeAtravessar`. Ela é marcada
como `volatile` justamente porque é escrita por uma tarefa e lida por outra.

```cpp title="ex5_semaforo_buzzer.ino"
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
```

[Baixar ex5_semaforo_buzzer.ino](../codigo/ex5_semaforo_buzzer/ex5_semaforo_buzzer.ino){ .baixar download }

??? question "Por que `volatile`?"
    Sem isso, o compilador pode "otimizar" a leitura da variável, guardando o valor antigo
    num registrador por achar que ninguém mais a modifica. O `volatile` avisa que o valor
    pode mudar a qualquer momento, vindo de fora — e obriga a reler da memória sempre.

## :material-chip: Falando com o display

O display OLED não usa um pino por informação. Ele conversa por **I²C**, um protocolo em
que dois fios bastam: um de **dados** (SDA) e um de **relógio** (SCL). Cada dispositivo no
barramento tem um endereço; o nosso display é o `0x3C`.

| Pino do display | Vai para |
|---|---|
| SDA (dados) | **GPIO 21** |
| SCL (relógio) | **GPIO 22** |
| VCC | 3V3 |
| GND | GND |

!!! tip "Por que 21 e 22?"
    São os pinos de I²C **padrão da ESP32**. Quase todo tutorial e todo módulo assume
    esses dois, então usar o padrão evita confusão na hora de procurar ajuda. Nada impede
    usar outros pinos — o `Wire.begin(PINO_SDA, PINO_SCL)` aceita qualquer par —, mas aí é
    preciso lembrar de trocar também no código.

Os dois exercícios a seguir precisam das bibliotecas **Adafruit SSD1306** e
**Adafruit GFX**, instaladas pelo Gerenciador de Bibliotecas da Arduino IDE.

## :material-numeric-3-circle: Hello, world! no display

O primeiro texto na tela. `clearDisplay()` limpa o rascunho, os comandos de texto desenham
nele, e `display()` é o que **envia tudo de uma vez** para a tela.

```cpp title="ex6_oled_helloworld.ino"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define LARGURA 128    // largura do display em pixels
#define ALTURA 64      // altura do display em pixels
#define OLED_RESET -1  // nosso display nao tem pino de reset
#define ENDERECO 0x3C  // endereço I2C do nosso display

#define PINO_SDA 21  // dados
#define PINO_SCL 22  // clock

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
```

[Baixar ex6_oled_helloworld.ino](../codigo/ex6_oled_helloworld/ex6_oled_helloworld.ino){ .baixar download }

??? question "Por que precisa chamar `display()` no fim?"
    Porque tudo que você desenha vai para um rascunho na memória da ESP32, não direto para
    a tela. O `display()` despeja esse rascunho no OLED. Sem ele, você desenha e nada
    aparece — é o erro mais comum de quem está começando com esse display.

## :material-numeric-4-circle: Mandar mensagens para o display

Agora o display mostra o que você digitar no Monitor Serial. É a primeira vez na oficina
que o computador e a placa **conversam nos dois sentidos**.

```cpp title="ex7_oled_mensagem.ino"
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
```

[Baixar ex7_oled_mensagem.ino](../codigo/ex7_oled_mensagem/ex7_oled_mensagem.ino){ .baixar download }

??? question "Para que serve o `trim()`?"
    Quando você aperta Enter no Monitor Serial, junto do texto vem um caractere invisível
    de fim de linha. O `trim()` remove esses restos das pontas — sem ele, o display recebe
    um espaço fantasma no fim da mensagem.

## :material-help-circle: Se algo não funcionar

| Sintoma | Causa mais comum |
|---|---|
| `OLED nao encontrado!` no serial | fios de SDA e SCL trocados, ou endereço diferente de `0x3C` |
| Display aceso mas em branco | faltou chamar `display.display()` no fim |
| Semáforo ignora o botão | botão sem `INPUT_PULLUP`, ou ligado ao 3V3 em vez do GND |
| Placa reinicia sozinha | pilha da tarefa pequena demais no `xTaskCreate` |
| Buzzer mudo | buzzer ativo precisa de `digitalWrite`; passivo precisa de `tone()` |
