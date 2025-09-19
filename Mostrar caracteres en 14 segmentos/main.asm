; Se debe hacer un programa donde se carguen los valores de A a F 
; en memoria, se busque el caracter en Ascii  en la tabla de la memoria flash
; y se muestre  en un display de 14 segmentos.
; el 0 hace que el programa se detenga.

.org 0x150
tabla:	.dw 0b0001110111001000 ; A
		.dw 0b0011010101110000 ; B
		.dw 0b0011111111000110 ; C
		.dw 0b0011011101110000 ; D
		.dw 0b0001110111000110 ; E
		.dw 0b0001110111001110 ; F
	          ; NMLKJHGPFEDCBA

.org 0x00
conf:
	ser r16
	out DDRF, r16
	sts DDRk, r16
	ldi r16, 'a' 
	sts 0x200, r16
	ldi r16, 'B'
	sts 0x201, r16
	ldi r16, 'd'
	sts 0x202, r16
	ldi r16, 'C'
	sts 0x203, r16
	clr r16

reset:
	ldi XL, low(0x0200)
    ldi XH, high(0x0200)

output:
	ld r16, X+
	cpi r16, 0x00
	breq reset

	; Obtenemos el dato, lo pasamos a mayúscula y obtenemos un 'desplazamiento'
	; para poder ubicar en memoria de programa la letra
    andi  r16, 0b11011111  
	subi r16, 'A'

	; Obtenemos posición de la tabla y le sumamos el desplazamiento (el cual se multiplica por 2
	; para obtener la posición en memoria de programa, ya que cada letra ocupa dos bytes)
	ldi r30, low(tabla*2)
    ldi r31, high(tabla*2)
	lsl r16
    add r30, r16
    adc r31, r1

	; Leemos los bits de la posición en la tabla y los mostramos
	lpm r17, Z+
    lpm r18, Z
	out PORTf, r17
	sts PORTk, r18

	call delay
	rjmp output

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