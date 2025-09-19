; Replace with your application code
start:
    ldi r30, low(0x0202)
    ldi r31, high(0x0202)
	ldi r26, low(0x0212)
    ldi r27, high(0x0212)
	lds r18, 0x212
	sts 0x21f, r18

loop:
    RCALL CARGAR_VALORES_PUNTEROS
    RCALL CONVERSION_A_MAYUSCULA
	RCALL COMPARACIONES
    brne loop

CARGAR_VALORES_PUNTEROS:
	ld   r16, Z+
	ld   r17, X+
	RET

CONVERSION_A_MAYUSCULA:
	andi r16, 0b11011111
	andi r17, 0b11011111 
	RET

COMPARACIONES:
	cp r16, r17
	brne DIFERENTE

	cpi  r16, 0x00
	breq PALABRA_ACABO
	RET

PALABRA_ACABO:
	ld r16, -Z
	ld r16, -Z
	cpi r16, 0x00
	breq ERROR
    rjmp IGUAL

ERROR:
	ldi r18, '-'
	ldi r19, '-'
	RCALL ESCRIBIR_RESULTADO
	rjmp start

DIFERENTE:
	ldi r18, 'N'
	ldi r19, 'O'
	RCALL ESCRIBIR_RESULTADO
	rjmp start

IGUAL:
	ldi r18, 'S'
	ldi r19, 'I'
	RCALL ESCRIBIR_RESULTADO
	rjmp start


ESCRIBIR_RESULTADO:
	sts 0x022C, r18
	sts 0x022D, r19
	RET