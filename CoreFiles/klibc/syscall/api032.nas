[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api032.nas"]

		GLOBAL	_api_get_day_of_week

[SECTION .text]

_api_get_day_of_week:		; unsigned int api_get_day_of_week(void);
		MOV		EDX,32
		INT		0x40
		RET
