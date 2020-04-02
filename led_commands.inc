
set_led:
	cpi r22, 0x01
	brsh led_up
led_down:
	cpi r17, 0b00000001
	breq set_up
	lsr r17
	rjmp pre_pause
led_up:
	cpi r17, 0b00010000
	brge set_down
	lsl r17
	rjmp pre_pause

set_down:
	ldi r22, 0x00
	call led_down
set_up:
	ldi r22, 0x01
	ldi r17, 0x01
	call led_up