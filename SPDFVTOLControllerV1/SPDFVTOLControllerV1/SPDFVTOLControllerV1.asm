
;-*-         SPDVTOL Controller V1            -*-
;-*-     All Code And Hardware Design         -*-
;-*-      By Rolf R Bakke,  Januar 2011       -*-

; (I have not peeked in others projects or code :-)

; Code is best viewed in monospace font and tab = 8




;   View from above

;      Forward

;         S1
;         *
;         |
;   S2    |     S3
;   *-----+-----*
;         |
;         |
;         *
;         S4





;******************* SETTINGS **************************

; This one determines the range of the gain potmeters.
; Reducing the value by one doubles the range.
; Increasing the value by one halves the range.

.equ	PotScaleRoll	= 8
.equ	PotScalePitch	= 8
.equ	PotScaleYaw	= 8


; This one determines the stick sensitivity.
; Reducing the value by one doubles the sensitivity.
; Increasing the value by one halves the sensitivity.

.equ	StickScaleRoll	= 9
.equ	StickScalePitch	= 9
.equ	StickScaleYaw	= 9


; This one determines the maximum Yaw command applied, in percent.

.equ	YawLimit	= 30


; This one should be set to the chip you are using.
; Atmega48  = "m48def.inc"
; Atmega88  = "m88def.inc"
; Atmega168 = "m168def.inc"
; Atmega328 = "m328def.inc"

.include "m48def.inc"	

;*******************************************************
	


;---  16.16 bit signed registers ---

.equ	Temp				=0

.equ	RxInRoll			=1
.equ	RxInPitch			=2
.equ	RxInYaw				=3
.equ	RxInCollective			=4

.equ	RxChannel			=5

.equ	GyroZeroRoll			=9
.equ	GyroZeroPitch			=10
.equ	GyroZeroYaw			=11

.equ	ServoOutFront			=12
.equ	ServoOutLeft			=13
.equ	ServoOutRight			=14
.equ	ServoOutBack			=15

.equ	GyroRoll			=17
.equ	GyroPitch			=18
.equ	GyroYaw				=19

.equ	LowpassServoOutFront		=20
.equ	LowpassServoOutLeft		=21
.equ	LowpassServoOutRight		=22
.equ	LowpassServoOutBack		=23

.equ	LowpassIn			=24
.equ	LowpassOut			=25

.equ	GainInRoll			=26
.equ	GainInPitch			=27
.equ	GainInYaw			=28





.equ	B16_RegAdd=0x0100	;base address for the math library register array

.equ	B16_WorkAdd=0x0200	;base address for the math library work area


.def	RxChannel1StartL	=r0
.def	RxChannel1StartH	=r1

.def	RxChannel2StartL	=r2
.def	RxChannel2StartH	=r3

.def	RxChannel3StartL	=r4
.def	RxChannel3StartH	=r5
	
.def	RxChannel4StartL	=r6
.def	RxChannel4StartH	=r7

.def	RxChannel1L		=r8
.def	RxChannel1H		=r9

.def	RxChannel2L		=r10
.def	RxChannel2H		=r11

.def	RxChannel3L		=r12
.def	RxChannel3H		=r13

.def	RxChannel4L		=r14
.def	RxChannel4H		=r15




.equ	RollGyroDirection	=b16_workadd+26
.equ	PitchGyroDirection	=b16_workadd+27
.equ	YawGyroDirection	=b16_workadd+28

.equ	RateDivider		=b16_workadd+29

.equ	PotReverser		=b16_workadd+30





.def	t=r16				;Main temporary register

;r16,r17,r18,r19 is destroyed by the math lib


.def	counterl=r20
.def	counterh=r21

.def	RxChannelsUpdatingFlag=r22	;ISR (do not read channels while this is true) 

.def	tisp=r23			;ISR temporary register

					;the registers marked ISR is not to be used for any other purpose 


.macro led0_on			;macros for the LED's
	sbi portb,6
.endmacro
.macro led0_off
	cbi portb,6
.endmacro
.macro led2_on
	sbi portd,5
.endmacro
.macro led2_off
	cbi portd,5
.endmacro
.macro led3_on
	sbi portd,6
.endmacro
.macro led3_off
	cbi portd,6
.endmacro



#define motor_out_pin_1 portb,2	;motor output pin assignments
#define motor_out_pin_2 portb,1
#define motor_out_pin_3 portb,0
#define motor_out_pin_4 portd,7


