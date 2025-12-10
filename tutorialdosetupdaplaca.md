# Tutorial Completo de Setup da Placa ESP32-2432S028 (Cheap Yellow Display – CYD)

Este documento descreve, passo a passo, como configurar a placa **Cheap Yellow Display (ESP32-2432S028)** na Arduino IDE, incluindo instalação do suporte ao ESP32, bibliotecas, ajuste do arquivo `User_Setup.h`, seleção de placa, porta e teste de hardware.

Todas as URLs estão incluídas diretamente neste markdown, em texto normal.

---

## 1. Referência Utilizada

O tutorial original que serviu de base para este setup está em:

https://randomnerdtutorials.com/cheap-yellow-display-esp32-2432s028r/

---

## 2. Instalar Suporte ao ESP32 na Arduino IDE

### 2.1. Abrir as Preferências

1. Abrir a Arduino IDE.  
2. Acessar o menu: **File > Preferences**.  

Na janela que abrir, localizar o campo **Additional Board Manager URLs**.

### 2.2. Adicionar URL do ESP32

Adicionar a seguinte URL no campo **Additional Board Manager URLs**:

https://dl.espressif.com/dl/package_esp32_index.json

Se já houver outras URLs, separar por vírgula ou por nova linha.  
Confirmar clicando em **OK**.

---

## 3. Instalar o Pacote de Placas ESP32

1. Acessar o menu: **Tools > Board > Boards Manager…**  
2. Na barra de busca, digitar: `ESP32`.  
3. Localizar **ESP32 by Espressif Systems**.  
4. Clicar em **Install**.  

A instalação pode demorar alguns minutos, pois o pacote de placas é grande.

---

## 4. Selecionar a Placa Correta

Após instalar o pacote:

1. Acessar: **Tools > Board**.  
2. Na lista de placas, escolher a opção compatível com o CYD. As mais comuns são:
   - **ESP32 Dev Module**  
   - **ESP32S2 Dev Module**  

A escolha deve seguir o código impresso na parte de trás da placa (por exemplo, se começar com ESP32-S2, escolher a família S2).

---

## 5. Selecionar a Porta Serial

1. Conectar a placa ESP32-2432S028 ao computador via cabo USB.  
2. Acessar: **Tools > Port**.  
3. Selecionar a porta COM que aparecer (por exemplo, COM4).  

Para identificar a porta corretamente no Windows:

- Abrir o **Gerenciador de Dispositivos** (buscar por “Gerenciador de Dispositivos” no menu Iniciar).  
- Expandir a seção **Portas (COM & LPT)**.  
- Conectar e desconectar a placa para ver qual porta COM aparece e desaparece (essa é a porta correta do ESP32).

---

## 6. Instalar Bibliotecas Necessárias

1. Acessar o menu: **Sketch > Include Library > Manage Libraries…**  
2. No campo de busca, instalar as seguintes bibliotecas:
   - `TFT_eSPI`
   - `XPT2046_Touchscreen`

Essas bibliotecas são usadas, respectivamente, para controlar o display TFT (driver ILI9341) e o touchscreen XPT2046.

---

## 7. Configurar o Arquivo User_Setup.h da Biblioteca TFT_eSPI

A biblioteca **TFT_eSPI** exige uma configuração específica para o Cheap Yellow Display. Isso é feito editando o arquivo `User_Setup.h`.

### 7.1. Localizar a pasta da biblioteca

1. Na Arduino IDE, acessar **File > Preferences**.  
2. No campo **Sketchbook location**, copiar o caminho exibido.  
3. Abrir esse caminho no Explorer (ou outro gerenciador de arquivos).  
4. Navegar até:

`Arduino/libraries/TFT_eSPI/`

Dentro dessa pasta está o arquivo `User_Setup.h`.

### 7.2. Fazer backup e substituir conteúdo

1. Fazer uma cópia de segurança do arquivo original `User_Setup.h`.  
2. Abrir `User_Setup.h` em um editor de texto.  
3. Apagar o conteúdo existente e substituir pelo bloco abaixo:

