;CodeVisionAVR C Compiler V1.23.6a Standard
;(C) Copyright 1998-2002 HP InfoTech s.r.l.
;http://www.hpinfotech.ro
;e-mail:office@hpinfotech.ro , hpinfotech@xnet.ro

;Chip type           : AT90S8535
;Clock frequency     : 4.000000 MHz
;Memory model        : Small
;Optimize for        : Size
;(s)printf features  : int, width
;Internal SRAM size  : 512
;External SRAM size  : 0
;Data Stack size     : 128
;Promote char to int : No
;char is unsigned    : Yes
;8 bit enums         : Yes
;Automatic register allocation : On
;Use AVR Studio Terminal I/O   : No

	.DEVICE AT90S8535
	.LISTMAC
	.EQU UDRE=0x5
	.EQU RXC=0x7
	.EQU USR=0xB
	.EQU UDR=0xC
	.EQU EERE=0x0
	.EQU EEWE=0x1
	.EQU EEMWE=0x2
	.EQU SPSR=0xE
	.EQU SPDR=0xF
	.EQU EECR=0x1C
	.EQU EEDR=0x1D
	.EQU EEARL=0x1E
	.EQU EEARH=0x1F
	.EQU WDTCR=0x21
	.EQU MCUCR=0x35
	.EQU RAMPZ=0x3B
	.EQU SPL=0x3D
	.EQU SPH=0x3E
	.EQU SREG=0x3F
	.DEF R0X0=R0
	.DEF R0X1=R1
	.DEF R0X2=R2
	.DEF R0X3=R3
	.DEF R0X4=R4
	.DEF R0X5=R5
	.DEF R0X6=R6
	.DEF R0X7=R7
	.DEF R0X8=R8
	.DEF R0X9=R9
	.DEF R0XA=R10
	.DEF R0XB=R11
	.DEF R0XC=R12
	.DEF R0XD=R13
	.DEF R0XE=R14
	.DEF R0XF=R15
	.DEF R0X10=R16
	.DEF R0X11=R17
	.DEF R0X12=R18
	.DEF R0X13=R19
	.DEF R0X14=R20
	.DEF R0X15=R21
	.DEF R0X16=R22
	.DEF R0X17=R23
	.DEF R0X18=R24
	.DEF R0X19=R25
	.DEF R0X1A=R26
	.DEF R0X1B=R27
	.DEF R0X1C=R28
	.DEF R0X1D=R29
	.DEF R0X1E=R30
	.DEF R0X1F=R31
	.EQU __se_bit=0x40
	.EQU __sm_mask=0x30
	.EQU __sm_powerdown=0x20

	.MACRO __CPD1N
	CPI  R30,LOW(@0)
	LDI  R26,HIGH(@0)
	CPC  R31,R26
	LDI  R26,BYTE3(@0)
	CPC  R22,R26
	LDI  R26,BYTE4(@0)
	CPC  R23,R26
	.ENDM

	.MACRO __CPD2N
	CPI  R26,LOW(@0)
	LDI  R30,HIGH(@0)
	CPC  R27,R30
	LDI  R30,BYTE3(@0)
	CPC  R24,R30
	LDI  R30,BYTE4(@0)
	CPC  R25,R30
	.ENDM

	.MACRO __CPWRR
	CP   R@0,R@2
	CPC  R@1,R@3
	.ENDM

	.MACRO __CPWRN
	CPI  R@0,LOW(@2)
	LDI  R30,HIGH(@2)
	CPC  R@1,R30
	.ENDM

	.MACRO __ADDD1N
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	SBCI R22,BYTE3(-@0)
	SBCI R23,BYTE4(-@0)
	.ENDM

	.MACRO __ADDD2N
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	SBCI R24,BYTE3(-@0)
	SBCI R25,BYTE4(-@0)
	.ENDM

	.MACRO __SUBD1N
	SUBI R30,LOW(@0)
	SBCI R31,HIGH(@0)
	SBCI R22,BYTE3(@0)
	SBCI R23,BYTE4(@0)
	.ENDM

	.MACRO __SUBD2N
	SUBI R26,LOW(@0)
	SBCI R27,HIGH(@0)
	SBCI R24,BYTE3(@0)
	SBCI R25,BYTE4(@0)
	.ENDM

	.MACRO __ANDD1N
	ANDI R30,LOW(@0)
	ANDI R31,HIGH(@0)
	ANDI R22,BYTE3(@0)
	ANDI R23,BYTE4(@0)
	.ENDM

	.MACRO __ORD1N
	ORI  R30,LOW(@0)
	ORI  R31,HIGH(@0)
	ORI  R22,BYTE3(@0)
	ORI  R23,BYTE4(@0)
	.ENDM

	.MACRO __DELAY_USB
	LDI  R24,LOW(@0)
__DELAY_USB_LOOP:
	DEC  R24
	BRNE __DELAY_USB_LOOP
	.ENDM

	.MACRO __DELAY_USW
	LDI  R24,LOW(@0)
	LDI  R25,HIGH(@0)
__DELAY_USW_LOOP:
	SBIW R24,1
	BRNE __DELAY_USW_LOOP
	.ENDM

	.MACRO __CLRD1S
	CLR  R30
	STD  Y+@0,R30
	STD  Y+@0+1,R30
	STD  Y+@0+2,R30
	STD  Y+@0+3,R30
	.ENDM

	.MACRO __GETD1S
	LDD  R30,Y+@0
	LDD  R31,Y+@0+1
	LDD  R22,Y+@0+2
	LDD  R23,Y+@0+3
	.ENDM

	.MACRO __PUTD1S
	STD  Y+@0,R30
	STD  Y+@0+1,R31
	STD  Y+@0+2,R22
	STD  Y+@0+3,R23
	.ENDM

	.MACRO __POINTB1MN
	LDI  R30,LOW(@0+@1)
	.ENDM

	.MACRO __POINTW1MN
	LDI  R30,LOW(@0+@1)
	LDI  R31,HIGH(@0+@1)
	.ENDM

	.MACRO __POINTW1FN
	LDI  R30,LOW(2*@0+@1)
	LDI  R31,HIGH(2*@0+@1)
	.ENDM

	.MACRO __POINTB2MN
	LDI  R26,LOW(@0+@1)
	.ENDM

	.MACRO __POINTW2MN
	LDI  R26,LOW(@0+@1)
	LDI  R27,HIGH(@0+@1)
	.ENDM

	.MACRO __GETD1N
	LDI  R30,LOW(@0)
	LDI  R31,HIGH(@0)
	LDI  R22,BYTE3(@0)
	LDI  R23,BYTE4(@0)
	.ENDM

	.MACRO __GETD2N
	LDI  R26,LOW(@0)
	LDI  R27,HIGH(@0)
	LDI  R24,BYTE3(@0)
	LDI  R25,BYTE4(@0)
	.ENDM

	.MACRO __GETD2S
	LDD  R26,Y+@0
	LDD  R27,Y+@0+1
	LDD  R24,Y+@0+2
	LDD  R25,Y+@0+3
	.ENDM

	.MACRO __GETB1MN
	LDS  R30,@0+@1
	.ENDM

	.MACRO __GETW1MN
	LDS  R30,@0+@1
	LDS  R31,@0+@1+1
	.ENDM

	.MACRO __GETD1MN
	LDS  R30,@0+@1
	LDS  R31,@0+@1+1
	LDS  R22,@0+@1+2
	LDS  R23,@0+@1+3
	.ENDM

	.MACRO __GETBRMN
	LDS  R@2,@0+@1
	.ENDM

	.MACRO __GETWRMN
	LDS  R@2,@0+@1
	LDS  R@3,@0+@1+1
	.ENDM

	.MACRO __GETB2MN
	LDS  R26,@0+@1
	.ENDM

	.MACRO __GETW2MN
	LDS  R26,@0+@1
	LDS  R27,@0+@1+1
	.ENDM

	.MACRO __GETD2MN
	LDS  R26,@0+@1
	LDS  R27,@0+@1+1
	LDS  R24,@0+@1+2
	LDS  R25,@0+@1+3
	.ENDM

	.MACRO __PUTB1MN
	STS  @0+@1,R30
	.ENDM

	.MACRO __PUTW1MN
	STS  @0+@1,R30
	STS  @0+@1+1,R31
	.ENDM

	.MACRO __PUTD1MN
	STS  @0+@1,R30
	STS  @0+@1+1,R31
	STS  @0+@1+2,R22
	STS  @0+@1+3,R23
	.ENDM

	.MACRO __PUTBMRN
	STS  @0+@1,R@2
	.ENDM

	.MACRO __PUTWMRN
	STS  @0+@1,R@2
	STS  @0+@1+1,R@3
	.ENDM

	.MACRO __GETW1R
	MOV  R30,R@0
	MOV  R31,R@1
	.ENDM

	.MACRO __GETW2R
	MOV  R26,R@0
	MOV  R27,R@1
	.ENDM

	.MACRO __GETWRN
	LDI  R@0,LOW(@2)
	LDI  R@1,HIGH(@2)
	.ENDM

	.MACRO __PUTW1R
	MOV  R@0,R30
	MOV  R@1,R31
	.ENDM

	.MACRO __PUTW2R
	MOV  R@0,R26
	MOV  R@1,R27
	.ENDM

	.MACRO __ADDWRN
	SUBI R@0,LOW(-@2)
	SBCI R@1,HIGH(-@2)
	.ENDM

	.MACRO __ADDWRR
	ADD  R@0,R@2
	ADC  R@1,R@3
	.ENDM

	.MACRO __SUBWRN
	SUBI R@0,LOW(@2)
	SBCI R@1,HIGH(@2)
	.ENDM

	.MACRO __SUBWRR
	SUB  R@0,R@2
	SBC  R@1,R@3
	.ENDM

	.MACRO __ANDWRN
	ANDI R@0,LOW(@2)
	ANDI R@1,HIGH(@2)
	.ENDM

	.MACRO __ANDWRR
	AND  R@0,R@2
	AND  R@1,R@3
	.ENDM

	.MACRO __ORWRN
	ORI  R@0,LOW(@2)
	ORI  R@1,HIGH(@2)
	.ENDM

	.MACRO __ORWRR
	OR   R@0,R@2
	OR   R@1,R@3
	.ENDM

	.MACRO __EORWRR
	EOR  R@0,R@2
	EOR  R@1,R@3
	.ENDM

	.MACRO __GETWRS
	LDD  R@0,Y+@2
	LDD  R@1,Y+@2+1
	.ENDM

	.MACRO __PUTWSR
	STD  Y+@2,R@0
	STD  Y+@2+1,R@1
	.ENDM

	.MACRO __MOVEWRR
	MOV  R@0,R@2
	MOV  R@1,R@3
	.ENDM

	.MACRO __INWR
	IN   R@0,@2
	IN   R@1,@2+1
	.ENDM

	.MACRO __OUTWR
	OUT  @2+1,R@1
	OUT  @2,R@0
	.ENDM

	.MACRO __CALL1MN
	LDS  R30,@0+@1
	LDS  R31,@0+@1+1
	ICALL
	.ENDM

	.MACRO __NBST
	BST  R@0,@1
	IN   R30,SREG
	LDI  R31,0x40
	EOR  R30,R31
	OUT  SREG,R30
	.ENDM


	.MACRO __PUTB1SN
	LDD  R26,Y+@0
	LDD  R27,Y+@0+1
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	ST   X,R30
	.ENDM

	.MACRO __PUTW1SN
	LDD  R26,Y+@0
	LDD  R27,Y+@0+1
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1SN
	LDD  R26,Y+@0
	LDD  R27,Y+@0+1
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	RCALL __PUTDP1
	.ENDM

	.MACRO __PUTB1SNS
	LDD  R26,Y+@0
	LDD  R27,Y+@0+1
	ADIW R26,@1
	ST   X,R30
	.ENDM

	.MACRO __PUTW1SNS
	LDD  R26,Y+@0
	LDD  R27,Y+@0+1
	ADIW R26,@1
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1SNS
	LDD  R26,Y+@0
	LDD  R27,Y+@0+1
	ADIW R26,@1
	RCALL __PUTDP1
	.ENDM

	.MACRO __PUTB1PMN
	LDS  R26,@0
	LDS  R27,@0+1
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	ST   X,R30
	.ENDM

	.MACRO __PUTW1PMN
	LDS  R26,@0
	LDS  R27,@0+1
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1PMN
	LDS  R26,@0
	LDS  R27,@0+1
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	RCALL __PUTDP1
	.ENDM

	.MACRO __PUTB1PMNS
	LDS  R26,@0
	LDS  R27,@0+1
	ADIW R26,@1
	ST   X,R30
	.ENDM

	.MACRO __PUTW1PMNS
	LDS  R26,@0
	LDS  R27,@0+1
	ADIW R26,@1
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1PMNS
	LDS  R26,@0
	LDS  R27,@0+1
	ADIW R26,@1
	RCALL __PUTDP1
	.ENDM

	.MACRO __GETB1SX
	MOV  R30,R28
	MOV  R31,R29
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	LD   R30,Z
	.ENDM

	.MACRO __GETW1SX
	MOV  R30,R28
	MOV  R31,R29
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	LD   R0,Z+
	LD   R31,Z
	MOV  R30,R0
	.ENDM

	.MACRO __GETD1SX
	MOV  R30,R28
	MOV  R31,R29
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	LD   R0,Z+
	LD   R1,Z+
	LD   R22,Z+
	LD   R23,Z
	MOV  R30,R0
	MOV  R31,R1
	.ENDM

	.MACRO __GETB2SX
	MOV  R26,R28
	MOV  R27,R29
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	LD   R26,X
	.ENDM

	.MACRO __GETW2SX
	MOV  R26,R28
	MOV  R27,R29
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	LD   R0,X+
	LD   R27,X
	MOV  R26,R0
	.ENDM

	.MACRO __GETD2SX
	MOV  R26,R28
	MOV  R27,R29
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	LD   R0,X+
	LD   R1,X+
	LD   R24,X+
	LD   R25,X
	MOV  R26,R0
	MOV  R27,R1
	.ENDM

	.MACRO __GETBRSX
	MOV  R30,R28
	MOV  R31,R29
	SUBI R30,LOW(-@1)
	SBCI R31,HIGH(-@1)
	LD   R@0,Z
	.ENDM

	.MACRO __GETWRSX
	MOV  R30,R28
	MOV  R31,R29
	SUBI R30,LOW(-@2)
	SBCI R31,HIGH(-@2)
	LD   R@0,Z+
	LD   R@1,Z
	.ENDM

	.MACRO __LSLW8SX
	MOV  R30,R28
	MOV  R31,R29
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	LD   R31,Z
	CLR  R30
	.ENDM

	.MACRO __PUTB1SX
	MOV  R26,R28
	MOV  R27,R29
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	ST   X,R30
	.ENDM

	.MACRO __PUTW1SX
	MOV  R26,R28
	MOV  R27,R29
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1SX
	MOV  R26,R28
	MOV  R27,R29
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	ST   X+,R30
	ST   X+,R31
	ST   X+,R22
	ST   X,R23
	.ENDM

	.MACRO __CLRW1SX
	MOV  R30,R28
	MOV  R31,R29
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	CLR  R0
	ST   Z+,R0
	ST   Z,R0
	.ENDM

	.MACRO __CLRD1SX
	MOV  R30,R28
	MOV  R31,R29
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	CLR  R0
	ST   Z+,R0
	ST   Z+,R0
	ST   Z+,R0
	ST   Z,R0
	.ENDM

	.MACRO __PUTB2SX
	MOV  R30,R28
	MOV  R31,R29
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	ST   Z,R26
	.ENDM

	.MACRO __PUTW2SX
	MOV  R30,R28
	MOV  R31,R29
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	ST   Z+,R26
	ST   Z,R27
	.ENDM

	.MACRO __PUTBSRX
	MOV  R30,R28
	MOV  R31,R29
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	ST   Z,R@1
	.ENDM

	.MACRO __PUTWSRX
	MOV  R30,R28
	MOV  R31,R29
	SUBI R30,LOW(-@2)
	SBCI R31,HIGH(-@2)
	ST   Z+,R@0
	ST   Z,R@1
	.ENDM

	.MACRO __PUTB1SNX
	MOV  R26,R28
	MOV  R27,R29
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	LD   R0,X+
	LD   R27,X
	MOV  R26,R0
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	ST   X,R30
	.ENDM

	.MACRO __PUTW1SNX
	MOV  R26,R28
	MOV  R27,R29
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	LD   R0,X+
	LD   R27,X
	MOV  R26,R0
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1SNX
	MOV  R26,R28
	MOV  R27,R29
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	LD   R0,X+
	LD   R27,X
	MOV  R26,R0
	SUBI R26,LOW(-@1)
	SBCI R27,HIGH(-@1)
	ST   X+,R30
	ST   X+,R31
	ST   X+,R22
	ST   X,R23
	.ENDM

	.CSEG
	.ORG 0

	.INCLUDE "Main.vec"
	.INCLUDE "Main.inc"
__RESET:
	CLI
	CLR  R30
	OUT  EECR,R30
	OUT  MCUCR,R30

;DISABLE WATCHDOG
	LDI  R31,0x18
	OUT  WDTCR,R31
	LDI  R31,0x10
	OUT  WDTCR,R31

;CLEAR R2-R14
	LDI  R24,13
	LDI  R26,2
	CLR  R27
__CLEAR_REG:
	ST   X+,R30
	DEC  R24
	BRNE __CLEAR_REG

;CLEAR SRAM
	LDI  R24,LOW(0x200)
	LDI  R25,HIGH(0x200)
	LDI  R26,0x60
__CLEAR_SRAM:
	ST   X+,R30
	SBIW R24,1
	BRNE __CLEAR_SRAM

;GLOBAL VARIABLES INITIALIZATION
	LDI  R30,LOW(__GLOBAL_INI_TBL*2)
	LDI  R31,HIGH(__GLOBAL_INI_TBL*2)
__GLOBAL_INI_NEXT:
	LPM
	ADIW R30,1
	MOV  R1,R0
	LPM
	ADIW R30,1
	MOV  R22,R30
	MOV  R23,R31
	MOV  R31,R0
	MOV  R30,R1
	SBIW R30,0
	BREQ __GLOBAL_INI_END
	LPM
	ADIW R30,1
	MOV  R26,R0
	LPM
	ADIW R30,1
	MOV  R27,R0
	LPM
	ADIW R30,1
	MOV  R24,R0
	LPM
	ADIW R30,1
	MOV  R25,R0
__GLOBAL_INI_LOOP:
	LPM
	ADIW R30,1
	ST   X+,R0
	SBIW R24,1
	BRNE __GLOBAL_INI_LOOP
	MOV  R30,R22
	MOV  R31,R23
	RJMP __GLOBAL_INI_NEXT
__GLOBAL_INI_END:

;STACK POINTER INITIALIZATION
	LDI  R30,LOW(0x25F)
	OUT  SPL,R30
	LDI  R30,HIGH(0x25F)
	OUT  SPH,R30

;DATA STACK POINTER INITIALIZATION
	LDI  R28,LOW(0xDF)
	LDI  R29,HIGH(0xDF)

	RJMP _main

	.ESEG
	.ORG 0
	.DB  0 ; FIRST EEPROM LOCATION NOT USED, SEE ATMEL ERRATA SHEETS

	.DSEG
	.ORG 0xE0
;       1 /*
;       2 Project : X50Cpu
;       3 Version : 1.01
;       4 Date    : 1/9/2002
;       5 Author  : Jeff Greason                    
;       6 Company : True Logic
;       7 Comments: 
;       8 
;       9 
;      10 Chip type           : AT90S8535
;      11 Clock frequency     : 4.000000 MHz
;      12 Memory model        : Small
;      13 Internal SRAM size  : 512
;      14 External SRAM size  : 0
;      15 Data Stack size     : 128
;      16 *********************************************/
;      17 
;      18 #include <90s8535.h>  
;      19 
;      20 // Standard Input/Output functions
;      21 #include <stdio.h>      
;      22 #include <string.h>
;      23 #include <math.h>                       
;      24                   
;      25                   
;      26 //#define __48VOLTS	
;      27 #define __REV_3_95A
;      28 
;      29 
;      30 //#define _DEBUG_FREQ_OUTPUT
;      31 
;      32 #define _SOFTWARE_VERSION "\015\012V100 R3.12\015\012" 
;      33 #define __CURRENT_SCALE_FACTOR	1.0
;      34 
;      35 #ifdef __48VOLTS 
;      36 	#define	__NEW_NIMH_VOLTAGE	5225
;      37 	#define __NEW_SLA_VOLTAGE	5092
;      38 	#define __MIN_STARTUP_VOLTAGE	4400
;      39 	#define __LOW_BATTERY_VOLTAGE	3750
;      40 	#define __MAX_WATT_SECONDS 	1866000
;      41 	#define __USED_WATT_SECONDS	1492000
;      42 	#define __DEFAULT_WATT_SECONDS	380000
;      43 	#define __ADC0_DIV_FACTOR	0.186
;      44 
;      45 #else
;      46 	#define __NEW_NIMH_VOLTAGE     3920
;      47 	#define __NEW_SLA_VOLTAGE      3820
;      48 	#define __MIN_STARTUP_VOLTAGE	3300
;      49 	#define __LOW_BATTERY_VOLTAGE	2800
;      50 	#define __MAX_WATT_SECONDS 	1400000
;      51 	#define __USED_WATT_SECONDS	1120000
;      52 	#define __DEFAULT_WATT_SECONDS	280000
;      53 	#define __ADC0_DIV_FACTOR	0.2046
;      54 #endif
;      55 
;      56 
;      57 #define TIMER_FREQ     		29440 
;      58 #define TIMER_HALF_SECOND     14720  
;      59 #define TIEMR_LED_SCAN		3000
;      60 #define TIMER_LONG_BLINK       22080 
;      61 #define AUTO_SHUTOFF_TIME	120
;      62                                           
;      63 
;      64 
;      65 #define COM_SEQ_FREQ	1275   // 8 com outputs / second...
;      66 #define _CALC_SECOND	15
;      67 #define _MAX_LOW_VOLTAGE_TIME	80
;      68                           
;      69 #pragma warn-     
;      70 
;      71 
;      72 //eeprom int LastBattVoltage = 0;
;      73 eeprom long PowerWattSeconds[4] = { __DEFAULT_WATT_SECONDS, __DEFAULT_WATT_SECONDS, __DEFAULT_WATT_SECONDS, __DEFAULT_WATT_SECONDS };

	.ESEG