.include "1616mathlib_ram_macros_v1.asm"

.org 0


#if defined(__ATmega48__) || defined(__ATmega88__)
#message "rjmp"
	rjmp reset
	rjmp RxChannel2
	rjmp RxChannel3
	rjmp RxChannel4
	rjmp unused
	rjmp RxChannel1
	rjmp unused
	rjmp unused
	rjmp unused
	rjmp unused
	rjmp unused
	rjmp unused
	rjmp unused
	rjmp unused
	rjmp unused
	rjmp unused
	rjmp unused
	rjmp unused
	rjmp unused
	rjmp unused
	rjmp unused
	rjmp unused
	rjmp unused
	rjmp unused
	rjmp unused
	rjmp unused

#elif defined(__ATmega168__) || defined(__ATmega328__)
#message "jmp"
	jmp reset
	jmp RxChannel2
	jmp RxChannel3
	jmp RxChannel4
	jmp unused
	jmp RxChannel1
	jmp unused
	jmp unused
	jmp unused
	jmp unused
	jmp unused
	jmp unused
	jmp unused
	jmp unused
	jmp unused
	jmp unused
	jmp unused
	jmp unused
	jmp unused
	jmp unused
	jmp unused
	jmp unused
	jmp unused
	jmp unused
	jmp unused
	jmp unused
#else
#error "Unsupported part:" __PART_NAME__
#endif

unused:	reti

reset:	ldi t,low(ramend)	;initalize stack pointer
	out spl,t
	ldi t,high(ramend)
	out sph,t

	
;--- setup IO ---

	;       76543210
	ldi t,0b01111111
	out ddrb,t

	;       76543210
	ldi t,0b11000000
	out ddrc,t
	
	;       76543210
	ldi t,0b11110001
	out ddrd,t


;--- Setup pin change interrupt on PD1, PD2, PD3, PB4

	;       76543210
	ldi t,0b00000101	;PB7, PB1
	sts pcicr,t

	;       76543210	
	ldi t,0b10000000	;PB7
	sts pcmsk0,t
	
	;       76543210
	ldi t,0b00000010	;PD1
	sts pcmsk2,t


	;       76543210
	ldi t,0b00000101	;PD2, PD3
	sts eicra,t
	

	;       76543210
	ldi t,0b00000011	;PD2, PD3
	out eimsk,t		;STS? OUT?  Come on!



;---- Setup timer1 to run at 1MHz ----

	;       76543210
	ldi t,0b00000010
	sts tccr1b,t


;---- Initalize variables ---

	
	clr RxChannelsUpdatingFlag

	clr t
	sts RateDivider, t
	

	ldi xl,low(1520)		;prime the channels
	ldi xh,high(1520)

	mov RxChannel1L,xl
	mov RxChannel1H,xh
	mov RxChannel2L,xl
	mov RxChannel2H,xh
	mov RxChannel3L,xl
	mov RxChannel3H,xh
	mov RxChannel4L,xl
	mov RxChannel4H,xh


	b16ldi Temp, 50
	b16mov LowpassServoOutFront, Temp
	b16mov LowpassServoOutLeft, Temp
	b16mov LowpassServoOutRight, Temp
	b16mov LowpassServoOutBack, Temp



;----
	sei

	led0_on				;Flash LED once, I am alive!

	ldi zl,0
	rcall waitms

	led0_off

	ldi zh,100			;2 second delay
ca6:	ldi zl,200
	rcall waitms
	dec zh
	brne ca6





;---- Gyro direction reversing ----
;---- 1: Set roll gain pot to zero.
;---- 2: Turn on flight controller.
;---- 3: LED flashes rapidly 10 times.
;---- 4: Move the stick for the gyro you want to reverse.
;---- 5: LED will blink continually.
;---- 6: Turn off flight controller.
;---- 7: If there is more gyros to be reversed, goto step 2, else set roll gain pot back. 

	rcall ReadGainPots
	rcall ReadGainPots

	b16ldi Temp, 51
	b16cmp GainInRoll,Temp
	brlt ca9			;enter gyro direction reversing?
	rjmp ca1

ca9:	ldi counterl,10			;yes, flash led 10 times
ca7:	led0_on
	ldi zl,0
	rcall waitms
	led0_off
	ldi zl,0
	rcall waitms
	dec counterl
	brne ca7



