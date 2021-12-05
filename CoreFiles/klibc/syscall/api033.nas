[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api033.nas"]

		GLOBAL	_api_get_day_of_month

[SECTION .text]

_api_get_day_of_month:		; unsigned int api_get_day_of_month(void);
		MOV		EDX,33
		INT		0x40
		RET
