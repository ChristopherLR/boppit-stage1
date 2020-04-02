
lcd_init:
	cbi DDRC,4          ; set the I2C pins as inputs will pullup resistors turned on.
	cbi DDRC,5
	sbi PORTC,4
	sbi PORTC,5
	
	sbi DDRC,0          ; port C, bit 0 is out debug bit.
	cbi PORTC,0

	;
; initialise I/O ports and peripherals
;
; PC4 SDA0	Bidirectional
; PC5 SCL0	Bidirectional
;
;
;
; I2C clock rate:	assume wants 40KHz
; Rate:
; SCL = Fosc / (16 + 2(TWBR).(TWPS[1:0]))
;     = 16,000,000 / (16 + 2(TWBR).(TWPS[1:0]))
;
; 40,000 = 16,000,000 / (16 + 2(TWBR).(TWPS[1:0]))
;
; (16 + 2(TWBR).(TWPS[1:0])) = 16,000,000 / 40,000
;
; (16 + 2(TWBR).(TWPS[1:0])) = 400
;
; 2(TWBR).(TWPS[1:0]) = 400 - 16
;
; 2(TWBR).(TWPS[1:0]) = 386
;
; (TWBR).(TWPS[1:0]) = 193
;
; TWBR = 193,   TWPS[1:0] = 0:0 (scale of 1)
;
;						  Setup TWI interface
	ldi		r16,193		; setup TWI frequency scaling
	sts		TWBR,r16	; Two Wire Interface Bit-rate Register
	ldi		r16,0x00
	sts		TWSR,r16
	

	ldi		r24,0x27	; Setup LCD display at this address (Maybe 0x3f instead)
	call	LCD_Setup
	call	LCD_Clear
	ret

lcd_startup_msg:
	ldi		ZL,LOW(msg_buffer)	; Converting the message from the compilers address space to the machines address space
	ldi		ZH,HIGH(msg_buffer)
	ldi		r25, 48
	st		Z, r25
	call	LCD_Text
	ret

;
; Helper Functions
;


;
; Send TWI start address.
; On return Z flag is set if completed correctly
; r15 and r16 destroyed
sendTWI_Start:
	ldi		r16,(1<<TWINT) | (1<<TWSTA) | (1<<TWEN)
	sts		TWCR,r16

	call	waitTWI

	lds		r16,TWSR
	andi	r16,0xf8		; mask out 
	cpi		r16,0x08		; TWSR = START (0x08)
	ret
;
; Send TWI slave address. Address is in r16
; On return Z flag is set if completed correctly
; r15 and r16 destroyed
sendTWI_SLA:
	sts		TWDR,r16
	ldi		r16,(1<<TWINT) | (1<<TWEN)
	sts		TWCR,r16

	call	waitTWI
	
	lds		r16,TWSR
	andi	r16,0xf8		; mask out 
	cpi		r16,0x18		; TWSR = SLA+W sent, ACK received (0x18)
	ret
;
; Send 8 bits of data as two 4 bit nibbles.
; The data is in r16, the lower 4 bits are in r17
; we assume the TWI operation is waiting for data to be sent.
; r15, r18 and r19 all destroyed
sendTWI_Byte:
	mov		r18,r16
	andi	r18,0xF0
	or		r18,r17
	call	sendTWI_Nibble
	mov		r18,r16
	swap	r18
	andi	r18,0xF0
	or		r18,r17
	call	sendTWI_Nibble
	ret

;
; send 4 bits of data, changing the enable bit as we send it.
; data is in r18. r15, r18 and r19 are destroyed
;
sendTWI_Nibble:
	ori		r18,0x04
	sts		TWDR,r18
	ldi		r19,(1<<TWINT) | (1<<TWEN)
	sts		TWCR,r19

	call	waitTWI			; destroys r15
	
	lds		r19,TWSR
	andi	r19,0xf8		; mask out 
	cpi		r19,0x28		; TWSR = data sent, ACK received (0x28)
	brne	sendTWI_Nibble_exit

	andi	r18,0xFB		; set enable bit low
	
	sts		TWDR,r18
	ldi		r19,(1<<TWINT) | (1<<TWEN)
	sts		TWCR,r19

	call	waitTWI
	
	lds		r19,TWSR
	andi	r19,0xf8		; mask out 
	cpi		r19,0x28		; TWSR = data sent, ACK received (0x28)
sendTWI_Nibble_exit:
	ret

;
;	Send the data pointed to by the Z register to the TWI interface.
;	r25 contains the number of bytes to send
;	r24 contains the address of the I2C controller
;	r17 contains the lower 4 bits of each nibble to send
;
SendTWI_Data:
	call	sendTWI_Start
	brne	serror

	mov		r16,r24			; use this address
	add		r16,r16			; and move over the r/w bit
	call	sendTWI_SLA
	brne	serror

	cpi		r25,0x00		; any bytes left?
	breq	sendTWI_done	; if not all done
	
sendTWI_loop:
	lpm		r16,Z+
	call	sendTWI_Byte
	brne	serror

	dec		r25
	brne	sendTWI_loop

sendTWI_done:
serror:
;
; send stop bit and we're done
;
sendTWI_Stop:
	ldi		r16,(1<<TWINT) | (1<<TWEN) | (1<<TWSTO)		; and send stop
	sts		TWCR,r16
	ldi		r16,0
