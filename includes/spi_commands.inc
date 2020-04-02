;
;
; initialise I/O ports and peripherals
;
; PB0 LED	Output	1
; PB1 ??	Output	1
; PB2 !SS	Output	1
; PB3 MOSI0	Output	1
; PB4 MISO0	Input	0
; PB5 SCK	Output	1
; PB6 XTAL	X	0
; PB7 XTAL	X	0
init_spi:
	ldi	r16,0b00101111		; set pin directions
	out	DDRB,r16
	sbi	PORTB,2			; and SS back high
;
; Setup SPI operations
; See pp217-218 of the data sheet
;
	ldi	r16,(1<<SPE)|(1<<MSTR); set master SPI, (SPI mode 0 operation is 00)
	out	SPCR,r16			; SCK is set fosc/4 => 4MHz
	clr	r16				; clear interrupt flags and oscillator mode.
	out	SPSR,r16
	ret

;
; Send a command + byte to SPI interface
; CMD is in r20, DATA is in r21
; r16 is destroyed by this subroutine
SPI_Send_Command:
	cbi	PORTB,2		; SS low
	ldi	r16,0x40
	call	SPI_SendByte
	mov	r16,r20
	call	SPI_SendByte
	mov	r16,r21
	call	SPI_SendByte
	sbi	PORTB,2		; and SS back high
	ret
; Send a command + byte to SPI interface
; CMD is in r20, DATA is in r21 (if necessary)
SPI_Read_Command:
	cbi	PORTB,2		; SS low
	ldi	r16,0x41
	call	SPI_SendByte
	mov	r16,r20
	call	SPI_SendByte
	mov	r16,r21
	call	SPI_SendByte
	sbi	PORTB,2		; and SS back high
	ret
; Send one SPI byte (Returned data in r16)
SPI_SendByte:
	out	SPDR,r16
SPI_wait:
	in	r16,SPSR
	sbrs	r16,SPIF
	rjmp	SPI_wait
	in	r16,SPDR
	ret