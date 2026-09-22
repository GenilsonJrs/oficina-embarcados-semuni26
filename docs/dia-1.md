# Dia 1 — O primeiro sinal

No primeiro encontro a placa sai da caixa e faz a primeira coisa visivel: **acender uma
luz**. Parece pouco, mas e o momento em que o codigo deixa a tela e vira eletricidade.

A ideia do dia inteiro cabe numa frase: **um pino so tem dois estados, e voce decide qual.**
Tudo que vem depois — motor, sensor, display, rede — e elaboracao em cima disso.

## O que voce vai aprender

| Conceito | Onde aparece |
|---|---|
| O que e um sistema embarcado | abertura |
| GPIO: o pino como saida | acender o LED |
| Tempo e `delay()` | fazer o LED piscar |
| Varias saidas coordenadas | o semaforo |
| GPIO como entrada, e `INPUT_PULLUP` | o botao |

## Antes de comecar

Voce vai precisar da **Arduino IDE** com suporte a ESP32 instalado. Se ainda nao tem:

1. Baixe a Arduino IDE em [arduino.cc/en/software](https://www.arduino.cc/en/software)
2. Abra **Arquivo → Preferencias** e, em *URLs adicionais para Gerenciadores de Placas*, cole:
   `https://espressif.github.io/arduino-esp32/package_esp32_index.json`
3. Va em **Ferramentas → Placa → Gerenciador de Placas**, procure por **esp32** e instale
4. Selecione a sua placa em **Ferramentas → Placa** e a porta em **Ferramentas → Porta**

!!! tip "Nao precisa instalar nada antes de vir"
    Se algo der errado na instalacao, tudo bem — a primeira meia hora da oficina e
    justamente para deixar todo mundo com o ambiente funcionando.

## Slides da aula

<div class="moldura-slide" markdown="0">
<iframe src="https://canva.link/z4i35ryhj6hrqdg" loading="lazy" allowfullscreen></iframe>
</div>

[Abrir os slides em tela cheia](https://canva.link/z4i35ryhj6hrqdg){ .baixar target="_blank" }

## Pratica 1 — Piscar um LED

O "ola, mundo" dos sistemas embarcados. O LED acende por um segundo, apaga por um segundo,
e repete para sempre.

O `setup()` roda **uma vez**, quando a placa liga: e onde se diz que o pino sera usado como
saida. O `loop()` roda **para sempre**, do comeco ao fim, repetidamente.

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
    nao faz mais nada. Isso e simples e funciona bem aqui, mas e a primeira limitacao que
    voce vai querer superar quando o projeto precisar fazer duas coisas ao mesmo tempo.

## Pratica 2 — Controlar um semaforo

Agora sao tres LEDs em vez de um. Nada de novo em conceito: e o mesmo `digitalWrite`, so
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

??? question "Da para mudar o tempo de cada cor?"
    Da, e e um bom exercicio. Um semaforo de verdade nao deixa as tres cores acesas pelo
    mesmo tempo: o amarelo e sempre bem mais curto. Troque os numeros dentro dos `delay()`
    e veja.

## Pratica 3 — Acender um LED com botao

Ate aqui o pino so mandou sinal para fora. Agora ele **le** o mundo: o programa pergunta se
o botao esta apertado e decide o que fazer.

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

??? question "Por que apertado e LOW, e nao HIGH?"
    Por causa do `INPUT_PULLUP`. A placa liga um resistor interno que mantem o pino em
    **nivel alto** quando nada acontece. O botao, quando apertado, conecta esse pino ao
    **terra** — ou seja, puxa para **nivel baixo**.

    Parece invertido, mas e o padrao em eletronica digital, e tem uma vantagem: sem o
    pull-up, um pino solto fica "no ar" e le valores aleatorios.

## Se algo nao funcionar

| Sintoma | Causa mais comum |
|---|---|
| A IDE nao acha a placa | driver USB faltando, ou cabo que so carrega e nao transmite dados |
| Trava em `Connecting...` | segure o botao **BOOT** da placa ate a gravacao comecar |
| O LED nao acende | perna curta do LED (catodo) precisa ir ao **GND**, e sempre com resistor |
| Compila mas nada acontece | numero do pino diferente do que esta ligado na protoboard |

!!! note "Sobre os numeros dos pinos"
    Os pinos 48, 47, 21 e 42 usados aqui sao os da placa que vamos usar na oficina. Se voce
    testar em casa com outra ESP32, confira a pinagem da **sua** placa — os numeros mudam
    de modelo para modelo, e esse e um dos erros mais comuns de quem esta comecando.
