;
; Taller_1_punto_2_sensores.asm
;
; Created: 02/09/2025 21:15:14
; Author : jonat
;
; ontrol de depósito de agua
; PORTF[3:0] = sensores (switches)
; PORTK[3:0] = barra de nivel
; PORTK4     = bomba (ON/OFF)
; PORTK5     = tanque lleno (parpadeo doble)
; Replace with your application code


start:
    ; Configurar Puerto F como entrada (sensores)
    ldi r16, 0x00
    out DDRF, r16

    ; Configurar Puerto K como salida (LEDs + bomba)
    ldi r16, 0xFF
    sts DDRK, r16

    ; Inicializar estado de bomba en 0 (apagada)
    ldi r21, 0x00

main_loop:
    ; Leer sensores
    in r17, PINF
    andi r17, 0x0F        ; Solo 4 bits bajos

    ; Mostrar nivel en barra de LEDs (K0..K3)
    mov r18, r17
    sts PORTK, r18

    ; ----- Control de bomba -----
    ; Nivel máximo -> apagar bomba y parpadear LED max
    cpi r17, 0x0F
    breq bomba_off

    ; Si la bomba ya está encendida, mantenerla
    cpi r21, 1
    breq bomba_keep

    ; Si estamos en nivel mínimo (0001), encender bomba
    cpi r17, 0x01
    breq bomba_on

    ; En cualquier otro caso ? bomba apagada
    rjmp bomba_idle


; ----------- Rutinas de control -----------
bomba_on:
    ldi r21, 1            ; marcar estado ON
    lds r18, PORTK
    ori r18,  0x10       ; encender LED bomba (PK4)
    andi r18, 0b11011111  ; asegurar que PK5 apagado
    sts PORTK, r18
    rjmp main_loop

bomba_keep:
    ; mantener bomba encendida mientras no llegue al máximo
    lds r18, PORTK
    ori r18,  0x10         ; PK4 = ON
    andi r18, 0b11011111  ; PK5 apagado
    sts PORTK, r18
    rjmp main_loop

bomba_off:
    ldi r21, 0            ; marcar estado OFF
    lds r18, PORTK
    andi r18, 0b11101111  ; apagar PK4
    sts PORTK, r18

    ; Parpadear LED máximo en PK5
    lds r18, PORTK
    ldi r19, (1<<5)
    eor r18, r19          ; toggle PK5
    sts PORTK, r18

    rcall delay
    rjmp main_loop

bomba_idle:
    ; bomba apagada sin parpadeo (PK4 y PK5 = 0)
    lds r18, PORTK
    andi r18, 0b11001111
    sts PORTK, r18
    rjmp main_loop


; ---------- Rutina de retardo ----------
delay:
    ldi r19, 100
d1: ldi r20, 255
d2: dec r20
    brne d2
    dec r19
    brne d1
    ret