;
; Verificar si esta en rango.asm
;
; Created: 22/08/2025 3:00:17 p. m.
; Author : FB0100
;


; Verificar si el contenido de la posición de memoria 200h se encuentra en el rango min < 200h < max ...
start:
    lds r16, 0x0200
	lds r17, 0x0201
	lds r18, 0x0202	
	cp r16, r17
	brlo NO_CUMPLE
	breq NO_CUMPLE
	cp r16, r18
	brsh NO_CUMPLE
	ser r20
	sts 0x0203, r20
    rjmp start

NO_CUMPLE:
	clr r20
	sts 0x0203, r20
	rjmp start