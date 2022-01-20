; naskfunc
; TAB=4
; 这个文件的存在是为了解决C语言不能解决的问题

[BITS 32]						; 制作32位模式用的机器语言
		
		GLOBAL	_io_hlt, _io_cli, _io_sti, _io_stihlt
		GLOBAL	_io_in8,  _io_in16,  _io_in32
		GLOBAL	_io_out8, _io_out16, _io_out32
		GLOBAL	_io_load_eflags, _io_store_eflags
		GLOBAL	_port_read, _port_write
		GLOBAL	_load_gdtr, _load_idtr
		GLOBAL	_load_cr0, _store_cr0
		GLOBAL	_load_tr
		GLOBAL  _clts, _io_fnsave, _io_frstor
		GLOBAL	_check_cpuid, _read_cpuid
		GLOBAL	_fwait
		GLOBAL	_asm_inthandler_divzero
		GLOBAL	_asm_inthandler_device_not_available
		GLOBAL	_asm_inthandler_timer, _asm_inthandler21
		GLOBAL	_asm_inthandler26
		GLOBAL	_asm_inthandler27, _asm_inthandler2c
		GLOBAL	_asm_inthandler_stack_segment, _asm_inthandler_general_protection
		GLOBAL	_asm_end_app, _memtest_sub
		GLOBAL	_farjmp, _farcall
		GLOBAL	_asm_kal_api, _start_app
		EXTERN  _inthandler_divzero
		EXTERN	_inthandler_device_not_available
		EXTERN	_inthandler_timer, _inthandler21
		EXTERN	_inthandler26
		EXTERN	_inthandler27, _inthandler2c
		EXTERN	_inthandler_stack_segment, _inthandler_general_protection
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

_port_read:						; void port_read(unsigned short port, void* buf, int n);
		MOV	EDX, [ESP + 4]			; port
		MOV	EDI, [ESP + 8]			; buf
		MOV	ECX, [ESP + 12]			; n
		SHR	ECX, 1
		CLD
		REP	INSW
		RET

_port_write:					; void port_write(unsigned short port, void* buf, int n);
		MOV	EDX, [ESP + 4]			; port
		MOV	ESI, [ESP + 8]			; buf
		MOV	ECX, [ESP + 12]			; n
		SHR	ECX, 1
		CLD
		REP	OUTSW
		RET

_load_gdtr:						; void load_gdtr(int limit, int addr);
		MOV		AX,[ESP+4]		; 段上限(limit，GDT有效字节数-1)
		MOV		[ESP+6],AX
		LGDT	[ESP+6]			; 将6字节数据写入GDTR寄存器，GDTR的低16位为段上限(limit，GDT有效字节数-1)，高32位为GDT开始的地址
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

_clts:          ; void clts(void);
        CLTS
        RET

_io_fnsave:        ; void io_fnsave(int *addr);
        MOV     EAX,[ESP+4]     ; addr
        FNSAVE  [EAX]
        RET

_io_frstor:        ; void io_frstor(int *addr);
        MOV     EAX,[ESP+4]     ; addr
        FRSTOR  [EAX]
        RET

_check_cpuid:	; int check_cpuid(void);		/* 检查CPUID是否可用 */
		PUSHFD                               	; 保存 EFLAGS
		PUSHFD                               	; 储存 EFLAGS
		XOR DWORD [ESP],0x00200000           	; 反转存储的EFLAG中的ID位
		POPFD                                	; 加载存储的EFLAG（ID位反转）
		PUSHFD                               	; 再次储存 EFLAGS (ID位可能反转，也可能不反转)
		POP EAX                              	; eax = 修改的 EFLAGS (ID位可能反转，也可能不反转)
		XOR EAX,[ESP]                        	; eax = 无论哪一位被改变
		POPFD                                	; 恢复原始EFLAGS
		AND EAX,0x00200000                   	; eax = 如果无法更改ID位，则为零，否则为非零
		RET

