	.file	"i2c_test.c"
	.arch msp430x2619

/* Hardware multiplier registers: */
__MPY=0x130
__MPYS=0x132
__MAC=0x134
__MACS=0x136
__OP2=0x138
__RESLO=0x13a
__RESHI=0x13c
__SUMEXT=0x13e

	.stabs	"C:\\mspgcc\\Projetos\\Anderson/",100,0,0,.Ltext0
	.stabs	"i2c_test.c",100,0,0,.Ltext0
	.text
.Ltext0:
	.stabs	"gcc2_compiled.",60,0,0,0
	.stabs	"int:t(0,1)=r(0,1);-32768;32767;",128,0,0,0
	.stabs	"char:t(0,2)=@s8;r(0,2);0;255;",128,0,0,0
	.stabs	"long int:t(0,3)=@s32;r(0,3);0020000000000;0017777777777;",128,0,0,0
	.stabs	"unsigned int:t(0,4)=r(0,4);0000000000000;0000000177777;",128,0,0,0
	.stabs	"long unsigned int:t(0,5)=@s32;r(0,5);0000000000000;0037777777777;",128,0,0,0
	.stabs	"long long int:t(0,6)=@s64;r(0,6);01000000000000000000000;0777777777777777777777;",128,0,0,0
	.stabs	"long long unsigned int:t(0,7)=@s64;r(0,7);0000000000000;01777777777777777777777;",128,0,0,0
	.stabs	"short int:t(0,8)=r(0,8);-32768;32767;",128,0,0,0
	.stabs	"short unsigned int:t(0,9)=r(0,9);0000000000000;0000000177777;",128,0,0,0
	.stabs	"signed char:t(0,10)=@s8;r(0,10);-128;127;",128,0,0,0
	.stabs	"unsigned char:t(0,11)=@s8;r(0,11);0;255;",128,0,0,0
	.stabs	"float:t(0,12)=r(0,1);4;0;",128,0,0,0
	.stabs	"double:t(0,13)=r(0,1);4;0;",128,0,0,0
	.stabs	"long double:t(0,14)=r(0,1);4;0;",128,0,0,0
	.stabs	"complex int:t(0,15)=s4real:(0,1),0,16;imag:(0,1),16,16;;",128,0,0,0
	.stabs	"complex float:t(0,16)=R3;8;0;",128,0,0,0
	.stabs	"complex double:t(0,17)=R3;8;0;",128,0,0,0
	.stabs	"complex long double:t(0,18)=R3;8;0;",128,0,0,0
	.stabs	"__builtin_va_list:t(0,19)=*(0,20)=(0,20)",128,0,0,0
	.stabs	"_Bool:t(0,21)=@s8;-16;",128,0,0,0
	.stabs	"i2c_test.c",130,0,0,0
	.stabs	"c:/mspgcc/msp430/include/msp430x20x3.h",130,0,0,0
	.stabs	"c:/mspgcc/msp430/include/iomacros.h",130,0,0,0
	.stabs	"c:/mspgcc/msp430/include/sys/inttypes.h",130,0,0,0
	.stabs	"int8_t:t(4,1)=(0,10)",128,0,38,0
	.stabs	"uint8_t:t(4,2)=(0,11)",128,0,39,0
	.stabs	"int16_t:t(4,3)=(0,1)",128,0,41,0
	.stabs	"uint16_t:t(4,4)=(0,4)",128,0,42,0
	.stabs	"int32_t:t(4,5)=(0,3)",128,0,44,0
	.stabs	"uint32_t:t(4,6)=(0,5)",128,0,45,0
	.stabs	"int64_t:t(4,7)=(0,6)",128,0,47,0
	.stabs	"uint64_t:t(4,8)=(0,7)",128,0,48,0
	.stabs	"intptr_t:t(4,9)=(4,3)",128,0,53,0
	.stabs	"uintptr_t:t(4,10)=(4,4)",128,0,54,0
	.stabn	162,0,0,0
	.stabn	162,0,0,0
	.stabs	"c:/mspgcc/msp430/include/msp430/basic_clock.h",130,0,0,0
	.stabn	162,0,0,0
	.stabs	"c:/mspgcc/msp430/include/msp430/flash.h",130,0,0,0
	.stabn	162,0,0,0
	.stabs	"c:/mspgcc/msp430/include/msp430/eprom.h",130,0,0,0
	.stabn	162,0,0,0
	.stabs	"c:/mspgcc/msp430/include/msp430/timera.h",130,0,0,0
	.stabs	"tactl_t:t(8,1)=(8,2)=s2taifg:(0,4),0,1;taie:(0,4),1,1;taclr:(0,4),2,1;dummy:(0,4),3,1;tamc:(0,4),4,2;taid:(0,4),6,2;tassel:(0,4),8,2;;",128,0,127,0
	.stabs	"tacctl_t:t(8,3)=(8,4)=s2ccifg:(0,4),0,1;cov:(0,4),1,1;out:(0,4),2,1;cci:(0,4),3,1;ccie:(0,4),4,1;outmod:(0,4),5,3;cap:(0,4),8,1;dummy:(0,4),9,1;scci:(0,4),10,1;scs:(0,4),11,1;ccis:(0,4),12,2;cm:(0,4),14,2;;",128,0,143,0
	.stabs	"timera_t:T(8,5)=s24ctl:(8,1),0,16;cctl0:(8,3),16,16;cctl1:(8,3),32,16;cctl2:(8,3),48,16;dummy:(8,6)=ar(8,7)=r(8,7);0000000000000;0000000177777;;0;3;(8,8)=B(0,4),64,64;tar:(0,4),128,16;taccr0:(0,4),144,16;taccr1:(0,4),160,16;taccr2:(0,4),176,16;;",128,0,0,0
	.stabn	162,0,0,0
	.stabs	"c:/mspgcc/msp430/include/msp430/gpio.h",130,0,0,0
	.stabs	"c:/mspgcc/msp430/include/msp430/iostructures.h",130,0,0,0
	.stabs	"port:T(10,1)=u1reg_p:(0,11),0,8;__pin:(10,2)=s1__p0:(0,11),0,1;__p1:(0,11),1,1;__p2:(0,11),2,1;__p3:(0,11),3,1;__p4:(0,11),4,1;__p5:(0,11),5,1;__p6:(0,11),6,1;__p7:(0,11),7,1;;,0,8;;",128,0,0,0
	.stabs	"ioregister_t:t(10,3)=(10,1)",128,0,39,0
	.stabs	"port_full_t:T(10,4)=s8in:(10,3),0,8;out:(10,3),8,8;dir:(10,3),16,8;ifg:(10,3),24,8;ies:(10,3),32,8;ie:(10,3),40,8;sel:(10,3),48,8;ren:(10,3),56,8;;",128,0,0,0
	.stabn	162,0,0,0
	.stabn	162,0,0,0
	.stabs	"c:/mspgcc/msp430/include/msp430/sd16.h",130,0,0,0
	.stabn	162,0,0,0
	.stabs	"c:/mspgcc/msp430/include/msp430/usi.h",130,0,0,0
	.stabn	162,0,0,0
	.stabs	"c:/mspgcc/msp430/include/msp430/common.h",130,0,0,0
	.stabn	162,0,0,0
	.stabn	162,0,0,0