_PowerWattSeconds:
	.DW  0x45C0,0x4,0x45C0,0x4,0x45C0,0x4,0x45C0,0x4
;      74 
;      75 #pragma warn+
;      76         
;      77 //char szVerStr[19] = { _SOFTWARE_VERSION };
;      78 unsigned char SOC_LEDS[6] = { 0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f };

	.DSEG
_SOC_LEDS:
	.BYTE 0x6
;      79 int DischargePoints[5][5] = {
_DischargePoints:
;      80 { 3890, 3740, 3650, 3530, 3410 },
;      81 { 3640, 3550, 3490, 3340, 3220 },
;      82 { 3440, 3410, 3320, 3170, 2960 },
;      83 { 3280, 3250, 3160, 3010, 2800 },
;      84 { 3140, 3110, 2990, 2780, 2630 }
;      85 };
	.BYTE 0x32
;      86 
;      87 char BatteryPowerLow;
;      88 
;      89 
;      90 //char eeprom *eepptr;
;      91 //char *eepptr;  
;      92 //char *tptr;
;      93 
;      94 // Gear ratios
;      95 // Motor 11
;      96 // Wheel 45
;      97 // Tire  68.5" around
;      98 // Interupt freq 31.25 Khz
;      99 // Halls / Rotation 4:1
;     100            
;     101 #define	__PWM_RES_TOTAL 5 
;     102 
;     103 #define _LED_DATA_MSK	0x04;
;     104 #define _LED_CLK_MSK	0x08;
;     105 //#define _LED_SEL_MSK	0x10;
;     106 
;     107          
;     108             
;     109 #define TCOUNT_0	 78 
;     110 #define TCOUNT_1	65535 - 28 //  10Khz interupt rate... __PWM_RES_TOTAL
;     111 
;     112 #define _COM_BUFFER_SIZE	15              
;     113 unsigned char ComStr[ _COM_BUFFER_SIZE + 5 ];
_ComStr:
	.BYTE 0x14
;     114 unsigned char ComRecBuff[ _COM_BUFFER_SIZE + 1 ];   
_ComRecBuff:
	.BYTE 0x10
;     115 //unsigned char ComRecStr[ _COM_BUFFER_SIZE + 1 ];
;     116 
;     117 unsigned char RecBuffPtr; 
;     118 unsigned char RecByteRdy;
;     119  
;     120 
;     121 //unsigned char PhaseOutREV[8] = { 0x07, 0x26, 0x0d, 0x25, 0x13, 0x16, 0x0b, 0x07 };
;     122 unsigned char PhaseOutFWD[8] = { 0x07, 0x0b, 0x16, 0x13, 0x25, 0x0d, 0x26, 0x07 };
_PhaseOutFWD:
	.BYTE 0x8
;     123 //unsigned char PhaseSeq[8] = { 0, 3, 6, 2, 5, 1, 4, 0 };
;     124 unsigned char PhasePtr;
;     125 unsigned char PhaseErr;
;     126 
;     127 unsigned char OvrTemp;
;     128 unsigned char CruiseMode;
;     129                                                                                     
;     130 long MaxWattSeconds; // = 1400000; //1425000; 
_MaxWattSeconds:
	.BYTE 0x4
;     131 
;     132 unsigned char ActiveOut[8];
_ActiveOut:
	.BYTE 0x8
;     133 
;     134 void WriteDataToConsoleLEDS( unsigned char cdata );
;     135 
;     136 int AdcPWMVal;           
;     137 int AdcTempVal;
;     138 unsigned char HeatSinkTemp;    
_HeatSinkTemp:
	.BYTE 0x1
;     139  
;     140 unsigned char BatteryCurrent;
_BatteryCurrent:
	.BYTE 0x1
;     141 unsigned int BatteryVoltage; 
_BatteryVoltage:
	.BYTE 0x2
;     142 unsigned int AvgBatteryVoltage;
_AvgBatteryVoltage:
	.BYTE 0x2
;     143 unsigned int LowBatteryVoltage;
_LowBatteryVoltage:
	.BYTE 0x2
;     144 
;     145 //unsigned char PhaseCurrent;
;     146 char AutoUpdate;
_AutoUpdate:
	.BYTE 0x1
;     147 int iMaxEconoSpeed;   
_iMaxEconoSpeed:
	.BYTE 0x2
;     148 int iPhaseZero;
_iPhaseZero:
	.BYTE 0x2
;     149 int iPhaseCurrent;
_iPhaseCurrent:
	.BYTE 0x2
;     150 int iPhaseZeroPoints[ 6 ];
_iPhaseZeroPoints:
	.BYTE 0xC
;     151 
;     152 
;     153 unsigned char cPWMVal;
_cPWMVal:
	.BYTE 0x1
;     154 unsigned char OldcPWMVal;
_OldcPWMVal:
	.BYTE 0x1
;     155 
;     156 unsigned char hSensors;  
_hSensors:
	.BYTE 0x1
;     157 unsigned char prevhSensors;  
_prevhSensors:
	.BYTE 0x1
;     158 
;     159 
;     160 
;     161 unsigned char LEDbyte; 
_LEDbyte:
	.BYTE 0x1
;     162 unsigned char OldLEDbyte;
_OldLEDbyte:
	.BYTE 0x1
;     163 unsigned char calcLEDbyte;
_calcLEDbyte:
	.BYTE 0x1
;     164 unsigned char LEDStatus;
_LEDStatus:
	.BYTE 0x1
;     165 
;     166 unsigned char iTimerSOC;  // state of charge timer...
_iTimerSOC:
	.BYTE 0x1
;     167 
;     168 long CurrentWattSeconds;    
_CurrentWattSeconds:
	.BYTE 0x4
;     169 long SavedWattSeconds;
_SavedWattSeconds:
	.BYTE 0x4
;     170            
;     171      
;     172      
;     173 //unsigned char phase_drive;
;     174 long itrDelay;
_itrDelay:
	.BYTE 0x4
;     175 char isecTmr;
_isecTmr:
	.BYTE 0x1
;     176 int isecTimer;
_isecTimer:
	.BYTE 0x2
;     177 char isecTmrWatts;
_isecTmrWatts:
	.BYTE 0x1
;     178                     
;     179 unsigned char DriveEnable; 
_DriveEnable:
	.BYTE 0x1
;     180                          
;     181 unsigned int iMotorTimer; // Timer for Speed measurement Timer Routine
_iMotorTimer:
	.BYTE 0x2
;     182 unsigned int iMotorSpeed; // Raw counts for Timer Routine
_iMotorSpeed:
	.BYTE 0x2
;     183 int iMotorRPM; //
_iMotorRPM:
	.BYTE 0x2
;     184 //int iBikeMPH;                 
;     185 unsigned char CurrentLimitByte;
_CurrentLimitByte:
	.BYTE 0x1
;     186   
;     187 char ComSequence;
_ComSequence:
	.BYTE 0x1
;     188 float vtmp, ftmp;
_vtmp:
	.BYTE 0x4
_ftmp:
	.BYTE 0x4
;     189 
;     190 int itmp;
_itmp:
	.BYTE 0x2
;     191 unsigned char tmpChar = 0;            
_tmpChar:
	.BYTE 0x1
;     192 unsigned char tmpPWMVal;
_tmpPWMVal:
	.BYTE 0x1
;     193 
;     194 
;     195 //------------------------------------------------------
;     196 //  Function Declarations
;     197 //------------------------------------------------------
;     198 
;     199 unsigned int read_adc(unsigned char adc_input);   
;     200 void ComSend( char *UART_Str );    
;     201 unsigned char CalcStateOfChargeForConsole( void );
;     202 
;     203                           
;     204 unsigned char ConvertTempSensor( int ADCData );
;     205 void CalcCruiseControl( void );
;     206 void OutputComData( void );
;     207 
;     208 
;     209 void ProcessCommand( char *pRecStr );
;     210 void SendCRLFPrompt( void );
;     211               
;     212 void SaveWattSecondsToEPPROM( void );
;     213 void ReadWattSecondsFromEEPROM( void );
;     214 
;     215 int ReadBatteryVoltage( void );
;     216 
;     217 void ShutdownSystem( void );
;     218 
;     219 
;     220 void ShutdownSystem( void )
;     221 {       

	.CSEG
_ShutdownSystem:
;     222 	char tmpChar;
;     223 	cPWMVal = 0;  // turn throttle off and wait to power down...
	ST   -Y,R16
;	tmpChar -> R16
	RCALL SUBOPT_0x0
;     224 	
;     225 	tmpChar = 0;   
	LDI  R16,LOW(0)
;     226 	while( 1 )
_0x64:
;     227 	{
;     228 	  	WriteDataToConsoleLEDS( 0x01 );	
	RCALL SUBOPT_0x1
	RCALL SUBOPT_0x2
;     229 	  	
;     230 		itrDelay = 0;
;     231 		while( itrDelay < TIMER_LONG_BLINK ) 
_0x67:
	RCALL SUBOPT_0x3
	BRLT _0x67
;     232 		{
;     233 		}
;     234 		WriteDataToConsoleLEDS( 0x00 );	
	RCALL SUBOPT_0x4
	RCALL SUBOPT_0x2
;     235 	  	
;     236 	  	itrDelay = 0;
;     237 		while( itrDelay < TIMER_LONG_BLINK ) 
_0x6A:
	RCALL SUBOPT_0x3
	BRLT _0x6A
;     238 		{
;     239 		}
;     240 		tmpChar++;
	SUBI R16,-1
;     241 		            
;     242 		if( tmpChar > 5 )
	LDI  R30,LOW(5)
	CP   R30,R16
	BRSH _0x6D
;     243 		{
;     244 			cPWMVal = 0;
	RCALL SUBOPT_0x0
;     245 			PORTD = 0x20;	
	LDI  R30,LOW(32)
	OUT  0x12,R30
;     246 		}
;     247 	}	
_0x6D:
	RJMP _0x64
;     248 
;     249 }
	LD   R16,Y+
	RET
;     250 
;     251 int ReadBatteryVoltage( void )
;     252 {
_ReadBatteryVoltage:
;     253 	AdcTempVal = read_adc( 0 );
	RCALL SUBOPT_0x4
	RCALL SUBOPT_0x5
;     254 	ftmp = AdcTempVal;
;     255       	vtmp = ftmp / __ADC0_DIV_FACTOR;
;     256       	return( (int)vtmp );
	RCALL SUBOPT_0x6
	RET
;     257 }
;     258 
;     259 
;     260 void SaveWattSecondsToEPPROM( void )
;     261 {	
_SaveWattSecondsToEPPROM:
;     262 	// Read Battery Voltage      
;     263 	AdcTempVal = read_adc( 0 );
	RCALL SUBOPT_0x4
	RCALL SUBOPT_0x5
;     264 	ftmp = AdcTempVal;
;     265       	vtmp = ftmp / __ADC0_DIV_FACTOR;
;     266 	
;     267 	if( vtmp < 2700 )
	LDS  R26,_vtmp
	LDS  R27,_vtmp+1
	LDS  R24,_vtmp+2
	LDS  R25,_vtmp+3
	__GETD1N 0x4528C000
	RCALL __CMPF12
	BRSH _0x70
;     268 		return;
	RET
;     269 	                       
;     270 	if( CurrentWattSeconds < 50 )
_0x70:
	LDS  R26,_CurrentWattSeconds
	LDS  R27,_CurrentWattSeconds+1
	LDS  R24,_CurrentWattSeconds+2
	LDS  R25,_CurrentWattSeconds+3
	__CPD2N 0x32
	BRGE _0x71
;     271 		CurrentWattSeconds = 50;
	__GETD1N 0x32
	STS  _CurrentWattSeconds,R30
	STS  _CurrentWattSeconds+1,R31
	STS  _CurrentWattSeconds+2,R22
	STS  _CurrentWattSeconds+3,R23
;     272 		
;     273  	PowerWattSeconds[0] = CurrentWattSeconds;
_0x71:
	LDS  R30,_CurrentWattSeconds
	LDS  R31,_CurrentWattSeconds+1
	LDS  R22,_CurrentWattSeconds+2
	LDS  R23,_CurrentWattSeconds+3
	LDI  R26,LOW(_PowerWattSeconds)
	LDI  R27,HIGH(_PowerWattSeconds)
	RCALL __EEPROMWRD
;     274  	PowerWattSeconds[1] = CurrentWattSeconds;
	__POINTW2MN _PowerWattSeconds,4
	RCALL SUBOPT_0x7
;     275  	PowerWattSeconds[2] = CurrentWattSeconds;
	__POINTW2MN _PowerWattSeconds,8
	RCALL SUBOPT_0x7
;     276  	PowerWattSeconds[3] = CurrentWattSeconds; 		
	__POINTW2MN _PowerWattSeconds,12
	RCALL SUBOPT_0x7
;     277 }     
	RET
;     278 void ReadWattSecondsFromEEPROM( void )
;     279 {
_ReadWattSecondsFromEEPROM:
;     280  	// read the values and compare them to eachother...
;     281 	
;     282 	if( PowerWattSeconds[2] == PowerWattSeconds[0] )
	__POINTW2MN _PowerWattSeconds,8
	RCALL __EEPROMRDD
	PUSH R23
	PUSH R22
	PUSH R31
	PUSH R30
	RCALL SUBOPT_0x8
	POP  R26
	POP  R27
	POP  R24
	POP  R25
	RCALL __CPD12
	BRNE _0x73
;     283 	{
;     284 		SavedWattSeconds = PowerWattSeconds[0];
	RCALL SUBOPT_0x8
	STS  _SavedWattSeconds,R30
	STS  _SavedWattSeconds+1,R31
	STS  _SavedWattSeconds+2,R22
	STS  _SavedWattSeconds+3,R23
;     285         }
;     286         else if( PowerWattSeconds[3] == PowerWattSeconds[1] )
	RJMP _0x74
_0x73:
	__POINTW2MN _PowerWattSeconds,12
	RCALL __EEPROMRDD
	PUSH R23
	PUSH R22
	PUSH R31
	PUSH R30
	__POINTW2MN _PowerWattSeconds,4
	RCALL __EEPROMRDD
	POP  R26
	POP  R27
	POP  R24
	POP  R25
	RCALL __CPD12
	BRNE _0x75
;     287         {
;     288 		SavedWattSeconds = PowerWattSeconds[1];
	__POINTW2MN _PowerWattSeconds,4
	RCALL __EEPROMRDD
	RJMP _0x1C4
;     289         }                                            
;     290         else
_0x75:
;     291         {
;     292         	SavedWattSeconds = __DEFAULT_WATT_SECONDS;
	__GETD1N 0x445C0
_0x1C4:
	STS  _SavedWattSeconds,R30
	STS  _SavedWattSeconds+1,R31
	STS  _SavedWattSeconds+2,R22
	STS  _SavedWattSeconds+3,R23
;     293         }
_0x74:
;     294         
;     295         if( SavedWattSeconds > MaxWattSeconds )
	LDS  R30,_MaxWattSeconds
	LDS  R31,_MaxWattSeconds+1
	LDS  R22,_MaxWattSeconds+2
	LDS  R23,_MaxWattSeconds+3
	LDS  R26,_SavedWattSeconds
	LDS  R27,_SavedWattSeconds+1
	LDS  R24,_SavedWattSeconds+2
	LDS  R25,_SavedWattSeconds+3
	RCALL __CPD12
	BRGE _0x77
;     296         {
;     297          	// bad reading...
;     298          	SavedWattSeconds = __DEFAULT_WATT_SECONDS;
	RCALL SUBOPT_0x9
;     299         }                        
;     300         if( SavedWattSeconds < 0 )
_0x77:
	LDS  R26,_SavedWattSeconds
	LDS  R27,_SavedWattSeconds+1
	LDS  R24,_SavedWattSeconds+2
	LDS  R25,_SavedWattSeconds+3
	RCALL __CPD20
	BRGE _0x78
;     301         {
;     302         	SavedWattSeconds = __DEFAULT_WATT_SECONDS;
	RCALL SUBOPT_0x9
;     303 	}
;     304                                        
;     305 }
_0x78:
	RET
;     306 
;     307 // ComSend waits til the string is sent before returning 
;     308 void ComSend( char *UART_Str )
;     309 {
_ComSend:
;     310 	int ptr, tilt;
;     311   	tilt = 0;
	SBIW R28,4
;	*UART_Str -> Y+4
;	ptr -> Y+2
;	tilt -> Y+0
	CLR  R30
	STD  Y+0,R30
	STD  Y+0+1,R30
;     312 	ptr = 0;
	CLR  R30
	STD  Y+2,R30
	STD  Y+2+1,R30
;     313 	while( UART_Str[ptr] != 0 )
_0x7A:
	RCALL SUBOPT_0xA
	CPI  R30,0
	BREQ _0x7C
;     314 	{  	
;     315      		UDR = UART_Str[ptr];
	RCALL SUBOPT_0xA
	OUT  0xC,R30
;     316      		ptr++; 
	LDD  R30,Y+2
	LDD  R31,Y+2+1
	ADIW R30,1
	STD  Y+2,R30
	STD  Y+2+1,R31
;     317      		while(( USR & 0x40 ) == 0 )
_0x7D:
	SBIC 0xB,6
	RJMP _0x7F
;     318      		{               
;     319      			itrDelay = 0;
	CLR  R30
	STS  _itrDelay,R30
	STS  _itrDelay+1,R30
	STS  _itrDelay+2,R30
	STS  _itrDelay+3,R30
;     320   			while( itrDelay < 10 ) 
_0x80:
	LDS  R26,_itrDelay
	LDS  R27,_itrDelay+1
	LDS  R24,_itrDelay+2
	LDS  R25,_itrDelay+3
	__CPD2N 0xA
	BRLT _0x80
;     321 			{
;     322 			}
;     323 	
;     324 	 		//delay_it( 100 );
;     325      		}   
	RJMP _0x7D
_0x7F:
;     326       		//delay_it( 200 );     	
;     327 		itrDelay = 0;
	CLR  R30
	STS  _itrDelay,R30
	STS  _itrDelay+1,R30
	STS  _itrDelay+2,R30
	STS  _itrDelay+3,R30
;     328 		while( itrDelay < 20 ) 
_0x83:
	RCALL SUBOPT_0xB
	BRLT _0x83
;     329 		{
;     330 		}
;     331     }
	RJMP _0x7A
_0x7C:
;     332     while(( USR & 0x40 ) == 0 )
_0x86:
	SBIC 0xB,6
	RJMP _0x88
;     333     {             
;     334     		itrDelay = 0;
	CLR  R30
	STS  _itrDelay,R30
	STS  _itrDelay+1,R30
	STS  _itrDelay+2,R30
	STS  _itrDelay+3,R30
;     335 		while( itrDelay < 20 ) 
_0x89:
	RCALL SUBOPT_0xB
	BRLT _0x89
;     336 		{
;     337 		}
;     338 		//delay_it( 100 );    	     	
;     339     }
	RJMP _0x86
_0x88:
;     340 }
	ADIW R28,6
	RET
;     341 
;     342 interrupt [UART_RXC] void UART_IntReceived(void) 
;     343 {
_UART_IntReceived:
	RCALL __SAVEISR
;     344     unsigned char newchar;
;     345     newchar = UDR; 
	ST   -Y,R16
;	newchar -> R16
	IN   R16,12
;     346  
;     347         if (USR & 0x18) newchar = 0xff;              
	IN   R30,0xB
	ANDI R30,LOW(0x18)
	BREQ _0x8D
	LDI  R16,LOW(255)
;     348         ComRecBuff[RecBuffPtr] = newchar; 
_0x8D:
	RCALL SUBOPT_0xC
	ST   X,R16
;     349 		RecBuffPtr++; 
	INC  R5
;     350        
;     351         if (newchar == 0x0A)
	CPI  R16,10
	BRNE _0x8E
;     352         {          
;     353         	ComRecBuff[RecBuffPtr] = 0;   
	RCALL SUBOPT_0xC
	RCALL SUBOPT_0xD
;     354 
;     355            	RecByteRdy = RecBuffPtr;
	MOV  R6,R5
;     356         	RecBuffPtr = 0;
	CLR  R5
;     357         }
;     358 }
_0x8E:
	RJMP _0x1C3