ca5:	ldi zl,180
	rcall waitms

	rcall RxGetChannels

	b16ldi Temp, 30

	b16cmp RxInRoll,Temp		;Roll TX stick moved?
	ldi zl,0
	brge ca8

	b16cmp RxInPitch,Temp		;Pitch TX stick moved?
	ldi zl,1
	brge ca8

	b16cmp RxInYaw,Temp		;Yaw TX stick moved?
	ldi zl,2
	brge ca8

	b16cmp RxInCollective,Temp	;Throttle TX stick moved?
	ldi zl,3
	brge ca8

	rjmp ca5			;no

ca8:	ldi zh,0			;yes, reverse direction
	rcall ReadEeprom

	ldi xl,0x80
	eor t,xl					

	rcall WriteEeprom		;Store gyro direction to EEPROM

ca4:	led0_on				;flash LED
	ldi zl,0
	rcall waitms
	led0_off
	ldi zl,0
	rcall waitms
	rjmp ca4



	
ca1:  	b16clr GyroZeroRoll				;yes, set gyro zero value (average of 16 readings)
	b16clr GyroZeroPitch
	b16clr GyroZeroYaw

	ldi counterl,16

ma20:	rcall ReadGyros

	b16add GyroZeroRoll, GyroRoll
	b16add GyroZeroPitch, GyroPitch
	b16add GyroZeroYaw, GyroYaw

	dec counterl
	brne ma20

	b16load GyroZeroRoll				;divide by 16
	ldi t,4
	rcall FastDivide
	b16store GyroZeroRoll

	b16load GyroZeroPitch				;divide by 16
	ldi t,4
	rcall FastDivide
	b16store GyroZeroPitch

	b16load GyroZeroYaw				;divide by 16
	ldi t,4
	rcall FastDivide
	b16store GyroZeroYaw



	led0_on	


	;--- Main loop ---

ma1:	rcall GetGyroDirections

	rcall ReadGainPots

	rcall RxGetChannels



	;--- Read gyros ---


	rcall ReadGyros
	
	b16sub GyroRoll, GyroZeroRoll			;remove offset from gyro output
	b16sub GyroPitch, GyroZeroPitch
	b16sub GyroYaw, GyroZeroYaw
 

;rcall ShowChannels
;rcall ShowGyros


	;--- Start mixing ---

	b16ldi Temp, 50
	b16mov ServoOutFront, Temp
	b16ldi Temp, 50
	b16mov ServoOutLeft, Temp
	b16ldi Temp, 50
	b16mov ServoOutRight, Temp
	b16ldi Temp, 50
	b16mov ServoOutBack, Temp



	;--- Calculate roll command output ---

	b16load GainInRoll				;scale gyro output
	ldi t, PotScaleRoll
	rcall FastDivide				;  divide by 2^t
	
	lds t, RollGyroDirection
	tst t
	brpl ma60
	
	rcall NegateXY

ma60:	b16store Temp

	b16mul GyroRoll, Temp	

	
	b16load GainInRoll				;scale stick output
	ldi t, StickScaleRoll
	rcall FastDivide				;  divide by 2^t
	b16store Temp

	b16mul RxInRoll, Temp


	b16add RxInRoll, GyroRoll			;add gyro output to stick output


	
	;--- Add roll command output to motor 2 and 3 ---

	b16add ServoOutFront, RxInRoll
	b16sub ServoOutBack, RxInRoll



	;--- Calculate pitch command output ---

	b16load GainInPitch				;scale gyro output
	ldi t, PotScalePitch
	rcall FastDivide				;  divide by 2^t
	
	lds t, PitchGyroDirection
	tst t
	brpl ma61
	
	rcall NegateXY

ma61:	b16store Temp

	b16mul GyroPitch, Temp	

	
	b16load GainInPitch				;scale stick output
	ldi t, StickScalePitch
	rcall FastDivide				;  divide by 2^t
	b16store Temp

	b16mul RxInPitch, Temp


	b16add RxInPitch, GyroPitch			;add gyro output to stick output


	
	;--- Add pitch command output to motor 1 and 4 ---

	b16add ServoOutRight, RxInPitch
	b16sub ServoOutLeft, RxInPitch



	;--- Calculate yaw command output ---

	b16load GainInYaw				;scale gyro output
	ldi t, PotScaleYaw
	rcall FastDivide				;  divide by 2^t
	
	lds t, YawGyroDirection
	tst t
	brpl ma62
	
	rcall NegateXY