.global	mask
	.type	mask,@object
	.size	mask,8
mask:
	.byte	-128
	.byte	64
	.byte	32
	.byte	16
	.byte	8
	.byte	4
	.byte	2
	.byte	1
	.p2align 1,0
	.stabs	"main:F(0,1)",36,0,43,main
.global	main
	.type	main,@function
/***********************
 * Function `main' 
 ***********************/
main:
	.stabn 68,0,43,.LM1-main
.LM1:
	/* prologue: frame size = 0 */
.L__FrameSize_main=0x0
.L__FrameOffset_main=0x0
	mov	#(__stack-0), r1
	/* prologue end (size=2) */

	.stabn 68,0,44,.LM2-main
.LM2:
	call	#configura_cpu
	.stabn 68,0,45,.LM3-main
.LM3:
	call	#init_i2c
	.stabn 68,0,47,.LM4-main
.LM4:
	mov	#0x0020+1, r15 
	and.b	#llo(-17), @r15
	.stabn 68,0,48,.LM5-main
.LM5:
	and.b	#llo(-33), @r15
	.stabn 68,0,49,.LM6-main
.LM6:
	mov	#mem_found, r15 
	cmp	#llo(0), r15
	jeq	.L7
	bis.b	#llo(32), &0x0020+1
.L3:
	.stabn 68,0,51,.LM7-main
.LM7:
.L4:
	jmp	.L4
.L7:
	bis.b	#llo(16), &0x0020+1
	jmp	.L3
	.stabn 68,0,56,.LM8-main
