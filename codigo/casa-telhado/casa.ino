const int PINO_DO_SERVO = 13;
const int PINO_DO_SENSOR = 34;

const int ANGULO_SEM_CHUVA = 62;
const int ANGULO_COM_CHUVA = 0;

const int LIMIAR_CHUVA = 3600;
const int LIMIAR_SECO = 3950;

const unsigned long ESPERA_PARA_ABRIR_MS = 10000;
const int MS_POR_GRAU = 40;

const int PULSO_MINIMO = 500;
const int PULSO_MAXIMO = 2400;
const int FREQUENCIA = 50;
const int RESOLUCAO = 16;
const int AMOSTRAS = 15;
const int CONFIRMACOES = 5;
const unsigned long INTERVALO_DA_LEITURA_MS = 200;

int anguloAtual = -1;
int votosMolhado = 0;
int votosSeco = 0;
bool estaChovendo = false;
bool naPosicaoDeChuva = false;
unsigned long secouDesde = 0;
unsigned long ultimaLeitura = 0;

int dutyDoAngulo(int angulo) {
  long pulso = map(angulo, 0, 180, PULSO_MINIMO, PULSO_MAXIMO);
  long periodo = 1000000L / FREQUENCIA;
  return pulso * ((1L << RESOLUCAO) - 1) / periodo;
}

int dentroDoCurso(int angulo) {
  int menor = min(ANGULO_SEM_CHUVA, ANGULO_COM_CHUVA);
  int maior = max(ANGULO_SEM_CHUVA, ANGULO_COM_CHUVA);
  if (angulo < menor) {
    return menor;
  }
  if (angulo > maior) {
    return maior;
  }
  return angulo;
}

void moverPara(int alvo) {
  alvo = dentroDoCurso(alvo);
  if (anguloAtual < 0) {
    anguloAtual = alvo;
    ledcWrite(PINO_DO_SERVO, dutyDoAngulo(alvo));
    return;
  }
  int passo = (alvo > anguloAtual) ? 1 : -1;
  while (anguloAtual != alvo) {
    anguloAtual += passo;
    ledcWrite(PINO_DO_SERVO, dutyDoAngulo(anguloAtual));
    delay(MS_POR_GRAU);
  }
}

void reagirAChuva() {
  Serial.print(">> CHUVA detectada - indo para ");
  Serial.print(ANGULO_COM_CHUVA);
  Serial.println(" graus");
  moverPara(ANGULO_COM_CHUVA);
  naPosicaoDeChuva = true;
}

void voltarAoRepouso() {
  Serial.print(">> seco ha 10s - voltando para ");
  Serial.print(ANGULO_SEM_CHUVA);
  Serial.println(" graus");
  moverPara(ANGULO_SEM_CHUVA);
  naPosicaoDeChuva = false;
}

int lerSensor() {
  int v[AMOSTRAS];
  for (int i = 0; i < AMOSTRAS; i++) {
    v[i] = analogRead(PINO_DO_SENSOR);
    delay(2);
  }
  for (int i = 1; i < AMOSTRAS; i++) {
    int chave = v[i];
    int j = i - 1;
    while (j >= 0 && v[j] > chave) {
      v[j + 1] = v[j];
      j--;
    }
    v[j + 1] = chave;
  }
  return v[AMOSTRAS / 2];
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  ledcAttach(PINO_DO_SERVO, FREQUENCIA, RESOLUCAO);
  delay(400);

  Serial.println();
  Serial.println("=== CASA COM TELHADO NA CHUVA ===");
  Serial.print("Sem chuva (repouso): ");
  Serial.print(ANGULO_SEM_CHUVA);
  Serial.print(" graus   Com chuva: ");
  Serial.print(ANGULO_COM_CHUVA);
  Serial.println(" graus");
  Serial.print("Chove abaixo de ");
  Serial.print(LIMIAR_CHUVA);
  Serial.print("   seco acima de ");
  Serial.println(LIMIAR_SECO);
  Serial.print("Espera para reabrir: ");
  Serial.print(ESPERA_PARA_ABRIR_MS / 1000);
  Serial.println(" s");
  Serial.println("=================================");

  anguloAtual = ANGULO_COM_CHUVA;
  ledcWrite(PINO_DO_SERVO, dutyDoAngulo(ANGULO_COM_CHUVA));
  delay(400);
  moverPara(ANGULO_SEM_CHUVA);
  Serial.print("repouso: ");
  Serial.print(ANGULO_SEM_CHUVA);
  Serial.println(" graus");
}

void loop() {
  if (millis() - ultimaLeitura < INTERVALO_DA_LEITURA_MS) {
    return;
  }
  ultimaLeitura = millis();

  int valor = lerSensor();

  if (valor < LIMIAR_CHUVA) {
    votosMolhado++;
    votosSeco = 0;
  } else if (valor > LIMIAR_SECO) {
    votosSeco++;
    votosMolhado = 0;
  }

  if (!estaChovendo && votosMolhado >= CONFIRMACOES) {
    estaChovendo = true;
    Serial.print("sensor ");
    Serial.print(valor);
    Serial.print("  -> molhado (confirmado ");
    Serial.print(CONFIRMACOES);
    Serial.println("x)");
  } else if (estaChovendo && votosSeco >= CONFIRMACOES) {
    estaChovendo = false;
    secouDesde = millis();
    Serial.print("sensor ");
    Serial.print(valor);
    Serial.print("  -> secou (confirmado ");
    Serial.print(CONFIRMACOES);
    Serial.println("x), contando 10s");
  }

  if (estaChovendo && !naPosicaoDeChuva) {
    reagirAChuva();
  }

  if (!estaChovendo && naPosicaoDeChuva) {
    if (millis() - secouDesde >= ESPERA_PARA_ABRIR_MS) {
      voltarAoRepouso();
    }
  }
}
