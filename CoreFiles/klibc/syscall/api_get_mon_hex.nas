[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api031.nas"]

		GLOBAL	_api_get_mon_hex

[SECTION .text]

_api_get_mon_hex:		; unsigned int api_get_mon_hex(void);
		MOV		EDX,31
		INT		0x40
		RET