.LM8:

	/* epilogue: frame size=0 */
	br	#__stop_progExec__
	/* epilogue end (size=4) */
	/* function main size 32 (26) */
.Lfe1:
	.size	main,.Lfe1-main
/********* End of function ******/

.Lscope0:
	.stabs	"",36,0,0,.Lscope0-main
	.p2align 1,0
	.stabs	"configura_cpu:F(0,20)",36,0,59,configura_cpu
.global	configura_cpu
	.type	configura_cpu,@function
/***********************
 * Function `configura_cpu' 
 ***********************/
configura_cpu:
	.stabn 68,0,59,.LM9-configura_cpu
.LM9:
	/* prologue: frame size = 0 */
.L__FrameSize_configura_cpu=0x0
.L__FrameOffset_configura_cpu=0x0
	/* prologue end (size=0) */

	.stabn 68,0,60,.LM10-configura_cpu
.LM10:
	mov	#llo(23168), &0x0120 
	.stabn 68,0,62,.LM11-configura_cpu
.LM11:
	mov.b	&0x10FD, &0x0057
	.stabn 68,0,63,.LM12-configura_cpu
.LM12:
	mov.b	&0x10FC, &0x0056
	.stabn 68,0,64,.LM13-configura_cpu
.LM13:
	mov.b	#llo(0), &0x0058
	.stabn 68,0,65,.LM14-configura_cpu
.LM14:
	mov.b	#llo(0), &0x0053
	.stabn 68,0,67,.LM15-configura_cpu
.LM15:
	mov.b	#llo(51), &0x0022
	.stabn 68,0,68,.LM16-configura_cpu
.LM16:
	ret

	/* epilogue: not required */
	/* function configura_cpu size 17 (16) */
.Lfe2:
	.size	configura_cpu,.Lfe2-configura_cpu
/********* End of function ******/

.Lscope1:
	.stabs	"",36,0,0,.Lscope1-configura_cpu
	.p2align 1,0
	.stabs	"delayus:F(0,20)",36,0,73,delayus
	.stabs	"tempo:P(0,4)",64,0,72,15
.global	delayus
	.type	delayus,@function
/***********************
 * Function `delayus' 
 ***********************/
delayus:
	.stabn 68,0,73,.LM17-delayus
.LM17:
	/* prologue: frame size = 0 */
.L__FrameSize_delayus=0x0
.L__FrameOffset_delayus=0x0
	/* prologue end (size=0) */

	.stabn 68,0,76,.LM18-delayus
.LM18:
.LBB2:
	mov	#llo(0), r14 
	cmp	r15, r14
	jhs	.L16
.L14:
/* #APP */
	nop
/* #NOAPP */
	add	#llo(1), r14
	cmp	r15, r14
	jlo	.L14
.L16:
	.stabn 68,0,77,.LM19-delayus
.LM19:
	ret
.LBE2:

	/* epilogue: not required */
	/* function delayus size 9 (9) */
.Lfe3:
	.size	delayus,.Lfe3-delayus
/********* End of function ******/

	.stabs	"i:r(0,1)",64,0,74,14
	.stabn	192,0,0,.LBB2-delayus
	.stabn	224,0,0,.LBE2-delayus
.Lscope2:
	.stabs	"",36,0,0,.Lscope2-delayus
	.p2align 1,0
	.stabs	"delayms:F(0,20)",36,0,82,delayms
	.stabs	"tempo:P(0,4)",64,0,81,10
.global	delayms
	.type	delayms,@function
/***********************
 * Function `delayms' 
 ***********************/
delayms:
	.stabn 68,0,82,.LM20-delayms
.LM20:
	/* prologue: frame size = 0 */
.L__FrameSize_delayms=0x0
.L__FrameOffset_delayms=0x4
	push	r11
	push	r10
	/* prologue end (size=2) */

	mov	r15, r10 
	.stabn 68,0,85,.LM21-delayms
.LM21:
.LBB3:
	mov	#llo(0), r11 
	cmp	r15, r11
	jhs	.L24
.L22:
	mov	#llo(800), r15 
	call	#delayus
	add	#llo(1), r11
	cmp	r10, r11
	jlo	.L22
.L24:
	.stabn 68,0,86,.LM22-delayms
.LM22:
.LBE3:

	/* epilogue: frame size=0 */
	pop	r10
	pop	r11
	ret
	/* epilogue end (size=3) */
	/* function delayms size 16 (11) */
.Lfe4:
	.size	delayms,.Lfe4-delayms
