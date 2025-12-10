# Projeto Fermento – Integração de Sensores no ESP32 / Cheap Yellow Display  
## Documento Técnico – README.md  
Trabalho Acadêmico – Engenharia / Sistemas Embarcados

Este documento descreve:  
1. O funcionamento do programa atual (somente sensor de distância ativo).  
2. O cabeamento utilizado.  
3. Explicação dos trechos relevantes do código.  
4. Um tutorial completo de configuração do ambiente *Arduino IDE* para o **Cheap Yellow Display (ESP32-2432S028)**, baseado na experiência prática do processo de instalação.  

Este README serve como base de estudo técnico e documentação oficial do grupo.

---

# 1. Visão Geral do Projeto

O programa atual (**todosSensoresConectados.c**) está configurado para:

- Inicializar o display TFT do Cheap Yellow Display (CYD)  
- Configurar o touch XPT2046  
- Testar preenchimento de tela e escrita  
- Exibir mensagens  
- Ler um único sensor no momento: **sensor de distância VL53L0X** (ainda em integração)  

Outros sensores serão adicionados posteriormente, mas no momento o foco está no **sensor de distância**, que já foi parcialmente integrado e testado.

---

# 2. Cabeamento do Sensor de Distância VL53L0X com ESP32 (CYD)

A ligação foi realizada seguindo o padrão I²C:

| Pino do Sensor | Função | Cor (exemplo) | Pino no ESP32 (CYD) | Observação |
|----------------|---------|----------------|-----------------------|------------|
| VIN | Alimentação 3V3 | Vermelho | **3V3** | Não usar 5 V |
| GND | Terra | Preto | **GND** | Terra comum |
| SDA | Dados I²C | Amarelo | **IO21** | Linha SDA |
| SCL | Clock I²C | Azul | **IO22** | Linha SCL |
| XSHUT | Reset | Não conectado | — | Não utilizado |

A alimentação deve ser estável em **3V3**.

---

# 3. Funcionamento do Código (Versão Atual)

A versão atual do programa foi construída para:

1. **Inicializar o display TFT**  
2. **Exibir mensagens simples**
3. **Inicializar e testar o touchscreen**
4. **Executar mudança de cores na tela para confirmar operação**
5. **Capturar toques e exibir coordenadas**
6. **Preparar base para integração do sensor VL53L0X**

O código ainda **não contém leituras finalizadas do sensor de distância**, mas está configurado para instalação da biblioteca correta e futura integração.

### Trechos importantes

#### Inicialização do display

```c
tft.init();
tft.setRotation(1);
tft.fillScreen(TFT_BLACK);
tft.drawString("Teste CYD", 40, 120, 4);
