; naskfunc
; TAB=4
; 这个文件的存在是为了解决C语言不能解决的问题

[FORMAT "WCOFF"]				; 制作目标文件的模式
[INSTRSET "i486p"]				; 给486用的指令
[BITS 32]						; 制作32位模式用的机器语言
[FILE "naskfunc.nas"]			; 源文件名信息
		
		GLOBAL	_io_hlt, _io_cli, _io_sti, _io_stihlt
		GLOBAL	_io_in8,  _io_in16,  _io_in32
		GLOBAL	_io_out8, _io_out16, _io_out32
		GLOBAL	_io_load_eflags, _io_store_eflags
		GLOBAL	_load_gdtr, _load_idtr
		GLOBAL	_load_cr0, _store_cr0
		GLOBAL	_load_tr
		GLOBAL	_asm_inthandler20, _asm_inthandler21
		GLOBAL	_asm_inthandler27, _asm_inthandler2c
		GLOBAL	_asm_inthandler0c, _asm_inthandler0d
		GLOBAL	_asm_end_app, _memtest_sub
		GLOBAL	_farjmp, _farcall
		GLOBAL	_asm_kal_api, _start_app
		GLOBAL  _asm_shutdown
		EXTERN	_inthandler20, _inthandler21
		EXTERN	_inthandler27, _inthandler2c
		EXTERN	_inthandler0c, _inthandler0d
		EXTERN	_kal_api

[SECTION .text]					; 目标文件中写了这些之后在写程序

_io_hlt:						; void io_hlt(void);
		HLT
		RET
		
_io_cli:						; void io_cli(void);
		CLI
		RET

_io_sti:						; void io_sti(void);
		STI
		RET

_io_stihlt:						; void io_stihlt(void);
		STI
		HLT
		RET

_io_in8:						; int io_in8(int port);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,0
		IN		AL,DX
		RET

_io_in16:						; int io_in16(int port);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,0
		IN		AX,DX
		RET

_io_in32:						; int io_in32(int port);
		MOV		EDX,[ESP+4]		; port
		IN		EAX,DX
		RET

_io_out8:						; void io_out8(int port, int data);
		MOV		EDX,[ESP+4]		; port
		MOV		AL,[ESP+8]		; data
		OUT		DX,AL
		RET

_io_out16:						; void io_out16(int port, int data);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,[ESP+8]		; data
		OUT		DX,AX
		RET

_io_out32:						; void io_out32(int port, int data);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,[ESP+8]		; data
		OUT		DX,EAX
		RET

_io_load_eflags:				; int io_load_eflags(void);
		PUSHFD					; 指PUSH EFLAGS
		POP		EAX
		RET

_io_store_eflags:				; void io_store_eflags(int eflags);
		MOV		EAX,[ESP+4]
		PUSH	EAX
		POPFD					; 指POP EFLAGS
		RET

_load_gdtr:						; void load_gdtr(int limit, int addr);
		MOV		AX,[ESP+4]		; limit
		MOV		[ESP+6],AX
		LGDT	[ESP+6]
		RET

_load_idtr:						; void load_idtr(int limit, int addr);
		MOV		AX,[ESP+4]		; limit
		MOV		[ESP+6],AX
		LIDT	[ESP+6]
		RET
		
_load_cr0:						; int load_cr0(void);
		MOV		EAX,CR0
		RET

_store_cr0:						; void store_cr0(int cr0);
		MOV		EAX,[ESP+4]
		MOV		CR0,EAX
		RET

_load_tr:		; void load_tr(int tr);
		LTR		[ESP+4]			; tr
		RET

_asm_inthandler20:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler20
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD

_asm_inthandler21:
		PUSH	ES
		PUSH	DS
		PUSHAD								; 这一句的功能相当于PUSH EAX,ECX,EDX,EBX,ESP,EBP,ESI,EDI，下面的POPAD同理，只是顺序相反
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler21				; 这里调用了C语言写的 void inthandler21(int *esp); (keyboard.c)
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD

_asm_inthandler27:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler27
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD

_asm_inthandler2c:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler2c
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD

_asm_inthandler0c:
		STI
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler0c
		CMP		EAX,0
		JNE		_asm_end_app
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		ADD		ESP,4
		IRETD

_asm_inthandler0d:						; 异常中断
		STI
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler0d
		CMP		EAX,0
		JNE		_asm_end_app
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		ADD		ESP,4					; INT 0x0d 中需要
		IRETD

_memtest_sub:							; unsigned int memtest_sub(unsigned int start, unsigned int end)
		PUSH	EDI						; （还要使用EBX, ESI, EDI）
		PUSH	ESI
		PUSH	EBX
		MOV		ESI,0xaa55aa55			; pat0 = 0xaa55aa55;
		MOV		EDI,0x55aa55aa			; pat1 = 0x55aa55aa;
		MOV		EAX,[ESP+12+4]			; i = start;
mts_loop:
		MOV		EBX,EAX
		ADD		EBX,0xffc				; p = i + 0xffc;
		MOV		EDX,[EBX]				; old = *p;
		MOV		[EBX],ESI				; *p = pat0;
		XOR		DWORD [EBX],0xffffffff	; *p ^= 0xffffffff;
		CMP		EDI,[EBX]				; if (*p != pat1) goto fin;
		JNE		mts_fin
		XOR		DWORD [EBX],0xffffffff	; *p ^= 0xffffffff;
		CMP		ESI,[EBX]				; if (*p != pat0) goto fin;
		JNE		mts_fin
		MOV		[EBX],EDX				; *p = old;
		ADD		EAX,0x1000				; i += 0x1000;
		CMP		EAX,[ESP+12+8]			; if (i <= end) goto mts_loop;
		JBE		mts_loop
		POP		EBX
		POP		ESI
		POP		EDI
		RET
