;
; AssemblerApplication11.asm
;
; Created: 9/3/2025 13:48:22
; Author : Liceth Usma
;


; Replace with your application code
;
; AssemblerApplication9.asm
;
; Created: 8/3/2025 21:22:44
; Author : Liceth Usma
;


; Replace with your application code
;
; AssemblerApplication8.asm
;
; Created: 8/3/2025 04:28:45
; Author : Liceth Usma
;


; Replace with your application code
;
; AssemblerApplication6.asm
;
; Created: 5/3/2025 00:13:57
; Author : Liceth Usma
;


; Replace with your application code
start:
    ldi r17, 0b00001111  
    ldi r18, 0b11110000  
    ldi r19, 0b11111100
    out ddrf, r17
    out ddra, r19
    sts ddrk, r18
    ser r21
    ldi r23, 2
    ldi r24, 1
    ldi r25, 1

comparar:
	clr r18
	clr r19
    in r17, pinf        
    cpi r17, 0          
    breq bomba_agua     
    cpi r17, 1          
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
    ldi r23, 0b00000001  
    ldi r25, 0
llenando:
	or r19, r23          
	out PORTA, r24       
	sts PORTk, r19      
	cpi r19, 0b00001111  
	breq mostrar_nivel   
    lsl r23              
    rjmp llenando        

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
	sts PORTk, r19  
    cpi r21, 4
    breq nivel_maximo
    rjmp comparar   


nivel_maximo:
    ldi r23, 2  

parpadeo:
    ldi r22, 2
    out PORTA, r22   
	clr r22
    out PORTA, r22  
    dec r23         
    brne parpadeo    

    rjmp comparar    