/********* End of function ******/

	.stabs	"i:r(0,1)",64,0,83,11
	.stabn	192,0,0,.LBB3-delayms
	.stabn	224,0,0,.LBE3-delayms
.Lscope3:
	.stabs	"",36,0,0,.Lscope3-delayms
	.p2align 1,0
	.stabs	"init_i2c:F(0,20)",36,0,92,init_i2c
.global	init_i2c
	.type	init_i2c,@function
/***********************
 * Function `init_i2c' 
 ***********************/
init_i2c:
	.stabn 68,0,92,.LM23-init_i2c
.LM23:
	/* prologue: frame size = 0 */
.L__FrameSize_init_i2c=0x0
.L__FrameOffset_init_i2c=0x0
	/* prologue end (size=0) */

	.stabn 68,0,93,.LM24-init_i2c
.LM24:
	mov.b	#llo(51), &0x0022
	.stabn 68,0,94,.LM25-init_i2c
.LM25:
	mov	#0x0020+1, r15 
	bis.b	#llo(1), @r15
	.stabn 68,0,95,.LM26-init_i2c
.LM26:
	bis.b	#llo(2), @r15
	.stabn 68,0,96,.LM27-init_i2c
.LM27:
	ret

	/* epilogue: not required */
	/* function init_i2c size 10 (9) */
.Lfe5:
	.size	init_i2c,.Lfe5-init_i2c
/********* End of function ******/

.Lscope4:
	.stabs	"",36,0,0,.Lscope4-init_i2c
	.p2align 1,0
	.stabs	"start_i2c:F(0,20)",36,0,99,start_i2c
.global	start_i2c
	.type	start_i2c,@function
/***********************
 * Function `start_i2c' 
 ***********************/
start_i2c:
	.stabn 68,0,99,.LM28-start_i2c
.LM28:
	/* prologue: frame size = 0 */
.L__FrameSize_start_i2c=0x0
.L__FrameOffset_start_i2c=0x2
	push	r11
	/* prologue end (size=1) */

	.stabn 68,0,100,.LM29-start_i2c
.LM29:
	mov	#0x0020+1, r11 
	bis.b	#llo(1), @r11
	.stabn 68,0,101,.LM30-start_i2c
.LM30:
	mov	#llo(2), r15 
	call	#delayus
	.stabn 68,0,102,.LM31-start_i2c
.LM31:
	bis.b	#llo(2), @r11
	.stabn 68,0,103,.LM32-start_i2c
.LM32:
	mov	#llo(4), r15 
	call	#delayus
	.stabn 68,0,104,.LM33-start_i2c
.LM33:
	bic.b	#llo(1),@r11
	.stabn 68,0,105,.LM34-start_i2c
.LM34:
	mov	#llo(4), r15 
	call	#delayus
	.stabn 68,0,106,.LM35-start_i2c
.LM35:
	bic.b	#llo(2),@r11
	.stabn 68,0,107,.LM36-start_i2c
.LM36:
	mov	#llo(2), r15 
	call	#delayus
	.stabn 68,0,108,.LM37-start_i2c
.LM37:

	/* epilogue: frame size=0 */
	pop	r11
	ret
	/* epilogue end (size=2) */
	/* function start_i2c size 25 (22) */
.Lfe6:
	.size	start_i2c,.Lfe6-start_i2c
/********* End of function ******/

.Lscope5:
	.stabs	"",36,0,0,.Lscope5-start_i2c
	.p2align 1,0
	.stabs	"stop_i2c:F(0,20)",36,0,111,stop_i2c
.global	stop_i2c
	.type	stop_i2c,@function
/***********************
 * Function `stop_i2c' 
 ***********************/
stop_i2c:
	.stabn 68,0,111,.LM38-stop_i2c
.LM38:
	/* prologue: frame size = 0 */
.L__FrameSize_stop_i2c=0x0
.L__FrameOffset_stop_i2c=0x2
	push	r11
	/* prologue end (size=1) */

	.stabn 68,0,112,.LM39-stop_i2c
.LM39:
	mov	#0x0020+1, r11 
	bic.b	#llo(1),@r11
	.stabn 68,0,113,.LM40-stop_i2c
.LM40:
	mov	#llo(2), r15 
	call	#delayus
	.stabn 68,0,114,.LM41-stop_i2c
.LM41:
	bis.b	#llo(2), @r11
	.stabn 68,0,115,.LM42-stop_i2c
