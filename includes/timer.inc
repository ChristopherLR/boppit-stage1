
timer_init:
	push r16
	push r17
	push r18
	lds r18, TIMSK1		; save current value
	clr r16				; diables all interrupts
	sts TIMSK1, r16		; [-][-][-][ICIE1][-][-][OCIE1B][TOIE1]
	sts TCCR1B, r16		; stop the clock
	ldi r17, 0b00000100	; [-][-][ICF1][-][-][OCF1B][OCF1A][TOV1]
	out TIFR1, r17		; Output Compare Match B -> OCF1B 
	ldi r16, 0b00000000 ; TCCR1A = [COM1A1][COM1A0][COM1B1][COM1B0][][][WGM11][WGM10]
	sts TCCR1A, r16		; PORTA - Normal, PORTB - Normal, WGM=0000(Normal)
	ldi r17, HIGH(1562)	; 1562 is value of counter
	ldi r16, LOW(1562)
	sts OCR1BH, r17		; Setting the top of the compare
	sts OCR1BL, r16		; Setting the bottom of the compare
	clr r17				; Clear the current count
	sts TCNT1H, r17		; TCNT is how you access the timer counter
	sts TCNT1L, r17		; The value that OCR1B is looking to compare
	ldi r16, 0b00000101	; Noise = 0; WGM =0000, clk =/1024
	sts TCCR1B, r16		;
	ldi r16, 0b00000000	;
	sts TCCR1C, r16
	sts TIMSK1, r18
	pop r18
	pop r17
	pop r16
	ret

t1_loop:
	push r17
t1_inner:
	sbis TIFR1, 2
	rjmp t1_inner
	ldi r17, 0b00000100
	out TIFR1, r17
	clr r17				; Clear the current count
	sts TCNT1H, r17		; TCNT is how you access the timer counter
	sts TCNT1L, r17		; The value that OCR1B is looking to compare
	pop r17
	ret
