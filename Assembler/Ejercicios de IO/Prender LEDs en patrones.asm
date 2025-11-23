;
; IO exercises.asm
;
; Created: 29/08/2025 2:33:52 p. m.
; Author : FB0100
;

start:
	; Ponemos pin f en entrada
    ldi r16, 0x00
	out ddrf, r16
	; Ponemos pin k en salida
	ldi r17, 0xff
	sts ddrk, r17
	ldi r19, 0b00010000
	ldi r20, 0b00001000
	ldi r21, 0xff
	ldi r22, 0b10000000
	ldi r23, 0xff

select:
	in r16, pinf
	cpi r16, 0x01
	breq PARPADEO
	cpi r16, 0x02
	breq RECORRIDO
	cpi r16, 0x03
	breq IZQ_DER
	rjmp select

PARPADEO:
	; Patrón de parpadeo de los 8 bits
	ldi r18, 0xff
	sts portk, r18
	ldi r18, 0x00
	sts portk, r18
	rjmp select

RECORRIDO:
	; Patrón de recorrido del centro hacia fuera y de regreso
	ldi r18, 0x00
	eor r18, r19
	eor r18, r20
	sts portk, r18
	cpi r18, 0b10000001
	breq CAMBIO_FLAG
	cpi r18, 0b00011000
	breq CAMBIO_FLAG
	rjmp MOVER

CAMBIO_FLAG:
	com r21
	rjmp MOVER

MOVER:
	cpi r21, 0x00
	breq RESET_ADENTRO
	brne RESET_AFUERA

RESET_ADENTRO:
	lsl r19
	lsr r20
	rjmp select

RESET_AFUERA:
	lsl r20
	lsr r19
	rjmp select

IZQ_DER:
	; Patrón de izquierda a derecha y de regreso
	ldi r18, 0x00
	eor r18, r22
	sts portk, r18
	cpi r18, 0b10000000
	breq CAMBIO_FLAG_B
	cpi r18, 0b00000001
	breq CAMBIO_FLAG_B
	rjmp MOVER_B

CAMBIO_FLAG_B:
	com r23

MOVER_B:
	SBRS r23, 0
	rjmp MOVER_DER
	lsl r22
	rjmp select

MOVER_DER:
	lsr r22
	rjmp select