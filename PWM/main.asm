;===================================================================
;   CONTROL DE PWM MEDIANTE ENTRADA ANALÓGICA (POTENCIÓMETRO)
;
; DESCRIPCIÓN:
; - Configura el Timer1 en modo Fast PWM de 8 bits (salida en PB5).
; - Configura el ADC para leer una señal analógica en el pin PF0.
; - Lee el valor del ADC, lo ajusta a 8 bits y lo usa para
;   controlar el ciclo de trabajo del PWM.
;
; CONEXIONES:
; - PF0 (ADC0): Entrada analógica. Conecta el pin central de un
;               potenciómetro. Los otros dos pines del potenciómetro
;               van a 5V y GND.
; - PB5 (OC1A): Salida de la señal PWM para el LED.
;===================================================================

.cseg
.org 0x0000
main:
    ; --- 1. CONFIGURACIÓN DE PUERTOS ---
    ; Configura el pin PF0 (ADC0) como entrada
    cbi     ddrf, 0
    ; Configura el pin PB5 (OC1A) como salida
    sbi     ddrb, 5

    ; --- 2. CONFIGURACIÓN DEL ADC ---
    ; Referencia de voltaje en AVCC (REFS0=1)
    ; Resultado ajustado a la izquierda (ADLAR=1) para leer 8 bits fácilmente
    ; Canal de entrada seleccionado: ADC0 (PF0)
    ldi     r16, (1<<REFS0) | (1<<ADLAR)
    sts     ADMUX, r16

    ; Habilita el ADC (ADEN=1)
    ; Prescalador del ADC a 128 (16MHz / 128 = 125kHz)
    ldi     r16, (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0)
    sts     ADCSRA, r16

    ; --- 3. CONFIGURACIÓN DEL TIMER1 PARA FAST PWM (Sin cambios) ---
    ldi     r16, (1<<COM1A1) | (1<<WGM11)
    sts     TCCR1A, r16
    ldi     r16, (1<<WGM13) | (1<<WGM12) | (1<<CS11) | (1<<CS10)
    sts     TCCR1B, r16
    ldi     r16, high(255)
    sts     ICR1H, r16
    ldi     r16, low(255)
    sts     ICR1L, r16

; --- BUCLE PRINCIPAL ---
bucle_principal:
    ; Inicia una nueva conversión del ADC (forma segura para ATmega2560)
    lds     r16, ADCSRA
    ori     r16, (1<<ADSC)
    sts     ADCSRA, r16

espera_conversion:
    ; Revisa si el bit ADSC en el registro r16 está limpio (0).
    ; Si está limpio (conversión terminada), salta la siguiente instrucción.
	lds     r16, ADCSRA
    sbrc    r16, ADSC
    rjmp    espera_conversion

    ; Lee el resultado de 8 bits del registro alto del ADC (ADCH)
    ; y lo carga en el registro de comparación del PWM.
    lds     r17, ADCH
    sts     OCR1AL, r17

    ; Repite el ciclo para leer y actualizar constantemente
    rjmp    bucle_principal