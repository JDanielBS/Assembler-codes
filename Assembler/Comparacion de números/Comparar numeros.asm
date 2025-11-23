;
; Escribir palabra.asm
;
; Created: 22/08/2025 2:07:14 p. m.
; Author : FB0100
;


; Comparar si dos números son iguales o mayores
start:
	lds r16, 0x0200
	lds r17, 0x0201
	cp r16, r17
	breq IGUAL
	brlt MENOR
	brge MAYOR
	rjmp start

IGUAL:
	ldi r18, 'I'
	ldi r19, 'G'
	ldi r20, 'U'
	ldi r21, 'A'
	ldi r22, 'L'
	rjmp GUARDAR

MENOR:
	ldi r18, 'M'
	ldi r19, 'E'
	ldi r20, 'N'
	ldi r21, 'O'
	ldi r22, 'R'
	rjmp GUARDAR

MAYOR:
	ldi r18, 'M'
	ldi r19, 'A'
	ldi r20, 'Y'
	ldi r21, 'O'
	ldi r22, 'R'
	rjmp GUARDAR

GUARDAR:
	sts 0x0202, r18
	sts 0x0203, r19
	sts 0x0204, r20
	sts 0x0205, r21
	sts 0x0206, r22
	rjmp start