
;
; Delay a number of milliseconds
; number of milliseconds is in r4
; destroys r2,r3 and r4
;
.equ	LED_DELAY =	(4000/2)*2		; for a 1 ms delay
delay_1ms:
	ldi	r16,1
	mov	r4,r16
delay_ms:
	ldi	r16,HIGH(LED_DELAY)
	mov	r3,r16
	ldi	r16,LOW(LED_DELAY)
	mov	r2,r16
delay:
	dec	r2
	brne	delay
	dec	r3
	brne	delay
	dec	r4
	brne	delay_ms
	ret