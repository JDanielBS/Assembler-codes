/*
 * CÓDIGO PARA POTENCIÓMETRO Y 2 DISPLAYS 7-SEGMENTOS
 * - Lee un potenciómetro (0-1023).
 * - Mapea el valor a un rango de 0.0 a 9.9.
 * - Muestra la parte entera en el Display 1.
 * - Muestra la parte decimal en el Display 2.
 * - Usa multiplexación en un Arduino Mega 2560.
 * - PINES DE SEGMENTO: Puerto F (A0-A6)
 * - TIPO DE DISPLAY: Ánodo Común (LOW = Encendido)
 */


const int SEGMENT_PINS[] = {A0, A1, A2, A3, A4, A5, A6};

const int DISPLAY1_PIN = 10;
const int DISPLAY2_PIN = 11;

const int POT_PIN = A8;


const byte TABLA_7SEG_ANODO[10] = {
  B11000000,
  B11111001,
  B10100100,
  B10110000,
  B10011001,
  B10010010,
  B10000010,
  B11111000,
  B10000000,
  B10010000, 
};

int parteEntera = 0;
int parteDecimal = 0;



/*
 * Escribe un patrón de bits en los 7 pines de segmento.
 * Esta función es "tonta", solo obedece el patrón que se le envía.
 */
void escribirPatron(byte patron) {
  for (int i = 0; i < 7; i++) {
  
    bool bitEstaEncendido = (patron >> i) & 0x01;
    digitalWrite(SEGMENT_PINS[i], bitEstaEncendido);
  }
}



void setup() {

  for (int i = 0; i < 7; i++) {
    pinMode(SEGMENT_PINS[i], OUTPUT);
  }


  pinMode(DISPLAY1_PIN, OUTPUT);
  pinMode(DISPLAY2_PIN, OUTPUT);



  digitalWrite(DISPLAY1_PIN, HIGH);
  digitalWrite(DISPLAY2_PIN, HIGH);
  

  pinMode(POT_PIN, INPUT);


  Serial.begin(9600);
}



void loop() {
  

  

  int valorADC = analogRead(POT_PIN);
  

  int valorMapeado = map(valorADC, 0, 1023, 0, 50);
  

  parteEntera = valorMapeado / 10;
  parteDecimal = valorMapeado % 10;


  Serial.print("Valor: ");
  Serial.print(parteEntera);
  Serial.print(".");
  Serial.println(parteDecimal);



  

  digitalWrite(DISPLAY2_PIN, HIGH);     
  escribirPatron(TABLA_7SEG_ANODO[parteEntera]);

  digitalWrite(DISPLAY1_PIN, LOW);      
  delay(5);                             
  

  digitalWrite(DISPLAY1_PIN, HIGH);     
  escribirPatron(TABLA_7SEG_ANODO[parteDecimal]);
  
  digitalWrite(DISPLAY2_PIN, LOW);      
  delay(5);                             
}