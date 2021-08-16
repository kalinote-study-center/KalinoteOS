[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api039.nas"]

		GLOBAL	_api_send_key_to_cmd

[SECTION .text]

_api_send_key_to_cmd:       ; int api_send_key_to_cmd(char *);
        PUSH    EBX
        MOV     EDX,31
        MOV     EBX,[ESP+8]
        INT     0x40
        POP     EBX
        RET
		