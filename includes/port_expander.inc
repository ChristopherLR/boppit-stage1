; We will configure port A as all outputs, port B as all inputs.
; use the SPI_send_command to send the SPI commands.
; This will require the register address in r20 and the register data in r21
;
init_port_expander:
	ldi	r20,0x00	; register IODIRA (port A data direction)
	ldi	r21,0x00	; all outputs
	call	SPI_Send_Command
	ldi	r20,0x01	; register IODIRB (port B data direction)
	ldi	r21,0xff	; all inputs
	call	SPI_Send_Command
	ldi	r20,0x0d	; register GPPUB (port B GPIO Pullups)
	ldi	r21,0xff	; turn on all pullups
	call	SPI_Send_Command
	ret