;     359 
;     360 
;     361 // Timer 1 overflow interrupt service routine
;     362 interrupt [TIM1_OVF] void timer1_ovf_isr(void)
;     363 {                             
_timer1_ovf_isr:
	RCALL __SAVEISR
;     364 	unsigned char pcbyte;
;     365 		
;     366 	TCNT1=TCOUNT_1;
	ST   -Y,R16
;	pcbyte -> R16
	RCALL SUBOPT_0xE
;     367 
;     368 #ifdef _DEBUG_FREQ_OUTPUT
;     369 	PORTD = 0x04;
;     370 #endif
;     371 
;     372 	// save old value
;     373 	prevhSensors = hSensors;
	LDS  R30,_hSensors
	STS  _prevhSensors,R30
;     374 	
;     375 	// scan for new  HALL sensors
;     376 	hSensors = PINB & 0x07;
	IN   R30,0x16
	ANDI R30,LOW(0x7)
	STS  _hSensors,R30
;     377 //	hSensors = hSensors & 0x07;
;     378     	
;     379         pcbyte = ActiveOut[ hSensors ];
	CLR  R31
	SUBI R30,LOW(-_ActiveOut)
	SBCI R31,HIGH(-_ActiveOut)
	LD   R16,Z
;     380         
;     381 	if( prevhSensors != hSensors )
	LDS  R30,_hSensors
	LDS  R26,_prevhSensors
	CP   R30,R26
	BREQ _0x90
;     382 	{
;     383         	pcbyte = pcbyte | 0x07;
	ORI  R16,LOW(7)
;     384 	}
;     385         PORTC = pcbyte;
_0x90:
	OUT  0x15,R16
;     386 	PORTC = pcbyte + 0x40;
	MOV  R30,R16
	SUBI R30,-LOW(64)
	RCALL SUBOPT_0xF
;     387 	PORTC = 0x00;
;     388 
;     389 	PORTC = cPWMVal;    
	LDS  R30,_cPWMVal
	OUT  0x15,R30
;     390 	PORTC = cPWMVal + 0x80;
	SUBI R30,-LOW(128)
	RCALL SUBOPT_0xF
;     391 	PORTC = 0x00;
;     392   	
;     393 	// do freq measurement here
;     394 	
;     395 	if( iMotorTimer < TIMER_FREQ )
	LDS  R26,_iMotorTimer
	LDS  R27,_iMotorTimer+1
	CPI  R26,LOW(0x7300)
	LDI  R30,HIGH(0x7300)
	CPC  R27,R30
	BRSH _0x91
;     396 	{ 
;     397 		iMotorTimer++;               		
	LDS  R30,_iMotorTimer
	LDS  R31,_iMotorTimer+1
	ADIW R30,1
	STS  _iMotorTimer,R30
	STS  _iMotorTimer+1,R31
;     398 	}
;     399 	else
	RJMP _0x92
_0x91:
;     400 	{
;     401 	   	iMotorSpeed = iMotorTimer;
	RCALL SUBOPT_0x10
;     402 	}
_0x92:
;     403 	if((( prevhSensors & 0x01 ) == 0 )&&(( hSensors & 0x01 ) == 1 )) 
	LDS  R30,_prevhSensors
	ANDI R30,LOW(0x1)
	BRNE _0x94
	LDS  R30,_hSensors
	ANDI R30,LOW(0x1)
	CPI  R30,LOW(0x1)
	BREQ _0x95
_0x94:
	RJMP _0x93
_0x95:
;     404 	{
;     405 	   // hall A just went high again...
;     406 		iMotorSpeed = iMotorTimer;
	RCALL SUBOPT_0x10
;     407 		iMotorTimer = 0;   
	CLR  R30
	STS  _iMotorTimer,R30
	STS  _iMotorTimer+1,R30
;     408 	}
;     409 	itrDelay++;
_0x93:
	LDS  R30,_itrDelay
	LDS  R31,_itrDelay+1
	LDS  R22,_itrDelay+2
	LDS  R23,_itrDelay+3
	__SUBD1N -1
	STS  _itrDelay,R30
	STS  _itrDelay+1,R31
	STS  _itrDelay+2,R22
	STS  _itrDelay+3,R23
;     410 
;     411 #ifdef _DEBUG_FREQ_OUTPUT
;     412 	PORTD = 0x00;
;     413 #endif
;     414 
;     415 }
_0x1C3:
	LD   R16,Y+
	RCALL __LOADISR
	RETI
