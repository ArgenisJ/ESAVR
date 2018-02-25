;
; DA1.asm
;
; Created: 2/23/2018 7:09:53 PM
; Author : Argen
;


; Replace with your application code
.ORG 0	
	.EQU STARTADDS = 0x0222
	.EQU DIVISIBLE = 0x0400
	.EQU NOTDIV = 0x0600

	LDI XL, LOW(STARTADDS)
	LDI XH, HIGH(STARTADDS)
	LDI YL, LOW(DIVISIBLE)
	LDI YH, HIGH(DIVISIBLE)
	LDI ZL, LOW(NOTDIV)
	LDI ZH, HIGH(NOTDIV)

	LDI R20, LOW(300) 
	LDI R21, HIGH(300)

	CLR R22					; i = 0
	LDI R22, LOW(0x0001)	; i = 1
	ADD R23, R22			; StrN = i
	CLR R24					; low bit count = 0
	CLR R25					; high bit count = 0
SLOOP:
	ADD R23, R22			; StrN = StrN + i
	ST X+, R23				; StrN is stored and address increases
	INC	R24					; low bit count++
	
	CPI R24, 0x00			; if (low bit count = 0x00)				
	BREQ IncHigh			; go to increase high bit count
	
	CP R20, R24				; if (low bit count != 0x2D)
	BRNE SLOOP				; go to store loop
	CP R21, R25				; if (high bit count != 0x01)
	BRNE SLOOP				; go to store loop
	JMP DONE				; storing done

IncHigh:
	CLR R24					; reset low bit count	 
	INC R25					; increase high bit count
	JMP SLOOP				; go to store loop
DONE:	
	CLR R22
	CLR R23	
	CLR R24
	CLR R25
	
GetNum:
	LD R22, -X
	MOV R23, R22

DIV5:
	CPI R23, 0x05
	BREQ Yes5
	CPI R23, 0x05
	BRLO Not5
	SUBI R23, 0x05
	CPI R23, 0x05
	BRSH DIV5
	JMP Not5
Yes5:
	ST Y+, R22
	ADD R17, R22
	CP R17, R22
	BRLO IncR16
	JMP Back1
Not5:
	ST Z+, R22
	ADD R19, R22
	CP R19, R22
	BRLO IncR18
Back1:	
	INC R24
	CPI R24, 0x00
	BREQ IncHigh2
	JMP Chk300
IncR16:
	INC R16
	JMP Back1
IncR18:
	INC R18
	JMP Back1
IncHigh2:
	CLR R24
	INC R25
	JMP GetNum
Chk300:
	CP R20, R24				; if (low bit count != 0x2D)
	BRNE GetNum				; go to store loop
	CP R21, R25				; if (high bit count != 0x01)
	BRNE GetNum			; go to store loop
