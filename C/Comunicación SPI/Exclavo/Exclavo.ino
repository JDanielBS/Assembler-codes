/*
 * VISOR_SPI_ESCLAVO_LCD.INO
 * -------------------------
 * Recibe datos por SPI y los muestra en LCD
 */

#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "LCD.h"
#include <SPI.h>


const int DATA_PINS[] = {30, 31, 32, 33, 34, 35, 36, 37};
LCD lcd(22, 23, DATA_PINS);


volatile byte spiReceivedValue = 0;
volatile bool dataReceived = false;
volatile unsigned long lastReceiveTime = 0;



/**
 * @brief Actualiza el display con el valor recibido por SPI
 */
void updateDisplayWithSPI() {
  lcd.clear();
  lcd.setCursor(0, 0); 
  
  
  String line1 = "RX SPI: '";
  line1 += (char)spiReceivedValue;
  line1 += "'";
  lcd.print(line1);
}



/**
 * @brief Configura el Arduino Mega 2560 como esclavo SPI.
 */
void spiSlaveInit() {
  
  pinMode(MISO, OUTPUT); 
  pinMode(MOSI, INPUT);  
  pinMode(SCK, INPUT);   
  pinMode(SS, INPUT);    
  
  
  
  
  SPCR = (1 << SPE) | (1 << SPIE);
  
  
  
  
  Serial.println("SPI Esclavo inicializado");
  Serial.println("SS Pin: 53 (INPUT)");
}

/**
 * @brief ISR - Rutina de Interrupción de Transferencia Completa del SPI
 */
ISR (SPI_STC_vect) {
  
  spiReceivedValue = SPDR;
  dataReceived = true;
  lastReceiveTime = millis();
  
  
  byte dummy = SPSR;
  dummy = SPDR;
}



void setup() {
  Serial.begin(9600);
  Serial.println("=== SPI ESCLAVO ===");
  
  lcd.begin();
  spiSlaveInit(); 
  
  sei(); 
  
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SPI Esclavo LISTO");
  lcd.setCursor(0, 1);
  lcd.print("Esperando datos...");
  
  Serial.println("Esperando datos SPI...");
}

void loop() {
  
  if (dataReceived) {
    
    cli();
    byte receivedData = spiReceivedValue;
    dataReceived = false;
    sei();
    
    
    updateDisplayWithSPI();
    
    
    String serialMsg = "Dato recibido: '";
    serialMsg += (char)receivedData;
    serialMsg += "' (Dec: ";
    serialMsg += String((int)receivedData);
    serialMsg += ", Hex: 0x";
    
    String hexStr = String(receivedData, HEX);
    if(receivedData < 0x10) {
      hexStr = "0" + hexStr;
    }
    serialMsg += hexStr;
    serialMsg += ")";
    
    Serial.println(serialMsg);
  }
  
  
  if (dataReceived == false && (millis() - lastReceiveTime) > 5000 && lastReceiveTime != 0) {
    
  }
}