.LM42:
	mov	#llo(4), r15 
	call	#delayus
	.stabn 68,0,116,.LM43-stop_i2c
.LM43:
	bis.b	#llo(1), @r11
	.stabn 68,0,117,.LM44-stop_i2c
.LM44:
	mov	#llo(4), r15 
	call	#delayus
	.stabn 68,0,118,.LM45-stop_i2c
.LM45:

	/* epilogue: frame size=0 */
	pop	r11
	ret
	/* epilogue end (size=2) */
	/* function stop_i2c size 20 (17) */
.Lfe7:
	.size	stop_i2c,.Lfe7-stop_i2c
/********* End of function ******/

.Lscope6:
	.stabs	"",36,0,0,.Lscope6-stop_i2c
	.p2align 1,0
	.stabs	"send_0:F(0,20)",36,0,121,send_0
.global	send_0
	.type	send_0,@function
/***********************
 * Function `send_0' 
 ***********************/
send_0:
	.stabn 68,0,121,.LM46-send_0
.LM46:
	/* prologue: frame size = 0 */
.L__FrameSize_send_0=0x0
.L__FrameOffset_send_0=0x2
	push	r11
	/* prologue end (size=1) */

	.stabn 68,0,122,.LM47-send_0
.LM47:
	mov	#0x0020+1, r11 
	bic.b	#llo(1),@r11
	.stabn 68,0,123,.LM48-send_0
.LM48:
	mov	#llo(2), r15 
	call	#delayus
	.stabn 68,0,124,.LM49-send_0
.LM49:
	bis.b	#llo(2), @r11
	.stabn 68,0,125,.LM50-send_0
.LM50:
	mov	#llo(4), r15 
	call	#delayus
	.stabn 68,0,126,.LM51-send_0
.LM51:
	bic.b	#llo(2),@r11
	.stabn 68,0,127,.LM52-send_0
.LM52:
	mov	#llo(2), r15 
	call	#delayus
	.stabn 68,0,128,.LM53-send_0
.LM53:

	/* epilogue: frame size=0 */
	pop	r11
	ret
	/* epilogue end (size=2) */
	/* function send_0 size 20 (17) */
.Lfe8:
	.size	send_0,.Lfe8-send_0
/********* End of function ******/

.Lscope7:
	.stabs	"",36,0,0,.Lscope7-send_0
	.p2align 1,0
	.stabs	"send_1:F(0,20)",36,0,131,send_1
.global	send_1
	.type	send_1,@function
/***********************
 * Function `send_1' 
 ***********************/
send_1:
	.stabn 68,0,131,.LM54-send_1
.LM54:
	/* prologue: frame size = 0 */
.L__FrameSize_send_1=0x0
.L__FrameOffset_send_1=0x2
	push	r11
	/* prologue end (size=1) */

	.stabn 68,0,132,.LM55-send_1
.LM55:
	mov	#0x0020+1, r11 
	bis.b	#llo(1), @r11
	.stabn 68,0,133,.LM56-send_1
.LM56:
	mov	#llo(2), r15 
	call	#delayus
	.stabn 68,0,134,.LM57-send_1
.LM57:
	bis.b	#llo(2), @r11
	.stabn 68,0,135,.LM58-send_1
.LM58:
	mov	#llo(4), r15 
	call	#delayus
	.stabn 68,0,136,.LM59-send_1
.LM59:
	bic.b	#llo(2),@r11
	.stabn 68,0,137,.LM60-send_1
.LM60:
	mov	#llo(2), r15 
	call	#delayus
	.stabn 68,0,138,.LM61-send_1
.LM61:
	bic.b	#llo(1),@r11
	.stabn 68,0,139,.LM62-send_1
.LM62:
	mov	#llo(2), r15 
	call	#delayus
	.stabn 68,0,140,.LM63-send_1
.LM63:

	/* epilogue: frame size=0 */
	pop	r11
	ret
	/* epilogue end (size=2) */
	/* function send_1 size 25 (22) */
.Lfe9:
	.size	send_1,.Lfe9-send_1
/********* End of function ******/

.Lscope8:
	.stabs	"",36,0,0,.Lscope8-send_1
	.p2align 1,0
	.stabs	"lebit:F(0,11)",36,0,143,lebit
.global	lebit
	.type	lebit,@function
/***********************
 * Function `lebit' 
 ***********************/
lebit:
	.stabn 68,0,143,.LM64-lebit
.LM64:
	/* prologue: frame size = 0 */