;     416 
;     417 
;     418 
;     419 #define ADC_VREF_TYPE 0x00
;     420 // Read the ADC conversion result
;     421 unsigned int read_adc(unsigned char adc_input)
;     422 {      
_read_adc:
;     423 	int ret = 0; //, av[4];
;     424 	char indx;
;     425 	
;     426 	for( indx = 0; indx <= 7; indx++ )
	RCALL __SAVELOCR3
;	adc_input -> Y+3
;	ret -> R16,R17
;	indx -> R18
	LDI  R16,0
	LDI  R17,0
	LDI  R18,LOW(0)
_0x98:
	LDI  R30,LOW(7)
	CP   R30,R18
	BRLO _0x99
;     427 	{	
;     428 		ADMUX=adc_input|ADC_VREF_TYPE;
	LDD  R30,Y+3
	OUT  0x7,R30
;     429 		ADCSR|=0x40;
	SBI  0x6,6
;     430 		while ((ADCSR&0x10)==0);
_0x9A:
	SBIS 0x6,4
	RJMP _0x9A
;     431 		ADCSR|=0x10;
	SBI  0x6,4
;     432 		ret+= ADCW;
	IN   R30,0x4
	IN   R31,0x4+1
	__ADDWRR 16,17,30,31
;     433 	}
	SUBI R18,-1
	RJMP _0x98
_0x99:
;     434 	ret = ret >> 3;     
	ASR  R17
	ROR  R16
	ASR  R17
	ROR  R16
	ASR  R17
	ROR  R16
;     435 	return ret;	
	__GETW1R 16,17
	RCALL __LOADLOCR3
	ADIW R28,4
	RET
;     436 }
;     437 
;     438 
;     439 void main( void )
;     440 {                             
_main:
;     441  
;     442 	PhasePtr = 1;
	LDI  R30,LOW(1)
	MOV  R7,R30
;     443 	PhaseErr = 0;    
	CLR  R8
;     444 	iTimerSOC = 0;
	RCALL SUBOPT_0x11
;     445     
;     446 	// Declare your local variables here
;     447 
;     448 	// Port A
;     449 	PORTA=0x00;
	CLR  R30
	OUT  0x1B,R30
;     450 	DDRA=0x00;
	OUT  0x1A,R30
;     451 
;     452 	// Port B
;     453 	PORTB=0x00;
	OUT  0x18,R30
;     454 	DDRB=0x00;
	OUT  0x17,R30
;     455 
;     456 	// Port C
;     457 	PORTC=0x00;
	RCALL SUBOPT_0x12
;     458 	DDRC=0xFF;
	LDI  R30,LOW(255)
	OUT  0x14,R30
;     459 
;     460 	// Port D
;     461 	PORTD = 0x00;
	CLR  R30
	OUT  0x12,R30
;     462 	DDRD=0xfc;
	LDI  R30,LOW(252)
	OUT  0x11,R30
;     463 
;     464 	// Timer/Counter 0 initialization
;     465 	//TCCR0=0x04;  //    CLK / 64
;     466 	//TCNT0=0x00;
;     467 
;     468 	// Timer/Counter 1 initialization
;     469 	TCCR1A=0x00;
	CLR  R30
	OUT  0x2F,R30
;     470 	TCCR1B=0x02; // CLK / 8
	LDI  R30,LOW(2)
	OUT  0x2E,R30
;     471 	TCNT1H=0xff;
	LDI  R30,LOW(255)
	OUT  0x2D,R30
;     472 	TCNT1L=0x00;
	CLR  R30
	OUT  0x2C,R30
;     473 
;     474 	// External Interrupt(s) initialization
;     475 	GIMSK=0x00;
	OUT  0x3B,R30
;     476 	MCUCR=0x00;
	OUT  0x35,R30
;     477 
;     478 	// Timer(s)/Counter(s) Interrupt(s) initialization
;     479 	TIMSK=0x04;
	LDI  R30,LOW(4)
	OUT  0x39,R30
;     480 
;     481 	// UART initialization
;     482 	UCR=0x98;   //0x18;
	LDI  R30,LOW(152)
	OUT  0xA,R30
;     483 	UBRR=0x19;  //0x0c for 4 MHZ 0x19 for 8MHZ
	LDI  R30,LOW(25)
	OUT  0x9,R30
;     484 
;     485 	// Analog Comparator initialization
;     486 	ACSR=0x80;
	LDI  R30,LOW(128)
	OUT  0x8,R30
;     487 
;     488 	// ADC initialization
;     489 	ADMUX=ADC_VREF_TYPE;
	CLR  R30
	OUT  0x7,R30
;     490 	ADCSR=0x81;
	LDI  R30,LOW(129)
	OUT  0x6,R30
;     491 
;     492 	// Prep Commutation array for forward motion
;     493 	for( itmp = 0; itmp <= 7; itmp++ )
	CLR  R30
	STS  _itmp,R30
	STS  _itmp+1,R30
_0x9F:
	LDS  R26,_itmp
	LDS  R27,_itmp+1
	LDI  R30,LOW(7)
	LDI  R31,HIGH(7)
	CP   R30,R26
	CPC  R31,R27
	BRLT _0xA0
;     494 	{
;     495 		ActiveOut[ itmp ] = PhaseOutFWD[ itmp ];
	LDS  R30,_itmp
	LDS  R31,_itmp+1
	SUBI R30,LOW(-_ActiveOut)
	SBCI R31,HIGH(-_ActiveOut)
	PUSH R31
	PUSH R30
	LDS  R30,_itmp
	LDS  R31,_itmp+1
	SUBI R30,LOW(-_PhaseOutFWD)
	SBCI R31,HIGH(-_PhaseOutFWD)
	LD   R30,Z
	POP  R26
	POP  R27
	ST   X,R30
;     496 	}
	RCALL SUBOPT_0x13
	RJMP _0x9F
_0xA0:
;     497                     
;     498 
;     499 	//TCNT0 = TCOUNT_0;
;     500 	TCNT1 = TCOUNT_1;
	RCALL SUBOPT_0xE
;     501 
;     502        
;     503     	PORTC = 0;    
	RCALL SUBOPT_0x12
;     504 	PORTC = PhaseOutFWD[ 0 ];
	RCALL SUBOPT_0x14
;     505 	PORTC = PhaseOutFWD[ 0 ];
	RCALL SUBOPT_0x14
;     506 
;     507 	PORTC = 0x40 | PhaseOutFWD[ 0 ];
	RCALL SUBOPT_0x15
	OUT  0x15,R30
;     508 	PORTC = 0x40 | PhaseOutFWD[ 0 ];
	RCALL SUBOPT_0x15
	RCALL SUBOPT_0xF
;     509 	PORTC = 0x00;                  
;     510      	
;     511      	// Global enable interrupts
;     512 
;     513 	#asm("sei")
	sei
;     514 
;     515 	LEDbyte = 0;
	RCALL SUBOPT_0x16
;     516 	OldLEDbyte = 0;
	RCALL SUBOPT_0x17
;     517 	      
;     518 	
;     519 	ComSequence = 0;
	RCALL SUBOPT_0x18
;     520 	DriveEnable = 1;
	LDI  R30,LOW(1)
	STS  _DriveEnable,R30
;     521 	
;     522 	CurrentLimitByte = 0;
	RCALL SUBOPT_0x19
;     523 	OvrTemp = 0;             
	CLR  R9
;     524 	
;     525 	isecTmr = 0;
	RCALL SUBOPT_0x1A
;     526 	isecTimer = 0;  
	CLR  R30
	STS  _isecTimer,R30
	STS  _isecTimer+1,R30
;     527 	isecTmrWatts = 0;
	RCALL SUBOPT_0x1B
;     528 		
;     529 	AutoUpdate = 1;
	LDI  R30,LOW(1)
	STS  _AutoUpdate,R30
;     530 	iMaxEconoSpeed = 1025; 
	LDI  R30,LOW(1025)
	LDI  R31,HIGH(1025)
	STS  _iMaxEconoSpeed,R30
	STS  _iMaxEconoSpeed+1,R31
;     531      	BatteryPowerLow = 0;
	CLR  R4
;     532 	
;     533 	//PORTD = 0x00;
;     534 
;     535 	MaxWattSeconds = __MAX_WATT_SECONDS; //1400000;
	__GETD1N 0x155CC0
	STS  _MaxWattSeconds,R30
	STS  _MaxWattSeconds+1,R31
	STS  _MaxWattSeconds+2,R22
	STS  _MaxWattSeconds+3,R23
;     536 	
;     537 	sprintf( ComStr, _SOFTWARE_VERSION );
	RCALL SUBOPT_0x1C
	__POINTW1FN _157,0
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	RCALL SUBOPT_0x1D
;     538 	ComSend( ComStr );
	RCALL _ComSend
;     539 	
;     540 	// Turn off all lights...
;     541 	for( itmp = 0; itmp <= 5; itmp++ )
	CLR  R30
	STS  _itmp,R30
	STS  _itmp+1,R30
_0xA2:
	LDS  R26,_itmp
	LDS  R27,_itmp+1
	LDI  R30,LOW(5)
	LDI  R31,HIGH(5)
	CP   R30,R26
	CPC  R31,R27
	BRGE PC+2
	RJMP _0xA3
;     542 	{
;     543 		WriteDataToConsoleLEDS( SOC_LEDS[ itmp ] );
	LDS  R30,_itmp
	LDS  R31,_itmp+1
	RCALL SUBOPT_0x1E
	ST   -Y,R30
	RCALL SUBOPT_0x2
;     544 		itrDelay = 0;
;     545 		while( itrDelay < TIEMR_LED_SCAN ) 
_0xA4:
	LDS  R26,_itrDelay
	LDS  R27,_itrDelay+1
	LDS  R24,_itrDelay+2
	LDS  R25,_itrDelay+3
	__CPD2N 0xBB8
	BRLT _0xA4
;     546 		{
;     547 		} 
;     548 	 	
;     549 		//Read Phase Current
;     550 		AdcTempVal = read_adc( 1 );  
	RCALL SUBOPT_0x1
	RCALL SUBOPT_0x1F
;     551 		iPhaseZero = AdcTempVal;
	__PUTWMRN _iPhaseZero,0,13,14
;     552 		AdcTempVal = read_adc( 1 );  
	RCALL SUBOPT_0x1
	RCALL SUBOPT_0x1F
;     553 	         
;     554 		iPhaseZero = iPhaseZero + AdcTempVal;
	__GETW1R 13,14
	LDS  R26,_iPhaseZero
	LDS  R27,_iPhaseZero+1
	ADD  R30,R26
	ADC  R31,R27
	STS  _iPhaseZero,R30
	STS  _iPhaseZero+1,R31
;     555 		iPhaseZero = iPhaseZero / 2; 
	ASR  R31
	ROR  R30
	STS  _iPhaseZero,R30
	STS  _iPhaseZero+1,R31
;     556 		iPhaseZeroPoints[ itmp ] = iPhaseZero;
	LDS  R30,_itmp
	LDS  R31,_itmp+1
	LDI  R26,LOW(_iPhaseZeroPoints)
	LDI  R27,HIGH(_iPhaseZeroPoints)
	LSL  R30
	ROL  R31
	ADD  R26,R30
	ADC  R27,R31
	LDS  R30,_iPhaseZero
	LDS  R31,_iPhaseZero+1
	RCALL __PUTWP1
;     557 	}
	RCALL SUBOPT_0x13
	RJMP _0xA2
_0xA3:
;     558 	
;     559        
;     560 	// Turn on all lights...
;     561 	WriteDataToConsoleLEDS( 0x00 );
	RCALL SUBOPT_0x4
	RCALL _WriteDataToConsoleLEDS
;     562 
;     563 	//////////////////////////////////////////////////////////////
;     564 	// Startup Flash lights... and Check the Battery Voltage...
;     565 
;     566 	// Read Battery Voltage      
;     567 	BatteryVoltage = ReadBatteryVoltage();	      	
	RCALL _ReadBatteryVoltage
	STS  _BatteryVoltage,R30
	STS  _BatteryVoltage+1,R31
;     568 
;     569 	itrDelay = 0;
	CLR  R30
	STS  _itrDelay,R30
	STS  _itrDelay+1,R30
	STS  _itrDelay+2,R30
	STS  _itrDelay+3,R30
;     570 	while( itrDelay < TIMER_HALF_SECOND ) 
_0xA7:
	LDS  R26,_itrDelay
	LDS  R27,_itrDelay+1
	LDS  R24,_itrDelay+2
	LDS  R25,_itrDelay+3
	__CPD2N 0x3980
	BRLT _0xA7
;     571 	{
;     572 		
;     573 	}       
;     574 	// Read Battery Voltage      
;     575 	BatteryVoltage = BatteryVoltage + ReadBatteryVoltage();
	RCALL _ReadBatteryVoltage
	LDS  R26,_BatteryVoltage
	LDS  R27,_BatteryVoltage+1
	ADD  R30,R26
	ADC  R31,R27
	STS  _BatteryVoltage,R30
	STS  _BatteryVoltage+1,R31
;     576 	BatteryVoltage = BatteryVoltage / 2;
	LSR  R31
	ROR  R30
	STS  _BatteryVoltage,R30
	STS  _BatteryVoltage+1,R31
;     577 	                                
;     578 	
;     579 	//check low voltage on startup...
;     580 	// if voltage is too low, disable the bike...
;     581 	if( BatteryVoltage < __MIN_STARTUP_VOLTAGE )
	LDS  R26,_BatteryVoltage
	LDS  R27,_BatteryVoltage+1
	CPI  R26,LOW(0xCE4)
	LDI  R30,HIGH(0xCE4)
	CPC  R27,R30
	BRSH _0xAA
;     582 	{
;     583 		// if battery is weak, shut it off ...                  
;     584 		sprintf( ComStr, "Low Voltage\r\n" );
	RCALL SUBOPT_0x1C
	__POINTW1FN _157,15
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	RCALL SUBOPT_0x1D
;     585 		ComSend( ComStr );
	RCALL _ComSend
;     586 		
;     587      		ShutdownSystem();
	RCALL _ShutdownSystem
;     588      	}                           
;     589 
;     590 //	sprintf( ComStr, "BV,%d,", BatteryVoltage );
;     591   //	ComSend( ComStr );
;     592  	
;     593  	
;     594 	// do New cruise control init here...
;     595 	CruiseMode = 0;  // deefault is performance mode...
_0xAA:
	CLR  R10
;     596 	if(( PINB & 0x08 ) != 0 ) // j5 is shorted economy mode
	SBIC 0x16,3
;     597 	{
;     598 	 	CruiseMode = 1;
	RCALL SUBOPT_0x20
;     599 	}
;     600 	
;     601 	ReadWattSecondsFromEEPROM();          
	RCALL _ReadWattSecondsFromEEPROM
;     602 	CurrentWattSeconds = SavedWattSeconds;	
	LDS  R30,_SavedWattSeconds
	LDS  R31,_SavedWattSeconds+1
	LDS  R22,_SavedWattSeconds+2
	LDS  R23,_SavedWattSeconds+3
	STS  _CurrentWattSeconds,R30
	STS  _CurrentWattSeconds+1,R31
	STS  _CurrentWattSeconds+2,R22
	STS  _CurrentWattSeconds+3,R23
;     603 	// Reset watt / second counting... 
;     604 	// calculate the state of charge on the battery...
;     605 	if(( PINB & 0x10 ) == 0 ) // j6 is shorted = SLA mode
	SBIC 0x16,4
	RJMP _0xAC
;     606 	{       
;     607         	if(( BatteryVoltage > __NEW_SLA_VOLTAGE )&&( SavedWattSeconds < __USED_WATT_SECONDS ))  // consider this battery new...
	LDS  R26,_BatteryVoltage
	LDS  R27,_BatteryVoltage+1
	LDI  R30,LOW(3820)
	LDI  R31,HIGH(3820)
	CP   R30,R26
	CPC  R31,R27
	BRSH _0xAE
	RCALL SUBOPT_0x21
	BRLT _0xAF
_0xAE:
	RJMP _0xAD
_0xAF:
;     608 		{      
;     609 			//Reset the Watt / Second counter here...
;     610 	 		CurrentWattSeconds = MaxWattSeconds;
	RCALL SUBOPT_0x22
;     611 		}
;     612 	}
_0xAD:
;     613 	else
	RJMP _0xB0
_0xAC:
;     614 	{
;     615         	if(( BatteryVoltage > __NEW_NIMH_VOLTAGE )&&( SavedWattSeconds < __USED_WATT_SECONDS ))  // consider this battery new...
	LDS  R26,_BatteryVoltage
	LDS  R27,_BatteryVoltage+1
	LDI  R30,LOW(3920)
	LDI  R31,HIGH(3920)
	CP   R30,R26
	CPC  R31,R27
	BRSH _0xB2
	RCALL SUBOPT_0x21
	BRLT _0xB3
_0xB2:
	RJMP _0xB1
_0xB3:
;     616 		{
;     617 		 	//Reset the Watt / Second counter here...
;     618 		 	CurrentWattSeconds = MaxWattSeconds;		 	
	RCALL SUBOPT_0x22
;     619 		}	
;     620 	}	
_0xB1:
_0xB0:
;     621 	
;     622 	LowBatteryVoltage = __LOW_BATTERY_VOLTAGE;		
	LDI  R30,LOW(2800)
	LDI  R31,HIGH(2800)
	STS  _LowBatteryVoltage,R30
	STS  _LowBatteryVoltage+1,R31
;     623 
;     624 
;     625 	// SHOW THE CURRENT STATE OF CHARGE ON THE DISPLAY
;     626 	LEDStatus = CalcStateOfChargeForConsole();
	RCALL SUBOPT_0x23
;     627 	
;     628 
;     629 	calcLEDbyte = SOC_LEDS[ LEDStatus ]; 
	STS  _calcLEDbyte,R30
;     630 	LEDbyte = calcLEDbyte;
	RCALL SUBOPT_0x24
;     631 	OldLEDbyte = LEDbyte;
	RCALL SUBOPT_0x25
;     632 		
;     633 	WriteDataToConsoleLEDS( LEDbyte );
	RCALL SUBOPT_0x26
;     634 	
;     635 	// start of main loop
;     636 	while( 1 )
_0xB4:
;     637       	{         
;     638                 // timer routine for "real time" clock, calculated second...
;     639 		isecTmr++;
	LDS  R30,_isecTmr
	SUBI R30,-LOW(1)
	STS  _isecTmr,R30
;     640 		if( isecTmr > _CALC_SECOND )
	LDS  R26,_isecTmr
	RCALL SUBOPT_0x27
	BRSH _0xB7
;     641  		{
;     642 			isecTimer++;
	LDS  R30,_isecTimer
	LDS  R31,_isecTimer+1
	ADIW R30,1
	STS  _isecTimer,R30
	STS  _isecTimer+1,R31
;     643 			isecTmrWatts++;
	LDS  R30,_isecTmrWatts
	SUBI R30,-LOW(1)
	STS  _isecTmrWatts,R30
;     644 				
;     645    			isecTmr = 0;
	RCALL SUBOPT_0x1A
;     646      		}                   		
;     647 		
;     648       		CurrentLimitByte = 0;  // clear out the current limit control
_0xB7:
	RCALL SUBOPT_0x19
;     649       		if(( PINB & 0x08 ) != 0 ) // j5 is shorted economy mode
	SBIS 0x16,3
	RJMP _0xB8
;     650 		{                
;     651 		 	CurrentLimitByte = 0x80;
	LDI  R30,LOW(128)
	STS  _CurrentLimitByte,R30
;     652 		 	PhaseErr = 0;
	CLR  R8
;     653 		}
;     654 
;     655       		// Read The Throttle Demand 
;     656 	      	AdcPWMVal = read_adc( 7 );
_0xB8:
	LDI  R30,LOW(7)
	ST   -Y,R30
	RCALL _read_adc
	__PUTW1R 11,12
;     657       		if( AdcPWMVal > 700 )
	LDI  R30,LOW(700)
	LDI  R31,HIGH(700)
	CP   R30,R11
	CPC  R31,R12
	BRGE _0xB9
;     658 	      	{
;     659       			AdcPWMVal = 0;
	CLR  R11
	CLR  R12
;     660 	      	}                           
;     661 	      	if( AdcPWMVal > 511 )
_0xB9:
	LDI  R30,LOW(511)
	LDI  R31,HIGH(511)
	CP   R30,R11
	CPC  R31,R12
	BRGE _0xBA
;     662 	      	{
;     663 	      	 	AdcPWMVal = 511;
	__PUTW1R 11,12
;     664 	      	}
;     665       		ftmp = AdcPWMVal;
_0xBA:
	__GETW1R 11,12
	RCALL SUBOPT_0x28
;     666 	      	vtmp = (float)(512 - ftmp);
	__GETD1N 0x44000000
	RCALL SUBOPT_0x29
;     667       		vtmp = 1 / vtmp;
	RCALL SUBOPT_0x2A
;     668 	      	ftmp = sqrt( vtmp );
	RCALL __PUTPARD1
	RCALL _sqrt
	RCALL SUBOPT_0x2B
;     669       		ftmp = (ftmp * 360) - 16;
	__GETD1N 0x43B40000
	RCALL SUBOPT_0x2C
	__GETD1N 0x41800000
	RCALL SUBOPT_0x2D
	RCALL SUBOPT_0x2B
;     670 		if( ftmp < 0 )
	RCALL __CPD20
	BRGE _0xBB
;     671 		{
;     672 			ftmp = 0;
	__GETD1N 0x0
	STS  _ftmp,R30
	STS  _ftmp+1,R31
	STS  _ftmp+2,R22
	STS  _ftmp+3,R23
;     673 		}      	
;     674       	
;     675 		itmp = (int)ftmp;     	
_0xBB:
	RCALL SUBOPT_0x2E
	RCALL SUBOPT_0x2F
;     676       	
;     677 	      	//itmp = AdcPWMVal>>2;
;     678       		if( itmp > 63  ) //;;__PWM_RES_TOTAL )
	LDI  R30,LOW(63)
	LDI  R31,HIGH(63)
	CP   R30,R26
	CPC  R31,R27
	BRGE _0xBC
;     679       			itmp = 63; //__PWM_RES_TOTAL;
	STS  _itmp,R30
	STS  _itmp+1,R31
;     680 	        
;     681 	        //	oldPWMVal = tmpPWMVal;	
;     682       		tmpPWMVal = ( unsigned char )itmp & 0x3f;
_0xBC:
	LDS  R30,_itmp
	ANDI R30,LOW(0x3F)
	STS  _tmpPWMVal,R30
;     683       	 
;     684 	 	// do the Speed calculations here.
;     685 		vtmp = iMotorSpeed;
	LDS  R30,_iMotorSpeed
	LDS  R31,_iMotorSpeed+1
	CLR  R22
	CLR  R23
	RCALL __CDF1
	RCALL SUBOPT_0x2A
;     686 		ftmp = TIMER_FREQ / vtmp;
	__GETD2N 0x46E60000
	RCALL __DIVF21
	RCALL SUBOPT_0x2B
;     687 		ftmp = ftmp * 15; // 4 halls / rev 60 revs / sec = RPM
	__GETD1N 0x41700000
	RCALL SUBOPT_0x30
;     688 		iMotorRPM = (int)ftmp;  
	RCALL SUBOPT_0x2E
	STS  _iMotorRPM,R30
	STS  _iMotorRPM+1,R31
;     689 
;     690 		if( iMotorRPM < 30 )
	LDS  R26,_iMotorRPM
	LDS  R27,_iMotorRPM+1
	CPI  R26,LOW(0x1E)
	LDI  R30,HIGH(0x1E)
	CPC  R27,R30
	BRGE _0xBD
;     691 		{
;     692 			iMotorRPM = 0;
	CLR  R30
	STS  _iMotorRPM,R30
	STS  _iMotorRPM+1,R30
;     693 		}  
;     694 	
;     695 	
;     696 		//if(( tmpPWMVal > 6 )&&( iMotorRPM < 75 ))  // 
;     697 		if( iMotorRPM < 75 )
_0xBD:
	LDS  R26,_iMotorRPM
	LDS  R27,_iMotorRPM+1
	CPI  R26,LOW(0x4B)
	LDI  R30,HIGH(0x4B)
	CPC  R27,R30
	BRGE _0xBE
;     698 		{                     
;     699 			CurrentLimitByte = 0xc0; //CurrentLimitByte | 0x40;  // set at lowest limit...
	RCALL SUBOPT_0x31
;     700 		}                         
;     701       	
;     702 	      	// Read Battery Voltage      
;     703  	      	AvgBatteryVoltage = AvgBatteryVoltage + ReadBatteryVoltage();
_0xBE:
	RCALL _ReadBatteryVoltage
	LDS  R26,_AvgBatteryVoltage
	LDS  R27,_AvgBatteryVoltage+1
	ADD  R30,R26
	ADC  R31,R27
	STS  _AvgBatteryVoltage,R30
	STS  _AvgBatteryVoltage+1,R31
;     704 	      	
;     705 	      	if(( ComSequence == 0 )||( ComSequence == 4 ))
	LDS  R26,_ComSequence
	CPI  R26,LOW(0x0)
	BREQ _0xC0
	RCALL SUBOPT_0x32
	BRNE _0xBF
_0xC0:
;     706 	      	{
;     707 	      	 	BatteryVoltage = ( AvgBatteryVoltage / 4 ) + 25;	      	
	LDS  R30,_AvgBatteryVoltage
	LDS  R31,_AvgBatteryVoltage+1
	RCALL __LSRW2
	ADIW R30,25
	STS  _BatteryVoltage,R30
	STS  _BatteryVoltage+1,R31
;     708 	      	 	AvgBatteryVoltage = 0;
	CLR  R30
	STS  _AvgBatteryVoltage,R30
	STS  _AvgBatteryVoltage+1,R30
;     709 		}
;     710 		
;     711 		// if batter is too low start timer to shut it off...
;     712 	      	if( BatteryVoltage < LowBatteryVoltage )
_0xBF:
	LDS  R30,_LowBatteryVoltage
	LDS  R31,_LowBatteryVoltage+1
	RCALL SUBOPT_0x33
	BRSH _0xC2
;     713       		{
;     714       	 		DriveEnable++;
	LDS  R30,_DriveEnable
	SUBI R30,-LOW(1)
	STS  _DriveEnable,R30
;     715 	      	}
;     716       		else
	RJMP _0xC3
_0xC2:
;     717 	      	{      
;     718       			if( DriveEnable < _MAX_LOW_VOLTAGE_TIME )
	RCALL SUBOPT_0x34
	BRSH _0xC4
;     719 			{
;     720 	    	 		DriveEnable = 0;
	CLR  R30
	STS  _DriveEnable,R30
;     721 		 	}
;     722       		}	
_0xC4:
_0xC3:
;     723       	
;     724 	      	//Read Phase Current
;     725       		AdcTempVal = read_adc( 1 );     		
	RCALL SUBOPT_0x1
	RCALL SUBOPT_0x1F
;     726 	      	iPhaseCurrent = AdcTempVal - iPhaseZeroPoints[ LEDStatus ];
	LDS  R30,_LEDStatus
	LDI  R26,LOW(_iPhaseZeroPoints)
	LDI  R27,HIGH(_iPhaseZeroPoints)
	CLR  R31
	LSL  R30
	ROL  R31
	RCALL SUBOPT_0x35
	MOV  R26,R30
	MOV  R27,R31
	__GETW1R 13,14
	SUB  R30,R26
	SBC  R31,R27
	STS  _iPhaseCurrent,R30
	STS  _iPhaseCurrent+1,R31
;     727 		if( iPhaseCurrent < 0 )
	LDS  R26,_iPhaseCurrent
	LDS  R27,_iPhaseCurrent+1
	SBIW R26,0
	BRGE _0xC5
;     728 		{
;     729 			iPhaseCurrent = 0;
	CLR  R30
	STS  _iPhaseCurrent,R30
	STS  _iPhaseCurrent+1,R30
;     730 		}
;     731 		
;     732 	      	ftmp = iPhaseCurrent;
_0xC5:
	LDS  R30,_iPhaseCurrent
	LDS  R31,_iPhaseCurrent+1
	RCALL SUBOPT_0x28
;     733 	      	//PhaseCurrent = iPhaseCurrent;
;     734 	      	
;     735                 if( ftmp > 36 )
	__GETD1N 0x42100000
	RCALL __CMPF12
	BREQ PC+2
	BRCC PC+2
	RJMP _0xC6
;     736                 {
;     737                  	ftmp = ftmp * 0.9333;
	LDS  R26,_ftmp
	LDS  R27,_ftmp+1
	LDS  R24,_ftmp+2
	LDS  R25,_ftmp+3
	__GETD1N 0x3F6EECC0
	RCALL SUBOPT_0x30
;     738                 }
;     739                 BatteryCurrent = ftmp;
_0xC6:
	RCALL SUBOPT_0x36
	STS  _BatteryCurrent,R30
;     740 	      	if( BatteryCurrent > 52 )
	LDS  R26,_BatteryCurrent
	LDI  R30,LOW(52)
	CP   R30,R26
	BRSH _0xC7
;     741 	      	{
;     742 	      		BatteryCurrent = 52;
	STS  _BatteryCurrent,R30
;     743 	      	}	      	
;     744 	      	if( BatteryCurrent <= 1 )
_0xC7:
	LDS  R26,_BatteryCurrent
	LDI  R30,LOW(1)
	CP   R30,R26
	BRLO _0xC8
;     745 	      	{
;     746 	      		BatteryCurrent = 0;
	CLR  R30
	STS  _BatteryCurrent,R30
;     747 	      	}
;     748      
;     749 
;     750 		// Caclulate heat sink temperature 
;     751 		AdcTempVal = read_adc( 2 ); 
_0xC8:
	LDI  R30,LOW(2)
	ST   -Y,R30
	RCALL SUBOPT_0x1F
;     752 		HeatSinkTemp = ConvertTempSensor( AdcTempVal );
	ST   -Y,R14
	ST   -Y,R13
	RCALL _ConvertTempSensor
	STS  _HeatSinkTemp,R30
;     753     
;     754     		if(( HeatSinkTemp > 62 )||( OvrTemp == 1 ))
	LDS  R26,_HeatSinkTemp
	LDI  R30,LOW(62)
	CP   R30,R26
	BRLO _0xCA
	RCALL SUBOPT_0x37
	BRNE _0xC9
_0xCA:
;     755 	    	{        
;     756                         OvrTemp = 1;
	LDI  R30,LOW(1)
	MOV  R9,R30
;     757 			CurrentLimitByte = 0xc0;
	RCALL SUBOPT_0x31
;     758 			if( tmpPWMVal > 27 )
	LDS  R26,_tmpPWMVal
	LDI  R30,LOW(27)
	CP   R30,R26
	BRSH _0xCC
;     759 			{
;     760 				tmpPWMVal = 27;
	STS  _tmpPWMVal,R30
;     761 //				CurrentLimitByte = 0xc0;  // set at lowest limit...
;     762 			}		
;     763     			// too hot... shut it off completely
;     764     			if( HeatSinkTemp > 75 )
_0xCC:
	LDS  R26,_HeatSinkTemp
	LDI  R30,LOW(75)
	CP   R30,R26
	BRSH _0xCD
;     765 	    		{
;     766     				tmpPWMVal = 0;
	RCALL SUBOPT_0x38
;     767     			} 
;     768     		}
_0xCD:
;     769     		if( HeatSinkTemp < 60 )
_0xC9:
	LDS  R26,_HeatSinkTemp
	CPI  R26,LOW(0x3C)
	BRSH _0xCE
;     770     		{
;     771  	 	 	OvrTemp = 0;
	CLR  R9
;     772     		}
;     773 
;     774                 
;     775     		
;     776     		// do Throttle Reprofileing here
;     777 		if(( iMotorRPM > 300 )&&( iMotorRPM < 650 )) //900 ))
_0xCE:
	LDS  R26,_iMotorRPM
	LDS  R27,_iMotorRPM+1
	LDI  R30,LOW(300)
	LDI  R31,HIGH(300)
	CP   R30,R26
	CPC  R31,R27
	BRGE _0xD0
	RCALL SUBOPT_0x39
	BRLT _0xD1
_0xD0:
	RJMP _0xCF
_0xD1:
;     778 		{
;     779 		 	ftmp = iMotorRPM - 300;
	LDS  R30,_iMotorRPM
	LDS  R31,_iMotorRPM+1
	SUBI R30,LOW(300)
	SBCI R31,HIGH(300)
	RCALL SUBOPT_0x28
;     780 		 	ftmp = ftmp / 15; // down to 37
	__GETD1N 0x41700000
	RCALL SUBOPT_0x3A
;     781 		 	tmpChar = 63 - (char)ftmp;
	LDI  R26,LOW(63)
	SUB  R26,R30
	STS  _tmpChar,R26
;     782  			if( tmpChar < tmpPWMVal )
	RCALL SUBOPT_0x3B
	BRSH _0xD2
;     783  			{
;     784  			 	tmpPWMVal = tmpChar;
	RCALL SUBOPT_0x3C
;     785  			}	 	
;     786 		}        
_0xD2:
;     787 		if(( iMotorRPM >= 650 )&&( iMotorRPM < 1700 )) //if(( iMotorRPM >= 900 )&&( iMotorRPM < 1400 ))
_0xCF:
	RCALL SUBOPT_0x39
	BRLT _0xD4
	LDS  R26,_iMotorRPM
	LDS  R27,_iMotorRPM+1
	CPI  R26,LOW(0x6A4)
	LDI  R30,HIGH(0x6A4)
	CPC  R27,R30
	BRLT _0xD5
_0xD4:
	RJMP _0xD3
_0xD5:
;     788 		{
;     789 		 	ftmp = iMotorRPM - 650;  // 900
	LDS  R30,_iMotorRPM
	LDS  R31,_iMotorRPM+1
	SUBI R30,LOW(650)
	SBCI R31,HIGH(650)
	RCALL SUBOPT_0x28
;     790 		 	ftmp = ftmp / 44; //34;
	__GETD1N 0x42300000
	RCALL SUBOPT_0x3A
;     791 		 	tmpChar = 39 + (char)ftmp;
	SUBI R30,-LOW(39)
	STS  _tmpChar,R30
;     792  			if( tmpChar < tmpPWMVal )
	RCALL SUBOPT_0x3B
	BRSH _0xD6
;     793  			{
;     794  			 	tmpPWMVal = tmpChar;
	RCALL SUBOPT_0x3C
;     795  			}	 	
;     796 		}
_0xD6:
;     797 
;     798 	      	// Calculate Battery Temperature
;     799      	        /*
;     800   		AdcTempVal = read_adc( 6 ); 
;     801 		BatteryTemp = ConvertTempSensor( AdcTempVal ); 	
;     802       	        */
;     803       	                     
;     804       		if( AutoUpdate == 1 )
_0xD3:
	LDS  R26,_AutoUpdate
	CPI  R26,LOW(0x1)
	BRNE _0xD7
;     805 			OutputComData();	  
	RCALL _OutputComData
;     806 	  
;     807 		// perform delay for RS-232 output...
;     808 		itrDelay = 0;
_0xD7:
	CLR  R30
	STS  _itrDelay,R30
	STS  _itrDelay+1,R30
	STS  _itrDelay+2,R30
	STS  _itrDelay+3,R30
;     809 		while( itrDelay < COM_SEQ_FREQ ) // )
_0xD8:
	LDS  R26,_itrDelay
	LDS  R27,_itrDelay+1
	LDS  R24,_itrDelay+2
	LDS  R25,_itrDelay+3
	__CPD2N 0x4FB
	BRLT _0xD8
;     810 		{
;     811 		}
;     812 		
;     813 		ComSequence++;
	LDS  R30,_ComSequence
	SUBI R30,-LOW(1)
	STS  _ComSequence,R30
;     814 		if( ComSequence > 7 )
	LDS  R26,_ComSequence
	LDI  R30,LOW(7)
	CP   R30,R26
	BRSH _0xDB
;     815 		{
;     816 			ComSequence = 0;
	RCALL SUBOPT_0x18
;     817 		}	                                                
;     818 
;     819 		CalcCruiseControl();
_0xDB:
	RCALL _CalcCruiseControl
;     820 
;     821 	 	// Check low voltage here... 
;     822  		if( DriveEnable >= _MAX_LOW_VOLTAGE_TIME )
	RCALL SUBOPT_0x34
	BRLO _0xDC
;     823 	 	{
;     824  		   	tmpPWMVal = 0; 
	RCALL SUBOPT_0x38
;     825  		   	DriveEnable = _MAX_LOW_VOLTAGE_TIME;
	LDI  R30,LOW(80)
	STS  _DriveEnable,R30
;     826 			
;     827 			// shut the drive off
;     828 			ShutdownSystem();
	RCALL _ShutdownSystem
;     829 	 	}      
;     830 
;     831 
;     832 		// set the throttle to the new value...		
;     833 		cPWMVal = tmpPWMVal;
_0xDC:
	LDS  R30,_tmpPWMVal
	STS  _cPWMVal,R30
;     834 		
;     835 		
;     836 		// count up the watts used every second and subtract from the total 
;     837 		// to calculate the state of charge...
;     838 		iTimerSOC++;		
	LDS  R30,_iTimerSOC
	SUBI R30,-LOW(1)
	STS  _iTimerSOC,R30
;     839 		if( iTimerSOC > 15 )
	LDS  R26,_iTimerSOC
	RCALL SUBOPT_0x27
	BRLO PC+2
	RJMP _0xDD
;     840 		{         
;     841     			// Calculate the Watts / 0.5 seconds / power consumption...
;     842     			ftmp = BatteryVoltage / 100;
	LDS  R26,_BatteryVoltage
	LDS  R27,_BatteryVoltage+1
	LDI  R30,LOW(100)
	LDI  R31,HIGH(100)
	RCALL __DIVW21U
	CLR  R22
	CLR  R23
	RCALL __CDF1
	STS  _ftmp,R30
	STS  _ftmp+1,R31
	STS  _ftmp+2,R22
	STS  _ftmp+3,R23
;     843     			CurrentWattSeconds = CurrentWattSeconds - ((long)( BatteryCurrent ) * ftmp);
	RCALL SUBOPT_0x3D
	RCALL __MOVED12
	LDS  R30,_ftmp
	LDS  R31,_ftmp+1
	LDS  R22,_ftmp+2
	LDS  R23,_ftmp+3
	RCALL __CDF2
	RCALL __MULF12
	LDS  R26,_CurrentWattSeconds
	LDS  R27,_CurrentWattSeconds+1
	LDS  R24,_CurrentWattSeconds+2
	LDS  R25,_CurrentWattSeconds+3
	RCALL __CDF2
	RCALL SUBOPT_0x2D
	RCALL __CFD1
	STS  _CurrentWattSeconds,R30
	STS  _CurrentWattSeconds+1,R31
	STS  _CurrentWattSeconds+2,R22
	STS  _CurrentWattSeconds+3,R23
;     844                 	if( CurrentWattSeconds < 0 )
	LDS  R26,_CurrentWattSeconds
	LDS  R27,_CurrentWattSeconds+1
	LDS  R24,_CurrentWattSeconds+2
	LDS  R25,_CurrentWattSeconds+3
	RCALL __CPD20
	BRGE _0xDE
;     845 	                {
;     846         	        	CurrentWattSeconds = 0;
	CLR  R30
	STS  _CurrentWattSeconds,R30
	STS  _CurrentWattSeconds+1,R30
	STS  _CurrentWattSeconds+2,R30
	STS  _CurrentWattSeconds+3,R30
;     847                 	} 
;     848     		
;     849 		
;     850 		 	iTimerSOC = 0;
_0xDE:
	RCALL SUBOPT_0x11
;     851 		 	
;     852 			LEDStatus = CalcStateOfChargeForConsole();
	RCALL SUBOPT_0x23
;     853 
;     854 			calcLEDbyte = SOC_LEDS[ LEDStatus ];
	STS  _calcLEDbyte,R30
;     855 			
;     856 		}
;     857 	       	OldLEDbyte = LEDbyte;	
_0xDD:
	RCALL SUBOPT_0x25
;     858                 LEDbyte = calcLEDbyte;
	RCALL SUBOPT_0x24
;     859 
;     860 		// make the lowest voltage blink here...
;     861 	     	if( calcLEDbyte == 0 )
	RCALL SUBOPT_0x3E
	BRNE _0xDF
;     862 	     	{       
;     863 		     	if( ComSequence < 2 )
	RCALL SUBOPT_0x3F
	BRSH _0xE0
;     864 	     		{       
;     865 	     			LEDbyte = 0x01;
	LDI  R30,LOW(1)
	RJMP _0x1C5
;     866 	   		}
;     867 	     		else
_0xE0:
;     868 	     		{
;     869 	     			LEDbyte = 0;
	CLR  R30
_0x1C5:
	STS  _LEDbyte,R30
;     870 	     		}
;     871        		}
;     872        		else if( OvrTemp == 1 )
	RJMP _0xE2
_0xDF:
	RCALL SUBOPT_0x37
	BRNE _0xE3
;     873 	     	{       
;     874 		     	if(( ComSequence & 0x03 ) == 1 )
	LDS  R30,_ComSequence
	ANDI R30,LOW(0x3)
	CPI  R30,LOW(0x1)
	BRNE _0xE4
;     875 	     		{       
;     876 	     			LEDbyte = calcLEDbyte;
	RCALL SUBOPT_0x24
;     877 	     			
;     878 				if( calcLEDbyte == 0 )
	RCALL SUBOPT_0x3E
	BRNE _0xE5
;     879 				{               
;     880 	     				LEDbyte = 0x01;
	LDI  R30,LOW(1)
	STS  _LEDbyte,R30
;     881 	   			}
;     882 	     		}
_0xE5:
;     883 	     		else
	RJMP _0xE6
_0xE4:
;     884 	     		{
;     885 	     			LEDbyte = 0;
	RCALL SUBOPT_0x16
;     886 	     		}
_0xE6:
;     887        		}
;     888        		       		                
;     889 		if( isecTmrWatts > 10 )
_0xE3:
_0xE2:
	LDS  R26,_isecTmrWatts
	LDI  R30,LOW(10)
	CP   R30,R26
	BRSH _0xE7
;     890 		{
;     891 		 	isecTmrWatts = 0;
	RCALL SUBOPT_0x1B
;     892 		 	OldLEDbyte = 0; // update the LEDS every 10 seconds
	RCALL SUBOPT_0x17
;     893 		 	SaveWattSecondsToEPPROM();		 	
	RCALL _SaveWattSecondsToEPPROM
;     894 		}
;     895        		       		
;     896 		// do led routines here...
;     897 	      	if( LEDbyte != OldLEDbyte )
_0xE7:
	LDS  R30,_OldLEDbyte
	LDS  R26,_LEDbyte
	CP   R30,R26
	BREQ _0xE8
;     898 	     	{
;     899      		        // voltage is really low.... blink the red light...
;     900 #ifndef _DEBUG_FREQ_OUTPUT
;     901        			WriteDataToConsoleLEDS( LEDbyte );
	RCALL SUBOPT_0x26
;     902 #endif
;     903 
;     904 	     	}   	
;     905 
;     906 		// Apply Current limit data here...                          
;     907 	     	tmpChar = PORTD;
_0xE8:
	IN   R30,0x12
	RCALL SUBOPT_0x40
;     908 	     	tmpChar = tmpChar & 0x3f;
	ANDI R30,LOW(0x3F)
	STS  _tmpChar,R30
;     909 	     	tmpChar = tmpChar | CurrentLimitByte;
	LDS  R30,_CurrentLimitByte
	LDS  R26,_tmpChar
	OR   R30,R26
	RCALL SUBOPT_0x40
;     910 		PORTD = tmpChar;
	OUT  0x12,R30
;     911 		
;     912 		
;     913 		// auto shut down timer and control
;     914 		if(( cPWMVal > 0 )&&( iMotorRPM > 0 ))
	LDS  R26,_cPWMVal
	CLR  R30
	CP   R30,R26
	BRSH _0xEA
	LDS  R26,_iMotorRPM
	LDS  R27,_iMotorRPM+1
	RCALL __CPW02
	BRLT _0xEB
_0xEA:
	RJMP _0xE9
_0xEB:
;     915 		{
;     916 			isecTimer = 0;
	CLR  R30
	STS  _isecTimer,R30
	STS  _isecTimer+1,R30
;     917 		}
;     918 		
;     919 		if( isecTimer > AUTO_SHUTOFF_TIME )
_0xE9:
	LDS  R26,_isecTimer
	LDS  R27,_isecTimer+1
	LDI  R30,LOW(120)
	LDI  R31,HIGH(120)
	CP   R30,R26
	CPC  R31,R27
	BRGE _0xEC
;     920 		{
;     921   			ShutdownSystem();	
	RCALL _ShutdownSystem
;     922 		}
;     923 			
;     924         	// check com port and service any command there...
;     925    		itmp = RecByteRdy;
_0xEC:
	MOV  R30,R6
	CLR  R31
	STS  _itmp,R30
	STS  _itmp+1,R31
;     926 		RecByteRdy = 0;
	CLR  R6
;     927 		if( itmp > 0 )
	LDS  R26,_itmp
	LDS  R27,_itmp+1
	RCALL __CPW02
	BRGE _0xED
;     928 		{      
;     929 			ProcessCommand( ComRecBuff );
	LDI  R30,LOW(_ComRecBuff)
	LDI  R31,HIGH(_ComRecBuff)
	ST   -Y,R31
	ST   -Y,R30
	RCALL _ProcessCommand
;     930 			SendCRLFPrompt();		
	RCALL _SendCRLFPrompt
;     931 		}
;     932       };
_0xED:
	RJMP _0xB4
;     933 }                               
_0xEE:
	RJMP _0xEE
