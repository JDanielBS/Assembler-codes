#include "LCD.h"

const int DATA_PINS[] = {30, 31, 32, 33, 34, 35, 36, 37};
LCD lcd(22, 23, DATA_PINS);

void setup() {
    lcd.begin();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Hola Mundo");
}

void loop() {
   
}