.L__FrameSize_lebit=0x0
.L__FrameOffset_lebit=0x2
	push	r11
	/* prologue end (size=1) */

	.stabn 68,0,146,.LM65-lebit
.LM65:
.LBB4:
	bis.b	#llo(2), &0x0020+1
	.stabn 68,0,147,.LM66-lebit
.LM66:
	mov	#llo(2), r15 
	call	#delayus
	.stabn 68,0,148,.LM67-lebit
.LM67:
	bit.b	#llo(1),&0x0020
	jeq	.L31
	mov.b	#llo(1), r11
	jmp	.L32
.L31:
	mov.b	#llo(0), r11
.L32:
	.stabn 68,0,149,.LM68-lebit
.LM68:
	mov	#llo(2), r15 
	call	#delayus
	.stabn 68,0,150,.LM69-lebit
.LM69:
	bic.b	#llo(2),&0x0020+1
	.stabn 68,0,151,.LM70-lebit
.LM70:
	mov	#llo(2), r15 
	call	#delayus
	.stabn 68,0,153,.LM71-lebit
.LM71:
	mov.b	r11, r15
	.stabn 68,0,154,.LM72-lebit
.LM72:
.LBE4:

	/* epilogue: frame size=0 */
	pop	r11
	ret
	/* epilogue end (size=2) */
	/* function lebit size 24 (21) */
.Lfe10:
	.size	lebit,.Lfe10-lebit
/********* End of function ******/

	.stabs	"v:r(0,11)",64,0,144,11
	.stabn	192,0,0,.LBB4-lebit
	.stabn	224,0,0,.LBE4-lebit
.Lscope9:
	.stabs	"",36,0,0,.Lscope9-lebit
	.p2align 1,0
	.stabs	"le_i2c:F(0,11)",36,0,157,le_i2c
	.stabs	"acknowledge:P(0,11)",64,0,156,8
.global	le_i2c
	.type	le_i2c,@function
/***********************
 * Function `le_i2c' 
 ***********************/
le_i2c:
	.stabn 68,0,157,.LM73-le_i2c
.LM73:
	/* prologue: frame size = 0 */
.L__FrameSize_le_i2c=0x0
.L__FrameOffset_le_i2c=0x8
	push	r11
	push	r10
	push	r9
	push	r8
	/* prologue end (size=4) */

	mov.b	r15, r8
	.stabn 68,0,158,.LM74-le_i2c
.LM74:
.LBB5:
	mov.b	#llo(0), r9
	.stabn 68,0,160,.LM75-le_i2c
.LM75:
	mov.b	#llo(50), &0x0022
	.stabn 68,0,161,.LM76-le_i2c
.LM76:
	mov.b	#llo(0), r11
	mov	#mask, r10 
.L39:
	.stabn 68,0,162,.LM77-le_i2c
.LM77:
	call	#lebit
	cmp.b	#llo(0), r15
	jeq	.L36
	add.b	@r10, r9
	.stabn 68,0,161,.LM78-le_i2c
.LM78:
.L36:
	add	#llo(1), r10
	add.b	#llo(1), r11
	cmp.b	#llo(8), r11
	jlo	.L39
	.stabn 68,0,164,.LM79-le_i2c
.LM79:
	mov.b	#llo(51), &0x0022
	.stabn 68,0,165,.LM80-le_i2c
.LM80:
	cmp.b	#llo(0), r8
	jne	.L40
	call	#send_0
	jmp	.L41
.L40:
	call	#send_1
.L41:
	.stabn 68,0,167,.LM81-le_i2c
.LM81:
	mov.b	r9, r15
	.stabn 68,0,168,.LM82-le_i2c
.LM82:
.LBE5:

	/* epilogue: frame size=0 */
	pop	r8
	pop	r9
	pop	r10
	pop	r11
	ret
	/* epilogue end (size=5) */
	/* function le_i2c size 38 (29) */
.Lfe11:
	.size	le_i2c,.Lfe11-le_i2c
/********* End of function ******/

	.stabs	"i:r(0,11)",64,0,158,11
	.stabs	"x:r(0,11)",64,0,158,9
	.stabn	192,0,0,.LBB5-le_i2c
	.stabn	224,0,0,.LBE5-le_i2c
.Lscope10:
	.stabs	"",36,0,0,.Lscope10-le_i2c
	.p2align 1,0
	.stabs	"escreve_i2c:F(0,11)",36,0,171,escreve_i2c
	.stabs	"dado:P(0,11)",64,0,170,9
