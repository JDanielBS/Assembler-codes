.cseg
.org 0x0000
    rjmp start

.org OC1Aaddr          
    rjmp TIMER1_ISR

.org 0x00B6   ; Dirección de la ISR de captura del Timer3
    rjmp TIMER1_ISR

.org 0x0002  
rjmp TIMER1_ISR

.org 0x150
SEGMENT_TABLE: .db 0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x18,0x08,0x03,0x46,0x21,0x06,0x0E

start:

    ldi r16, 0xFF
    out DDRF, r16       
    clr r21             

    ; Configuración del Timer1 en modo CTC
    ldi r16, high(156240)
    sts OCR1AH, r16
    ldi r16, low(156240)
    sts OCR1AL, r16

    ldi r16, (1 << WGM12) | (1 << CS12) | (1 << CS10)
    sts TCCR1B, r16

    ldi r16, (1 << OCIE1A)
    sts TIMSK1, r16

    ; ===================== CONFIGURACIÓN TIMER3 =====================
    ; Timer3 en modo normal con prescaler 1024
    ldi r16, (1 << CS32) | (1 << CS30)
    sts TCCR3B, r16

    ; Captura por flanco ascendente (ICES3)
    ldi r17, (1 << ICES3)
    or r17, r16
    sts TCCR3B, r17

    ; Activar interrupción de captura de Timer3
    ldi r16, (1 << ICIE3)
    sts TIMSK3, r16
    ; ===============================================================

    ; Configurar INT0
    ldi r16, 0x01 
    out EIMSK, r16
    ldi r16, 0x03 ; Flanco de subida
    sts EICRA, r16


    sei                 

main_loop:
    cpi r21, 0x10       
    brlo sigue
    clr r21             

sigue:
    ldi ZL, low(SEGMENT_TABLE << 1)
    ldi ZH, high(SEGMENT_TABLE << 1)

    clr r22
    add ZL, r21
    adc ZH, r22

    lpm r17, Z
    out PORTF, r17

    rjmp main_loop

TIMER1_ISR:
    inc r21
    reti