ma62:	b16store Temp

	b16mul GyroYaw, Temp	

	
	b16load GainInYaw				;scale stick output
	ldi t, StickScaleYaw
	rcall FastDivide				;  divide by 2^t
	b16store Temp

	b16mul RxInYaw, Temp


	b16add RxInYaw, GyroYaw				;add gyro output to stick output


	
	b16ldi Temp, -YawLimit				;limit Yaw command to -YawLimit and YawLimit
	b16cmp RxInYaw, Temp
	brge ma90
	b16mov RxInYaw, Temp

ma90:	b16ldi Temp, Yawlimit
	b16cmp RxInYaw, Temp
	brlt ma91
	b16mov RxInYaw, Temp
ma91:

	;--- Add yaw command output to motor 1,2,3 and 4 ---

	b16sub ServoOutFront, RxInYaw
	b16sub ServoOutLeft, RxInYaw
	b16sub ServoOutRight, RxInYaw
	b16sub ServoOutBack, RxInYaw


	;--- Filter the servo outputs ---


	b16mov LowpassIn, ServoOutFront
	b16mov LowpassOut, LowpassServoOutFront 
	rcall LPF
	b16mov ServoOutFront, LowpassOut
	b16mov LowpassServoOutFront, LowpassOut

	b16mov LowpassIn, ServoOutLeft
	b16mov LowpassOut, LowpassServoOutLeft 
	rcall LPF
	b16mov ServoOutLeft, LowpassOut
	b16mov LowpassServoOutLeft, LowpassOut

	b16mov LowpassIn, ServoOutRight
	b16mov LowpassOut, LowpassServoOutRight 
	rcall LPF
	b16mov ServoOutRight, LowpassOut
	b16mov LowpassServoOutRight, LowpassOut

	b16mov LowpassIn, ServoOutBack
	b16mov LowpassOut, LowpassServoOutBack 
	rcall LPF
	b16mov ServoOutBack, LowpassOut
	b16mov LowpassServoOutBack, LowpassOut


	;--- Output to motor ESC's ---

	lds xl, RateDivider
	dec xl
	brne ma8
	rcall output_motor_ppm			;output ESC signal
	ldi xl,4
ma8:	sts RateDivider, xl
	rjmp ma1



	;--- End of main loop ---


	;--- Subroutines ---


	;--- Output motor ppm channels 1-4 in parallell ---
	; Input is 0 to 100

output_motor_ppm:

	b16ldi Temp, 0			;limit to zero
	b16cmp ServoOutFront,Temp
	brge ou1
	b16mov ServoOutFront,Temp
ou1:
	b16cmp ServoOutLeft,Temp
	brge ou2
	b16mov ServoOutLeft,Temp
ou2:
	b16cmp ServoOutRight,Temp
	brge ou3
	b16mov ServoOutRight,Temp
ou3:
	b16cmp ServoOutBack,Temp
	brge ou4
	b16mov ServoOutBack,Temp
ou4:

	b16ldi Temp, 100		;limit to 100
	b16cmp ServoOutFront,Temp
	brlt ou5
	b16mov ServoOutFront,Temp
ou5:
	b16cmp ServoOutLeft,Temp
	brlt ou6
	b16mov ServoOutLeft,Temp
ou6:
	b16cmp ServoOutRight,Temp
	brlt ou7
	b16mov ServoOutRight,Temp
ou7:
	b16cmp ServoOutBack,Temp
	brlt ou8
	b16mov ServoOutBack,Temp
ou8:

	
	b16ldi Temp, 100		;add 1ms to all channels
	b16add ServoOutFront,Temp
	b16add ServoOutLeft,Temp
	b16add ServoOutRight,Temp
	b16add ServoOutBack,Temp


	b16load ServoOutFront		;scale from 0-200 to 0-800 (multiply by 4)
	ldi t,2
	rcall FastMultiply
	b16store ServoOutFront

	b16load ServoOutLeft
	ldi t,2
	rcall FastMultiply
	b16store ServoOutLeft

	b16load ServoOutRight
	ldi t,2
	rcall FastMultiply
	b16store ServoOutRight

	b16load ServoOutBack
	ldi t,2
	rcall FastMultiply
	b16store ServoOutBack


	b16load ServoOutBack		;transfer to 16bit counters
	push xl
	push xh
	b16load ServoOutRight		
	push xl
	push xh
	b16load ServoOutLeft		
	push xl
	push xh
	b16load ServoOutFront		
	movw r25:r24,xh:xl

	pop r27
	pop r26

	pop r29
	pop r28

	pop r31
	pop r30

	sbi motor_out_pin_1		;turn on pins
	sbi motor_out_pin_2
	sbi motor_out_pin_3
	sbi motor_out_pin_4

	clr t

	ldi counterl,low(801)
	ldi counterh,high(801)


