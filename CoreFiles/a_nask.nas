[FORMAT "WCOFF"]				; 生成对象文件的模式
[INSTRSET "i486p"]				; 486指令集
[BITS 32]						; 32位
[FILE "a_nask.nas"]				; 源文件名

		GLOBAL	_api_putchar
		GLOBAL	_api_end

[SECTION .text]

_api_putchar:					; void api_putchar(int c);
		MOV		EDX,1
		MOV		AL,[ESP+4]		; c
		INT		0x40
		RET

_api_end:						; void api_end(void);
		MOV		EDX,4
		INT		0x40