.global	escreve_i2c
	.type	escreve_i2c,@function
/***********************
 * Function `escreve_i2c' 
 ***********************/
escreve_i2c:
	.stabn 68,0,171,.LM83-escreve_i2c
.LM83:
	/* prologue: frame size = 0 */
.L__FrameSize_escreve_i2c=0x0
.L__FrameOffset_escreve_i2c=0x6
	push	r11
	push	r10
	push	r9
	/* prologue end (size=3) */

	mov.b	r15, r9
	.stabn 68,0,174,.LM84-escreve_i2c
.LM84:
.LBB6:
	mov.b	#llo(0), r10
	mov	#mask, r11 
.L51:
	.stabn 68,0,175,.LM85-escreve_i2c
.LM85:
	mov.b	@r11, r15
	add	#llo(1), r11
	and.b	r9, r15
	jeq	.L49
	call	#send_1
	.stabn 68,0,174,.LM86-escreve_i2c
.LM86:
.L47:
	add.b	#llo(1), r10
	cmp.b	#llo(8), r10
	jlo	.L51
	jmp	.L54
.L49:
	call	#send_0
	jmp	.L47
.L54:
	.stabn 68,0,177,.LM87-escreve_i2c
.LM87:
	mov.b	#llo(50), &0x0022
	.stabn 68,0,178,.LM88-escreve_i2c
.LM88:
	call	#lebit
	.stabn 68,0,179,.LM89-escreve_i2c
.LM89:
	mov.b	#llo(51), &0x0022
	.stabn 68,0,181,.LM90-escreve_i2c
.LM90:
	and.b	#-1,r15
	.stabn 68,0,182,.LM91-escreve_i2c
.LM91:
.LBE6:

	/* epilogue: frame size=0 */
	pop	r9
	pop	r10
	pop	r11
	ret
	/* epilogue end (size=4) */
	/* function escreve_i2c size 35 (28) */
.Lfe12:
	.size	escreve_i2c,.Lfe12-escreve_i2c
/********* End of function ******/

	.stabs	"i:r(0,11)",64,0,172,10
	.stabn	192,0,0,.LBB6-escreve_i2c
	.stabn	224,0,0,.LBE6-escreve_i2c
.Lscope11:
	.stabs	"",36,0,0,.Lscope11-escreve_i2c
	.p2align 1,0
	.stabs	"mem_found:F(0,11)",36,0,187,mem_found
	.stabs	"address:P(0,11)",64,0,186,11
.global	mem_found
	.type	mem_found,@function
/***********************
 * Function `mem_found' 
 ***********************/
mem_found:
	.stabn 68,0,187,.LM92-mem_found
.LM92:
	/* prologue: frame size = 0 */
.L__FrameSize_mem_found=0x0
.L__FrameOffset_mem_found=0x2
	push	r11
	/* prologue end (size=1) */

	mov.b	r15, r11
.LBB7:
	.stabn 68,0,190,.LM93-mem_found
.LM93:
	call	#start_i2c
	.stabn 68,0,191,.LM94-mem_found
.LM94:
	mov.b	r11, r15
	call	#escreve_i2c
	mov.b	r15, r11
	.stabn 68,0,192,.LM95-mem_found
.LM95:
	call	#stop_i2c
	.stabn 68,0,194,.LM96-mem_found
.LM96:
	mov.b	r11, r15
.LBE7:
	.stabn 68,0,195,.LM97-mem_found
.LM97:

	/* epilogue: frame size=0 */
	pop	r11
	ret
	/* epilogue end (size=2) */
	/* function mem_found size 13 (10) */
.Lfe13:
	.size	mem_found,.Lfe13-mem_found
/********* End of function ******/

	.stabs	"v:r(0,11)",64,0,188,11
	.stabn	192,0,0,.LBB7-mem_found
	.stabn	224,0,0,.LBE7-mem_found
