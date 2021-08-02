[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api030.nas"]

		GLOBAL	_api_get_year

[SECTION .text]

_api_get_year:		; unsigned int api_get_year(void);
		MOV		EDX,30
		INT		0x40
		RET
