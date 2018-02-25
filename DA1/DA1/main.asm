; Author: ArgenisJ
; DA1
; 1. Store 300 numbers starting from the STARTADDS=0x0222 location. Populate the value 
;	 of the memory location by adding high(STARTADDS) and low(STARTADDS) . Use the X/
;	 Y/Z registers as pointers to fill up 300 numbers.
; 2. Use X/Y/Z register addressing to parse through the 300 numbers, if the number is
;	 divisible by 5 store the number starting from memory location 0x0400, else store at
;	 location starting at 0x0600.
; 3. Use X/Y/Z register addressing to simultaneously add numbers from memory location
;	 0x0400 and 0x0600 and store the sums at R16:R17 and R18:R19 respectively. Do not
;	 worry about the overflow.
; 4. Verify your algorithm and answers using C programming.
; 5. Determine the execution time @ 16MHz/#cycles of your algorithm using the simulation.

; Replace with your application code
.ORG 0	
	.EQU STARTADDS = 0x0222
	.EQU DIVISIBLE = 0x0400
	.EQU NOTDIV = 0x0600

; pointers x, y and z
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

; store numbers
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

; increase high bit count
IncHigh:
	CLR R24					; reset low bit count	 
	INC R25					; increase high bit count
	JMP SLOOP				; go to store loop
DONE:	

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	CLR R22					; Num = 0 
	CLR R23					; TestNum = 0 
	CLR R24					; counter = 0 low bit
	CLR R25					; counter = 0 high bit

; Load Num and TestNum	
GetNum:	
	LD R22, -X				; load Num at (0x222 + 0x12C) and decrease address
	MOV R23, R22			; move Num to TestNum register	

; Check TestNum
DIV5:						
	CPI R23, 0x05			; If TestNum = 5
	BREQ Yes5				; go to Yes5 (it is divisible)
	CPI R23, 0x05			; if TestNum < 5
	BRLO Not5				; go to Not5 (not divisible)
	SUBI R23, 0x05			; TestNum = TestNum - 5
	CPI R23, 0x05			; if TestNum >= 5
	BRSH DIV5				; go to DIV5
	JMP Not5				; else go to Not5 (not divisible)

; load IF divisible by 5
Yes5:				
	ST Y+, R22				; load Num to 0x0400 and increase address
	ADD R17, R22			; DivSum = DivSum + Num
	CP R17, R22				; if Divsum < Num
	BRLO IncR16				; increase high bit of DivSum
	JMP Back1				; else go to Back1 to prepare for next Num

; load IF NOT divisible by 5
Not5:
	ST Z+, R22				; load Num to 0x0600 and increase address
	ADD R19, R22			; NotSum = NotSum + Num
	CP R19, R22				; if NotSum < Num
	BRLO IncR18				; increase high bit of NotSum

; prepare for next Num
Back1:	
	INC R24					; increase counter
	CPI R24, 0x00			; if counter = 0x00
	BREQ IncHigh2			; go to Increase the high bit of counter
	JMP Chk300				; else check for 300 numbers
IncR16:				
	INC R16					; increase high bit of DivSum
	JMP Back1				; go to Back1 to prepare for next Num
IncR18:
	INC R18					; increase high bit of NotSum
	JMP Back1				; go to Back1 to prepare for next Num
IncHigh2:
	CLR R24					; set counter low bit to 0
	INC R25					; increase counter high bit
	JMP GetNum				; go to next Num
Chk300:
	CP R20, R24				; if (low bit counter != 0x2D)
	BRNE GetNum				; go to get next Num
	CP R21, R25				; if (high bit count != 0x01)
	BRNE GetNum				; go to get next Num
							; else DONE!
