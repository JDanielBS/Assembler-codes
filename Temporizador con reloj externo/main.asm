.include "m2560def.inc"

;===============================================================
; PROGRAMA PRINCIPAL
;===============================================================
.cseg
.org 0x0000
    rjmp    setup

.org 0x00A0 ; Inicio seguro del código después de la tabla de vectores
setup:
    ; Inicializar Stack Pointer
    ldi     r16, high(RAMEND)
    out     SPH, r16
    ldi     r16, low(RAMEND)
    out     SPL, r16

    ; --- Configuración de Puertos ---
    ser     r16
    out     DDRF, r16       ; Puerto F como salida para el display
    
    ; --- Configura el pin T5 (PL2) como entrada con pull-up (FORMA CORRECTA) ---
    ; Limpiar el bit 2 de DDRL para hacerlo entrada
	clr r16
	sts DDRL, r16

    ; --- Configuración de Registros ---
    clr     r22             ; Registro temporal para tabla de 7 segmentos

    ; --- Configuración del Timer5 como Contador de Pulsos Externos ---
    ldi     r16, (1<<CS52) | (1<<CS51)
    sts     TCCR5B, r16

main_loop:
    rcall   actualizar_display
    rjmp    main_loop

;===============================================================
; SUBRUTINAS
;===============================================================
actualizar_display:
    lds     r21, TCNT5L
    cpi     r21, 16
    brlo    mostrar_valor
    clr     r21
    sts     TCNT5L, r21

mostrar_valor:
    rcall   buscar_en_tabla
    out     PORTF, r17
    ret

buscar_en_tabla:
    ldi     ZL, low(tabla_segmentos << 1)
    ldi     ZH, high(tabla_segmentos << 1)
    add     ZL, r21
    adc     ZH, r22
    lpm     r17, Z
    ret

;===============================================================
; TABLA DE DATOS
;===============================================================
tabla_segmentos:
    .db 0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78
    .db 0x00, 0x18, 0x08, 0x03, 0x46, 0x21, 0x06, 0x0E