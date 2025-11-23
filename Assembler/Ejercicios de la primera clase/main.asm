;
; first_exercises.asm
;
; Created: 20/08/2025 10:09:51 a. m.
; Author : FB0100
;

; Tomar los datos de las posiciones de memoria 200h y 201h. los van a sumar y poner el resultado en la posición 202h
start1:
	lds r16, 0x0200
	lds r17, 0x0201
	add r16, r17
	sts 0x0202, r16
	rjmp start1

; Eliminar nibble menos significativo
start2:
	lds r16, 0x0200
	andi r16, 0b11110000
	sts 0x200, r16
	rjmp start2

; Nibble menos significativo intacto y el más significativo F
start3:
	lds r16, 0x0200
	ori r16, 0b11110000
	sts 0x0200, r16
	rjmp start3

; Nibble más significativo intacto y nibble menos significativo el complemento
start4:
	lds r16, 0x0200
	ldi r17, 0b00001111
	eor r16, r17
	sts 0x0202, r16
	rjmp start3

; Comparar las posiciones de memoria 200h y 201h y almacenar el mayor número en la posición 202h
start5:
	lds r16, 0x0200
	lds r17, 0x0201
	cp r16, r17
	brge mayor
	sts 0x0202, r17
	rjmp start5

mayor:
	sts 0x0202, r16
	rjmp start5