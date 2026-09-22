int dutyEsquerdaFrente = 0;
int dutyEsquerdaTras = 0;
int dutyDireitaFrente = 0;
int dutyDireitaTras = 0;

void prepararMotores() {
  pinMode(PINO_ESQUERDA_FRENTE, OUTPUT);
  pinMode(PINO_ESQUERDA_TRAS, OUTPUT);
  pinMode(PINO_DIREITA_FRENTE, OUTPUT);
  pinMode(PINO_DIREITA_TRAS, OUTPUT);
}

int forca(int potencia) {
  if (potencia < 0) {
    potencia = 0;
  }
  if (potencia > 100) {
    potencia = 100;
  }
  return map(potencia, 0, 100, 0, 255);
}

void ladoEsquerdo(bool paraFrente, bool paraTras, int potencia) {
  if (INVERTER_LADO_ESQUERDO) {
    bool guarda = paraFrente;
    paraFrente = paraTras;
    paraTras = guarda;
  }
  dutyEsquerdaFrente = paraFrente ? forca(potencia) : 0;
  dutyEsquerdaTras = paraTras ? forca(potencia) : 0;
  analogWrite(PINO_ESQUERDA_FRENTE, dutyEsquerdaFrente);
  analogWrite(PINO_ESQUERDA_TRAS, dutyEsquerdaTras);
}

void ladoDireito(bool paraFrente, bool paraTras, int potencia) {
  if (INVERTER_LADO_DIREITO) {
    bool guarda = paraFrente;
    paraFrente = paraTras;
    paraTras = guarda;
  }
  dutyDireitaFrente = paraFrente ? forca(potencia) : 0;
  dutyDireitaTras = paraTras ? forca(potencia) : 0;
  analogWrite(PINO_DIREITA_FRENTE, dutyDireitaFrente);
  analogWrite(PINO_DIREITA_TRAS, dutyDireitaTras);
}

void frente() {
  ladoEsquerdo(true, false, POTENCIA);
  ladoDireito(true, false, POTENCIA);
}

void tras() {
  ladoEsquerdo(false, true, POTENCIA);
  ladoDireito(false, true, POTENCIA);
}

void esquerda() {
  ladoEsquerdo(false, true, POTENCIA_DA_CURVA);
  ladoDireito(true, false, POTENCIA_DA_CURVA);
}

void direita() {
  ladoEsquerdo(true, false, POTENCIA_DA_CURVA);
  ladoDireito(false, true, POTENCIA_DA_CURVA);
}

void parar() {
  ladoEsquerdo(false, false, 0);
  ladoDireito(false, false, 0);
}

bool executar(String acao) {
  if (acao == "frente") {
    frente();
    return true;
  }
  if (acao == "tras") {
    tras();
    return true;
  }
  if (acao == "esquerda") {
    esquerda();
    return true;
  }
  if (acao == "direita") {
    direita();
    return true;
  }
  if (acao == "parar") {
    parar();
    return true;
  }
  return false;
}