mts_fin:
		MOV		[EBX],EDX				; *p = old;
		POP		EBX
		POP		ESI
		POP		EDI
		RET
		
_farjmp:								; void farjmp(int eip, int cs);
		JMP		FAR	[ESP+4]				; eip, cs
		RET

_farcall:								; void farcall(int eip, int cs);
		CALL	FAR	[ESP+4]				; eip, cs
		RET

_asm_kal_api:
		STI
		PUSH	DS
		PUSH	ES
		PUSHAD		; 用于保存的PUSH
		PUSHAD		; 用于向kal_api传值的PUSH
		MOV		AX,SS
		MOV		DS,AX		; 将操作系统用段地址存入DS和SS
		MOV		ES,AX
		CALL	_kal_api
		CMP		EAX,0		; 当EAX不为0时程序结束
		JNE		_asm_end_app
		ADD		ESP,32
		POPAD
		POP		ES
		POP		DS
		IRETD
		
_asm_end_app:
		MOV		ESP,[EAX]
		MOV		DWORD [EAX+4],0
		POPAD
		RET

_start_app:								; void start_app(int eip, int cs, int esp, int ds, int *tss_esp0);
		PUSHAD							; 32位寄存器的值全部保存下来
		MOV		EAX,[ESP+36]			; 应用程序用的EIP
		MOV		ECX,[ESP+40]			; 应用程序用的CS
		MOV		EDX,[ESP+44]			; 应用程序用的ESP
		MOV		EBX,[ESP+48]			; 应用程序用的DS/SS
		MOV		EBP,[ESP+52]			; tss.esp0的番地
		MOV		[EBP  ],ESP				; 保存操作系统用的ESP
		MOV		[EBP+4],SS				; 保存操作系统用的SS
		MOV		ES,BX
		MOV		DS,BX
		MOV		FS,BX
		MOV		GS,BX
;	下面调整栈，避免用RETF跳转到应用程序
		OR		ECX,3					; 应用程序用的段号和3进行OR运算
		OR		EBX,3					; 应用程序用的段号和3进行OR运算
		PUSH	EBX						; 应用程序的SS
		PUSH	EDX						; 应用程序的ESP
		PUSH	ECX						; 应用程序的CS
		PUSH	EAX						; 应用程序的EIP
		RETF
;	应用程序结束后不会回到这里

_asm_shutdown:							; 关机
		JMP		start2
		db 0x00, 0x00
protect16:
		db 0xb8, 0x08, 0x00, 0x8e, 0xd8, 0x8e, 0xc0, 0x8e, 0xd0
		db 0x0f, 0x20, 0xc0, 0x66, 0x25, 0xfe,0xff,0xff, 0x7f
		db 0x0f, 0x22, 0xc0
		db 0xea
		dw 0x0650,0x0000
		ALIGNB 16
		protect16_len EQU	$ - protect16
realmod:
		db 0x8c, 0xc8
		db 0x8e, 0xd8
		db 0x8e, 0xc0
		db 0x8e, 0xd0
		db 0xbc, 0x00, 0x08
		db 0xe4, 0x92
		db 0x24, 0xfd
		db 0xe6, 0x92
		db 0x90, 0x90, 0x90
		db 0xfb, 0x90
		db 0xb8, 0x03, 0x00
		db 0xcd, 0x10
		;db 0xf4
		db 0xb8, 0x07, 0x53
		db 0xbb, 0x01, 0x00
		db 0xb9, 0x03, 0x00
		db 0xcd, 0x15
		ALIGNB 16
		realmod_len	EQU		$ - realmod
GDTIDT:
		dw 0x0000, 0x0000, 0x0000, 0x0000
		dw 0xffff, 0x0000, 0x9200, 0x0000
		dw 0xffff, 0x0000, 0x9800, 0x0000
		dw 0x0000
		dw 0x0017
		dw 0x0600, 0x0000
		dw 0x03ff
		dw 0x0000, 0x0000
		ALIGNB 16
		GDTIDT_lenth EQU	$ - GDTIDT
start2:
		MOV		EBX, GDTIDT
		MOV		EDX, 0x600
		MOV		CX, GDTIDT_lenth
.loop1:
		MOV		AL, [CS:EBX]
		MOV		[EDX], AL
		INC		EBX
		INC		EDX
		loop	.loop1
		MOV		EBX, protect16
		MOV		EDX, 0x630
		MOV		CX, protect16_len
.loop2:
		MOV		AL, [CS:EBX]
		MOV		[EDX], AL
		INC		EBX
		INC		EDX
		loop	.loop2	

		MOV		EBX, realmod
		MOV		EDX, 0x650
		MOV		CX, realmod_len
.loop3:
		MOV		AL, [CS:EBX]
		MOV		[EDX], AL
		INC		EBX
		INC		EDX
		loop	.loop3	
	
		LGDT	[0x061A]
		LIDT	[0x0620]
		JMP		2*8:0x0630
