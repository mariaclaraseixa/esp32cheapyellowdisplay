/*
___  ________ _   _ _____ _____ __________        
|  \/  |  _  | \ | |_   _|_   _|  _  | ___ \       
| .  . | | | |  \| | | |   | | | | | | |_/ /       
| |\/| | | | | . ` | | |   | | | | | |    /        
| |  | \ \_/ / |\  |_| |_  | | \ \_/ / |\ \        
\_|  |_/\___/\_| \_/\___/  \_/  \___/\_| \_|       
                                                   
                                                   
______ ______________  ___ _____ _   _ _____ _____ 
|  ___|  ___| ___ \  \/  ||  ___| \ | |_   _|  _  |
| |_  | |__ | |_/ / .  . || |__ |  \| | | | | | | |
|  _| |  __||    /| |\/| ||  __|| . ` | | | | | | |
| |   | |___| |\ \| |  | || |___| |\  | | | \ \_/ /
\_|   \____/\_| \_\_|  |_/\____/\_| \_/ \_/  \___/ 
                                                   
                                                   
   ============================================================ */

#define HC_TRIG_PIN  27   // TRIG no GPIO27 (CN1)
#define HC_ECHO_PIN  22   // ECHO no GPIO22 (CN1)

// Códigos de erro internos (não exibidos na tela)
#define ERR_OK                     0
#define ERR1_SENSOR_NAO_CONECTADO  1
#define ERR2_ERRO_LEITURA          2
#define ERR3_FORA_DE_FAIXA         3

int   codigoErroDistancia = ERR1_SENSOR_NAO_CONECTADO;
float distanciaFermentoCm = NAN;

/* ============================================================
   Estrutura de dados – TEMP/UMIDADE simuladas
   ============================================================ */
struct Fermento {
  float temperatura;
  int   umidade;
  float altura;
  unsigned long updates;
} F;

/* ============================================================
   Temperatura/umidade mocadas
   ============================================================ */
void simularLeituras() {
  F.temperatura += 0.1;
  if (F.temperatura > 30) F.temperatura = 22;

  if (F.updates % 5 == 0) {
    F.umidade++;
    if (F.umidade > 90) F.umidade = 70;
  }

  F.updates++;
}

/* ============================================================
   Inicializa HC-SR04
   ============================================================ */
bool inicializarSensorDistancia() {
  pinMode(HC_TRIG_PIN, OUTPUT);
  pinMode(HC_ECHO_PIN, INPUT);

  digitalWrite(HC_TRIG_PIN, LOW);
  delay(50);

  codigoErroDistancia = ERR_OK;
  distanciaFermentoCm = NAN;
  return true;
}

/* ============================================================
   Leitura do HC-SR04
   ============================================================ */
bool atualizarDistanciaFermento() {

  digitalWrite(HC_TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(HC_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(HC_TRIG_PIN, LOW);

  unsigned long duracao = pulseIn(HC_ECHO_PIN, HIGH, 30000);

  if (duracao == 0) {
    codigoErroDistancia = ERR1_SENSOR_NAO_CONECTADO;
    distanciaFermentoCm = NAN;
    return false;
  }

  float distancia = duracao / 58.0;

  if (distancia < 2 || distancia > 400) {
    codigoErroDistancia = ERR3_FORA_DE_FAIXA;
    distanciaFermentoCm = NAN;
    return false;
  }

  if (!isfinite(distancia)) {
    codigoErroDistancia = ERR2_ERRO_LEITURA;
    distanciaFermentoCm = NAN;
    return false;
  }

  distanciaFermentoCm = distancia;
  codigoErroDistancia = ERR_OK;
  F.altura = distanciaFermentoCm;
  return true;
}

/* ============================================================
   Desenho padrão de caixa
   ============================================================ */
void desenharCaixa(int x, int y, int w, int h, uint16_t corBorda) {
  tft.drawRoundRect(x, y, w, h, 6, corBorda);
}

/* ============================================================
   Texto centralizado
   ============================================================ */
void escreverValorCentro(String texto, int cx, int cy, uint8_t font, uint16_t cor) {
  tft.setTextDatum(4);
  tft.setTextColor(cor, TFT_BLACK);
  tft.drawString(texto, cx, cy, font);
}

/* ============================================================
   String da distância — APENAS "XX cm"
   ============================================================ */
String textoDistancia() {
  if (codigoErroDistancia == ERR_OK) {
    int d = (int)(distanciaFermentoCm + 0.5);
    return String(d) + " cm";
  }
  return "-- cm";
}

/* ============================================================
   Atualiza Tela
   ============================================================ */
void atualizarTela() {
  tft.fillScreen(TFT_BLACK);

  // TÍTULO
  tft.setTextDatum(4);
  tft.setTextColor(TFT_ORANGE, TFT_BLACK);
  tft.drawString("MONITOR DE FERMENTO", 160, 18, 4);
  tft.drawFastHLine(20, 38, 280, TFT_ORANGE);

  // TEMP
  desenharCaixa(20, 50, 130, 80, TFT_YELLOW);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("TEMP.", 85, 68, 2);
  escreverValorCentro(String(F.temperatura, 1) + " C",
                      85, 105, 6, TFT_CYAN);

  // UMIDADE
  desenharCaixa(170, 50, 130, 80, TFT_BLUE);
  tft.drawString("UMIDADE", 235, 68, 2);
  escreverValorCentro(String(F.umidade) + " %",
                      235, 105, 6, TFT_SKYBLUE);

  // DISTÂNCIA (apenas "XX cm")
  desenharCaixa(20, 150, 280, 80, TFT_GREEN);
  tft.drawString("DISTANCIA DO FERMENTO", 160, 165, 2);

  escreverValorCentro(textoDistancia(),
                      160, 205, 7, TFT_GREENYELLOW);

  // STATUS DE ATUALIZAÇÃO NA PARTE DE BAIXO
  tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  tft.drawString("Atualizacao: " + String(F.updates),
                 160, 260, 2);
}

/* ============================================================
   SETUP
   ============================================================ */
void setup() {
  Serial.begin(115200);

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  F.temperatura = 22.5;
  F.umidade     = 75;
  F.altura      = 0.0;
  F.updates     = 0;

  inicializarSensorDistancia();
  atualizarTela();
}

/* ============================================================
   LOOP
   ============================================================ */
void loop() {
  simularLeituras();
  atualizarDistanciaFermento();
  atualizarTela();
  delay(1500);
}