;     934 
;     935 void SendCRLFPrompt( void )
;     936 {
_SendCRLFPrompt:
;     937  	ComStr[0] = 0x0d;
	LDI  R30,LOW(13)
	STS  _ComStr,R30
;     938 	ComStr[1] = 0x0a;
	LDI  R30,LOW(10)
	__PUTB1MN _ComStr,1
;     939 	ComStr[2] = '>';
	LDI  R30,LOW(62)
	__PUTB1MN _ComStr,2
;     940 	ComStr[3] = 0;
	CLR  R30
	__PUTB1MN _ComStr,3
;     941 				
;     942 	ComSend( ComStr );
	RCALL SUBOPT_0x1C
	RCALL _ComSend
;     943 }  
	RET
;     944 
;     945 void OutputComData( void )
;     946 {
_OutputComData:
;     947 		//Prep Com Data 
;     948 		ComStr[ 0 ] = 0;
	CLR  R30
	STS  _ComStr,R30
;     949 		if( ComSequence == 0 )
	LDS  R30,_ComSequence
	CPI  R30,0
	BRNE _0xF1
;     950 		{
;     951 	      		if(( PINB & 0x08 ) != 0 ) // j5 is shorted economy mode
	SBIS 0x16,3
	RJMP _0xF2
;     952 			{                
;     953 		        	sprintf( ComStr, "Ec-TH,%d,", cPWMVal );
	RCALL SUBOPT_0x1C
	__POINTW1FN _240,0
	RCALL SUBOPT_0x41
	LDI  R24,4
	RJMP _0x1C6
;     954 		 	}
;     955 		 	else
_0xF2:
;     956 		 	{
;     957 		        	sprintf( ComStr, "Pf-TH,%d,", cPWMVal );
	RCALL SUBOPT_0x1C
	__POINTW1FN _240,10
	RCALL SUBOPT_0x41
	LDI  R24,4
_0x1C6:
	RCALL _sprintf
	ADIW R28,8
;     958 		 	}
;     959 		}
;     960 		if( ComSequence == 1 )
_0xF1:
	LDS  R26,_ComSequence
	CPI  R26,LOW(0x1)
	BRNE _0xF4
;     961 		{
;     962 		       	sprintf( ComStr, " RPM,%d,", iMotorRPM );
	RCALL SUBOPT_0x1C
	__POINTW1FN _240,20
	ST   -Y,R31
	ST   -Y,R30
	LDS  R30,_iMotorRPM
	LDS  R31,_iMotorRPM+1
	RCALL SUBOPT_0x42
	LDI  R24,4
	RCALL SUBOPT_0x43
;     963 		}
;     964 		if( ComSequence == 2 )
_0xF4:
	RCALL SUBOPT_0x3F
	BRNE _0xF5
;     965 		{
;     966 			sprintf( ComStr, "BV,%d,", BatteryVoltage );
	RCALL SUBOPT_0x1C
	__POINTW1FN _240,29
	ST   -Y,R31
	ST   -Y,R30
	LDS  R30,_BatteryVoltage
	LDS  R31,_BatteryVoltage+1
	CLR  R22
	CLR  R23
	RCALL __PUTPARD1
	LDI  R24,4
	RCALL SUBOPT_0x43
;     967 		}
;     968 		if( ComSequence == 3 )
_0xF5:
	LDS  R26,_ComSequence
	CPI  R26,LOW(0x3)
	BRNE _0xF6
;     969 		{
;     970 			sprintf( ComStr, " BC,%d,", BatteryCurrent );
	RCALL SUBOPT_0x1C
	__POINTW1FN _240,36
	ST   -Y,R31
	ST   -Y,R30
	RCALL SUBOPT_0x3D
	RCALL __PUTPARD1
	LDI  R24,4
	RCALL SUBOPT_0x43
;     971 		}
;     972 		if( ComSequence == 4 )
_0xF6:
	RCALL SUBOPT_0x32
	BRNE _0xF7
;     973 		{
;     974 			sprintf( ComStr, "CT,%d,", HeatSinkTemp );
	RCALL SUBOPT_0x1C
	__POINTW1FN _240,44
	ST   -Y,R31
	ST   -Y,R30
	LDS  R30,_HeatSinkTemp
	RCALL SUBOPT_0x44
	LDI  R24,4
	RCALL SUBOPT_0x43
;     975 		}
;     976 		if( ComSequence == 5 )
_0xF7:
	LDS  R26,_ComSequence
	CPI  R26,LOW(0x5)
	BRNE _0xF8
;     977 		{
;     978 			sprintf( ComStr, " PC,%d,", iPhaseCurrent );
	RCALL SUBOPT_0x1C
	__POINTW1FN _240,51
	ST   -Y,R31
	ST   -Y,R30
	LDS  R30,_iPhaseCurrent
	LDS  R31,_iPhaseCurrent+1
	RCALL SUBOPT_0x42
	LDI  R24,4
	RCALL SUBOPT_0x43
;     979 		}
;     980 		if( ComSequence == 6 )
_0xF8:
	LDS  R26,_ComSequence
	CPI  R26,LOW(0x6)
	BRNE _0xF9
;     981 		{                         
;     982 		        tmpChar = hSensors;
	LDS  R30,_hSensors
	STS  _tmpChar,R30
;     983 			sprintf( ComStr, " HS,%d", tmpChar );          
	RCALL SUBOPT_0x1C
	__POINTW1FN _240,59
	ST   -Y,R31
	ST   -Y,R30
	LDS  R30,_tmpChar
	RCALL SUBOPT_0x44
	LDI  R24,4
	RCALL SUBOPT_0x43
;     984 			//sprintf( ComStr, " SW:%d,", SavedWattSeconds / 1000 );
;     985 		}  
;     986 		if( ComSequence == 7 )
_0xF9:
	LDS  R26,_ComSequence
	CPI  R26,LOW(0x7)
	BRNE _0xFA
;     987 		{
;     988 			//sprintf( ComStr, " CW:%d\r\n", CurrentWattSeconds / 1000 );
;     989 			sprintf( ComStr, "\r\n" );
	RCALL SUBOPT_0x1C
	__POINTW1FN _240,66
	ST   -Y,R31
	ST   -Y,R30
	LDI  R24,0
	RCALL _sprintf
	ADIW R28,4
;     990 			// isecTimer
;     991 		}  
;     992 
;     993 		// Send Com Data
;     994  		ComSend( ComStr );	
_0xFA:
	RCALL SUBOPT_0x1C
	RCALL _ComSend
;     995 	        
;     996 }
	RET
;     997 
;     998 void CalcCruiseControl( void )
;     999 {
_CalcCruiseControl:
;    1000 	// do cruise control for economy mode here...
;    1001 	if(( PINB & 0x08 ) != 0 ) // j5 is shorted economy mode
	SBIS 0x16,3
	RJMP _0xFC
;    1002 	{                                                  
;    1003 		if( CruiseMode < 1 )
	LDI  R30,LOW(1)
	CP   R10,R30
	BRSH _0xFD
;    1004 		{ 
;    1005 			CruiseMode = 1;
	RCALL SUBOPT_0x20
;    1006 		}
;    1007 		else if( CruiseMode == 2 )
	RJMP _0xFE
_0xFD:
	LDI  R30,LOW(2)
	CP   R30,R10
	BRNE _0xFF
;    1008 		{
;    1009 			// Set new Mode here...    
;    1010 			if( iMotorRPM > 600 )
	LDS  R26,_iMotorRPM
	LDS  R27,_iMotorRPM+1
	LDI  R30,LOW(600)
	LDI  R31,HIGH(600)
	CP   R30,R26
	CPC  R31,R27
	BRGE _0x100
;    1011 			{
;    1012 				iMaxEconoSpeed = iMotorRPM;
	LDS  R30,_iMotorRPM
	LDS  R31,_iMotorRPM+1
	STS  _iMaxEconoSpeed,R30
	STS  _iMaxEconoSpeed+1,R31
;    1013 			}
;    1014 			CruiseMode = 3;		
_0x100:
	LDI  R30,LOW(3)
	MOV  R10,R30
;    1015 		}		       
;    1016 		
;    1017 		// limit speed if in Economy mode...
;    1018 		if( iMotorRPM > iMaxEconoSpeed + 50)
_0xFF:
_0xFE:
	LDS  R30,_iMaxEconoSpeed
	LDS  R31,_iMaxEconoSpeed+1
	ADIW R30,50
	LDS  R26,_iMotorRPM
	LDS  R27,_iMotorRPM+1
	CP   R30,R26
	CPC  R31,R27
	BRGE _0x101
;    1019 		{       
;    1020 			// reduce the speed of the motor...
;    1021 		 	if( OldcPWMVal > 12 )	               
	LDS  R26,_OldcPWMVal
	LDI  R30,LOW(12)
	CP   R30,R26
	BRSH _0x102
;    1022 		 	{
;    1023 		 		OldcPWMVal = OldcPWMVal - 1;
	LDS  R30,_OldcPWMVal
	SUBI R30,LOW(1)
	STS  _OldcPWMVal,R30
;    1024 		  	}
;    1025 		 		 		 	
;    1026 		}
_0x102:
;    1027 		if( iMotorRPM < iMaxEconoSpeed - 50 )
_0x101:
	LDS  R30,_iMaxEconoSpeed
	LDS  R31,_iMaxEconoSpeed+1
	SBIW R30,50
	LDS  R26,_iMotorRPM
	LDS  R27,_iMotorRPM+1
	CP   R26,R30
	CPC  R27,R31
	BRGE _0x103
;    1028 		{                                   
;    1029 			// increase the speed of the motor...
;    1030 			if( OldcPWMVal < tmpPWMVal )
	RCALL SUBOPT_0x45
	CP   R26,R30
	BRSH _0x104
;    1031 			{       
;    1032 				itmp = tmpPWMVal - OldcPWMVal;
	LDS  R26,_OldcPWMVal
	LDS  R30,_tmpPWMVal
	SUB  R30,R26
	CLR  R31
	RCALL SUBOPT_0x2F
;    1033 				if( itmp > 50 )
	LDI  R30,LOW(50)
	LDI  R31,HIGH(50)
	CP   R30,R26
	CPC  R31,R27
	BRGE _0x105
;    1034 				{
;    1035 					OldcPWMVal = tmpPWMVal / 2;
	LDS  R30,_tmpPWMVal
	LSR  R30
	STS  _OldcPWMVal,R30
;    1036 				}
;    1037 				else
	RJMP _0x106
_0x105:
;    1038 				{       if( itmp > 9 )
	LDS  R26,_itmp
	LDS  R27,_itmp+1
	LDI  R30,LOW(9)
	LDI  R31,HIGH(9)
	CP   R30,R26
	CPC  R31,R27
	BRGE _0x107
;    1039 					{
;    1040 					 	 OldcPWMVal = OldcPWMVal + 2;
	LDS  R30,_OldcPWMVal
	SUBI R30,-LOW(2)
	RJMP _0x1C7
;    1041 					}
;    1042 					else
_0x107:
;    1043 				 	 	OldcPWMVal = OldcPWMVal + 1;
	LDS  R30,_OldcPWMVal
	SUBI R30,-LOW(1)
_0x1C7:
	STS  _OldcPWMVal,R30
;    1044 				}
_0x106:
;    1045 				if( OldcPWMVal > tmpPWMVal )
	RCALL SUBOPT_0x45
	CP   R30,R26
	BRSH _0x109
;    1046 				{					
;    1047 					OldcPWMVal = tmpPWMVal;
	RCALL SUBOPT_0x46
;    1048 				}
;    1049 			}
_0x109:
;    1050 		}
_0x104:
;    1051 		else
_0x103:
;    1052 		{
;    1053 			// dont do anything
;    1054 		}   
;    1055 		if( tmpPWMVal < OldcPWMVal )
	LDS  R30,_OldcPWMVal
	LDS  R26,_tmpPWMVal
	CP   R26,R30
	BRSH _0x10B
;    1056 		{
;    1057 			OldcPWMVal = tmpPWMVal;
	RCALL SUBOPT_0x46
;    1058 		}			
;    1059 		tmpPWMVal = OldcPWMVal;
_0x10B:
	LDS  R30,_OldcPWMVal
	STS  _tmpPWMVal,R30
;    1060 	}
;    1061 	else
	RJMP _0x10C
_0xFC:
;    1062 	{
;    1063 		// permormance mode
;    1064 		if(( CruiseMode == 1 )||( CruiseMode == 3 ))
	LDI  R30,LOW(1)
	CP   R30,R10
	BREQ _0x10E
	LDI  R30,LOW(3)
	CP   R30,R10
	BRNE _0x10D
_0x10E:
;    1065 		{ 
;    1066 			CruiseMode = 2; 
	LDI  R30,LOW(2)
	MOV  R10,R30
;    1067 		}
;    1068 	
;    1069 	}
_0x10D:
_0x10C:
;    1070 }
	RET