.Lscope12:
	.stabs	"",36,0,0,.Lscope12-mem_found
	.comm nunchuck_buf,6,2
	.stabs	"DCOCTL:G(1,1)=B(0,11)",32,0,20,0
	.stabs	"BCSCTL1:G(1,1)",32,0,22,0
	.stabs	"BCSCTL2:G(1,1)",32,0,24,0
	.stabs	"BCSCTL3:G(1,1)",32,0,27,0
	.stabs	"FCTL1:G(8,8)",32,0,20,0
	.stabs	"FCTL2:G(8,8)",32,0,22,0
	.stabs	"FCTL3:G(8,8)",32,0,24,0
	.stabs	"EPCTL:G(1,1)",32,0,19,0
	.stabs	"TA0IV:G(8,8)",32,0,25,0
	.stabs	"TA0CTL:G(8,8)",32,0,27,0
	.stabs	"TA0R:G(8,8)",32,0,29,0
	.stabs	"TA0CCTL0:G(8,8)",32,0,32,0
	.stabs	"TA0CCTL1:G(8,8)",32,0,34,0
	.stabs	"TA0CCR0:G(8,8)",32,0,36,0
	.stabs	"TA0CCR1:G(8,8)",32,0,38,0
	.stabs	"timera:G(8,5)",32,0,160,0
	.stabs	"port1:G(10,4)",32,0,136,0
	.stabs	"port2:G(10,4)",32,0,140,0
	.stabs	"P1IN:G(1,1)",32,0,114,0
	.stabs	"P1OUT:G(1,1)",32,0,116,0
	.stabs	"P1DIR:G(1,1)",32,0,118,0
	.stabs	"P1IFG:G(1,1)",32,0,120,0
	.stabs	"P1IES:G(1,1)",32,0,122,0
	.stabs	"P1IE:G(1,1)",32,0,124,0
	.stabs	"P1SEL:G(1,1)",32,0,126,0
	.stabs	"P1REN:G(1,1)",32,0,129,0
	.stabs	"P2IN:G(1,1)",32,0,135,0
	.stabs	"P2OUT:G(1,1)",32,0,137,0
	.stabs	"P2DIR:G(1,1)",32,0,139,0
	.stabs	"P2IFG:G(1,1)",32,0,141,0
	.stabs	"P2IES:G(1,1)",32,0,143,0
	.stabs	"P2IE:G(1,1)",32,0,145,0
	.stabs	"P2SEL:G(1,1)",32,0,147,0
	.stabs	"P2REN:G(1,1)",32,0,150,0
	.stabs	"SD16CTL:G(8,8)",32,0,23,0
	.stabs	"SD16IV:G(8,8)",32,0,25,0
	.stabs	"SD16AE:G(1,1)",32,0,29,0
	.stabs	"SD16INCTL0:G(1,1)",32,0,39,0
	.stabs	"SD16PRE0:G(1,1)",32,0,41,0
	.stabs	"SD16CCTL0:G(8,8)",32,0,43,0
	.stabs	"SD16MEM0:G(8,8)",32,0,45,0
	.stabs	"USICTL0:G(1,1)",32,0,20,0
	.stabs	"USICTL1:G(1,1)",32,0,22,0
	.stabs	"USICKCTL:G(1,1)",32,0,24,0
	.stabs	"USICNT:G(1,1)",32,0,26,0
	.stabs	"USISRL:G(1,1)",32,0,28,0
	.stabs	"USISRH:G(1,1)",32,0,30,0
	.stabs	"USICTL:G(8,8)",32,0,32,0
	.stabs	"USICCTL:G(8,8)",32,0,35,0
	.stabs	"USISR:G(8,8)",32,0,38,0
	.stabs	"WDTCTL:G(8,8)",32,0,71,0
	.stabs	"IE1:G(1,1)",32,0,39,0
	.stabs	"IFG1:G(1,1)",32,0,46,0
	.stabs	"CALDCO_16MHZ:G(1,1)",32,0,58,0
	.stabs	"CALBC1_16MHZ:G(1,1)",32,0,60,0
	.stabs	"CALDCO_12MHZ:G(1,1)",32,0,62,0
	.stabs	"CALBC1_12MHZ:G(1,1)",32,0,64,0
	.stabs	"CALDCO_8MHZ:G(1,1)",32,0,66,0
	.stabs	"CALBC1_8MHZ:G(1,1)",32,0,68,0
	.stabs	"CALDCO_1MHZ:G(1,1)",32,0,70,0
	.stabs	"CALBC1_1MHZ:G(1,1)",32,0,72,0
	.stabs	"mask:G(1,2)=ar(8,7);0;7;(1,3)=k(0,11)",32,0,17,0
	.stabs	"nunchuck_buf:G(1,4)=ar(8,7);0;5;(0,11)",32,0,19,0
	.text
	.stabs "",100,0,0,Letext
Letext:

/*********************************************************************
 * File i2c_test.c: code size: 284 words (0x11c)
 * incl. words in prologues: 17, epilogues: 30
 *********************************************************************/
