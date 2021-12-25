[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api027.nas"]

		GLOBAL	_api_cls

[SECTION .text]

_api_cls:		; void api_cls(void);
		MOV		EDX,29
		INT		0x40
		RET