#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <LiquidCrystal.h>

// ----- CONFIG LCD -----
const int RS = 22;
const int EN = 23;
const int D0 = 30;
const int D1 = 31;
const int D2 = 32;
const int D3 = 33;
const int D4 = 34;
const int D5 = 35;
const int D6 = 36;
const int D7 = 37;
LiquidCrystal lcd(RS, EN, D0, D1, D2, D3, D4, D5, D6, D7);

// ----- CONFIG TECLADO -----
const byte ROW_PINS[4] = { 46, 47, 48, 49 };
const byte COL_PINS[4] = { 50, 51, 52, 53 };
const char KEYS[4][4] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

volatile bool keyDetected = false;
volatile byte currentRow = 0;
unsigned long lastDebounceTime = 0;
const unsigned long DEBOUNCE_DELAY = 200;

// ----- VARIABLES DEL SISTEMA -----
enum Estado {
  SELECCIONAR_MESA,
  SELECCIONAR_MENU
};

Estado estadoActual = SELECCIONAR_MESA;
String inputMesa = "";
String pedido = "";  // Aquí se guarda el número de mesa

// ----- TECLADO -----
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

ISR(PCINT0_vect) {
  keyDetected = true;
}

char keypadGetKey() {
  unsigned long currentTime = millis();
  if ((currentTime - lastDebounceTime) < DEBOUNCE_DELAY) return '\0';
  
  digitalWrite(ROW_PINS[currentRow], HIGH);
  currentRow = (currentRow + 1) % 4;
  digitalWrite(ROW_PINS[currentRow], LOW);
  
  if (keyDetected) {
    for (byte c = 0; c < 4; c++) {
      if (digitalRead(COL_PINS[c]) == LOW) {
        keyDetected = false;
        lastDebounceTime = currentTime;
        return KEYS[currentRow][c];
      }
    }
    keyDetected = false;
  }
  
  delayMicroseconds(500);
  return '\0';
}

// ----- FUNCIONES DEL SISTEMA -----
void mostrarPantallaSeleccionMesa() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("(Mesero)");
  lcd.setCursor(0, 1);
  lcd.print("Seleccione mesa");
}

void mostrarError(String mensaje) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ERROR:");
  lcd.setCursor(0, 1);
  lcd.print(mensaje);
  delay(2000);
  mostrarPantallaSeleccionMesa();
  inputMesa = "";
  lcd.setCursor(0, 1);
  lcd.print("Mesa:           ");
}

void mostrarMenuOpciones() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Elija el menu");
  lcd.setCursor(0, 1);
  lcd.print("A B C D");
}

void procesarSeleccionMesa(char key) {
  // Si es un número (0-9)
  if (key >= '0' && key <= '9') {
    inputMesa += key;
    
    // Mostrar en segunda línea
    lcd.setCursor(0, 1);
    lcd.print("Mesa: " + inputMesa + "  ");
    
    Serial.print("Input actual: ");
    Serial.println(inputMesa);
  }
  // Si presiona # para confirmar
  else if (key == '#') {
    if (inputMesa.length() > 0) {
      // Guardar en pedido
      pedido = inputMesa;
      
      Serial.print("Mesa confirmada: ");
      Serial.println(pedido);
      
      // Mostrar confirmación
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Mesa " + pedido);
      lcd.setCursor(0, 1);
      lcd.print("Confirmada!");
      delay(1500);
      
      // Cambiar a selección de menú
      estadoActual = SELECCIONAR_MENU;
      mostrarMenuOpciones();
      inputMesa = "";
    } else {
      mostrarError("Ingrese mesa");
    }
  }
  // Si presiona * para borrar
  else if (key == '*') {
    if (inputMesa.length() > 0) {
      inputMesa.remove(inputMesa.length() - 1);
      lcd.setCursor(0, 1);
      lcd.print("Mesa: " + inputMesa + "  ");
    }
  }
  // Si ingresa una letra (A, B, C, D) - Error
  else if (key == 'A' || key == 'B' || key == 'C' || key == 'D') {
    mostrarError("Solo numeros");
  }
}

void procesarSeleccionMenu(char key) {
  if (key == 'A' || key == 'B' || key == 'C' || key == 'D') {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Mesa: " + pedido);
    lcd.setCursor(0, 1);
    lcd.print("Menu: " + String(key));
    
    Serial.print("Pedido completo - Mesa: ");
    Serial.print(pedido);
    Serial.print(", Menu: ");
    Serial.println(key);
    
    delay(2000);
    
    // Reiniciar para nuevo pedido
    estadoActual = SELECCIONAR_MESA;
    pedido = "";
    mostrarPantallaSeleccionMesa();
  } else {
    mostrarError("Opcion invalida");
    mostrarMenuOpciones();
  }
}

// ---- MAIN ----
void setup() {
  Serial.begin(9600);
  
  // Inicializar LCD
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Sistema Pedidos");
  lcd.setCursor(0, 1);
  lcd.print("Iniciando...");
  delay(2000);
  
  // Inicializar teclado
  keypadInit();
  sei();
  
  // Mostrar pantalla inicial
  mostrarPantallaSeleccionMesa();
}

void loop() {
  char key = keypadGetKey();
  
  if (key != '\0') {
    Serial.print("Tecla presionada: ");
    Serial.println(key);
    
    switch (estadoActual) {
      case SELECCIONAR_MESA:
        procesarSeleccionMesa(key);
        break;
        
      case SELECCIONAR_MENU:
        procesarSeleccionMenu(key);
        break;
    }
  }
}