sendTWI_Delay:
	dec		r16
	brne	sendTWI_Delay
	ret
;
; Wait until the TWI (I2C) interface has sent the byte and received an ack/nak
; destroys r15
;
waitTWI:
	lds	r15,TWCR
	sbrs	r15,TWINT		; wait until transmitted
	rjmp	waitTWI
	ret
;
; Initialisation strings for the LCD panel
;


;
; LCD Position - set the write poswition in the DRAM
; r24 holds the LCD I2C address
; r25 holds the address (0-127)
; r17 holds the lower 4 bits
;
LCD_Position:
	call	sendTWI_Start
	brne	LCD_serror

	mov		r16,r24			; use this address
	add		r16,r16			; and move over the r/w bit
	call	sendTWI_SLA
	brne	LCD_serror

	mov		r16,r25
	ori		r16,0x80		; set DDRAM address command
	ldi		r17,8			; backlight
	call	sendTWI_Byte
	
	rjmp	sendTWI_Stop
	
;
; LCD Clear - Clears the LCD and places the cursor at location 0
; r24 holds the LCD I2C address
; r17 holds the lower 4 bits
;
LCD_Clear:
	call	sendTWI_Start
	brne	LCD_serror

	mov		r16,r24			; use this address
	add		r16,r16			; and move over the r/w bit
	call	sendTWI_SLA
	brne	LCD_serror

	ldi		r16,0x01		; set DDRAM address command
	ldi		r17,8			; backlight
	call	sendTWI_Byte
	
	rjmp	sendTWI_Stop
;
; LCD_Text - send a string to the LCD for displaying
; Z points to the string,
; r25 holds the number of characters to print,
; r24 is the address of the LCD
;
LCD_Text:
	call	sendTWI_Start
	brne	LCD_serror

	mov		r16,r24			; use this address
	add		r16,r16			; and move over the r/w bit
	call	sendTWI_SLA
	brne	LCD_serror

	cpi		r25,0x00		; any bytes left?
	breq	LCD_Text_done	; if not all done
	ldi		r17,9			; backlight + data byte
LCD_Text_loop:
	lpm		r16,Z+
	call	sendTWI_Byte
	brne	LCD_serror

	dec		r25
	brne	LCD_Text_loop

LCD_Text_done:
LCD_serror:
	rjmp	sendTWI_Stop
;
; LCDSetup - setup the LCD display connected at I2C port in r16
;
LCD_Setup:
	call	sendTWI_Start						; send start bit
	breq	LCD_Setup_0
	jmp		LCD_Setup_Err
LCD_Setup_0:
	mov		r16,r24
	add		r16,r16
	call	sendTWI_SLA
	breq	LCD_Setup_1
	jmp		LCD_Setup_Err
LCD_Setup_1:
	clr		r18
	clr		r19
	call	sendTWI_Nibble
	call	sendTWI_Stop

	ldi		r18,LOW(5)
	ldi		r19,HIGH(5)
;	call	delay_ms							; wait 5 ms
	
;
; Send the first of three 0x30 to the display
;

	call	sendTWI_Start						; send start bit
	breq	LCD_Setup_2
	jmp		LCD_Setup_Err
LCD_Setup_2:
	mov		r16,r24
	add		r16,r16
	call	sendTWI_SLA
	breq	LCD_Setup_3
	jmp		LCD_Setup_Err
LCD_Setup_3:
	ldi		r18,0x30
	clr		r19
	call	sendTWI_Nibble
	call	sendTWI_Stop

	ldi		r18,LOW(5)
	ldi		r19,HIGH(5)
;	call	delay_ms							; wait 5 ms
	
;
; Send the second of three 0x30 to the display
;

	call	sendTWI_Start						; send start bit
	brne	LCD_Setup_Err
	mov		r16,r24
	add		r16,r16
	call	sendTWI_SLA
	brne	LCD_Setup_Err
	ldi		r18,0x30
	clr		r19
	call	sendTWI_Nibble
	call	sendTWI_Stop

	ldi		r18,LOW(5)
	ldi		r19,HIGH(5)
;	call	delay_ms							; wait 5 ms
	
;
; Send the third of three 0x30 to the display
;

	call	sendTWI_Start						; send start bit
	brne	LCD_Setup_Err
	mov		r16,r24
	add		r16,r16
	call	sendTWI_SLA
	brne	LCD_Setup_Err
	ldi		r18,0x30
	clr		r19
	call	sendTWI_Nibble
	call	sendTWI_Stop

	
;
; Send 0x28 to the display to reset to 4 bit mode
;

	call	sendTWI_Start						; send start bit
	brne	LCD_Setup_Err
	mov		r16,r24
	add		r16,r16
	call	sendTWI_SLA
	brne	LCD_Setup_Err
	ldi		r18,0x28
	clr		r19
	call	sendTWI_Nibble
	call	sendTWI_Stop

	
	ldi		ZL,LOW(LCD_init_String0*2)
	ldi		ZH,HIGH(LCD_init_String0*2)
	ldi		r25,2								; all 2 bytes
	ldi		r17,8								; lower 4 bits zero (Backlight on)
	call	SendTWI_Data
	ret

	LCD_Setup_Err:
	jmp LCD_Err