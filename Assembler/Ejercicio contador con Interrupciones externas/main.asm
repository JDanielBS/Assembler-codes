.cseg
.org 0x0000
    rjmp start          
.org 0x0006
    rjmp INT0_ISR       







start:
    ; Configuración de puertos
    ldi r16, 0xFF
    out DDRF, r16      
    clr r21         
	
	
    ldi r16, 0x00
    out DDRD, r16       


    ldi r16, 0x00
    out PORTD, r16   

    ; Configurar INT2
    ldi r16, 0x04 
    out EIMSK, r16
    ldi r16, 0x03 ; Flanco de subida
    sts EICRA, r16
	
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


INT0_ISR:
    inc r21             
    reti



SEGMENT_TABLE: .db 0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x18,0x08,0x03,0x46,0x21,0x06,0x0E