```c
#define USER_SETUP_INFO "ESP32-2432S028 CYD"

#define ILI9341_DRIVER

#define TFT_MISO 12
#define TFT_MOSI 13
#define TFT_SCLK 14
#define TFT_CS   15
#define TFT_DC    2
#define TFT_RST  -1

#define TFT_BL    21
#define TFT_BACKLIGHT_ON HIGH

#define SPI_FREQUENCY       40000000
#define SPI_READ_FREQUENCY  20000000
#define SPI_TOUCH_FREQUENCY 2500000

#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF
#define SMOOTH_FONT

#define SUPPORT_TRANSACTIONS

## 8. Código de Teste do Cheap Yellow Display

O código abaixo testa:  
- Inicialização do display TFT  
- Funcionamento do backlight  
- Troca de cores da tela  
- Touchscreen XPT2046  
- Envio das coordenadas de toque pela porta serial  

Copiar o código completo para a Arduino IDE.  
Não inserir comentários adicionais dentro do bloco.

```c
#include <SPI.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>

TFT_eSPI tft = TFT_eSPI();

#define XPT2046_IRQ 36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK 25
#define XPT2046_CS 33

SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);

#define BACKLIGHT_PIN 21

void setup() {
  Serial.begin(115200);

  pinMode(BACKLIGHT_PIN, OUTPUT);
  digitalWrite(BACKLIGHT_PIN, HIGH);

  tft.init();
  tft.setRotation(1);

  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.drawString("Teste CYD", 40, 120, 4);

  touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  touchscreen.begin(touchscreenSPI);
  touchscreen.setRotation(1);
}

void loop() {
  tft.fillScreen(TFT_RED);
  delay(1000);
  tft.fillScreen(TFT_GREEN);
  delay(1000);
  tft.fillScreen(TFT_BLUE);
  delay(1000);
  tft.fillScreen(TFT_BLACK);
  delay(1000);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("Toque:", 80, 30, 4);

  if (touchscreen.tirqTouched() && touchscreen.touched()) {
    TS_Point p = touchscreen.getPoint();
    Serial.print("X = "); Serial.print(p.x);
    Serial.print(" Y = "); Serial.print(p.y);
    Serial.print(" Z = "); Serial.println(p.z);
    tft.fillCircle(p.x, p.y, 5, TFT_YELLOW);
  }
}

## 9. Upload do Código para a Placa

Para enviar o código de teste ao Cheap Yellow Display, siga os passos:

1. Confirmar que a placa correta está selecionada no menu:  
   **Tools > Board**
2. Confirmar que a porta correta está selecionada no menu:  
   **Tools > Port**
3. Clicar no botão **Upload** na barra de ferramentas da Arduino IDE.
4. Aguardar o processo de compilação e gravação.

Após o upload, o display deve apresentar:

- Alternância entre telas vermelha, verde, azul e preta.  
- A mensagem “Teste CYD” ao inicializar.  
- A palavra “Toque:” no topo da tela.  
- Círculos amarelos sendo desenhados na posição tocada.  
- Coordenadas X, Y e Z aparecendo no Serial Monitor da IDE.

## 10. Organização Sugerida do Projeto e 11. Notas Finais

### Organização Sugerida do Projeto

A estrutura abaixo é recomendada para manter o projeto organizado e facilitar o versionamento e colaboração da equipe:

```text
Projeto-CYD/
├── src/
│   └── teste_cyd.c
├── docs/
│   └── tutorial_setup.md
└── User_Setup.h


## 11. Notas Finais

- A configuração correta do arquivo `User_Setup.h` é indispensável para o funcionamento adequado do Cheap Yellow Display. Caso esse arquivo não seja ajustado, o display pode não ligar, exibir cores incorretas ou apresentar falhas de renderização.  
- Durante os testes com sensores I2C, especialmente o VL53L0X, a biblioteca da **Adafruit** demonstrou melhor compatibilidade com o ESP32, enquanto a biblioteca da Pololu apresentou erros de compilação.  
- O processo inicial de instalação do suporte ao ESP32 e das bibliotecas necessárias pode ser demorado, principalmente na primeira vez em que o ambiente é configurado. Isso é esperado.  
- Este tutorial foi desenvolvido como documentação acadêmica e como guia de referência rápida para estudantes e equipes que utilizarem o Cheap Yellow Display em projetos técnicos, protótipos ou atividades laboratoriais.