ou13:	subi r24,1			;20 cycles (1ms = 400 counts)
	sbc r25,t
	brcc ou9
	cbi motor_out_pin_1
ou9:	
	subi r26,1
	sbc r27,t
	brcc ou10
	cbi motor_out_pin_2
ou10:
	subi r28,1
	sbc r29,t
	brcc ou11
	cbi motor_out_pin_3
ou11:	
	subi r30,1
	sbc r31,t
	brcc ou12
	cbi motor_out_pin_4
ou12:	
	subi counterl,1
	sbc counterh,t
	brcc ou13

	ret	





	;--- Read ADC's ---

ReadGyros:
	;       76543210
	ldi t,0b00111111
	sts didr0,t

	;       76543210
	ldi t,0b00000000
	sts adcsrb,t


	;       76543210	;read roll
	ldi t,0b00000010
	rcall read_adc
	b16store GyroRoll

	;       76543210	;read pitch
	ldi t,0b00000001
	rcall read_adc
	b16store GyroPitch

	;       76543210	;read yaw
	ldi t,0b00000000
	rcall read_adc
	b16store GyroYaw

	ret



ReadGainPots:

	ldi zl,3		;get PotReverser from EEPROM
	ldi zh,0
	rcall ReadEeprom
	sts PotReverser,t


	;       76543210
	ldi t,0b00111111
	sts didr0,t

	;       76543210
	ldi t,0b00000000
	sts adcsrb,t


	;       76543210	;read roll gain
	ldi t,0b00000011
	rcall read_adc

	lds t,PotReverser
	tst t
	brmi ga1
	rcall invert
ga1:	b16store GainInRoll

	;       76543210	;read pitch gain
	ldi t,0b00000100
	rcall read_adc

	lds t,PotReverser
	tst t
	brmi ga2
	rcall invert
ga2:	b16store GainInPitch

	;       76543210	;read yaw gain
	ldi t,0b00000101
	rcall read_adc

	lds t,PotReverser
	tst t
	brmi ga3
	rcall invert
ga3:	b16store GainInYaw
	
	ret

invert:	ldi t,0x03		;Invert X
	eor xh,t
	ldi t,0xff
	eor xl,t
	ret


read_adc:			;x = adc    y = 0
;led2_on
	sts admux,t		;set ADC channel
	;       76543210
	ldi t,0b11000110	;start ADC
	sts adcsra,t

re1:	lds t,adcsra		;wait for ADC to complete
	sbrc t,6
	rjmp re1

	lds xl,adcl		;read ADC
	lds xh,adch

	clr yl
	clr yh
;led2_off
	ret






waitms:			;wait zl * 0.1ms
	ldi t,199
wa1:	nop
	dec t
	brne wa1
	dec zl
	brne waitms
	ret





	;--- Read RX channels 1-4, pin change interrupt driven ---
	;these have to be fast as possible to minimize jitter in loop timed events

RxChannel1:
;led2_on
	ser RxChannelsUpdatingFlag

	in tisp, sreg

	sbis pind,1				;rising or falling?
	rjmp rx1m1


	lds RxChannel1StartL, tcnt1l		;rising, store the start value
	lds RxChannel1StartH, tcnt1h

	clr RxChannelsUpdatingFlag

	out sreg,tisp				;exit	
;led2_off
	reti


rx1m1:	lds RxChannel1L, tcnt1l			;falling, calculate the pulse length
	lds RxChannel1H, tcnt1h

	sub RxChannel1L, RxChannel1StartL
	sbc RxChannel1H, RxChannel1StartH

	clr RxChannelsUpdatingFlag

	out sreg,tisp				;exit	
;led2_off
	reti


RxChannel2:
;led2_on
	ser RxChannelsUpdatingFlag

	in tisp, sreg

	sbis pind,2				;rising or falling?
	rjmp rx2m1


	lds RxChannel2StartL, tcnt1l		;rising, store the start value
	lds RxChannel2StartH, tcnt1h

	clr RxChannelsUpdatingFlag

	out sreg,tisp				;exit	
