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

// ----- MENÚS -----
const String menuEntradas[3] = {"1.Ensalada", "2.Sopa", "3.Ceviche"};
const String menuPlatoFuerte[3] = {"1.Carne", "2.Pollo", "3.Pescado"};
const String menuBebidas[3] = {"1.Jugo", "2.Gaseosa", "3.Agua"};
const String menuPostres[3] = {"1.Helado", "2.Pastel", "3.Flan"};

// ----- ESTRUCTURA DE PEDIDO -----
struct ItemPedido {
  String nombre;
  int cantidad;
};

const int MAX_ITEMS = 50;
struct Pedido {
  String mesa;
  ItemPedido items[MAX_ITEMS];
  int totalItems;
  
  void inicializar() {
    mesa = "";
    totalItems = 0;
  }
  
  void agregarItem(String nombre, int cantidad) {
    if (totalItems < MAX_ITEMS) {
      items[totalItems].nombre = nombre;
      items[totalItems].cantidad = cantidad;
      totalItems++;
    }
  }
  
  void mostrarPedido() {
    Serial.println("\n========================================");
    Serial.println("          PEDIDO COMPLETO");
    Serial.println("========================================");
    Serial.print("Mesa: ");
    Serial.println(mesa);
    Serial.println("----------------------------------------");
    
    if (totalItems == 0) {
      Serial.println("(Sin items)");
    } else {
      for (int i = 0; i < totalItems; i++) {
        Serial.print(i + 1);
        Serial.print(". ");
        Serial.print(items[i].nombre);
        Serial.print(" x");
        Serial.println(items[i].cantidad);
      }
    }
    
    Serial.println("========================================");
    Serial.print("Total de items: ");
    Serial.println(totalItems);
    Serial.println("========================================\n");
  }
};

Pedido pedido;

// ----- VARIABLES DEL SISTEMA -----
enum Estado {
  SELECCIONAR_MESA,
  SELECCIONAR_CATEGORIA,
  MOSTRAR_MENU,
  SELECCIONAR_CANTIDAD
};

Estado estadoActual = SELECCIONAR_MESA;
String inputMesa = "";
char categoriaActual = 'A';
String opcionSeleccionada = "";
String inputCantidad = "";

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
}

void mostrarCategoriasMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Elija el menu");
  lcd.setCursor(0, 1);
  lcd.print("A B C D");
}

void mostrarMenuEspecifico(char categoria) {
  lcd.clear();
  
  String titulo = "";
  const String* menuActual;
  
  switch(categoria) {
    case 'A':
      titulo = "ENTRADAS";
      menuActual = menuEntradas;
      break;
    case 'B':
      titulo = "PLATO FUERTE";
      menuActual = menuPlatoFuerte;
      break;
    case 'C':
      titulo = "BEBIDAS";
      menuActual = menuBebidas;
      break;
    case 'D':
      titulo = "POSTRES";
      menuActual = menuPostres;
      break;
  }
  
  lcd.setCursor(0, 0);
  lcd.print(titulo);
  lcd.setCursor(0, 1);
  lcd.print("1 2 3  *=Fin");
  
  Serial.println("--- " + titulo + " ---");
  for(int i = 0; i < 3; i++) {
    Serial.println(menuActual[i]);
  }
  Serial.println("*: Finalizar pedido");
}

String obtenerNombreOpcion(char categoria, char opcion) {
  int index = opcion - '1';
  
  switch(categoria) {
    case 'A': return menuEntradas[index];
    case 'B': return menuPlatoFuerte[index];
    case 'C': return menuBebidas[index];
    case 'D': return menuPostres[index];
  }
  return "";
}

void mostrarSeleccionCantidad(String nombreOpcion) {
  lcd.clear();
  lcd.setCursor(0, 0);
  String nombreLimpio = nombreOpcion.substring(2);
  lcd.print(nombreLimpio);
  lcd.setCursor(0, 1);
  lcd.print("Cant: ");
}

void finalizarPedido() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Pedido completo");
  lcd.setCursor(0, 1);
  lcd.print("Mesa: " + pedido.mesa);
  delay(2000);
  
  // Mostrar pedido en Serial
  pedido.mostrarPedido();
  
  // Reiniciar sistema
  pedido.inicializar();
  estadoActual = SELECCIONAR_MESA;
  inputMesa = "";
  inputCantidad = "";
  categoriaActual = 'A';
  
  mostrarPantallaSeleccionMesa();
}