;    1071                
;    1072 
;    1073 
;    1074 unsigned char CalcStateOfChargeForConsole( void )
;    1075 {                               
_CalcStateOfChargeForConsole:
;    1076         long longtmp;
;    1077 	int ptr = 0; // point to first voltage chart on DischargePoints[][]                    
;    1078 	ftmp = MaxWattSeconds;
	RCALL SUBOPT_0x47
;	longtmp -> Y+2
;	ptr -> R16,R17
	LDI  R16,0
	LDI  R17,0
	LDS  R30,_MaxWattSeconds
	LDS  R31,_MaxWattSeconds+1
	LDS  R22,_MaxWattSeconds+2
	LDS  R23,_MaxWattSeconds+3
	RCALL __CDF1
	STS  _ftmp,R30
	STS  _ftmp+1,R31
	STS  _ftmp+2,R22
	STS  _ftmp+3,R23
;    1079 	
;    1080  	if( BatteryCurrent > 8 ){ ptr = 1; }
	LDS  R26,_BatteryCurrent
	LDI  R30,LOW(8)
	CP   R30,R26
	BRSH _0x111
	__GETWRN 16,17,1
;    1081  	if( BatteryCurrent > 16 ){ ptr = 2; }
_0x111:
	LDS  R26,_BatteryCurrent
	LDI  R30,LOW(16)
	CP   R30,R26
	BRSH _0x112
	__GETWRN 16,17,2
;    1082  	if( BatteryCurrent > 25 ){ ptr = 3; }
_0x112:
	LDS  R26,_BatteryCurrent
	LDI  R30,LOW(25)
	CP   R30,R26
	BRSH _0x113
	__GETWRN 16,17,3
;    1083  	if( BatteryCurrent > 35 ){ ptr = 4; }
_0x113:
	LDS  R26,_BatteryCurrent
	LDI  R30,LOW(35)
	CP   R30,R26
	BRSH _0x114
	__GETWRN 16,17,4
;    1084  	
;    1085  	// if battery shows as really good then dont do anything...                                  
;    1086  	//if( BatteryVoltage > DischargePoints[ ptr ][ 0 ] ) { return( 5 ); } 
;    1087  	if( BatteryVoltage > DischargePoints[ ptr ][ 0 ] )
_0x114:
	RCALL SUBOPT_0x48
	PUSH R27
	PUSH R26
	RCALL SUBOPT_0x49
	POP  R26
	POP  R27
	RCALL SUBOPT_0x35
	LDS  R26,_BatteryVoltage
	LDS  R27,_BatteryVoltage+1
	CP   R30,R26
	CPC  R31,R27
	BRSH _0x115
;    1088  	{
;    1089  		if( BatteryPowerLow > 0 ){ BatteryPowerLow = 0; }	
	CLR  R30
	CP   R30,R4
	BRSH _0x116
	CLR  R4
;    1090  	} 
_0x116:
;    1091  	
;    1092  	if( BatteryVoltage < DischargePoints[ ptr ][ 3 ] )
_0x115:
	RCALL SUBOPT_0x48
	PUSH R27
	PUSH R26
	RCALL SUBOPT_0x49
	POP  R26
	POP  R27
	ADD  R30,R26
	ADC  R31,R27
	ADIW R30,6
	RCALL SUBOPT_0x4A
	BRSH _0x117
;    1093  	{       
;    1094  		vtmp = ftmp * 0.31;
	LDS  R26,_ftmp
	LDS  R27,_ftmp+1
	LDS  R24,_ftmp+2
	LDS  R25,_ftmp+3
	__GETD1N 0x3E9EB852
	RCALL SUBOPT_0x4B
;    1095 		longtmp = (long)vtmp;
	RCALL SUBOPT_0x4C
;    1096  	 	if( CurrentWattSeconds > longtmp )
	BRGE _0x118
;    1097  	 	{
;    1098  	 		BatteryPowerLow++;
	INC  R4
;    1099  	 		if( BatteryPowerLow > 30 )
	LDI  R30,LOW(30)
	CP   R30,R4
	BRSH _0x119
;    1100  	 		{
;    1101  	 			CurrentWattSeconds = CurrentWattSeconds - 100000;
	RCALL SUBOPT_0x4D
;    1102  	 			BatteryPowerLow = 0;
;    1103  	 		} 	
;    1104  	 	}
_0x119:
;    1105  	} 
_0x118:
;    1106  	if( BatteryVoltage < DischargePoints[ ptr ][ 4 ] )
_0x117:
	RCALL SUBOPT_0x48
	PUSH R27
	PUSH R26
	RCALL SUBOPT_0x49
	POP  R26
	POP  R27
	ADD  R30,R26
	ADC  R31,R27
	ADIW R30,8
	RCALL SUBOPT_0x4A
	BRSH _0x11A
;    1107 	{
;    1108 		vtmp = ftmp * 0.18;
	LDS  R26,_ftmp
	LDS  R27,_ftmp+1
	LDS  R24,_ftmp+2
	LDS  R25,_ftmp+3
	__GETD1N 0x3E3851EC
	RCALL SUBOPT_0x4B
;    1109 		longtmp = (long)vtmp;
	RCALL SUBOPT_0x4C
;    1110  	 	if( CurrentWattSeconds > longtmp )
	BRGE _0x11B
;    1111  	 	{
;    1112  	 		BatteryPowerLow++;
	INC  R4
;    1113  	 		if( BatteryPowerLow > 20 )
	LDI  R30,LOW(20)
	CP   R30,R4
	BRSH _0x11C
;    1114  	 		{
;    1115  	 			CurrentWattSeconds = CurrentWattSeconds - 100000;
	RCALL SUBOPT_0x4D
;    1116  	 			BatteryPowerLow = 0;
;    1117  	 		} 	
;    1118  	 	}
_0x11C:
;    1119   	}
_0x11B:
;    1120 	
;    1121 	vtmp = ftmp * 0.81;
_0x11A:
	LDS  R26,_ftmp
	LDS  R27,_ftmp+1
	LDS  R24,_ftmp+2
	LDS  R25,_ftmp+3
	__GETD1N 0x3F4F5C29
	RCALL SUBOPT_0x4B
;    1122 	longtmp = (long)vtmp;
	RCALL SUBOPT_0x4C
;    1123 	if( CurrentWattSeconds > longtmp ){ return(5); }
	BRGE _0x11D
	LDI  R30,LOW(5)
	RJMP _0x1C2
;    1124 
;    1125 	vtmp = ftmp * 0.61;
_0x11D:
	LDS  R26,_ftmp
	LDS  R27,_ftmp+1
	LDS  R24,_ftmp+2
	LDS  R25,_ftmp+3
	__GETD1N 0x3F1C28F6
	RCALL SUBOPT_0x4B
;    1126 	longtmp = (long)vtmp;
	RCALL SUBOPT_0x4C
;    1127 	if( CurrentWattSeconds > longtmp ){ return(4); }
	BRGE _0x11E
	LDI  R30,LOW(4)
	RJMP _0x1C2
;    1128 
;    1129 	vtmp = ftmp * 0.41;
_0x11E:
	LDS  R26,_ftmp
	LDS  R27,_ftmp+1
	LDS  R24,_ftmp+2
	LDS  R25,_ftmp+3
	__GETD1N 0x3ED1EB85
	RCALL SUBOPT_0x4B
;    1130 	longtmp = (long)vtmp;
	RCALL SUBOPT_0x4C
;    1131 	if( CurrentWattSeconds > longtmp ){ return(3); }
	BRGE _0x11F
	LDI  R30,LOW(3)
	RJMP _0x1C2
;    1132 	
;    1133 	vtmp = ftmp * 0.21;
_0x11F:
	LDS  R26,_ftmp
	LDS  R27,_ftmp+1
	LDS  R24,_ftmp+2
	LDS  R25,_ftmp+3
	__GETD1N 0x3E570A3D
	RCALL SUBOPT_0x4B
;    1134 	longtmp = (long)vtmp;
	RCALL SUBOPT_0x4C
;    1135 	if( CurrentWattSeconds > longtmp ){ return(2); }
	BRGE _0x120
	LDI  R30,LOW(2)
	RJMP _0x1C2
;    1136 
;    1137 	vtmp = ftmp * 0.051;
_0x120:
	LDS  R26,_ftmp
	LDS  R27,_ftmp+1
	LDS  R24,_ftmp+2
	LDS  R25,_ftmp+3
	__GETD1N 0x3D50E560
	RCALL SUBOPT_0x4B
;    1138 	longtmp = (long)vtmp;
	RCALL SUBOPT_0x4C
;    1139 	if( CurrentWattSeconds > longtmp ){ return(1); }
	BRGE _0x121
	LDI  R30,LOW(1)
	RJMP _0x1C2
;    1140 
;    1141 	return( 0 );	
_0x121:
	CLR  R30
_0x1C2:
	RCALL __LOADLOCR2
	ADIW R28,6
	RET
;    1142 }
;    1143 
;    1144 unsigned char ConvertTempSensor( int ADCData )
;    1145 {      
_ConvertTempSensor:
;    1146  	unsigned char CelsTemp;
;    1147 	
;    1148 	if( ADCData < 10 ) { ADCData = 10; } 
	ST   -Y,R16
;	ADCData -> Y+1
;	CelsTemp -> R16
	LDD  R26,Y+1
	LDD  R27,Y+1+1
	CPI  R26,LOW(0xA)
	LDI  R30,HIGH(0xA)
	CPC  R27,R30
	BRGE _0x123
	LDI  R30,LOW(10)
	LDI  R31,HIGH(10)
	STD  Y+1,R30
	STD  Y+1+1,R31
;    1149 	vtmp = ADCData;
_0x123:
	LDD  R30,Y+1
	LDD  R31,Y+1+1
	RCALL __CWD1
	RCALL __CDF1
	STS  _vtmp,R30
	STS  _vtmp+1,R31
	STS  _vtmp+2,R22
	STS  _vtmp+3,R23
;    1150 	vtmp = vtmp / 204.6;
	LDS  R26,_vtmp
	LDS  R27,_vtmp+1
	LDS  R24,_vtmp+2
	LDS  R25,_vtmp+3
	__GETD1N 0x434C999A
	RCALL SUBOPT_0x4E
;    1151 	//Vin = ADCVal / 204.6
;    1152                                   
;    1153 	ftmp = 10000 * vtmp;
	__GETD2N 0x461C4000
	RCALL SUBOPT_0x30
;    1154 	ftmp = ftmp / ( 5 - vtmp );
	LDS  R26,_vtmp
	LDS  R27,_vtmp+1
	LDS  R24,_vtmp+2
	LDS  R25,_vtmp+3
	__GETD1N 0x40A00000
	RCALL __SUBF12
	LDS  R26,_ftmp
	LDS  R27,_ftmp+1
	LDS  R24,_ftmp+2
	LDS  R25,_ftmp+3
	RCALL __DIVF21
	STS  _ftmp,R30
	STS  _ftmp+1,R31
	STS  _ftmp+2,R22
	STS  _ftmp+3,R23
;    1155 	// ftmp is now the resistor value...	
;    1156 	vtmp = 10000 / ftmp;
	__GETD2N 0x461C4000
	RCALL SUBOPT_0x4E
;    1157 	ftmp = log( vtmp ) * 0.0002439;
	RCALL __PUTPARD1
	RCALL _log
	__GETD2N 0x397FBF68
	RCALL __MULF12
	RCALL SUBOPT_0x2B
;    1158 	vtmp = 0.003354 - ftmp;
	__GETD1N 0x3B5BCEC8
	RCALL SUBOPT_0x29
;    1159 	ftmp = 1 / vtmp;
	RCALL SUBOPT_0x2B
;    1160 	ftmp = ftmp - 273.15;	
	__GETD1N 0x43889333
	RCALL SUBOPT_0x2D
	STS  _ftmp,R30
	STS  _ftmp+1,R31
	STS  _ftmp+2,R22
	STS  _ftmp+3,R23
;    1161 	CelsTemp = (char)ftmp;
	RCALL SUBOPT_0x36
	MOV  R16,R30
;    1162 	return( CelsTemp );
	MOV  R30,R16
	LDD  R16,Y+0
	ADIW R28,3
	RET
;    1163 }
;    1164 
;    1165 
;    1166 
;    1167 void WriteDataToConsoleLEDS( unsigned char cdata )
;    1168 {
_WriteDataToConsoleLEDS:
;    1169 	unsigned char pdata, indx;
;    1170 	cdata = cdata & 0x1f;
	RCALL __SAVELOCR2
;	cdata -> Y+2
;	pdata -> R16
;	indx -> R17
	LDD  R30,Y+2
	ANDI R30,LOW(0x1F)
	STD  Y+2,R30
;    1171 	cdata = cdata << 3;
	RCALL __LSLB3
	STD  Y+2,R30
;    1172 	
;    1173 	for( indx = 0; indx <= 4; indx++ )
	LDI  R17,LOW(0)
_0x126:
	LDI  R30,LOW(4)
	CP   R30,R17
	BRLO _0x127
;    1174 	{       
;    1175 		PORTD = PORTD & 0xe0;            
	IN   R30,0x12
	ANDI R30,LOW(0xE0)
	OUT  0x12,R30
;    1176 		// test the MSB for a non zero 
;    1177 		if(( cdata & 0x80 ) != 0 )
	LDD  R30,Y+2
	ANDI R30,LOW(0x80)
	BREQ _0x128
;    1178 		{
;    1179 			pdata = 0x04; //_LED_DATA_MSK; // | _LED_CLK_MSK | _LED_SEL_MSK;                
	LDI  R16,LOW(4)
;    1180 	 	}
;    1181 	 	else
	RJMP _0x129
_0x128:
;    1182 	 	{
;    1183 			pdata = 0x00; //_LED_CLK_MSK | _LED_SEL_MSK;                	 	
	LDI  R16,LOW(0)
;    1184 	 	}                                                           
_0x129:
;    1185 	 	PORTD = PORTD | pdata; // + _LED_CLK_MSK + _LED_SEL_MSK;
	IN   R30,0x12
	OR   R30,R16
	OUT  0x12,R30
;    1186 	 	                             
;    1187 	 	// now clock the data...
;    1188 	 	
;    1189 	 	PORTD = PORTD | _LED_CLK_MSK;
	SBI  0x12,3
;    1190 	 	
;    1191 	 	cdata = cdata<<1;	
	LDD  R30,Y+2
	LSL  R30
	STD  Y+2,R30
;    1192 	}	
	SUBI R17,-1
	RJMP _0x126
_0x127:
;    1193 }
	RCALL __LOADLOCR2
	ADIW R28,3
	RET
;    1194 
;    1195 void ProcessCommand( char *pRecStr )
;    1196 {
_ProcessCommand:
;    1197 }
	ADIW R28,2
	RET
;    1198 /*
;    1199 void ProcessCommand( char *pRecStr )
;    1200 {
;    1201     char ptr, uch, val; 
;    1202     //unsigned char *tptr; 
;    1203     long longtmp = 0;
;    1204     int multx;
;    1205   
;    1206 	// filter out the Set and Get commands...
;    1207     longtmp = 0;
;    1208   
;    1209  	uch = pRecStr[3] - 0x30;
;    1210 	// use 4 for low  voltage setting...
;    1211 	if( uch < 0 ){ uch = 0; }
;    1212 	if( uch > 3 ){ uch = 3; }
;    1213 
;    1214  
;    1215 	if( pRecStr[0] == 'S' )
;    1216 	{       
;    1217 		multx = 1;
;    1218 		for( ptr = (char)strlen( pRecStr ); ptr >= 3; ptr-- )
;    1219 		{
;    1220 			if(( pRecStr[ptr] >= 0x30 )&&( pRecStr[ptr] <= 0x39 ))
;    1221 			{
;    1222 				val = pRecStr[ptr] - 0x30;
;    1223 			 	longtmp = longtmp + ((int)val * (int)multx );
;    1224 			 	multx = multx * 10;			 					 				 		
;    1225 			}
;    1226 		}		
;    1227     	
;    1228 		if( pRecStr[1] == '0' )  // parameter 0 is the max economy speed
;    1229 	 	{    
;    1230                 	AutoUpdate = (char)longtmp;
;    1231 	 	} 
;    1232 
;    1233 		if( pRecStr[1] == '1' )  // parameter 0 is the max economy speed
;    1234 	 	{    
;    1235                 	iMaxEconoSpeed = (int)longtmp;
;    1236 	 	} 
;    1237     	
;    1238 		if( pRecStr[1] == '2' )
;    1239 	 	{    
;    1240                 	MaxWattSeconds = longtmp * 1000;  //MaxWattSeconds
;    1241 	 	} 
;    1242     	
;    1243 		if( pRecStr[1] == '3' )
;    1244 	 	{    
;    1245                 	CurrentWattSeconds = longtmp * 1000;
;    1246 	 	} 
;    1247     	
;    1248 		if( pRecStr[1] == '4' )
;    1249 	 	{    
;    1250                 	//SavedWattSeconds = (int)longtmp ;
;    1251 	 	} 
;    1252 		return;	   
;    1253 	}                              
;    1254 	if( pRecStr[0] == 'G' )
;    1255 	{
;    1256 	
;    1257 		if( pRecStr[1] == '0' )  // parameter 0 is the max economy speed
;    1258 	 	{    
;    1259                 	ComStr[ 0 ] = 0;
;    1260 	               	sprintf( ComStr, "G0:%d\r\n", AutoUpdate );
;    1261 	               	ComSend( ComStr );
;    1262 	 	} 
;    1263     	
;    1264 		if( pRecStr[1] == '1' )
;    1265 	 	{    
;    1266                 	ComStr[ 0 ] = 0;
;    1267 	               	sprintf( ComStr, "G1:%d\r\n", iMaxEconoSpeed );
;    1268 	               	ComSend( ComStr );
;    1269 	 	} 
;    1270     	
;    1271 		if( pRecStr[1] == '2' )
;    1272 	 	{       
;    1273 	 		longtmp = MaxWattSeconds / 1000;
;    1274 	 	
;    1275                 	ComStr[ 0 ] = 0;
;    1276 	               	sprintf( ComStr, "G2:%d\r\n", (int)longtmp );
;    1277 	               	ComSend( ComStr );
;    1278 	  	}	          
;    1279     	
;    1280 		if( pRecStr[1] == '3' )
;    1281 	 	{
;    1282 	 		longtmp = CurrentWattSeconds / 1000;
;    1283 	 	
;    1284                 	ComStr[ 0 ] = 0;
;    1285 	               	sprintf( ComStr, "G3:%d\r\n", (int)longtmp );
;    1286 	               	ComSend( ComStr );
;    1287 		}
;    1288 		if( pRecStr[1] == '4' )
;    1289 	 	{            
;    1290 	       		longtmp = SavedWattSeconds / 1000;	
;    1291                  	ComStr[ 0 ] = 0;
;    1292 	               	sprintf( ComStr, "G4:%d\r\n", longtmp );
;    1293 	               	ComSend( ComStr );
;    1294 	 	} 
;    1295  		if( pRecStr[1] == '5' )
;    1296 	 	{    
;    1297                  	ComStr[ 0 ] = 0;
;    1298 	               	sprintf( ComStr, "G5:%d\r\n", iPhaseZeroPoints[ LEDStatus ] );
;    1299 	               	ComSend( ComStr );
;    1300 	 	} 	 	
;    1301 	 	
;    1302 	 	
;    1303 	}          
;    1304 	
;    1305 	ComStr[0] = 0x0d;
;    1306 	ComStr[1] = 0;
;    1307  	ComSend( ComStr );			
;    1308   	                    
;    1309 }
;    1310 */
;    1311 /*
;    1312 void SendDataToCom( char eepindx, long data, char prec )
;    1313 {
;    1314 	char stimer;
;    1315 	char *chptr;
;    1316 	//stimer = tcounter;
;    1317 	         
;    1318 	eepptr = &szEppString[ eepindx ][0];
;    1319 
;    1320  	chptr = ComStr;
;    1321   	while(*eepptr != 0)
;    1322    	{             
;    1323    		*chptr = *eepptr;
;    1324    		chptr++;
;    1325    		eepptr++;
;    1326    	}
;    1327    *chptr = *eepptr;
;    1328    ComSend( ComStr );	  
;    1329 	
;    1330 	// provide a delay here...				 		
;    1331  //	while( stimer == tcounter )
;    1332  //	{
;    1333  //	}
;    1334  //	stimer = tcounter;
;    1335 	delay_it( 10000 );
;    1336 	
;    1337 	FormatString( ComStr, prec, data );
;    1338 	ComSend( ComStr ); 	
;    1339 //	while( stimer == tcounter )
;    1340 //	{
;    1341 //	}
;    1342 delay_it( 1000 );
;    1343 } 		 		
;    1344 
;    1345 char FormatString( char *szOutput, char Precision, long data )
;    1346 {
;    1347  	char indx = 0;
;    1348  	char cptr = 9;
;    1349  	char szNewStr[12];
;    1350 	long ltmp = 0;
;    1351  	
;    1352  	// value is negative...
;    1353  	if( data < 0 )
;    1354  	{
;    1355  	 	szOutput[indx] = '-';
;    1356  	 	indx++;
;    1357  	 	data = 0 - data;
;    1358  	}  	
;    1359 	while(( data != 0 )||(( 9 - cptr ) < Precision + 2 ))
;    1360  	{              
;    1361 		ltmp = data / 10;
;    1362 		szNewStr[cptr] = (char)( data - (ltmp * 10)) + 0x30;
;    1363 		cptr--;
;    1364 		if( (9 - cptr ) == Precision )
;    1365 		{
;    1366 			szNewStr[cptr] = '.';
;    1367 			cptr--;
;    1368 		}
;    1369 		data = ltmp;
;    1370  	}
;    1371 	cptr++;
;    1372  	while( cptr <= 9 )
;    1373  	{ 
;    1374 		szOutput[ indx ] = szNewStr[cptr];
;    1375 		cptr++;
;    1376 		indx++;
;    1377 	}  
;    1378 	szOutput[ indx ] = 0;
;    1379  	return( indx );	
;    1380 }
;    1381 */
;    1382 
;    1383 
;    1384 // Timer 0 overflow interrupt service routine 
;    1385 /*
;    1386 interrupt [TIM0_OVF] void timer0_ovf_isr(void)
;    1387 {                      
;    1388 	// 	Place your code here 
;    1389 	//  TCOUNT_0;
;    1390 	//	OCR1
;    1391 }
;    1392 */
;    1393 /*
;    1394  	if( BatteryCurrent > 8 ){ ptr = 1; }
;    1395  	if( BatteryCurrent > 16 ){ ptr = 2; }
;    1396  	if( BatteryCurrent > 25 ){ ptr = 3; }
;    1397  	if( BatteryCurrent > 35 ){ ptr = 4; }
;    1398  	                                  
;    1399  	if( BatteryVoltage > DischargePoints[ ptr ][ 0 ] ) { return( 5 ); } //0x1f ); }
;    1400  	if( BatteryVoltage > DischargePoints[ ptr ][ 1 ] ) { return( 4 ); } //0x0f ); }
;    1401  	if( BatteryVoltage > DischargePoints[ ptr ][ 2 ] ) { return( 3 ); } //0x07 ); }
;    1402  	if( BatteryVoltage > DischargePoints[ ptr ][ 3 ] ) { return( 2 ); } //0x03 ); }
;    1403  	if( BatteryVoltage > DischargePoints[ ptr ][ 0 ] ) { return( 1 ); } //0x01 ); } 	
;    1404 
;    1405 */
_getchar:
     sbis usr,rxc
     rjmp _getchar
     in   r30,udr
	RET
_putchar:
     sbis usr,udre
     rjmp _putchar
     ld   r30,y
     out  udr,r30
	ADIW R28,1
	RET

	.DSEG
_pp_G2:
	.BYTE 0x2

	.CSEG
__put_G2:
	put:
	LDS  R30,_pp_G2
	LDS  R31,_pp_G2+1
	SBIW R30,0
	BREQ _0x161
	LDS  R26,_pp_G2
	LDS  R27,_pp_G2+1
	ADIW R26,1
	STS  _pp_G2,R26
	STS  _pp_G2+1,R27
	SBIW R26,1
	LD   R30,Y
	ST   X,R30
	RJMP _0x162
_0x161:
	LD   R30,Y
	ST   -Y,R30
	RCALL _putchar
_0x162:
	ADIW R28,1
	RET
__print_G2:
	SBIW R28,8
	RCALL __SAVELOCR6
	LDI  R16,0
_0x164:
	LDD  R30,Y+16
	LDD  R31,Y+16+1
	ADIW R30,1
	STD  Y+16,R30
	STD  Y+16+1,R31
	SBIW R30,1
	LPM
	MOV  R30,R0
	MOV  R19,R30
	CPI  R30,0
	BRNE PC+2
	RJMP _0x166
	MOV  R30,R16
	CPI  R30,0
	BRNE _0x16A
	CPI  R19,37
	BRNE _0x16B
	LDI  R16,LOW(1)
	RJMP _0x16C
_0x16B:
	RCALL SUBOPT_0x4F
_0x16C:
	RJMP _0x169
_0x16A:
	CPI  R30,LOW(0x1)
	BRNE _0x16D
	CPI  R19,37
	BRNE _0x16E
	RCALL SUBOPT_0x4F
	LDI  R16,LOW(0)
	RJMP _0x169
