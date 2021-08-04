[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api037.nas"]

		GLOBAL	_api_get_system_runtime

[SECTION .text]

_api_get_system_runtime:		; unsigned int api_get_system_runtime(void);
		MOV		EDX,37
		INT		0x40
		RET
