
const int LCD_RS = 22; 
const int LCD_EN = 23; 

const int LCD_D0 = 30; 
const int LCD_D1 = 31;
const int LCD_D2 = 32;
const int LCD_D3 = 33;
const int LCD_D4 = 34;
const int LCD_D5 = 35;
const int LCD_D6 = 36;
const int LCD_D7 = 37;


const int DATA_PINS[] = {LCD_D0, LCD_D1, LCD_D2, LCD_D3, LCD_D4, LCD_D5, LCD_D6, LCD_D7};


void shortDelay() {
  delayMicroseconds(50); 
}


void lcd_instruc(uint8_t instruc) {
  
  digitalWrite(LCD_RS, LOW);

  
  for (int i = 0; i < 8; i++) {
    
    digitalWrite(DATA_PINS[i], (instruc >> i) & 0x01);
  }

  
  digitalWrite(LCD_EN, HIGH);
  shortDelay(); 

  
  digitalWrite(LCD_EN, LOW);
  delay(2); 
}


void lcd_escrib(uint8_t data) {
  
  digitalWrite(LCD_RS, HIGH);

  
  for (int i = 0; i < 8; i++) {
    
    digitalWrite(DATA_PINS[i], (data >> i) & 0x01);
  }

  
  digitalWrite(LCD_EN, HIGH);
  shortDelay(); 

  
  digitalWrite(LCD_EN, LOW);
  delayMicroseconds(50); 
}





void setup() {
  
  pinMode(LCD_RS, OUTPUT);
  pinMode(LCD_EN, OUTPUT);
  for (int i = 0; i < 8; i++) {
    pinMode(DATA_PINS[i], OUTPUT);
  }

  

  
  
  delay(150); 
  lcd_instruc(0x30); 

  
  
  lcd_instruc(0x0C); 

  
  
  lcd_instruc(0x01); 

  
  
  lcd_instruc(0x06); 
  
  
}





void loop() {
  

  
  
  
  
  
  
  lcd_instruc(0x80); 

  lcd_escrib('H'); 

  lcd_escrib('O'); 

  lcd_escrib('L'); 

  lcd_escrib('A'); 

  lcd_escrib(' '); 

  lcd_escrib('M'); 
 
  lcd_escrib('U'); 
 
  lcd_escrib('N'); 
 
  lcd_escrib('D'); 
 
  lcd_escrib('O'); 

 
  
 
  while (true);
}