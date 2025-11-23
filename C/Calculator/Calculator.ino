#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "LCD.h"

// --- 1. CONFIGURACIÓN DEL DISPLAY LCD ---
const int DATA_PINS[] = {30, 31, 32, 33, 34, 35, 36, 37};
LCD lcd(22, 23, DATA_PINS);


// --- 2. CONFIGURACIÓN DEL TECLADO ---
const byte ROW_PINS[4] = { 46, 47, 48, 49 };
const byte COL_PINS[4] = { 50, 51, 52, 53 };

const char KEYS[4][4] = {
  { '7', '8', '9', '/' },
  { '4', '5', '6', '*' },
  { '1', '2', '3', '-' },
  { 'C', '0', '=', '+' }
};

// Variables de estado del teclado
volatile bool keyDetected = false;
volatile byte currentRow = 0;
unsigned long lastDebounceTime = 0;
const unsigned long DEBOUNCE_DELAY = 200;


// --- 3. LÓGICA DE LA CALCULADORA ---

float currentNumber = 0;
float storedNumber = 0;
char pendingOperation = '\0';
bool enteringNumber = false;
bool errorState = false;

String expressionString = "";


// --- 4. FUNCIONES DE LA CALCULADORA (MODIFICADAS) ---
void updateDisplay(bool showResult = false) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(expressionString);

  if (errorState) {
    lcd.setCursor(0, 1);
    lcd.print("ERROR");
  } 
  else if (showResult) {
    lcd.setCursor(0, 1);
    lcd.print(String(currentNumber));
  }
}

void performCalculation() {
  if (pendingOperation == '\0') {
    return;
  }
  switch (pendingOperation) {
    case '+': storedNumber = storedNumber + currentNumber; break;
    case '-': storedNumber = storedNumber - currentNumber; break;
    case '*': storedNumber = storedNumber * currentNumber; break;
    case '/':
      if (currentNumber == 0) {
        errorState = true;
        storedNumber = 0;
      } else {
        storedNumber = storedNumber / currentNumber;
      }
      break;
  }
  currentNumber = storedNumber;
}

void handleDigit(char digit) {
  if (errorState) return;

  if (!enteringNumber) {
    currentNumber = 0;
    enteringNumber = true;

    if (pendingOperation == '\0') {
      expressionString = "";
    }
  }

  if (currentNumber < 99999999) {
     currentNumber = (currentNumber * 10) + (digit - '0');
  }
  
  expressionString += digit;
  updateDisplay();
}

void handleOperation(char op) {
  if (errorState) return;

  if (!enteringNumber) {
    if (pendingOperation != '\0') {
      pendingOperation = op;
      expressionString = expressionString.substring(0, expressionString.length() - 1);
      expressionString += op;
      updateDisplay();
    }
    else {
      pendingOperation = op;
      storedNumber = currentNumber;
      enteringNumber = false;
      expressionString += op;
      updateDisplay();
    }
    return;
  }

  // Lógica normal si se presiona un operador después de un número
  if (pendingOperation != '\0') {
    performCalculation();
    if (errorState) {
      updateDisplay(true);
      return;
    }
  }

  pendingOperation = op;
  storedNumber = currentNumber;
  enteringNumber = false;
  
  expressionString += op;
  updateDisplay();
}

void handleEquals() {
  if (errorState) return;
  
  performCalculation();
  updateDisplay(true);
  
  expressionString = String(currentNumber);
  pendingOperation = '\0';
  enteringNumber = false;
}

void handleClear() {
  currentNumber = 0;
  storedNumber = 0;
  pendingOperation = '\0';
  enteringNumber = false;
  errorState = false;
  expressionString = "";
  updateDisplay();
}

void handleKeyPress(char key) {
  switch (key) {
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
      handleDigit(key);
      break;
      
    case '/': case '*': case '-': case '+':
      handleOperation(key);
      break;
      
    case '=':
      handleEquals();
      break;
      
    case 'C':
      handleClear();
      break;
  }
}


// --- 5. FUNCIONES DEL TECLADO ---
void keypadInit() {
  for (byte i = 0; i < 4; i++) {
    pinMode(ROW_PINS[i], OUTPUT);
    digitalWrite(ROW_PINS[i], HIGH);
  }
  for (byte i = 0; i < 4; i++) {
    pinMode(COL_PINS[i], INPUT_PULLUP);
  }
  PCICR |= (1 << PCIE0);
  PCMSK0 |= (1 << PCINT0) | (1 << PCINT1) | (1 << PCINT2) | (1 << PCINT3);
}

char keypadGetKey() {
  unsigned long currentTime = millis();
  if ((currentTime - lastDebounceTime) < DEBOUNCE_DELAY) {
    return '\0';
  }

  digitalWrite(ROW_PINS[currentRow], HIGH);
  currentRow = (currentRow + 1) % 4;
  digitalWrite(ROW_PINS[currentRow], LOW);

  if (keyDetected) {
    for (byte columna = 0; columna < 4; columna++) {
      if (digitalRead(COL_PINS[columna]) == LOW) {
        keyDetected = false;
        lastDebounceTime = currentTime;
        return KEYS[currentRow][columna];
      }
    }
    keyDetected = false;
  }
  delayMicroseconds(500);
  return '\0';
}

// Rutina de Interrupción (ISR)
ISR(PCINT0_vect) {
  keyDetected = true;
}


// --- 6. PROGRAMA PRINCIPAL ---
void setup() {
  Serial.begin(9600);
  lcd.begin();
  keypadInit();
  sei();
  handleClear();
}

void loop() {
  char key = keypadGetKey();
  
  if (key != '\0') {
    Serial.print("Tecla detectada: ");
    Serial.println(key);
    handleKeyPress(key);
  }
}