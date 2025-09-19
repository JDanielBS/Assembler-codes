.cseg
.org 0x0000
    rjmp start          
.org 0x0004
    rjmp PCINT0_ISR     ; Vector de PCINT0 (PCINT0..7, es decir PORTB)

.org 0x150
SEGMENT_TABLE: .db 0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x18,0x08,0x03,0x46,0x21,0x06,0x0E

start:
    ; Configuración de PORTF como salida (display 7 segmentos)
    ldi r16, 0xFF
    out DDRF, r16      
    clr r21         

      ; Configurar PB7 (PCINT7) como entrada con pull-up
    ldi r16, 0x00
    out DDRB, r16          ; Todo PORTB como entrada
    ldi r16, 0b10000000
    out PORTB, r16         ; Pull-up en PB7

    ; Habilitar interrupción por cambio de pin (PCINT7 en PB7)
    ldi r16, 0b00000001
    sts PCICR, r16         ; Habilita grupo PCINT0 (PCINT0..7)

    ldi r16, 0b10000000
    sts PCMSK0, r16        ; Habilita solo PCINT7 (PB7)

    sei


main_loop:
    cpi r21, 0x10
    brlo sigue
    clr r21            

sigue:
    ldi ZL, low(SEGMENT_TABLE<<1)
    ldi ZH, high(SEGMENT_TABLE<<1)

    clr r22
    add ZL, r21
    adc ZH, r22

    lpm r17, Z
    out PORTF, r17

    rjmp main_loop      

;--------------------------
; Rutina de Interrupción PCINT0 (PORTB, PCINT0..7)
;--------------------------

PCINT0_ISR:
    inc r21             ; Incrementa el contador
    reti

