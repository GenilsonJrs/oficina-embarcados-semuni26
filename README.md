# Oficina de Introdução aos Sistemas Embarcados

**SEMUNI 2026 · FCTE — Campus UnB Gama**

Sistema embarcado é um computador que vive dentro de outra coisa: o carro, a geladeira, o
equipamento médico, o semáforo da esquina. Ele não tem tela nem teclado — tem **sensores**
para perceber o mundo e **atuadores** para agir sobre ele.

Em três tardes, os participantes programam um ESP32 e veem o efeito acontecer na frente
deles. Nada de simulação: a placa está na mesa, o LED acende de verdade e o motor gira de
verdade. A oficina é **gratuita**, aberta ao público e não exige conhecimento prévio nem
equipamento próprio.

### 🌐 [Acesse o site da oficina](https://genilsonjrs.github.io/oficina-embarcados-semuni26/)

---

## Slides do Dia 1

[![Slides do Dia 1](docs/assets/aula1-semaforo.jpg)](https://www.canva.com/design/DAHUb_scRPU/oeNBIgei-TgZdYx3fND73w/view)

**[▶ Abrir os slides do Dia 1](https://www.canva.com/design/DAHUb_scRPU/oeNBIgei-TgZdYx3fND73w/view)** — *SEMUNI — Aula 01*, por Gabriela de Oliveira Lemos.

> O GitHub não permite conteúdo incorporado no README. No
> [site](https://genilsonjrs.github.io/oficina-embarcados-semuni26/dia-1/) os slides abrem
> na própria página.

---

## Os três dias

| Dia | Data | Tema | Conteúdo |
|---|---|---|---|
| **1** | 22/09 · terça | O primeiro sinal | O que é um sistema embarcado, ambiente de desenvolvimento e GPIO: piscar um LED, controlar um semáforo e ler um botão |
| **2** | 23/09 · quarta | Várias coisas ao mesmo tempo | Concorrência e noções de FreeRTOS; comunicação por I²C e display OLED |
| **3** | 24/09 · quinta | Colocando em movimento | PWM, ponte H e acionamento de motores; desafio final em equipes |

Todos os encontros das **14h às 17h**, no **Laboratório MOCAP** — FCTE, Campus UnB Gama.
Carga horária total de **9 horas**, com **30 vagas**, metade reservada prioritariamente a
mulheres.

---

## Equipe

| | Pessoa | Papel |
|---|---|---|
| <img src="https://github.com/renatocoral.png" width="46"> | **Renato Coral Sampaio** | Coordenação — docente FCTE/UnB |
| <img src="https://github.com/heylisten64.png" width="46"> | **Gabriela de Oliveira Lemos** | Ministrante — [@heylisten64](https://github.com/heylisten64) |
| <img src="https://github.com/GenilsonJrs.png" width="46"> | **Genilson Junior** | Monitoria — [@GenilsonJrs](https://github.com/GenilsonJrs) |
| <img src="https://github.com/luizfaria1989.png" width="46"> | **Luiz Guilherme Faria** | Monitoria — [@luizfaria1989](https://github.com/luizfaria1989) |

---

## O que tem neste repositório

| Pasta | Conteúdo |
|---|---|
| `docs/` | as páginas do site |
| `docs/codigo/` | todos os sketches `.ino`, prontos para abrir na Arduino IDE |
| `docs/assets/` | fotos dos projetos e imagens do site |

Os exercícios do Dia 1 são o [`ex1_piscaled`](docs/codigo/ex1_piscaled),
o [`ex2_semaforo`](docs/codigo/ex2_semaforo) e o [`ex3_botao`](docs/codigo/ex3_botao).
Os projetos de mostra — carrinho Wi-Fi, casa na chuva, radar e jogo da memória — estão em
[`docs/codigo`](docs/codigo) e explicados
[no site](https://genilsonjrs.github.io/oficina-embarcados-semuni26/projetos/oficina/).

## Rodar o site localmente

```bash
pip install -r requirements.txt
mkdocs serve
```