_0x16E:
	LDI  R16,LOW(2)
	LDI  R21,LOW(0)
	LDI  R17,LOW(0)
	CPI  R19,45
	BRNE _0x16F
	LDI  R17,LOW(1)
	RJMP _0x169
_0x16F:
	CPI  R19,43
	BRNE _0x170
	LDI  R21,LOW(43)
	RJMP _0x169
_0x170:
	CPI  R19,32
	BRNE _0x171
	LDI  R21,LOW(32)
	RJMP _0x169
_0x171:
	RJMP _0x172
_0x16D:
	CPI  R30,LOW(0x2)
	BRNE _0x173
_0x172:
	LDI  R20,LOW(0)
	LDI  R16,LOW(3)
	CPI  R19,48
	BRNE _0x174
	ORI  R17,LOW(128)
	RJMP _0x169
_0x174:
	RJMP _0x175
_0x173:
	CPI  R30,LOW(0x3)
	BREQ PC+2
	RJMP _0x169
_0x175:
	CPI  R19,48
	BRLO _0x178
	CPI  R19,58
	BRLO _0x179
_0x178:
	RJMP _0x177
_0x179:
	MOV  R18,R20
	LSL  R20
	LSL  R20
	ADD  R20,R18
	LSL  R20
	MOV  R30,R19
	SUBI R30,LOW(48)
	ADD  R20,R30
	RJMP _0x169
_0x177:
	MOV  R30,R19
	CPI  R30,LOW(0x63)
	BRNE _0x17D
	RCALL SUBOPT_0x50
	LD   R30,X
	RCALL SUBOPT_0x51
	RJMP _0x17E
_0x17D:
	CPI  R30,LOW(0x73)
	BRNE _0x180
	RCALL SUBOPT_0x50
	RCALL __GETD1P
	STD  Y+12,R30
	STD  Y+12+1,R31
	ST   -Y,R31
	ST   -Y,R30
	RCALL _strlen
	MOV  R16,R30
	RJMP _0x181
_0x180:
	CPI  R30,LOW(0x70)
	BRNE _0x183
	RCALL SUBOPT_0x50
	RCALL __GETD1P
	STD  Y+6,R30
	STD  Y+6+1,R31
	ST   -Y,R31
	ST   -Y,R30
	RCALL _strlenf
	MOV  R16,R30
	ORI  R17,LOW(8)
_0x181:
	ORI  R17,LOW(2)
	ANDI R17,LOW(127)
	LDI  R18,LOW(0)
	RJMP _0x184
_0x183:
	CPI  R30,LOW(0x64)
	BREQ _0x187
	CPI  R30,LOW(0x69)
	BRNE _0x188
_0x187:
	ORI  R17,LOW(4)
	RJMP _0x189
_0x188:
	CPI  R30,LOW(0x75)
	BRNE _0x18A
_0x189:
	LDI  R30,LOW(_tbl10_G2*2)
	LDI  R31,HIGH(_tbl10_G2*2)
	STD  Y+6,R30
	STD  Y+6+1,R31
	LDI  R16,LOW(5)
	RJMP _0x18B
_0x18A:
	CPI  R30,LOW(0x58)
	BRNE _0x18D
	ORI  R17,LOW(8)
	RJMP _0x18E
_0x18D:
	CPI  R30,LOW(0x78)
	BREQ PC+2
	RJMP _0x1BC
_0x18E:
	LDI  R30,LOW(_tbl16_G2*2)
	LDI  R31,HIGH(_tbl16_G2*2)
	STD  Y+6,R30
	STD  Y+6+1,R31
	LDI  R16,LOW(4)
_0x18B:
	SBRS R17,2
	RJMP _0x190
	RCALL SUBOPT_0x50
	RCALL SUBOPT_0x52
	LDD  R26,Y+10
	LDD  R27,Y+10+1
	SBIW R26,0
	BRGE _0x191
	LDD  R30,Y+10
	LDD  R31,Y+10+1
	RCALL __ANEGW1
	STD  Y+10,R30
	STD  Y+10+1,R31
	LDI  R21,LOW(45)
_0x191:
	CPI  R21,0
	BREQ _0x192
	SUBI R16,-LOW(1)
	RJMP _0x193
_0x192:
	ANDI R17,LOW(251)
_0x193:
	RJMP _0x194
_0x190:
	RCALL SUBOPT_0x50
	RCALL SUBOPT_0x52
_0x194:
_0x184:
	SBRC R17,0
	RJMP _0x195
_0x196:
	CP   R16,R20
	BRSH _0x198
	SBRS R17,7
	RJMP _0x199
	SBRS R17,2
	RJMP _0x19A
	ANDI R17,LOW(251)
	MOV  R19,R21
	SUBI R16,LOW(1)
	RJMP _0x19B
_0x19A:
	LDI  R19,LOW(48)
_0x19B:
	RJMP _0x19C
_0x199:
	LDI  R19,LOW(32)
_0x19C:
	RCALL SUBOPT_0x4F
	SUBI R20,LOW(1)
	RJMP _0x196
_0x198:
_0x195:
	MOV  R18,R16
	SBRS R17,1
	RJMP _0x19D
_0x19E:
	CPI  R18,0
	BREQ _0x1A0
	SBRS R17,3
	RJMP _0x1A1
	LDD  R30,Y+6
	LDD  R31,Y+6+1
	ADIW R30,1
	STD  Y+6,R30
	STD  Y+6+1,R31
	SBIW R30,1
	LPM
	MOV  R30,R0
	RJMP _0x1C8
_0x1A1:
	LDD  R26,Y+12
	LDD  R27,Y+12+1
	LD   R30,X+
	STD  Y+12,R26
	STD  Y+12+1,R27
_0x1C8:
	ST   -Y,R30
	RCALL __put_G2
	CPI  R20,0
	BREQ _0x1A3
	SUBI R20,LOW(1)
_0x1A3:
	SUBI R18,LOW(1)
	RJMP _0x19E
_0x1A0:
	RJMP _0x1A4
_0x19D:
_0x1A6:
	LDI  R19,LOW(48)
	LDD  R30,Y+6
	LDD  R31,Y+6+1
	ADIW R30,2
	STD  Y+6,R30
	STD  Y+6+1,R31
	SBIW R30,2
	RCALL __GETW1PF
	STD  Y+8,R30
	STD  Y+8+1,R31
                                      ldd  r26,y+10  ;R26,R27=n
                                      ldd  r27,y+11
                                  calc_digit:
                                      cp   r26,r30
                                      cpc  r27,r31
                                      brlo calc_digit_done
	SUBI R19,-LOW(1)
	                                  sub  r26,r30
	                                  sbc  r27,r31
	                                  brne calc_digit
                                  calc_digit_done:
                                      std  Y+10,r26 ;n=R26,R27
                                      std  y+11,r27
	LDI  R30,LOW(57)
	CP   R30,R19
	BRSH _0x1A8
	SBRS R17,3
	RJMP _0x1A9
	SUBI R19,-LOW(7)
	RJMP _0x1AA
_0x1A9:
	SUBI R19,-LOW(39)
_0x1AA:
_0x1A8:
	SBRC R17,4
	RJMP _0x1AC
	LDI  R30,LOW(48)
	CP   R30,R19
	BRLO _0x1AE
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	CPI  R26,LOW(0x1)
	LDI  R30,HIGH(0x1)
	CPC  R27,R30
	BRNE _0x1AD
_0x1AE:
	ORI  R17,LOW(16)
	RJMP _0x1B0
_0x1AD:
	CP   R20,R18
	BRLO _0x1B2
	SBRS R17,0
	RJMP _0x1B3
_0x1B2:
	RJMP _0x1B1
_0x1B3:
	LDI  R19,LOW(32)
	SBRS R17,7
	RJMP _0x1B4
	LDI  R19,LOW(48)
	ORI  R17,LOW(16)
_0x1B0:
	SBRS R17,2
	RJMP _0x1B5
	ANDI R17,LOW(251)
	ST   -Y,R21
	RCALL __put_G2
	CPI  R20,0
	BREQ _0x1B6
	SUBI R20,LOW(1)
_0x1B6:
_0x1B5:
_0x1B4:
_0x1AC:
	RCALL SUBOPT_0x4F
	CPI  R20,0
	BREQ _0x1B7
	SUBI R20,LOW(1)
_0x1B7:
_0x1B1:
	SUBI R18,LOW(1)
	LDD  R26,Y+8
	LDD  R27,Y+8+1
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	CP   R30,R26
	CPC  R31,R27
	BRSH _0x1A7
	RJMP _0x1A6
_0x1A7:
_0x1A4:
	SBRS R17,0
	RJMP _0x1B8
_0x1B9:
	CPI  R20,0
	BREQ _0x1BB
	SUBI R20,LOW(1)
	LDI  R30,LOW(32)
	RCALL SUBOPT_0x51
	RJMP _0x1B9
_0x1BB:
_0x1B8:
_0x1BC:
_0x17E:
	LDI  R16,LOW(0)
_0x169:
	RJMP _0x164
_0x166:
	RCALL __LOADLOCR6
	ADIW R28,18
	RET
_sprintf:
	MOV  R15,R24
	SBIW R28,2
	MOV  R26,R28
	MOV  R27,R29
	SBIW R26,2
	RCALL __ADDW2R15
	ST   Y,R26
	STD  Y+1,R27
	MOV  R26,R28
	MOV  R27,R29
	ADIW R26,4
	RCALL SUBOPT_0x53
	STS  _pp_G2,R30
	STS  _pp_G2+1,R31
	MOV  R26,R28
	MOV  R27,R29
	ADIW R26,2
	RCALL SUBOPT_0x53
	ST   -Y,R31
	ST   -Y,R30
	LDD  R30,Y+2
	LDD  R31,Y+2+1
	ST   -Y,R31
	ST   -Y,R30
	RCALL __print_G2
	LDS  R26,_pp_G2
	LDS  R27,_pp_G2+1
	RCALL SUBOPT_0xD
	ADIW R28,2
	RET
_log:
	RCALL SUBOPT_0x47
	__GETD2S 6
	RCALL __CPD02
	BRLT _0x1BF
	__GETD1N 0xFF7FFFFF
	RJMP _0x1C1
_0x1BF:
	__GETD1S 6
	RCALL __PUTPARD1
	LDI  R30,LOW(16)
	LDI  R31,HIGH(16)
	ST   -Y,R31
	ST   -Y,R30
	RCALL _frexp
	__PUTD1S 6
	__GETD2S 6
	__GETD1N 0x3F3504F3
	RCALL __CMPF12
	BRSH _0x1C0
	RCALL SUBOPT_0x54
	RCALL __ADDF12
	__PUTD1S 6
	__SUBWRN 16,17,1
_0x1C0:
	__GETD2S 6
	__GETD1N 0x3F800000
	RCALL SUBOPT_0x2D
	PUSH R23
	PUSH R22
	PUSH R31
	PUSH R30
	__GETD1S 6
	__GETD2N 0x3F800000
	RCALL __ADDF12
	POP  R26
	POP  R27
	POP  R24
	POP  R25
	RCALL __DIVF21
	__PUTD1S 6
	RCALL SUBOPT_0x54
	RCALL __MULF12
	__PUTD1S 2
	__GETD2N 0x3F654226
	RCALL SUBOPT_0x2C
	__GETD1N 0x4054114E
	RCALL SUBOPT_0x2D
	__GETD2S 6
	RCALL __MULF12
	PUSH R23
	PUSH R22
	PUSH R31
	PUSH R30
	__GETD2S 2
	__GETD1N 0x3FD4114D
	RCALL SUBOPT_0x2D
	POP  R26
	POP  R27
	POP  R24
	POP  R25
	RCALL __DIVF21
	PUSH R23
	PUSH R22
	PUSH R31
	PUSH R30
	__GETW1R 16,17
	__GETD2N 0x3F317218
	RCALL __CWD1
	RCALL __CDF1
	RCALL __MULF12
	POP  R26
	POP  R27
	POP  R24
	POP  R25
	RCALL __ADDF12
_0x1C1:
	RCALL __LOADLOCR2
	ADIW R28,10
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x0:
	CLR  R30
	STS  _cPWMVal,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x1:
	LDI  R30,LOW(1)
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x2:
	RCALL _WriteDataToConsoleLEDS
	CLR  R30
	STS  _itrDelay,R30
	STS  _itrDelay+1,R30
	STS  _itrDelay+2,R30
	STS  _itrDelay+3,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x3:
	LDS  R26,_itrDelay
	LDS  R27,_itrDelay+1
	LDS  R24,_itrDelay+2
	LDS  R25,_itrDelay+3
	__CPD2N 0x5640
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x4:
	CLR  R30
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x5:
	RCALL _read_adc
	__PUTW1R 13,14
	__GETW1R 13,14
	RCALL __CWD1
	RCALL __CDF1
	STS  _ftmp,R30
	STS  _ftmp+1,R31
	STS  _ftmp+2,R22
	STS  _ftmp+3,R23
	LDS  R26,_ftmp
	LDS  R27,_ftmp+1
	LDS  R24,_ftmp+2
	LDS  R25,_ftmp+3
	__GETD1N 0x3E5182AA
	RCALL __DIVF21
	STS  _vtmp,R30
	STS  _vtmp+1,R31
	STS  _vtmp+2,R22
	STS  _vtmp+3,R23
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 8 TIMES
SUBOPT_0x6:
	LDS  R30,_vtmp
	LDS  R31,_vtmp+1
	LDS  R22,_vtmp+2
	LDS  R23,_vtmp+3
	RCALL __CFD1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x7:
	LDS  R30,_CurrentWattSeconds
	LDS  R31,_CurrentWattSeconds+1
	LDS  R22,_CurrentWattSeconds+2
	LDS  R23,_CurrentWattSeconds+3
	RCALL __EEPROMWRD
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x8:
	LDI  R26,LOW(_PowerWattSeconds)
	LDI  R27,HIGH(_PowerWattSeconds)
	RCALL __EEPROMRDD
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x9:
	__GETD1N 0x445C0
	STS  _SavedWattSeconds,R30
	STS  _SavedWattSeconds+1,R31
	STS  _SavedWattSeconds+2,R22
	STS  _SavedWattSeconds+3,R23
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xA:
	LDD  R30,Y+2
	LDD  R31,Y+2+1
	LDD  R26,Y+4
	LDD  R27,Y+4+1
	ADD  R26,R30
	ADC  R27,R31
	LD   R30,X
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xB:
	LDS  R26,_itrDelay
	LDS  R27,_itrDelay+1
	LDS  R24,_itrDelay+2
	LDS  R25,_itrDelay+3
	__CPD2N 0x14
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xC:
	MOV  R26,R5
	CLR  R27
	SUBI R26,LOW(-_ComRecBuff)
	SBCI R27,HIGH(-_ComRecBuff)
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xD:
	CLR  R30
	ST   X,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0xE:
	LDI  R30,LOW(65507)
	LDI  R31,HIGH(65507)
	OUT  0x2C+1,R31
	OUT  0x2C,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0xF:
	OUT  0x15,R30
	CLR  R30
	OUT  0x15,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x10:
	LDS  R30,_iMotorTimer
	LDS  R31,_iMotorTimer+1
	STS  _iMotorSpeed,R30
	STS  _iMotorSpeed+1,R31
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x11:
	CLR  R30
	STS  _iTimerSOC,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x12:
	CLR  R30
	OUT  0x15,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x13:
	LDS  R30,_itmp
	LDS  R31,_itmp+1
	ADIW R30,1
	STS  _itmp,R30
	STS  _itmp+1,R31
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x14:
	LDS  R30,_PhaseOutFWD
	OUT  0x15,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x15:
	LDS  R30,_PhaseOutFWD
	ORI  R30,0x40
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x16:
	CLR  R30
	STS  _LEDbyte,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x17:
	CLR  R30
	STS  _OldLEDbyte,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x18:
	CLR  R30
	STS  _ComSequence,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x19:
	CLR  R30
	STS  _CurrentLimitByte,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x1A:
	CLR  R30
	STS  _isecTmr,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x1B:
	CLR  R30
	STS  _isecTmrWatts,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 15 TIMES
SUBOPT_0x1C:
	LDI  R30,LOW(_ComStr)
	LDI  R31,HIGH(_ComStr)
	ST   -Y,R31
	ST   -Y,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x1D:
	RCALL _sprintf
	ADIW R28,4
	RJMP SUBOPT_0x1C

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x1E:
	SUBI R30,LOW(-_SOC_LEDS)
	SBCI R31,HIGH(-_SOC_LEDS)
	LD   R30,Z
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x1F:
	RCALL _read_adc
	__PUTW1R 13,14
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x20:
	LDI  R30,LOW(1)
	MOV  R10,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x21:
	LDS  R26,_SavedWattSeconds
	LDS  R27,_SavedWattSeconds+1
	LDS  R24,_SavedWattSeconds+2
	LDS  R25,_SavedWattSeconds+3
	__CPD2N 0x111700
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x22:
	LDS  R30,_MaxWattSeconds
	LDS  R31,_MaxWattSeconds+1
	LDS  R22,_MaxWattSeconds+2
	LDS  R23,_MaxWattSeconds+3
	STS  _CurrentWattSeconds,R30
	STS  _CurrentWattSeconds+1,R31
	STS  _CurrentWattSeconds+2,R22
	STS  _CurrentWattSeconds+3,R23
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x23:
	RCALL _CalcStateOfChargeForConsole
	STS  _LEDStatus,R30
	CLR  R31
	RJMP SUBOPT_0x1E

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x24:
	LDS  R30,_calcLEDbyte
	STS  _LEDbyte,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x25:
	LDS  R30,_LEDbyte
	STS  _OldLEDbyte,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x26:
	LDS  R30,_LEDbyte
	ST   -Y,R30
	RJMP _WriteDataToConsoleLEDS

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x27:
	LDI  R30,LOW(15)
	CP   R30,R26
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x28:
	RCALL __CWD1
	RCALL __CDF1
	STS  _ftmp,R30
	STS  _ftmp+1,R31
	STS  _ftmp+2,R22
	STS  _ftmp+3,R23
	LDS  R26,_ftmp
	LDS  R27,_ftmp+1
	LDS  R24,_ftmp+2
	LDS  R25,_ftmp+3
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x29:
	RCALL __SUBF12
	STS  _vtmp,R30
	STS  _vtmp+1,R31
	STS  _vtmp+2,R22
	STS  _vtmp+3,R23
	__GETD2N 0x3F800000
	RCALL __DIVF21
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x2A:
	STS  _vtmp,R30
	STS  _vtmp+1,R31
	STS  _vtmp+2,R22
	STS  _vtmp+3,R23
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 5 TIMES
SUBOPT_0x2B:
	STS  _ftmp,R30
	STS  _ftmp+1,R31
	STS  _ftmp+2,R22
	STS  _ftmp+3,R23
	LDS  R26,_ftmp
	LDS  R27,_ftmp+1
	LDS  R24,_ftmp+2
	LDS  R25,_ftmp+3
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x2C:
	RCALL __MULF12
	RCALL __MOVED12
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 6 TIMES
SUBOPT_0x2D:
	RCALL __SWAPD12
	RCALL __SUBF12
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x2E:
	LDS  R30,_ftmp
	LDS  R31,_ftmp+1
	LDS  R22,_ftmp+2
	LDS  R23,_ftmp+3
	RCALL __CFD1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x2F:
	STS  _itmp,R30
	STS  _itmp+1,R31
	LDS  R26,_itmp
	LDS  R27,_itmp+1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x30:
	RCALL __MULF12
	STS  _ftmp,R30
	STS  _ftmp+1,R31
	STS  _ftmp+2,R22
	STS  _ftmp+3,R23
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x31:
	LDI  R30,LOW(192)
	STS  _CurrentLimitByte,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x32:
	LDS  R26,_ComSequence
	CPI  R26,LOW(0x4)
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x33:
	LDS  R26,_BatteryVoltage
	LDS  R27,_BatteryVoltage+1
	CP   R26,R30
	CPC  R27,R31
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x34:
	LDS  R26,_DriveEnable
	CPI  R26,LOW(0x50)
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x35:
	ADD  R26,R30
	ADC  R27,R31
	RCALL __GETW1P
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x36:
	LDS  R30,_ftmp
	LDS  R31,_ftmp+1
	LDS  R22,_ftmp+2
	LDS  R23,_ftmp+3
	RCALL __CFD1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x37:
	LDI  R30,LOW(1)
	CP   R30,R9
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x38:
	CLR  R30
	STS  _tmpPWMVal,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x39:
	LDS  R26,_iMotorRPM
	LDS  R27,_iMotorRPM+1
	CPI  R26,LOW(0x28A)
	LDI  R30,HIGH(0x28A)
	CPC  R27,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x3A:
	RCALL __DIVF21
	STS  _ftmp,R30
	STS  _ftmp+1,R31
	STS  _ftmp+2,R22
	STS  _ftmp+3,R23
	RJMP SUBOPT_0x36

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x3B:
	LDS  R30,_tmpPWMVal
	LDS  R26,_tmpChar
	CP   R26,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x3C:
	LDS  R30,_tmpChar
	STS  _tmpPWMVal,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x3D:
	LDS  R30,_BatteryCurrent
	CLR  R31
	CLR  R22
	CLR  R23
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x3E:
	LDS  R30,_calcLEDbyte
	CPI  R30,0
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x3F:
	LDS  R26,_ComSequence
	CPI  R26,LOW(0x2)
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x40:
	STS  _tmpChar,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x41:
	ST   -Y,R31
	ST   -Y,R30
	LDS  R30,_cPWMVal
	CLR  R31
	CLR  R22
	CLR  R23
	RCALL __PUTPARD1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x42:
	RCALL __CWD1
	RCALL __PUTPARD1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 6 TIMES
