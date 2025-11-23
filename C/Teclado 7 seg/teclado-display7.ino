#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>

// Pines del display de 7 segmentos
const uint8_t PIN_A = 22;
const uint8_t PIN_B = 23;
const uint8_t PIN_C = 24;
const uint8_t PIN_D = 25;
const uint8_t PIN_E = 26;
const uint8_t PIN_F = 27;
const uint8_t PIN_G = 28;

// Patrones de bits para segmentos (0-9 y A-F)
const uint8_t SEGMENT_PATTERNS[16] = {
  0b00111111,  // 0
  0b00000110,  // 1
  0b01011011,  // 2
  0b01001111,  // 3
  0b01100110,  // 4
  0b01101101,  // 5
  0b01111101,  // 6
  0b00000111,  // 7
  0b01111111,  // 8
  0b01101111,  // 9
  0b01110111,  // A
  0b01111100,  // b
  0b00111001,  // C
  0b01011110,  // d
  0b01111001,  // E
  0b01110001,  // F
};

// Pines del teclado
const byte ROW_PINS[4] = { 46, 47, 48, 49 };
const byte COL_PINS[4] = { 50, 51, 52, 53 };
const char KEYS[4][4] = {
  { '7', '8', '9', 'A' },
  { '4', '5', '6', 'B' },
  { '1', '2', '3', 'C' },
  { 'F', '0', 'E', 'D' }
};

// Estado del display (comportamiento común: cathode = true)
const bool IS_COMMON_CATHODE = false;

// Variables para el teclado y debounce
volatile bool keyDetected = false;
volatile byte currentRow = 0;
unsigned long lastDebounceTime = 0;
const unsigned long DEBOUNCE_DELAY = 200;

// Pulsación detectada por el programa principal
volatile char pressedKey = '\0';

// -------- Display helpers (sin clases) --------
void displayInit() {
  pinMode(PIN_A, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  pinMode(PIN_C, OUTPUT);
  pinMode(PIN_D, OUTPUT);
  pinMode(PIN_E, OUTPUT);
  pinMode(PIN_F, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  // Dejar apagado inicialmente
  if (IS_COMMON_CATHODE) {
    digitalWrite(PIN_A, LOW);
    digitalWrite(PIN_B, LOW);
    digitalWrite(PIN_C, LOW);
    digitalWrite(PIN_D, LOW);
    digitalWrite(PIN_E, LOW);
    digitalWrite(PIN_F, LOW);
    digitalWrite(PIN_G, LOW);
  } else {
    digitalWrite(PIN_A, HIGH);
    digitalWrite(PIN_B, HIGH);
    digitalWrite(PIN_C, HIGH);
    digitalWrite(PIN_D, HIGH);
    digitalWrite(PIN_E, HIGH);
    digitalWrite(PIN_F, HIGH);
    digitalWrite(PIN_G, HIGH);
  }
}

void writeSegments(uint8_t pattern) {
  if (!IS_COMMON_CATHODE) {
    pattern = ~pattern;
  }
  digitalWrite(PIN_A, (pattern >> 0) & 0x01);
  digitalWrite(PIN_B, (pattern >> 1) & 0x01);
  digitalWrite(PIN_C, (pattern >> 2) & 0x01);
  digitalWrite(PIN_D, (pattern >> 3) & 0x01);
  digitalWrite(PIN_E, (pattern >> 4) & 0x01);
  digitalWrite(PIN_F, (pattern >> 5) & 0x01);
  digitalWrite(PIN_G, (pattern >> 6) & 0x01);
}

void showNumber(uint8_t number) {
  if (number > 9) {
    // clear
    writeSegments(IS_COMMON_CATHODE ? 0x00 : 0xFF);
    return;
  }
  writeSegments(SEGMENT_PATTERNS[number]);
}

void displayClear() {
  writeSegments(IS_COMMON_CATHODE ? 0x00 : 0xFF);
}

void displayTest() {
  for (uint8_t i = 0; i < 7; i++) {
    writeSegments(1 << i);
    delay(500);
  }
  for (uint8_t i = 0; i <= 9; i++) {
    showNumber(i);
    delay(500);
  }
  displayClear();
}

// -------- Keypad helpers (sin clases) --------
void keypadInit() {
  // Configurar filas como salidas, inicialmente HIGH
  for (byte i = 0; i < 4; i++) {
    pinMode(ROW_PINS[i], OUTPUT);
    digitalWrite(ROW_PINS[i], HIGH);
  }

  // Configurar columnas como entradas con pull-up
  for (byte i = 0; i < 4; i++) {
    pinMode(COL_PINS[i], INPUT_PULLUP);
  }

  // Configurar interrupciones PCINT0:3 para pines 50-53
  PCICR |= (1 << PCIE0);
  PCMSK0 |= (1 << PCINT0) | (1 << PCINT1) | (1 << PCINT2) | (1 << PCINT3);
}

char keypadGetKey() {
  unsigned long currentTime = millis();

  // Verificar debounce
  if ((currentTime - lastDebounceTime) < DEBOUNCE_DELAY) {
    return '\0';
  }

  // Avanzar la fila activa: poner la fila actual HIGH, avanzar y poner la siguiente LOW
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

void setKeyDetected(bool detected) {
  keyDetected = detected;
}

// -------- Main program (setup/loop/ISR) --------
void setup() {
  displayInit();
  keypadInit();
}

void loop() {
  char key = keypadGetKey();
  if (key != '\0') {
    pressedKey = key;
  }

  if (pressedKey != '\0') {
    // Mostrar en display
    if (pressedKey >= '0' && pressedKey <= '9') {
      showNumber(pressedKey - '0');
    } else if (pressedKey >= 'A' && pressedKey <= 'F') {
      writeSegments(SEGMENT_PATTERNS[10 + (pressedKey - 'A')]);
    } else {
      displayClear();
    }
  }
}

ISR(PCINT0_vect) {
  // Señal desde las columnas: marcar que hubo una pulsación
  keyDetected = true;
}