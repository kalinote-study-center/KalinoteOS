[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api034.nas"]

		GLOBAL	_api_get_hour_hex

[SECTION .text]

_api_get_hour_hex:		; unsigned int api_get_hour_hex(void);
		MOV		EDX,34
		INT		0x40
		RET