void procesarSeleccionMesa(char key) {
  if (key >= '0' && key <= '9') {
    inputMesa += key;
    lcd.setCursor(0, 1);
    lcd.print("Mesa: " + inputMesa + "  ");
    Serial.print("Input actual: ");
    Serial.println(inputMesa);
  }
  else if (key == '#') {
    if (inputMesa.length() > 0) {
      pedido.mesa = inputMesa;
      Serial.print("Mesa confirmada: ");
      Serial.println(pedido.mesa);
      
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Mesa " + pedido.mesa);
      lcd.setCursor(0, 1);
      lcd.print("Confirmada!");
      delay(1500);
      
      estadoActual = SELECCIONAR_CATEGORIA;
      mostrarCategoriasMenu();
      inputMesa = "";
    } else {
      mostrarError("Ingrese mesa");
      mostrarPantallaSeleccionMesa();
    }
  }
  else if (key == '*') {
    if (inputMesa.length() > 0) {
      inputMesa.remove(inputMesa.length() - 1);
      lcd.setCursor(0, 1);
      lcd.print("Mesa: " + inputMesa + "  ");
    }
  }
  else if (key == 'A' || key == 'B' || key == 'C' || key == 'D') {
    mostrarError("Solo numeros");
    mostrarPantallaSeleccionMesa();
    lcd.setCursor(0, 1);
    lcd.print("Mesa: " + inputMesa);
  }
}

void procesarSeleccionCategoria(char key) {
  if (key == 'A' || key == 'B' || key == 'C' || key == 'D') {
    categoriaActual = key;
    estadoActual = MOSTRAR_MENU;
    mostrarMenuEspecifico(categoriaActual);
    
    Serial.print("Categoría seleccionada: ");
    Serial.println(key);
  } else {
    mostrarError("Opcion invalida");
    mostrarCategoriasMenu();
  }
}

void procesarMenuEspecifico(char key) {
  // Finalizar pedido con *
  if (key == '*') {
    if (pedido.totalItems > 0) {
      finalizarPedido();
    } else {
      mostrarError("Pedido vacio");
      mostrarMenuEspecifico(categoriaActual);
    }
  }
  // Cambiar a otra categoría
  else if (key == 'A' || key == 'B' || key == 'C' || key == 'D') {
    categoriaActual = key;
    mostrarMenuEspecifico(categoriaActual);
    Serial.print("Cambio a categoría: ");
    Serial.println(key);
  }
  // Seleccionar opción del menú
  else if (key >= '1' && key <= '3') {
    opcionSeleccionada = obtenerNombreOpcion(categoriaActual, key);
    estadoActual = SELECCIONAR_CANTIDAD;
    mostrarSeleccionCantidad(opcionSeleccionada);
    
    Serial.print("Opción seleccionada: ");
    Serial.println(opcionSeleccionada);
  }
  // Opción inválida
  else if (key >= '4' && key <= '9' || key == '0') {
    mostrarError("Opcion no valida");
    mostrarMenuEspecifico(categoriaActual);
  }
}

void procesarSeleccionCantidad(char key) {
  // Ingresar cantidad
  if (key >= '0' && key <= '9') {
    inputCantidad += key;
    lcd.setCursor(6, 1);
    lcd.print(inputCantidad + "  ");
    
    Serial.print("Cantidad ingresada: ");
    Serial.println(inputCantidad);
  }
  // Confirmar con *
  else if (key == '*') {
    if (inputCantidad.length() > 0 && inputCantidad.toInt() > 0) {
      // Agregar item al pedido
      pedido.agregarItem(opcionSeleccionada, inputCantidad.toInt());
      
      Serial.println("--- Item agregado ---");
      Serial.print("Mesa: ");
      Serial.println(pedido.mesa);
      Serial.print("Item: ");
      Serial.println(opcionSeleccionada);
      Serial.print("Cantidad: ");
      Serial.println(inputCantidad);
      Serial.print("Total items en pedido: ");
      Serial.println(pedido.totalItems);
      Serial.println("---------------------");
      
      // Mostrar confirmación
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Agregado!");
      lcd.setCursor(0, 1);
      lcd.print("Items: " + String(pedido.totalItems));
      delay(1500);
      
      // Volver al menú actual
      inputCantidad = "";
      estadoActual = MOSTRAR_MENU;
      mostrarMenuEspecifico(categoriaActual);
    } else {
      mostrarError("Cantidad invalida");
      mostrarSeleccionCantidad(opcionSeleccionada);
      lcd.setCursor(6, 1);
      lcd.print(inputCantidad);
    }
  }
  // Borrar último dígito con #
  else if (key == '#') {
    if (inputCantidad.length() > 0) {
      inputCantidad.remove(inputCantidad.length() - 1);
      lcd.setCursor(6, 1);
      lcd.print(inputCantidad + "  ");
    }
  }
}

// ---- MAIN ----
void setup() {
  Serial.begin(9600);
  
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Sistema Pedidos");
  lcd.setCursor(0, 1);
  lcd.print("Iniciando...");
  delay(2000);
  
  keypadInit();
  sei();
  
  pedido.inicializar();
  mostrarPantallaSeleccionMesa();
}

void loop() {
  char key = keypadGetKey();
  
  if (key != '\0') {
    Serial.print("Tecla: ");
    Serial.println(key);
    
    switch (estadoActual) {
      case SELECCIONAR_MESA:
        procesarSeleccionMesa(key);
        break;
        
      case SELECCIONAR_CATEGORIA:
        procesarSeleccionCategoria(key);
        break;
        
      case MOSTRAR_MENU:
        procesarMenuEspecifico(key);
        break;
        
      case SELECCIONAR_CANTIDAD:
        procesarSeleccionCantidad(key);
        break;
    }
  }
}