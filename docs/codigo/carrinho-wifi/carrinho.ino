#include <WiFi.h>
#include <WebServer.h>

const int NUMERO_DO_CARRINHO = 1;
const char SENHA_DA_REDE[] = "semuni2026";
const int MAXIMO_DE_APARELHOS = 4;

char nomeDaRede[16];

int POTENCIA = 85;
int POTENCIA_DA_CURVA = 70;

const bool INVERTER_LADO_ESQUERDO = false;
const bool INVERTER_LADO_DIREITO = true;

const int PINO_ESQUERDA_FRENTE = 27;
const int PINO_ESQUERDA_TRAS = 26;
const int PINO_DIREITA_FRENTE = 25;
const int PINO_DIREITA_TRAS = 33;

const unsigned long PARADA_AUTOMATICA_MS = 1500;
const unsigned long TEMPO_SEM_USO_MS = 20000;

struct Usuario {
  const char *nome;
  const char *usuario;
};

const Usuario USUARIOS[] = {
  {"Ana Beatriz", "ana-beatriz"},
  {"Arthur Fernandes", "arthur-fernandes"},
  {"Arthur Noia", "arthur-noia"},
  {"Beatryz Pina", "beatryz-pina"},
  {"Daniel Abreu", "daniel-abreu"},
  {"Dominic Batista", "dominic-batista"},
  {"Eduardo Teles", "eduardo-teles"},
  {"Gabriel Mesquita", "gabriel-mesquita"},
  {"Gabriela Lemos", "gabriela-lemos"},
  {"Genilson Junior", "genilson-junior"},
  {"Graziele Alves", "graziele-alves"},
  {"Isadora Oliveira", "isadora-oliveira"},
  {"Joao Felipe Rosa", "joao-felipe-rosa"},
  {"Joao Gabryel", "joao-gabryel"},
  {"Joao Lucas Ramos", "joao-lucas-ramos"},
  {"Joao Vithor C.", "joao-vithor-c"},
  {"Joao Vitor B.", "joao-vitor-b"},
  {"Julia Amanda", "julia-amanda"},
  {"Laura Feitosa", "laura-feitosa"},
  {"Lorena Alves", "lorena-alves"},
  {"Lucas Leal", "lucas-leal"},
  {"Ludmila Lima", "ludmila-lima"},
  {"Luiz Faria", "luiz-faria"},
  {"Marco Antonio", "marco-antonio"},
  {"Marcos Queiroz", "marcos-queiroz"},
  {"Matheus Costa", "matheus-costa"},
  {"Pedro Urani", "pedro-urani"},
  {"Pietro Lima", "pietro-lima"},
  {"Rafael Trindade", "rafael-trindade"},
  {"Renato Sampaio", "renato-sampaio"},
  {"Rodrigo Ferreira", "rodrigo-ferreira"},
  {"Sheila Araujo", "sheila-araujo"},
  {"Sofia Lima", "sofia-lima"},
};

const int TOTAL_USUARIOS = sizeof(USUARIOS) / sizeof(USUARIOS[0]);

WebServer servidor(80);

void prepararMotores();
void parar();
extern int dutyEsquerdaFrente;
extern int dutyEsquerdaTras;
extern int dutyDireitaFrente;
extern int dutyDireitaTras;
bool executar(String acao);
void enviarLogin();
void enviarControle(String nome, String usuario);
void enviarRecado(String titulo, String texto);

String motorista = "";
String ultimaAcaoRegistrada = "";
unsigned long ultimoComando = 0;

void registrar(String quem, String acao, String resposta) {
  if (acao == ultimaAcaoRegistrada) {
    return;
  }
  ultimaAcaoRegistrada = acao;
  Serial.print("[");
  Serial.print(millis() / 1000);
  Serial.print("s] ");
  Serial.print(quem);
  Serial.print(" pediu ");
  Serial.print(acao);
  Serial.print(" -> ");
  Serial.print(resposta);
  Serial.print("  | duty esq ");
  Serial.print(dutyEsquerdaFrente);
  Serial.print("/");
  Serial.print(dutyEsquerdaTras);
  Serial.print("  dir ");
  Serial.print(dutyDireitaFrente);
  Serial.print("/");
  Serial.println(dutyDireitaTras);
}

