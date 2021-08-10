[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api038.nas"]

		GLOBAL	_api_showjpeg

[SECTION .text]

_api_showjpeg:	; void api_showjpeg(char *filename, int x, int y, int win);
		PUSH	EDI
		PUSH	ESI
		PUSH	EBX
		MOV		EDX,38
		MOV		EAX,[ESP+16]	; filename
		MOV		ESI,[ESP+20]	; x
		MOV		EDI,[ESP+24]	; y
		MOV		EBX,[ESP+28]	; win
		INT		0x40
		POP		EBX
		POP		ESI
		POP		EDI
		RET
