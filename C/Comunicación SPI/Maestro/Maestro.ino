/*
 * VISOR_TECLAS_SPI_MAESTRO.INO (CORREGIDO)
 * ----------------------------
 * Comunicación SPI Síncrona en modo MAESTRO
 * Envía teclas presionadas al Esclavo SPI
 */

#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "LCD.h"
#include <SPI.h>


const int DATA_PINS[] = {30, 31, 32, 33, 34, 35, 36, 37};
LCD lcd(22, 23, DATA_PINS);


const byte ROW_PINS[4] = { 46, 47, 48, 49 };
const byte COL_PINS[4] = { 42, 43, 44, 45 }; 

const char KEYS[4][4] = {
  { '7', '8', '9', '/' },
  { '4', '5', '6', '*' },
  { '1', '2', '3', '-' },
  { 'C', '0', '=', '+' }
};


volatile bool keyDetected = false;
volatile byte currentRow = 0;
unsigned long lastDebounceTime = 0;
const unsigned long DEBOUNCE_DELAY = 200;


String currentDisplay = ""; 


const int CS_PIN = 53; 



void updateSimpleDisplay() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("TX Tecla:");
  lcd.setCursor(0, 1);
  lcd.print(currentDisplay);
}

void handleKeyPress(char key) {
  if (key == 'C') {
    currentDisplay = "";
  } else {
    currentDisplay += key;
    if (currentDisplay.length() > 16) {
      currentDisplay = currentDisplay.substring(currentDisplay.length() - 16);
    }
  }
  
  
  if (key != '\0') {
    spiTransmit(key);
    Serial.print("TX SPI: ");
    Serial.print(key);
    Serial.print(" (0x");
    Serial.print((byte)key, HEX);
    Serial.println(")");
  }
  
  updateSimpleDisplay();
}



/**
 * @brief Configura el Arduino Mega 2560 como maestro SPI.
 */
void spiMasterInit() {
  
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH); 
  
  
  SPI.begin(); 
  
  
  
  SPI.setClockDivider(SPI_CLOCK_DIV16); 
  SPI.setBitOrder(MSBFIRST);            
  SPI.setDataMode(SPI_MODE0);           
  
  Serial.println("SPI Maestro inicializado");
  Serial.println("CS Pin: 53");
}

/**
 * @brief Transmite un byte al esclavo.
 * @param dataToSend El byte/carácter a enviar.
 */
void spiTransmit(char dataToSend) {
  
  digitalWrite(CS_PIN, LOW);
  
  
  delayMicroseconds(10);
  
  
  byte received = SPI.transfer(dataToSend);
  
  
  delayMicroseconds(10);
  
  
  digitalWrite(CS_PIN, HIGH);
  
  
  delay(50);
}



void keypadInit() {
  for (byte i = 0; i < 4; i++) {
    pinMode(ROW_PINS[i], OUTPUT);
    digitalWrite(ROW_PINS[i], HIGH);
  }
  for (byte i = 0; i < 4; i++) {
    pinMode(COL_PINS[i], INPUT_PULLUP);
  }
}

char keypadGetKey() {
  unsigned long currentTime = millis();
  
  if ((currentTime - lastDebounceTime) < DEBOUNCE_DELAY) {
    return '\0';
  }

  digitalWrite(ROW_PINS[currentRow], HIGH);
  currentRow = (currentRow + 1) % 4;
  digitalWrite(ROW_PINS[currentRow], LOW);
  
  for (byte columna = 0; columna < 4; columna++) {
    if (digitalRead(COL_PINS[columna]) == LOW) {
      lastDebounceTime = currentTime;
      return KEYS[currentRow][columna];
    }
  }

  delayMicroseconds(500);
  return '\0';
}



void setup() {
  Serial.begin(9600);
  Serial.println("=== SPI MAESTRO ===");
  
  lcd.begin();
  keypadInit();
  spiMasterInit(); 
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SPI Maestro ACTIVO");
  lcd.setCursor(0, 1);
  lcd.print("Presiona tecla...");
  
  delay(1000);
}

void loop() {
  char key = keypadGetKey();
  
  if (key != '\0') {
    Serial.print("Tecla detectada: ");
    Serial.println(key);
    handleKeyPress(key);
  }
}