.include "./m328Pdef.inc"
.DSEG
msg_buffer: .BYTE 30

.CSEG
	.org	0x0000					; start at beginning of program address

; Setup IVR to no_interrupt
	.include "setup.inc"
;	On reset we branch to here
RESET:						; Main program start
	ldi	r16,high(RAMEND)	; Set Stack Pointer to top of RAM
	out	SPH,r16
	ldi	r16,low(RAMEND)
	out	SPL,r16
	sei						; Enable interrupts

;	Calling all initialization routines
	call init_spi
	call timer_init
	call init_port_expander
	call lcd_init

	LCD_init_String0:
	.DB		0x0C,0x01		; turn on display, cursor and blink		
	ldi		r25,15			; The length of the message

	call lcd_startup_msg



; Main loop
; increment a count on port A until a button is pressed on port B (pin 0).
; While the button is pressed we pause the count.
LCD_Err:
	ldi r26, 0x01				; Set to boppit level
	ldi	r17, 0b00000001			; counter
	clr	r18
	ldi r22, 0x01	; To toggle the direction of the LED 1 - UP, 0 - DOWN
mainLoop:
    call t1_loop
	ldi	r20,0x14	; register OLATA (port A data output)
	mov	r21,r17		; value to write
	call	SPI_Send_Command
	call	set_led
pre_pause:
	call	pause
	rjmp mainLoop



pause:
	ldi	r20,0x13	; register GPIOB (port B data input)
;	clr	r21		; don't care. We could infact leave this instruction out
	call	SPI_Read_Command
	;
	; the read data is in r16. We want to loop while the data read is low.
	; If the data is low the andi instruction will result in zero (Z flag set)
	andi	r16,0b0000100	; test data from pin 0.
	breq	pause		; loop while returns zero
	rjmp	mainLoop
;
; Helper soubroutines
.include "spi_commands.inc"
.include "delay_commands.inc"
.include "led_commands.inc"
.include "timer.inc"
.include "port_expander.inc"
.include "lcd.inc"