;led2_off
	reti


rx2m1:	lds RxChannel2L, tcnt1l			;falling, calculate the pulse length
	lds RxChannel2H, tcnt1h

	sub RxChannel2L, RxChannel2StartL
	sbc RxChannel2H, RxChannel2StartH

	clr RxChannelsUpdatingFlag

	out sreg,tisp				;exit	
;led2_off
	reti



RxChannel3:
;led2_on
	ser RxChannelsUpdatingFlag

	in tisp, sreg

	sbis pind,3				;rising or falling?
	rjmp rx3m1


	lds RxChannel3StartL, tcnt1l		;rising, store the start value
	lds RxChannel3StartH, tcnt1h

	clr RxChannelsUpdatingFlag

	out sreg,tisp				;exit	
;led2_off
	reti


rx3m1:	lds RxChannel3L, tcnt1l			;falling, calculate the pulse length
	lds RxChannel3H, tcnt1h

	sub RxChannel3L, RxChannel3StartL
	sbc RxChannel3H, RxChannel3StartH

	clr RxChannelsUpdatingFlag

	out sreg,tisp				;exit	
;led2_off
	reti



RxChannel4:
;led2_on
	ser RxChannelsUpdatingFlag

	in tisp, sreg

	sbis pinb,7				;rising or falling?
	rjmp rx4m1


	lds RxChannel4StartL, tcnt1l		;rising, store the start value
	lds RxChannel4StartH, tcnt1h

	clr RxChannelsUpdatingFlag

	out sreg,tisp				;exit	
;led2_off
	reti


rx4m1:	lds RxChannel4L, tcnt1l			;falling, calculate the pulse length
	lds RxChannel4H, tcnt1h

	sub RxChannel4L, RxChannel4StartL
	sbc RxChannel4H, RxChannel4StartH

	clr RxChannelsUpdatingFlag

	out sreg,tisp				;exit	
;led2_off
	reti




	
	;--- Get and scale RX channel inputs ---


RxGetChannels:
	tst RxChannelsUpdatingFlag	;channel 1 (Roll)	
	brne RxGetChannels

	mov xl,RxChannel1L
	mov xh,RxChannel1H

	rcall XAbs

	clr yl
	clr yh
	b16store RxChannel

	rcall ScaleMinus100To100

	b16mov RxInRoll,RxChannel



c2:	tst RxChannelsUpdatingFlag	;channel 2 (Pitch)	
	brne c2

	mov xl,RxChannel2L
	mov xh,RxChannel2H

	rcall XAbs

	clr yl
	clr yh
	b16store RxChannel

	rcall ScaleMinus100To100

	b16mov RxInPitch,RxChannel



c3:	tst RxChannelsUpdatingFlag	;channel 3 (Collective)	
	brne c3

	mov xl,RxChannel3L
	mov xh,RxChannel3H

	rcall XAbs

	clr yl
	clr yh
	b16store RxChannel

	rcall Scale0To100

	b16mov RxInCollective,RxChannel



c4:	tst RxChannelsUpdatingFlag	;channel 4 (Yaw)	
	brne c4

	mov xl,RxChannel4L
	mov xh,RxChannel4H

	rcall XAbs

	clr yl
	clr yh
	b16store RxChannel

	rcall ScaleMinus100To100

	b16mov RxInYaw,RxChannel

	ret



XAbs:	tst xh		;X = ABS(X)
	brpl xa1

	ldi t,0xff
	eor xl,t
	eor xh,t
	
	ldi t,1
	add xl,t
	clr t
	adc xh,t

xa1:	ret


ScaleMinus100To100:
	b16ldi Temp, 1520
	b16sub RxChannel, Temp
	b16load RxChannel	;divide RxChannel by 4
	ldi t,2
	rcall FastDivide
	b16store RxChannel
	ret


Scale0To100:
	b16ldi Temp, 1120
	b16sub RxChannel, Temp
	brcc sc1
	b16ldi RxChannel, 0
sc1:	b16load RxChannel	;divide RxChannel by 8
	ldi t,3
	rcall FastDivide
	b16store RxChannel
	
	ret


LPF:	b16sub LowpassIn, LowpassOut	;Low pass filter

	b16load LowpassIn
	ldi t,3
	rcall FastDivide
	b16store LowpassIn

	b16add LowpassOut, LowpassIn

	ret


