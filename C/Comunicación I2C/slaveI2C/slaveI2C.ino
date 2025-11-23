#include <Wire.h>
#include <LiquidCrystal.h>

// Pines del LCD en el Mega (modo 8 bits)
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

String bufferOperacion = "";
String resultado = "";

// Procesa una operación tipo "12+3"
String calcular(String op) {
  int pos;
  long a, b;

  if ((pos = op.indexOf('+')) != -1) {
    a = op.substring(0, pos).toInt();
    b = op.substring(pos + 1).toInt();
    return String(a + b);
  }
  if ((pos = op.indexOf('-')) != -1) {
    a = op.substring(0, pos).toInt();
    b = op.substring(pos + 1).toInt();
    return String(a - b);
  }
  if ((pos = op.indexOf('*')) != -1) {
    a = op.substring(0, pos).toInt();
    b = op.substring(pos + 1).toInt();
    return String(a * b);
  }
  if ((pos = op.indexOf('/')) != -1) {
    a = op.substring(0, pos).toInt();
    b = op.substring(pos + 1).toInt();
    if (b == 0) return "ERR";
    return String((float)a / (float)b);
  }
  return "ERR";
}

// Cuando el maestro envía una tecla
void recibirEvento(int cuantos) {
  while (Wire.available()) {
    char c = Wire.read();

    if (c == '=') {
      resultado = calcular(bufferOperacion);
      bufferOperacion = "";

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Resultado:");
      lcd.setCursor(0, 1);
      lcd.print(resultado);
    } else {
      bufferOperacion += c;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(bufferOperacion);
    }
  }
}

// Cuando el maestro pide el resultado
void solicitarEvento() {
  Wire.write(resultado.c_str());
}

void setup() {
  lcd.begin(16, 2);

  Wire.begin(8); // Dirección del esclavo
  Wire.onReceive(recibirEvento);
  Wire.onRequest(solicitarEvento);

  lcd.clear();
  lcd.print("Esclavo Listo...");
}

void loop() {
  // Todo ocurre por interrupciones I²C
}

