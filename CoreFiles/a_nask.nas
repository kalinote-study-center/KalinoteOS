[FORMAT "WCOFF"]				; ���ɶ����ļ���ģʽ
[INSTRSET "i486p"]				; 486ָ�
[BITS 32]						; 32λ
[FILE "a_nask.nas"]				; Դ�ļ���

		GLOBAL	_api_putchar

[SECTION .text]

_api_putchar:					; void api_putchar(int c);
		MOV		EDX,1
		MOV		AL,[ESP+4]		; c
		INT		0x40
		RET
