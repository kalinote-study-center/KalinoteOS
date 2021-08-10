; naskfunc
; TAB=4
; ����ļ��Ĵ�����Ϊ�˽��C���Բ��ܽ��������

[FORMAT "WCOFF"]				; ����Ŀ���ļ���ģʽ
[INSTRSET "i486p"]				; ��486�õ�ָ��
[BITS 32]						; ����32λģʽ�õĻ�������
[FILE "naskfunc.nas"]			; Դ�ļ�����Ϣ
		
		GLOBAL	_io_hlt, _io_cli, _io_sti, _io_stihlt
		GLOBAL	_io_in8,  _io_in16,  _io_in32
		GLOBAL	_io_out8, _io_out16, _io_out32
		GLOBAL	_port_read, _port_write
		GLOBAL	_io_load_eflags, _io_store_eflags
		GLOBAL	_load_gdtr, _load_idtr
		GLOBAL	_load_cr0, _store_cr0
		GLOBAL	_load_tr
		GLOBAL  _clts, _fnsave, _frstor
		GLOBAL	_asm_inthandler00
		GLOBAL	_asm_inthandler07
		GLOBAL	_asm_inthandler20, _asm_inthandler21
		GLOBAL	_asm_inthandler27, _asm_inthandler2c
		GLOBAL	_asm_inthandler0c, _asm_inthandler0d
		GLOBAL	_asm_end_app, _memtest_sub
		GLOBAL	_farjmp, _farcall
		GLOBAL	_asm_kal_api, _start_app
		EXTERN  _inthandler00
		EXTERN	_inthandler07
		EXTERN	_inthandler20, _inthandler21
		EXTERN	_inthandler27, _inthandler2c
		EXTERN	_inthandler0c, _inthandler0d
		EXTERN	_kal_api

[SECTION .text]					; Ŀ���ļ���д����Щ֮����д����

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
		PUSHFD					; ָPUSH EFLAGS
		POP		EAX
		RET

_io_store_eflags:				; void io_store_eflags(int eflags);
		MOV		EAX,[ESP+4]
		PUSH	EAX
		POPFD					; ָPOP EFLAGS
		RET

_port_read:						; void port_read(unsigned short port, void* buf, int n);
	mov	edx, [esp + 4]			; port
	mov	edi, [esp + 8]			; buf
	mov	ecx, [esp + 12]			; n
	shr	ecx, 1
	cld
	rep	insw
	ret

_port_write:					; void port_write(unsigned short port, void* buf, int n);
	mov	edx, [esp + 4]			; port
	mov	esi, [esp + 8]			; buf
	mov	ecx, [esp + 12]			; n
	shr	ecx, 1
	cld
	rep	outsw
	ret

_load_gdtr:						; void load_gdtr(int limit, int addr);
		MOV		AX,[ESP+4]		; ������(limit��GDT��Ч�ֽ���-1)
		MOV		[ESP+6],AX
		LGDT	[ESP+6]			; ��6�ֽ�����д��GDTR�Ĵ�����GDTR�ĵ�16λΪ������(limit��GDT��Ч�ֽ���-1)����32λΪGDT��ʼ�ĵ�ַ
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

_fnsave:        ; void fnsave(int *addr);
        MOV     EAX,[ESP+4]     ; addr
        FNSAVE  [EAX]
        RET

_frstor:        ; void frstor(int *addr);
        MOV     EAX,[ESP+4]     ; addr
        FRSTOR  [EAX]
        RET

_asm_inthandler00:				; �����쳣
        STI
        PUSH    ES
        PUSH    DS
        PUSHAD
        MOV     EAX,ESP
        PUSH    EAX
        MOV     AX,SS
        MOV     DS,AX
        MOV     ES,AX
        CALL    _inthandler00
        CMP     EAX,0
        JNE     _asm_end_app
        POP     EAX
        POPAD
        POP     DS
        POP     ES
        IRETD

_asm_inthandler07:				; FPU�쳣�ж�
        STI
        PUSH    ES
        PUSH    DS
        PUSHAD
        MOV     EAX,ESP
        PUSH    EAX
        MOV     AX,SS
        MOV     DS,AX
        MOV     ES,AX
        CALL    _inthandler07
        CMP     EAX,0
        JNE     _asm_end_app
        POP     EAX
        POPAD
        POP     DS
        POP     ES
        IRETD                   ; INT07��ESP+=4������Ҫ

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
		PUSHAD								; ��һ��Ĺ����൱��PUSH EAX,ECX,EDX,EBX,ESP,EBP,ESI,EDI�������POPADͬ��ֻ��˳���෴
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler21				; ���������C����д�� void inthandler21(int *esp); (keyboard.c)
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

_asm_inthandler0d:						; �쳣�ж�
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
		ADD		ESP,4					; INT 0x0d ����Ҫ
		IRETD

_memtest_sub:							; unsigned int memtest_sub(unsigned int start, unsigned int end)
		PUSH	EDI						; ����Ҫʹ��EBX, ESI, EDI��
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
		PUSHAD		; ���ڱ����PUSH
		PUSHAD		; ������kal_api��ֵ��PUSH
		MOV		AX,SS
		MOV		DS,AX		; ������ϵͳ�öε�ַ����DS��SS
		MOV		ES,AX
		CALL	_kal_api
		CMP		EAX,0		; ��EAX��Ϊ0ʱ�������
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
		RET			; ���ﷵ��cmd_app

_start_app:								; void start_app(int eip, int cs, int esp, int ds, int *tss_esp0);
		PUSHAD							; 32λ�Ĵ�����ֵȫ����������
		MOV		EAX,[ESP+36]			; Ӧ�ó����õ�EIP
		MOV		ECX,[ESP+40]			; Ӧ�ó����õ�CS
		MOV		EDX,[ESP+44]			; Ӧ�ó����õ�ESP
		MOV		EBX,[ESP+48]			; Ӧ�ó����õ�DS/SS
		MOV		EBP,[ESP+52]			; tss.esp0�ķ���
		MOV		[EBP  ],ESP				; �������ϵͳ�õ�ESP
		MOV		[EBP+4],SS				; �������ϵͳ�õ�SS
		MOV		ES,BX
		MOV		DS,BX
		MOV		FS,BX
		MOV		GS,BX
;	����Ĳ������Ȱ����ݴ���ջ�У�Ȼ�����RETF��ת��Ӧ�ó���ִ��
		OR		ECX,3					; Ӧ�ó����õĶκź�3����OR����(һ��ʹ��RETF��ת��С����)
		OR		EBX,3					; Ӧ�ó����õĶκź�3����OR����
		PUSH	EBX						; Ӧ�ó����SS
		PUSH	EDX						; Ӧ�ó����ESP
		PUSH	ECX						; Ӧ�ó����CS
		PUSH	EAX						; Ӧ�ó����EIP
		RETF
;	Ӧ�ó�������󲻻�ص�����
