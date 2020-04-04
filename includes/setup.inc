; complete interrupt vector table.
setup_int:
  jmp RESET ; Reset
	jmp INT0_IR ; IRQ0
	jmp INT1_IR ; IRQ1
	jmp PCINT0_IR ; PCINT0
	jmp PCINT1_IR ; PCINT1
	jmp PCINT2_IR ; PCINT2
	jmp WDT_IR ; Watchdog Timeout
	jmp TIM2_COMPA ; Timer2 CompareA
	jmp TIM2_COMPB ; Timer2 CompareB
	jmp TIM2_OVF ; Timer2 Overflow
	jmp TIM1_CAPT ; Timer1 Capture
	jmp TIM1_COMPA ; Timer1 CompareA
	jmp TIM1_COMPB ; Timer1 CompareB
	jmp TIM1_OVF ; Timer1 Overflow
	jmp TIM0_COMPA ; Timer0 CompareA
	jmp TIM0_COMPB ; Timer0 CompareB
	jmp TIM0_OVF ; Timer0 Overflow
	jmp SPI_STC ; SPI Transfer Complete
	jmp USART_RXC ; USART RX Complete
	jmp USART_UDRE ; USART UDR Empty
	jmp USART_TXC ; USART TX Complete
	jmp ADC_CC ; ADC Conversion Complete
	jmp EE_RDY ; EEPROM Ready
	jmp ANA_COMP ; Analog Comparator
	jmp TWI_IR ; 2-wire Serial
	jmp SPM_RDY ; SPM Ready
;	and we branch to the final location from here.
;	Interrupts we cannot handle in this example we just loop at noint
;
INT0_IR: rjmp  noint
INT1_IR: rjmp  noint
PCINT0_IR: rjmp  noint
PCINT1_IR: rjmp  noint
PCINT2_IR: rjmp  noint
WDT_IR: rjmp  noint
TIM2_COMPA: rjmp  noint
TIM2_COMPB: rjmp  noint
TIM2_OVF: rjmp  noint
TIM1_CAPT: rjmp  noint
TIM1_COMPA: rjmp  noint
TIM1_COMPB: rjmp  noint
TIM1_OVF: rjmp  noint
TIM0_COMPA: rjmp  noint
TIM0_COMPB: rjmp  noint
TIM0_OVF: rjmp  noint
SPI_STC: rjmp  noint
USART_RXC: rjmp  noint
USART_UDRE: rjmp  noint
USART_TXC: rjmp  noint
ADC_CC: rjmp  noint
EE_RDY: rjmp  noint
ANA_COMP: rjmp  noint
TWI_IR:	rjmp  noint
SPM_RDY: rjmp  noint
;
error:
noint:	inc	r16
	rjmp	noint