void montarNomeDaRede() {
  sprintf(nomeDaRede, "CARRINHO-%02d", NUMERO_DO_CARRINHO);
}

int canalDoCarrinho() {
  const int CANAIS[3] = {1, 6, 11};
  return CANAIS[(NUMERO_DO_CARRINHO - 1) % 3];
}

int procurarUsuario(String usuario) {
  for (int i = 0; i < TOTAL_USUARIOS; i++) {
    if (usuario == USUARIOS[i].usuario) {
      return i;
    }
  }
  return -1;
}

String nomeDe(String usuario) {
  int indice = procurarUsuario(usuario);
  if (indice < 0) {
    return "";
  }
  return String(USUARIOS[indice].nome);
}

String comandar(String usuario, String acao) {
  if (procurarUsuario(usuario) < 0) {
    return "erro|faca login de novo";
  }
  if (acao == "assumir") {
    parar();
    motorista = usuario;
    ultimoComando = millis();
    return "ok|agora o controle e seu";
  }
  bool livre = (motorista == "") || (millis() - ultimoComando > TEMPO_SEM_USO_MS);
  if (motorista != usuario && !livre) {
    return "ocupado|" + nomeDe(motorista) + " esta dirigindo";
  }
  motorista = usuario;
  ultimoComando = millis();
  if (!executar(acao)) {
    return "erro|nao entendi o comando";
  }
  if (acao == "parar") {
    return "ok|parado";
  }
  return "ok|andando para " + acao;
}

void vigiar() {
  if (motorista == "") {
    return;
  }
  unsigned long paradoHa = millis() - ultimoComando;
  if (paradoHa > PARADA_AUTOMATICA_MS) {
    if (ultimaAcaoRegistrada != "" && ultimaAcaoRegistrada != "parar") {
      Serial.println("  ...parada automatica: ficou 1,5s sem receber comando");
      ultimaAcaoRegistrada = "parar";
    }
    parar();
  }
  if (paradoHa > TEMPO_SEM_USO_MS) {
    motorista = "";
  }
}

void atenderComando() {
  String usuario = servidor.arg("u");
  String acao = servidor.arg("a");
  String resposta = comandar(usuario, acao);
  registrar(nomeDe(usuario), acao, resposta);
  servidor.send(200, "text/plain", resposta);
}

void atenderOutros() {
  String caminho = servidor.uri();
  if (caminho.startsWith("/carro/")) {
    String usuario = caminho.substring(7);
    int indice = procurarUsuario(usuario);
    if (indice < 0) {
      enviarRecado("Nao achei esse nome", "Volte e escolha na lista.");
    } else {
      enviarControle(USUARIOS[indice].nome, usuario);
    }
    return;
  }
  servidor.send(404, "text/plain", "nao existe");
}

void setup() {
  Serial.begin(115200);
  prepararMotores();
  parar();

  montarNomeDaRede();
  bool redeNoAr = WiFi.softAP(nomeDaRede, SENHA_DA_REDE, canalDoCarrinho(), 0, MAXIMO_DE_APARELHOS);

  Serial.println();
  if (redeNoAr) {
    Serial.print("Rede Wi-Fi no ar: ");
    Serial.println(nomeDaRede);
    Serial.print("Senha: ");
    Serial.println(SENHA_DA_REDE);
    Serial.print("Canal: ");
    Serial.println(canalDoCarrinho());
    Serial.print("Aparelhos ao mesmo tempo: ");
    Serial.println(MAXIMO_DE_APARELHOS);
    Serial.print("Abra no navegador: http://");
    Serial.println(WiFi.softAPIP());
    Serial.print("Inverter lado esquerdo: ");
    Serial.println(INVERTER_LADO_ESQUERDO ? "sim" : "nao");
    Serial.print("Inverter lado direito: ");
    Serial.println(INVERTER_LADO_DIREITO ? "sim" : "nao");
    Serial.println("Pronto. Cada comando recebido aparece abaixo.");
  } else {
    Serial.println("FALHOU ao criar a rede Wi-Fi");
  }

  servidor.on("/", enviarLogin);
  servidor.on("/comando", atenderComando);
  servidor.onNotFound(atenderOutros);
  servidor.begin();
}

void loop() {
  servidor.handleClient();
  vigiar();
}