SUBOPT_0x43:
	RCALL _sprintf
	ADIW R28,8
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x44:
	CLR  R31
	CLR  R22
	CLR  R23
	RCALL __PUTPARD1
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x45:
	LDS  R30,_tmpPWMVal
	LDS  R26,_OldcPWMVal
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x46:
	LDS  R30,_tmpPWMVal
	STS  _OldcPWMVal,R30
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x47:
	SBIW R28,4
	RCALL __SAVELOCR2
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x48:
	__GETW1R 16,17
	LDI  R26,LOW(_DischargePoints)
	LDI  R27,HIGH(_DischargePoints)
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 3 TIMES
SUBOPT_0x49:
	LDI  R26,LOW(10)
	LDI  R27,HIGH(10)
	RCALL __MULW12U
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x4A:
	MOV  R26,R30
	MOV  R27,R31
	RCALL __GETW1P
	RJMP SUBOPT_0x33

;OPTIMIZER ADDED SUBROUTINE, CALLED 7 TIMES
SUBOPT_0x4B:
	RCALL __MULF12
	STS  _vtmp,R30
	STS  _vtmp+1,R31
	STS  _vtmp+2,R22
	STS  _vtmp+3,R23
	RJMP SUBOPT_0x6

;OPTIMIZER ADDED SUBROUTINE, CALLED 7 TIMES
SUBOPT_0x4C:
	__PUTD1S 2
	LDS  R26,_CurrentWattSeconds
	LDS  R27,_CurrentWattSeconds+1
	LDS  R24,_CurrentWattSeconds+2
	LDS  R25,_CurrentWattSeconds+3
	RCALL __CPD12
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x4D:
	LDS  R30,_CurrentWattSeconds
	LDS  R31,_CurrentWattSeconds+1
	LDS  R22,_CurrentWattSeconds+2
	LDS  R23,_CurrentWattSeconds+3
	__SUBD1N 100000
	STS  _CurrentWattSeconds,R30
	STS  _CurrentWattSeconds+1,R31
	STS  _CurrentWattSeconds+2,R22
	STS  _CurrentWattSeconds+3,R23
	CLR  R4
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x4E:
	RCALL __DIVF21
	RJMP SUBOPT_0x2A

;OPTIMIZER ADDED SUBROUTINE, CALLED 4 TIMES
SUBOPT_0x4F:
	ST   -Y,R19
	RJMP __put_G2

;OPTIMIZER ADDED SUBROUTINE, CALLED 5 TIMES
SUBOPT_0x50:
	LDD  R26,Y+14
	LDD  R27,Y+14+1
	SBIW R26,4
	STD  Y+14,R26
	STD  Y+14+1,R27
	ADIW R26,4
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x51:
	ST   -Y,R30
	RJMP __put_G2

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x52:
	RCALL __GETW1P
	STD  Y+10,R30
	STD  Y+10+1,R31
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x53:
	RCALL __ADDW2R15
	RCALL __GETW1P
	RET

;OPTIMIZER ADDED SUBROUTINE, CALLED 2 TIMES
SUBOPT_0x54:
	__GETD1S 6
	__GETD2S 6
	RET

_strlen:
	ld   r26,y+
	ld   r27,y+
	clr  r30
	clr  r31
__strlen0:
	ld   r22,x+
	tst  r22
	breq __strlen1
	adiw r30,1
	rjmp __strlen0
__strlen1:
	ret

_strlenf:
	clr  r26
	clr  r27
	ld   r30,y+
	ld   r31,y+
__strlenf0:
	lpm 
	tst  r0
	breq __strlenf1
	adiw r26,1
	adiw r30,1
	rjmp __strlenf0
__strlenf1:
	mov  r30,r26
	mov  r31,r27
	ret

_sqrt:
	sbiw r28,4
	push r21
	ldd  r25,y+7
	tst  r25
	brne __sqrt0
	adiw r28,8
	rjmp __zerores
__sqrt0:
	brpl __sqrt1
	adiw r28,8
	rjmp __maxres
__sqrt1:
	push r20
	ldi  r20,66
	ldd  r24,y+6
	ldd  r27,y+5
	ldd  r26,y+4
__sqrt2:
	st   y,r24
	std  y+1,r25
	std  y+2,r26
	std  y+3,r27
	mov  r30,r26
	mov  r31,r27
	mov  r22,r24
	mov  r23,r25
	ldd  r26,y+4
	ldd  r27,y+5
	ldd  r24,y+6
	ldd  r25,y+7
	rcall __divf21
	ld   r24,y
	ldd  r25,y+1
	ldd  r26,y+2
	ldd  r27,y+3
	rcall __addf12
	rcall __unpack1
	dec  r23
	rcall __repack
	ld   r24,y
	ldd  r25,y+1
	ldd  r26,y+2
	ldd  r27,y+3
	eor  r26,r30
	andi r26,0xf8
	brne __sqrt4
	cp   r27,r31
	cpc  r24,r22
	cpc  r25,r23
	breq __sqrt3
__sqrt4:
	dec  r20
	breq __sqrt3
	mov  r26,r30
	mov  r27,r31
	mov  r24,r22
	mov  r25,r23
	rjmp __sqrt2
__sqrt3:
	pop  r20
	pop  r21
	adiw r28,8
	ret

__SAVEISR:
	ST   -Y,R0
	ST   -Y,R1
	ST   -Y,R15
	ST   -Y,R22
	ST   -Y,R23
	ST   -Y,R24
	ST   -Y,R25
	ST   -Y,R26
	ST   -Y,R27
	ST   -Y,R30
	ST   -Y,R31
	IN   R0,SREG
	ST   -Y,R0
	RET

__LOADISR:
	LD   R0,Y+
	OUT  SREG,R0
	LD   R31,Y+
	LD   R30,Y+
	LD   R27,Y+
	LD   R26,Y+
	LD   R25,Y+
	LD   R24,Y+
	LD   R23,Y+
	LD   R22,Y+
	LD   R15,Y+
	LD   R1,Y+
	LD   R0,Y+
	RET

__ADDW2R15:
	CLR  R0
	ADD  R26,R15
	ADC  R27,R0
	RET

__ANEGW1:
	COM  R30
	COM  R31
	ADIW R30,1
	RET

__ANEGD1:
	COM  R30
	COM  R31
	COM  R22
	COM  R23
	SUBI R30,-1
	SBCI R31,-1
	SBCI R22,-1
	SBCI R23,-1
	RET

__LSRW2:
	LSR  R31
	ROR  R30
	LSR  R31
	ROR  R30
	RET

__CWD1:
	CLR  R22
	CLR  R23
	SBRS R31,7
	RET
	SER  R22
	SER  R23
	RET

__MOVED12:
	MOV  R26,R30
	MOV  R27,R31
	MOV  R24,R22
	MOV  R25,R23
	RET

__MULW12U:
	MOV  R0,R26
	MOV  R1,R27
	LDI  R24,17
	CLR  R26
	SUB  R27,R27
	RJMP __MULW12U1
__MULW12U3:
	BRCC __MULW12U2
	ADD  R26,R0
	ADC  R27,R1
__MULW12U2:
	LSR  R27
	ROR  R26
__MULW12U1:
	ROR  R31
	ROR  R30
	DEC  R24
	BRNE __MULW12U3
	RET

__DIVW21U:
	CLR  R0
	CLR  R1
	LDI  R25,16
__DIVW21U1:
	LSL  R26
	ROL  R27
	ROL  R0
	ROL  R1
	SUB  R0,R30
	SBC  R1,R31
	BRCC __DIVW21U2
	ADD  R0,R30
	ADC  R1,R31
	RJMP __DIVW21U3
__DIVW21U2:
	SBR  R26,1
__DIVW21U3:
	DEC  R25
	BRNE __DIVW21U1
	MOV  R30,R26
	MOV  R31,R27
	MOV  R26,R0
	MOV  R27,R1
	RET

__GETW1P:
	LD   R30,X+
	LD   R31,X
	SBIW R26,1
	RET

__GETD1P:
	LD   R30,X+
	LD   R31,X+
	LD   R22,X+
	LD   R23,X
	SBIW R26,3
	RET

__PUTWP1:
	ST   X+,R30
	ST   X,R31
	RET

__GETW1PF:
	LPM
	ADIW R30,1
	MOV  R1,R0
	LPM 
	MOV  R31,R0
	MOV  R30,R1
	RET

__PUTPARD1:
	ST   -Y,R23
	ST   -Y,R22
	ST   -Y,R31
	ST   -Y,R30
	RET

__CDF2U:
	SET
	RJMP __CDF2U0
__CDF2:
	CLT
__CDF2U0:
	RCALL __SWAPD12
	RCALL __CDF1U0

__SWAPD12:
	MOV  R1,R24
	MOV  R24,R22
	MOV  R22,R1
	MOV  R1,R25
	MOV  R25,R23
	MOV  R23,R1

__SWAPW12:
	MOV  R1,R27
	MOV  R27,R31
	MOV  R31,R1

__SWAPB12:
	MOV  R1,R26
	MOV  R26,R30
	MOV  R30,R1
	RET

__EEPROMRDD:
	ADIW R26,2
	RCALL __EEPROMRDW
	MOV  R23,R31
	MOV  R22,R30
	SBIW R26,2

__EEPROMRDW:
	ADIW R26,1
	RCALL __EEPROMRDB
	MOV  R31,R30
	SBIW R26,1

__EEPROMRDB:
	SBIC EECR,EEWE
	RJMP __EEPROMRDB
	IN   R0,SREG
	CLI
	OUT  EEARL,R26
	OUT  EEARH,R27
	SBI  EECR,EERE
	IN   R30,EEDR
	OUT  SREG,R0
	RET

__EEPROMWRD:
	RCALL __EEPROMWRW
	ADIW R26,2
	MOV  R0,R30
	MOV  R1,R31
	MOV  R30,R22
	MOV  R31,R23
	RCALL __EEPROMWRW
	MOV  R30,R0
	MOV  R31,R1
	SBIW R26,2
	RET

__EEPROMWRW:
	RCALL __EEPROMWRB
	ADIW R26,1
	PUSH R30
	MOV  R30,R31
	RCALL __EEPROMWRB
	POP  R30
	SBIW R26,1
	RET

__EEPROMWRB:
	SBIC EECR,EEWE
	RJMP __EEPROMWRB
	IN   R25,SREG
	CLI
	OUT  EEARL,R26
	OUT  EEARH,R27
	SBI  EECR,EERE
	IN   R24,EEDR
	CP   R30,R24
	BREQ __EEPROMWRB0
	OUT  EEDR,R30
	SBI  EECR,EEMWE
	SBI  EECR,EEWE
__EEPROMWRB0:
	OUT  SREG,R25
	RET

_frexp:
	LD   R26,Y+
	LD   R27,Y+
	LD   R30,Y+
	LD   R31,Y+
	LD   R22,Y+
	LD   R23,Y+
	BST  R23,7
	LSL  R22
	ROL  R23
	CLR  R24
	SUBI R23,0x7E
	SBC  R24,R24
	ST   X+,R23
	ST   X,R24
	LDI  R23,0x7E
	LSR  R23
	ROR  R22
	BRTS __ANEGF1
	RET

__ANEGF1:
	SBIW R30,0
	SBCI R22,0
	SBCI R23,0
	BREQ __ANEGF10
	SUBI R23,0x80
__ANEGF10:
	RET

__REPACK:
	LDI  R21,0x80
	EOR  R21,R23
	BRNE __REPACK0
	PUSH R21
	RJMP __ZERORES
__REPACK0:
	CPI  R21,0xFF
	BREQ __REPACK1
	LSL  R22
	LSL  R0
	ROR  R21
	ROR  R22
	MOV  R23,R21
	RET
__REPACK1:
	PUSH R21
	TST  R0
	BRMI __REPACK2
	RJMP __MAXRES
__REPACK2:
	RJMP __MINRES

__UNPACK:
	LDI  R21,0x80
	MOV  R1,R25
	AND  R1,R21
	LSL  R24
	ROL  R25
	EOR  R25,R21
	LSL  R21
	ROR  R24

__UNPACK1:
	LDI  R21,0x80
	MOV  R0,R23
	AND  R0,R21
	LSL  R22
	ROL  R23
	EOR  R23,R21
	LSL  R21
	ROR  R22
	RET

__CFD1:
	PUSH R21
	RCALL __UNPACK1
	CPI  R23,0x80
	BRLO __CFD10
	CPI  R23,0xFF
	BRLO __CFD18
__CFD10:
	LDI  R21,22
	SUB  R21,R23
	BRPL __CFD11
	RJMP __MAXRES
__CFD11:
	CPI  R21,24
	BRLO __CFD12
__CFD18:
	RJMP __ZERORES
__CFD12:
	CLR  R23
__CFD13:
	CPI  R21,8
	BRLO __CFD14
	MOV  R30,R31
	MOV  R31,R22
	MOV  R22,R23
	SUBI R21,8
	RJMP __CFD13
__CFD14:
	TST  R21
	BREQ __CFD16
__CFD15:
	LSR  R23
	ROR  R22
	ROR  R31
	ROR  R30
	DEC  R21
	BRNE __CFD15
__CFD16:
	CLR  R23
	TST  R0
	BRPL __CFD17
	RCALL __NEGMAN1
	SER  R23
__CFD17:
	POP  R21
	RET

__CDF1U:
	SET
	RJMP __CDF1U0
__CDF1:
	CLT
__CDF1U0:
	SBIW R30,0
	SBCI R22,0
	SBCI R23,0
	BREQ __CDF10
	CLR  R0
	BRTS __CDF11
	TST  R23
	BRPL __CDF11
	COM  R0
	RCALL __ANEGD1
__CDF11:
	MOV  R1,R23
	LDI  R23,30
	TST  R1
__CDF12:
	BRMI __CDF13
	DEC  R23
	LSL  R30
	ROL  R31
	ROL  R22
	ROL  R1
	RJMP __CDF12
__CDF13:
	MOV  R30,R31
	MOV  R31,R22
	MOV  R22,R1
	PUSH R21
	RCALL __REPACK
	POP  R21
__CDF10:
	RET

__SWAPACC:
	MOV  R21,R30
	MOV  R30,R26
	MOV  R26,R21
	MOV  R21,R31
	MOV  R31,R27
	MOV  R27,R21
	MOV  R21,R22
	MOV  R22,R24
	MOV  R24,R21
	MOV  R21,R23
	MOV  R23,R25
	MOV  R25,R21
	MOV  R21,R0
	MOV  R0,R1
	MOV  R1,R21
	RET

__UADD12:
	ADD  R30,R26
	ADC  R31,R27
	ADC  R22,R24
	RET

__NEGMAN1:
	COM  R30
	COM  R31
	COM  R22
	SUBI R30,-1
	SBCI R31,-1
	SBCI R22,-1
	RET

__SUBF12:
	PUSH R21
	RCALL __UNPACK
	CPI  R25,0x80
	BREQ __ADDF129
	LDI  R21,0x80
	EOR  R1,R21

	RJMP __ADDF120

__ADDF12:
	PUSH R21
	RCALL __UNPACK
	CPI  R25,0x80
	BREQ __ADDF129

__ADDF120:
	CPI  R23,0x80
	BREQ __ADDF128
__ADDF121:
	MOV  R21,R23
	SUB  R21,R25
	BRVS __ADDF129
	BRPL __ADDF122
	RCALL __SWAPACC
	RJMP __ADDF121
__ADDF122:
	CPI  R21,24
	BRLO __ADDF123
	CLR  R26
	CLR  R27
	CLR  R24
__ADDF123:
	CPI  R21,8
	BRLO __ADDF124
	MOV  R26,R27
	MOV  R27,R24
	CLR  R24
	SUBI R21,8
	RJMP __ADDF123
__ADDF124:
	TST  R21
	BREQ __ADDF126
__ADDF125:
	LSR  R24
	ROR  R27
	ROR  R26
	DEC  R21
	BRNE __ADDF125
__ADDF126:
	MOV  R21,R0
	EOR  R21,R1
	BRMI __ADDF127
	RCALL __UADD12
	BRCC __ADDF129
	ROR  R22
	ROR  R31
	ROR  R30
	INC  R23
	BRVC __ADDF129
	RJMP __MAXRES
__ADDF128:
	RCALL __SWAPACC
__ADDF129:
	RCALL __REPACK
	POP  R21
	RET
__ADDF127:
	SUB  R30,R26
	SBC  R31,R27
	SBC  R22,R24
	BREQ __ZERORES
	BRCC __ADDF1210
	COM  R0
	RCALL __NEGMAN1
__ADDF1210:
	TST  R22
	BRMI __ADDF129
	LSL  R30
	ROL  R31
	ROL  R22
	DEC  R23
	BRVC __ADDF1210

__MINRES:
	SER  R30
	SER  R31
	LDI  R22,0x7F
	SER  R23
	POP  R21
	RET

__ZERORES:
	CLR  R30
	CLR  R31
	CLR  R22
	CLR  R23
	POP  R21
	RET

__MAXRES:
	SER  R30
	SER  R31
	LDI  R22,0x7F
	LDI  R23,0x7F
	POP  R21
	RET

__MULF12:
	PUSH R21
	RCALL __UNPACK
	CPI  R23,0x80
	BREQ __ZERORES
	CPI  R25,0x80
	BREQ __ZERORES
	EOR  R0,R1
	SEC
	ADC  R23,R25
	BRVC __MULF124
	BRLT __ZERORES
__MULF125:
	TST  R0
	BRMI __MINRES
	RJMP __MAXRES
__MULF124:
	PUSH R19
	PUSH R20
	CLR  R1
	CLR  R19
	CLR  R20
	CLR  R21
	LDI  R25,24
__MULF120:
	LSL  R19
	ROL  R20
	ROL  R21
	ROL  R30
	ROL  R31
	ROL  R22
	BRCC __MULF121
	ADD  R19,R26
	ADC  R20,R27
	ADC  R21,R24
	ADC  R30,R1
	ADC  R31,R1
	ADC  R22,R1
__MULF121:
	DEC  R25
	BRNE __MULF120
	POP  R20
	POP  R19
	TST  R22
	BRMI __MULF122
	LSL  R21
	ROL  R30
	ROL  R31
	ROL  R22
	RJMP __MULF123
__MULF122:
	INC  R23
	BRVS __MULF125
__MULF123:
	RCALL __REPACK
	POP  R21
	RET

__DIVF21:
	PUSH R21
	RCALL __UNPACK
	CPI  R23,0x80
	BRNE __DIVF210
	TST  R1
__DIVF211:
	BRPL __MAXRES
	RJMP __MINRES
__DIVF210:
	CPI  R25,0x80
	BRNE __DIVF218
__DIVF217:
	RJMP __ZERORES
__DIVF218:
	EOR  R0,R1
	SEC
	SBC  R25,R23
	BRVC __DIVF216
	BRLT __DIVF217
	TST  R0
	RJMP __DIVF211
__DIVF216:
	MOV  R23,R25
	LSR  R22
	ROR  R31
	ROR  R30
	LSR  R24
	ROR  R27
	ROR  R26
	PUSH R20
	CLR  R1
	CLR  R20
	CLR  R21
	LDI  R25,24
__DIVF212:
	CP   R26,R30
	CPC  R27,R31
	CPC  R24,R22
	BRLO __DIVF213
	SUB  R26,R30
	SBC  R27,R31
	SBC  R24,R22
	SEC
	RJMP __DIVF214
__DIVF213:
	CLC
__DIVF214:
	ROL  R1
	ROL  R20
	ROL  R21
	ROL  R26
	ROL  R27
	ROL  R24
	DEC  R25
	BRNE __DIVF212
	MOV  R30,R1
	MOV  R31,R20
	MOV  R22,R21
	LSR  R26
	ADC  R30,R25
	ADC  R31,R25
	ADC  R22,R25
	POP  R20
	TST  R22
	BRMI __DIVF215
	LSL  R30
	ROL  R31
	ROL  R22
	DEC  R23
	BRVS __DIVF217
__DIVF215:
	RCALL __REPACK
	POP  R21
	RET

__CMPF12:
	TST  R25
	BRMI __CMPF120
	TST  R23
	BRMI __CMPF121
	CP   R25,R23
	BRLO __CMPF122
	BRNE __CMPF121
	CP   R26,R30
	CPC  R27,R31
	CPC  R24,R22
	BRLO __CMPF122
	BREQ __CMPF123
__CMPF121:
	CLZ
	CLC
	RET
__CMPF122:
	CLZ
	SEC
	RET
__CMPF123:
	SEZ
	CLC
	RET
__CMPF120:
	TST  R23
	BRPL __CMPF122
	CP   R25,R23
	BRLO __CMPF121
	BRNE __CMPF122
	CP   R30,R26
	CPC  R31,R27
	CPC  R22,R24
	BRLO __CMPF122
	BREQ __CMPF123
	RJMP __CMPF121

__CPW02:
	CLR  R0
	CP   R0,R26
	CPC  R0,R27
	RET

__CPD20:
	SBIW R26,0
	SBCI R24,0
	SBCI R25,0
	RET

__CPD02:
	CLR  R0
	CP   R0,R26
	CPC  R0,R27
	CPC  R0,R24
	CPC  R0,R25
	RET

__CPD12:
	CP   R30,R26
	CPC  R31,R27
	CPC  R22,R24
	CPC  R23,R25
	RET

__LSLB3:
	LSL  R30
	LSL  R30
	LSL  R30
	RET

__SAVELOCR6:
	ST   -Y,R21
__SAVELOCR5:
	ST   -Y,R20
__SAVELOCR4:
	ST   -Y,R19
__SAVELOCR3:
	ST   -Y,R18
__SAVELOCR2:
	ST   -Y,R17
	ST   -Y,R16
	RET

__LOADLOCR6:
	LDD  R21,Y+5
__LOADLOCR5:
	LDD  R20,Y+4
__LOADLOCR4:
	LDD  R19,Y+3
__LOADLOCR3:
	LDD  R18,Y+2
__LOADLOCR2:
	LDD  R17,Y+1
	LD   R16,Y
	RET

