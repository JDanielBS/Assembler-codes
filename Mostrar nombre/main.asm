.org 0x0000
start:
	; PUERTOS
    ser r17
	out ddrf, r17
	sts ddrk, r17
	ldi r18, 0     ; contador

	; LISTA
	ldi r20, 1   ; SALTO LISTA
	ldi ZL, LOW(catodo_comun<<1)
	ldi ZH, HIGH(catodo_comun<<1)
 
ciclo:
	call decodificador
	call delay
	cpi r18, 0b00000110
	breq reseteo
	rjmp ciclo

decodificador:
	clr r17
	LPM R17, Z+       
	LPM R23, Z+        

	COM R17         
	COM R23         

	OUT PORTf, R17     ; Salida de la parte baja
	sts PORTk, R23     ; Salida de la parte alta

	inc r18            
	ret

reseteo:
	ldi ZH, HIGH(catodo_comun<<1)
	ldi ZL, LOW(catodo_comun<<1)
	clr r18
	rjmp ciclo

delay:
    ldi  r20, 70
    ldi  r22, 0
loop_init:
	dec r20
	ldi  r21, 201
	brne loop_medium
	ret
loop_medium:
	dec r21
	breq loop_init
loop_low:
	dec r22
	breq loop_medium
	brne loop_low


.org 0x150
catodo_comun:
; Letras L, I, C, E, T, H para display de 14 segmentos cátodo común
    .dw 0b0000000000111000    ; L
	;		NMLKJHGPFEDCBA
    .dw 0b0000100010001001    ; I
    .dw 0b0000000000111001    ; C
    .dw 0b0010001000111001    ; E
    .dw 0b0000100010000001    ; T
	;       NMLKJHGPFEDCBA
    .dw 0b0010001000110110    ; H
	;       NMLKJHGPFEDCBA