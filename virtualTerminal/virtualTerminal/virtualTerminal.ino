#define F_CPU 16000000UL  // Frecuencia de reloj típica de Arduino Mega
#define BAUD_RATE 9600

volatile uint8_t received_byte = 0;
volatile uint8_t data_ready = 0;

void setup() {
  // Calcula y configura el divisor para el baudrate
  uint16_t ubrr_value = (F_CPU / (16UL * BAUD_RATE)) - 1;
  UBRR2H = (ubrr_value >> 8) & 0xFF;
  UBRR2L = ubrr_value & 0xFF;

  // Habilita RXCIE2 (recepción), UDRIE2 (para transmisión) solo cuando quieras usar interrupción de transmisión
  // Habilita RXEN2 y TXEN2 para activar receptor y transmisor
  UCSR2B = (1 << RXEN2) | (1 << TXEN2) | (1 << RXCIE2);

  // 8 datos, 1 stop bit, sin paridad, modo asíncrono
  UCSR2C = (1 << UCSZ21) | (1 << UCSZ20); // UCSZ2X: 011 -> 8 bits

  // Habilita interrupciones globales
  sei();
}

void loop() {
  // Si se recibió un dato, reenvía el mismo carácter (echo)
  if(data_ready) {
    data_ready = 0;
    // Espera buffer vacío, luego reenvía
    while (!(UCSR2A & (1 << UDRE2)));
    UDR2 = received_byte;
  }
}

// Rutina de interrupción de recepción
ISR(USART2_RX_vect) {
  received_byte = UDR2;  // Lee dato recibido
  data_ready = 1;        // Señaliza que hay dato disponible
}