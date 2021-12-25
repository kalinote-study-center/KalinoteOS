[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api035.nas"]

		GLOBAL	_api_get_min_hex

[SECTION .text]

_api_get_min_hex:		; unsigned int api_get_min_hex(void);
		MOV		EDX,35
		INT		0x40
		RET