_read_cpuid:	; int read_cpuid(int code, int *ebx, int *edx, int *ecx);	/* 从CPUID获取信息 */
		MOV EAX,[ESP+4]
		PUSH EBX
		CPUID									; 数据存在EBX,EDX,ECX中
		PUSH ESI
		MOV ESI,[ESP+16]
		MOV [ESI],EBX
		MOV ESI,[ESP+20]
		MOV [ESI],EDX
		MOV ESI,[ESP+24]
		MOV [ESI],ECX
		POP ESI
		POP EBX
		RET

_fwait:			; void fwait(void);
		FWAIT
		RET

_asm_inthandler_divzero:				; 除零异常
        STI
        PUSH    ES
        PUSH    DS
        PUSHAD
        MOV     EAX,ESP
        PUSH    EAX
        MOV     AX,SS
        MOV     DS,AX
        MOV     ES,AX
        CALL    _inthandler_divzero
        CMP     EAX,0
        JNE     _asm_end_app
        POP     EAX
        POPAD
        POP     DS
        POP     ES
        IRETD

_asm_inthandler_device_not_available:				; FPU异常中断
        STI
        PUSH    ES
        PUSH    DS
        PUSHAD
        MOV     EAX,ESP
        PUSH    EAX
        MOV     AX,SS
        MOV     DS,AX
        MOV     ES,AX
        CALL    _inthandler_device_not_available
        CMP     EAX,0
        JNE     _asm_end_app
        POP     EAX
        POPAD
        POP     DS
        POP     ES
        IRETD                   ; INT07中ESP+=4；不需要

_asm_inthandler_timer:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler_timer
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

_asm_inthandler26:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler26
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

_asm_inthandler_stack_segment:
		STI
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler_stack_segment
		CMP		EAX,0
		JNE		_asm_end_app
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		ADD		ESP,4
		IRETD

_asm_inthandler_general_protection:						; 异常中断
		STI
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler_general_protection
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

_asm_kal_api:							; 系统调用入口
		STI
		PUSH	DS
		PUSH	ES
		PUSHAD		; 用于保存的PUSH
		PUSHAD		; 用于向kal_api传值的PUSH
		MOV		AX,SS
		MOV		DS,AX		; 将操作系统用段地址存入DS和SS
		MOV		ES,AX
		CALL	_kal_api
		CMP		EAX,0		; 当EAX(返回值)不为0时程序结束，并将返回值当作task->tss.esp0来处理
		JNE		_asm_end_app
		ADD		ESP,32
		POPAD
		POP		ES
		POP		DS
.ret_from_kal_api:			; 从API返回后执行信号处理(do_signal)
		; 这里还没搞懂linux是怎么处理的，还要再学习一个
		IRETD
		
_asm_end_app:
		MOV		ESP,[EAX]
		MOV		DWORD [EAX+4],0
		POPAD
		RET			; 这里返回cmd_app

_start_app:								; void start_app(int eip, int cs, int esp, int ds, int *tss_esp0);
		PUSHAD							; 32位寄存器的值全部保存下来
		MOV		EAX,[ESP+36]			; 应用程序用的EIP
		MOV		ECX,[ESP+40]			; 应用程序用的CS
		MOV		EDX,[ESP+44]			; 应用程序用的ESP
		MOV		EBX,[ESP+48]			; 应用程序用的DS/SS
		MOV		EBP,[ESP+52]			; tss.esp0的地址
		MOV		[EBP  ],ESP				; 保存操作系统用的ESP
		MOV		[EBP+4],SS				; 保存操作系统用的SS
		MOV		ES,BX
		MOV		DS,BX
		MOV		FS,BX
		MOV		GS,BX
;	下面的操作是先把数据存入栈中，然后调用RETF跳转到应用程序执行
		OR		ECX,3					; 应用程序用的段号和3进行OR运算(一个使用RETF跳转的小技巧)
		OR		EBX,3					; 应用程序用的段号和3进行OR运算
		PUSH	EBX						; 应用程序的SS
		PUSH	EDX						; 应用程序的ESP
		PUSH	ECX						; 应用程序的CS
		PUSH	EAX						; 应用程序的EIP
		RETF
;	应用程序结束后不会回到这里
