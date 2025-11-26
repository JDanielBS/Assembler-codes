start:
    ldi r30, low(0x0200)
    ldi r31, high(0x0200)
    ldi r22, 6    

loop:
    ld   r16, Z         
    andi r16, 0b11011111 
    cpi  r16, 0x41     
    brlo NO_ES_LETRA
    cpi  r16, 0x5B      
    brsh NO_ES_LETRA
    st   Z+, r16         
    dec  r22           
    brne loop           

NO_ES_LETRA:
    rjmp start