start:
    ldi r17, 0b00001111  ; Configura los 4 bits bajos de PORTC como entrada
    ldi r18, 0b11110000  ; Configura los 4 bits altos de PORTB como salida
    ldi r19, 0b11111100
    out ddrc, r17
    out ddra, r19
    out ddrb, r18
    ser r21
    ldi r23, 2
    ldi r24, 1
    ldi r25, 1

comparar:
	clr r18
	clr r19
    in r17, pinc        ; Lee sensores en r17
    cpi r17, 0          ; Si no hay agua, bomba debe activarse
    breq bomba_agua     
    cpi r17, 1          ; Si el primer bit es 1, activa bomba de inmediato
    breq bomba_agua     
    brne cambiar_valor

cambiar_valor:
	ldi r25, 1
	ldi r20, 1
	rjmp nivel

bomba_agua:
	cpi r25, 0
	mov r18, r17
	breq mostrar_nivel
    ldi r23, 0b00000001  ; Inicializa el bit de llenado en 1
    ldi r25, 0
llenando:
	or r19, r23          ; Agrega el bit actual al nivel de agua
	out PORTA, r24       ; Indica el estado de la bomba
    call delay
	out PORTB, r19       ; Muestra el nivel en los LEDs
	cpi r19, 0b00001111  ; ¿Ya está lleno el depósito?
	breq mostrar_nivel   ; Si es así, salir y volver a monitoreo
    lsl r23              ; Desplaza a la izquierda para llenar más
    rjmp llenando        ; Repite el proceso

nivel:
    cpi r17, 0
    breq mostrar_nivel
    lsr r17
    inc r18
	or r19, r20
    lsl r20
    rjmp nivel

mostrar_nivel:
    mov r21, r18
    cpi r21, 1
    breq nivel_maximo
	cpi r21, 0
    breq nivel_maximo
	out PORTB, r19  ; Muestra el nivel en PORTB
    cpi r21, 4
    breq nivel_maximo
    rjmp comparar   ; Repite la lectura


nivel_maximo:
    ldi r23, 2  ; Contador del bucle (2 iteraciones)

parpadeo:
    ldi r22, 2
    out PORTA, r22   ; Enciende el LED
    call delay_100ms
	clr r22
    out PORTA, r22  ; Apaga el LED (usando el registro cero)
    call delay_100ms
    dec r23          ; Decrementa el contador
    brne parpadeo    ; Si no es 0, repite el ciclo

    rjmp comparar    ; Continua con la siguiente parte del programa




delay:
    ldi  r26, 82
    ldi  r27, 43
    ldi  r28, 0
L1: dec  r28
    brne L1
    dec  r27
    brne L1
    dec  r26
    brne L1
    lpm
    nop
	ret

delay_100ms:
    ldi  r26, 15
    ldi  r27, 15
L2: dec  r27
    brne L2
    dec  r26
    brne L2
	ret

