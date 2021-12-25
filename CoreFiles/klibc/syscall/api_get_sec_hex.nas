[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api036.nas"]

		GLOBAL	_api_get_sec_hex

[SECTION .text]

_api_get_sec_hex:		; unsigned int api_get_sec_hex(void);
		MOV		EDX,36
		INT		0x40
		RET
