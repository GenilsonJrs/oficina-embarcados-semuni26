# Dia 1 — O primeiro sinal

No primeiro encontro a placa sai da caixa e faz a primeira coisa visível: **acender uma
luz**. Parece pouco, mas é o momento em que o código deixa a tela e vira eletricidade.

A ideia do dia inteiro cabe numa frase: **um pino só tem dois estados, e você decide qual.**
Tudo que vem depois — motor, sensor, display, rede — é elaboração em cima disso.

## Programação do dia

| Horário | O que acontece |
|---|---|
| 14h00 – 14h40 | Abertura, apresentação da equipe e da oficina. O que são sistemas embarcados e onde eles estão: mostra de projetos e aplicações reais |
| 14h40 – 15h20 | Microcontroladores e a placa ESP32. Configuração do ambiente de desenvolvimento e estrutura de um projeto embarcado |
| 15h20 – 15h35 | Intervalo |
| 15h35 – 17h00 | **Prática:** GPIO. Primeiro programa, leitura de botão e montagem do circuito em protoboard |

## O que você vai aprender

| Conceito | Onde aparece |
|---|---|
| O que é um sistema embarcado | abertura |
| GPIO: o pino como saída | acender o LED |
| Tempo e `delay()` | fazer o LED piscar |
| Várias saídas coordenadas | o semáforo |
| GPIO como entrada, e `INPUT_PULLUP` | o botão |

## Antes de começar

Você vai precisar da **Arduino IDE** com suporte a ESP32 instalado. Se ainda não tem:

1. Baixe a Arduino IDE em [arduino.cc/en/software](https://www.arduino.cc/en/software)
2. Abra **Arquivo → Preferências** e, em *URLs adicionais para Gerenciadores de Placas*, cole:
   `https://espressif.github.io/arduino-esp32/package_esp32_index.json`
3. Vá em **Ferramentas → Placa → Gerenciador de Placas**, procure por **esp32** e instale
4. Selecione a sua placa em **Ferramentas → Placa** e a porta em **Ferramentas → Porta**

!!! tip "Não precisa instalar nada antes de vir"
    Se algo der errado na instalação, tudo bem — a primeira meia hora da oficina é
    justamente para deixar todo mundo com o ambiente funcionando.

## Slides da aula

<div class="moldura-slide" markdown="0">
<iframe src="https://www.canva.com/design/DAHUb_scRPU/oeNBIgei-TgZdYx3fND73w/view?embed"
        loading="lazy" allowfullscreen="allowfullscreen" allow="fullscreen"></iframe>
</div>

[Abrir os slides em tela cheia](https://www.canva.com/design/DAHUb_scRPU/oeNBIgei-TgZdYx3fND73w/view){ .baixar target="_blank" }

<small>*SEMUNI — Aula 01*, por Gabriela de Oliveira Lemos.</small>

## Prática 1 — Piscar um LED

O "olá, mundo" dos sistemas embarcados. O LED acende por um segundo, apaga por um segundo,
e repete para sempre.

O `setup()` roda **uma vez**, quando a placa liga: é onde se diz que o pino será usado como
saída. O `loop()` roda **para sempre**, do começo ao fim, repetidamente.

```cpp title="ex1_piscaled.ino"
#define LED 2

void setup() {
  pinMode(LED, OUTPUT);
}

void loop() {
  digitalWrite(LED, HIGH);
  delay(1000);
  digitalWrite(LED, LOW);
  delay(1000);
}
```

[Baixar ex1_piscaled.ino](codigo/ex1_piscaled/ex1_piscaled.ino){ .baixar download }

??? question "Por que o LED fica aceso exatamente 1 segundo?"
    Porque `delay(1000)` **para tudo** por 1000 milissegundos. Enquanto ele espera, a placa
    não faz mais nada. Isso é simples e funciona bem aqui, mas é a primeira limitação que
    você vai querer superar quando o projeto precisar fazer duas coisas ao mesmo tempo —
    que é exatamente o assunto do Dia 2.

## Prática 2 — Controlar um semáforo

Agora são três LEDs em vez de um. Nada de novo em conceito: é o mesmo `digitalWrite`, só
que repetido para cada cor, na ordem certa.

```cpp title="ex2_semaforo.ino"
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
```

[Baixar ex2_semaforo.ino](codigo/ex2_semaforo/ex2_semaforo.ino){ .baixar download }

??? question "Dá para mudar o tempo de cada cor?"
    Dá, e é um bom exercício. Um semáforo de verdade não deixa as três cores acesas pelo
    mesmo tempo: o amarelo é sempre bem mais curto. Troque os números dentro dos `delay()`
    e veja.

## O circuito montado

Ao final do dia a sua protoboard vai estar parecida com esta: a ESP32 no centro, o módulo
de semáforo com os três LEDs, e o botão numa protoboard menor ao lado.

![Circuito do dia 1 montado na protoboard](assets/aula1-semaforo.jpg){ .foto-projeto }

## Prática 3 — Acender um LED com botão

Até aqui o pino só mandou sinal para fora. Agora ele **lê** o mundo: o programa pergunta se
o botão está apertado e decide o que fazer.

```cpp title="ex3_botao.ino"
#define LED 2
#define BOTAO 42

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(BOTAO, INPUT_PULLUP);
}

void loop() {
  if (digitalRead(BOTAO) == LOW) {
    digitalWrite(LED, HIGH);
  } else {
    digitalWrite(LED, LOW);
  }
}
```

[Baixar ex3_botao.ino](codigo/ex3_botao/ex3_botao.ino){ .baixar download }

??? question "Por que apertado é LOW, e não HIGH?"
    Por causa do `INPUT_PULLUP`. A placa liga um resistor interno que mantém o pino em
    **nível alto** quando nada acontece. O botão, quando apertado, conecta esse pino ao
    **terra** — ou seja, puxa para **nível baixo**.

    Parece invertido, mas é o padrão em eletrônica digital, e tem uma vantagem: sem o
    pull-up, um pino solto fica "no ar" e lê valores aleatórios.

## Se algo não funcionar

| Sintoma | Causa mais comum |
|---|---|
| A IDE não acha a placa | driver USB faltando, ou cabo que só carrega e não transmite dados |
| Trava em `Connecting...` | segure o botão **BOOT** da placa até a gravação começar |
| O LED não acende | perna curta do LED (catodo) precisa ir ao **GND**, e sempre com resistor |
| Compila mas nada acontece | número do pino diferente do que está ligado na protoboard |

!!! note "Sobre os números dos pinos"
    Os pinos 48, 47, 21 e 42 usados aqui são os da placa que vamos usar na oficina. Se você
    testar em casa com outra ESP32, confira a pinagem da **sua** placa — os números mudam
    de modelo para modelo, e esse é um dos erros mais comuns de quem está começando.