FastDivide:
	asr xh		;X.Y Fast divide by 2n
	ror xl
	ror yh
	ror yl
	dec t
	brne Fastdivide
	ret


FastMultiply:
	lsl yl
	rol yh
	rol xl
	rol xh
	dec t
	brne FastMultiply
	ret



ReadEeprom:
	out eearl,zl	;(Z) -> t
	out 0x22,zh

	ldi t,0x01
	out eecr,t

	in t, eedr
	ret


WriteEeprom:
	cli		;t -> (Z)

wr1:	sbic eecr,1
	rjmp wr1

	out eearl,zl
	out 0x22,zh

	out eedr,t

	;       76543210
	ldi t,0b00000100
	out eecr,t

	;       76543210
	ldi t,0b00000010
	out eecr,t

	sei

	ret


GetGyroDirections:
	clr zl				;Get roll gyro directions from EEPROM
	clr zh
	rcall ReadEeprom
	sts RollGyroDirection,t

	ldi zl,1			;Get pitch gyro directions from EEPROM
	clr zh
	rcall ReadEeprom
	sts PitchGyroDirection,t

	ldi zl,2			;Get yaw gyro directions from EEPROM
	clr zh
	rcall ReadEeprom
	sts YawGyroDirection,t

	ret


NegateXY: clr t				;Negate X:Y
	sub t, yl
	mov yl, t

	clr t
	sbc t, yh
	mov yh, t

	clr t
	sbc t, xl
	mov xl, t

	clr t
	sbc t, xh
	mov xh, t

	ret



	;--- Debug: ----
/*
ShowGyros:
	ldi xl,0x0d
	rcall SerbyteOut

	ldi xl,0x0a
	rcall SerbyteOut


	b16load GyroRoll
	rcall B1616Out
	ldi xl,' '
	rcall SerByteOut

	b16load GyroPitch
	rcall B1616Out
	ldi xl,' '
	rcall SerByteOut

	b16load GyroYaw
	rcall B1616Out
	ldi xl,' '
	rcall SerByteOut

	ret



ShowChannels:
	
	ldi xl,0x0d
	rcall SerbyteOut

	ldi xl,0x0a
	rcall SerbyteOut


	b16load RxInRoll
	rcall B1616Out
	ldi xl,' '
	rcall SerByteOut

	b16load RxInPitch
	rcall B1616Out
	ldi xl,' '
	rcall SerByteOut

	b16load RxInCollective
	rcall B1616Out
	ldi xl,' '
	rcall SerByteOut

	b16load RxInYaw
	rcall B1616Out
	ldi xl,' '
	rcall SerByteOut

	ret




B1616Out:
	push yl
	push yh
	push xl

	mov xl,xh
	rcall SerByteAsciiOut
	pop xl
	rcall SerByteAsciiOut
	ldi xl,'.'
	rcall SerByteOut
	pop xl
	rcall SerByteAsciiOut
	pop xl
	rcall SerByteAsciiOut
	ret


B16Out:
	push xl

	mov xl,xh
	rcall SerByteAsciiOut
	pop xl
	rcall SerByteAsciiOut
	ret



	;--- Debug: Output byte xl (ASCII) to serial port pin at 115200 8N1 ----

SerByteAsciiOut:


	push xl
	swap xl
	rcall su1		;high nibble
	pop xl
	rjmp su1		;low nibble

su1:	andi xl,0x0f
	ldi zl,low(su2*2)	;output one nibble in ASCII
	add zl,xl
	ldi zh,high(su2*2)
	clr xl
	adc zh,xl
	lpm xl,z
	rjmp SerByteOut

su2:	.db "0123456789ABCDEF"


		



	;--- Debug: Output byte xl (binary) to serial port pin at 115200 8N1 ----

SerByteOut:
	led3_off		;startbit
	nop
	nop
	nop

	rcall BaudRateDelay	

	ldi xh,8		;databits

se3:	ror xl

	brcc se1
	nop
	led3_on
	rjmp se2
se1:	led3_off
	nop
	nop

se2:	rcall BaudRateDelay

	dec xh
	brne se3

	nop
	nop
	nop
	nop

	led3_on			;stopbit
	nop 
	nop
	nop
	rcall BaudRateDelay

	ret

BaudRatedelay:

	ldi t,17
ba1:	dec t
	brne ba1
	ret
	

*/


.undef t

.include "1616mathlib_ram_v1.asm"






