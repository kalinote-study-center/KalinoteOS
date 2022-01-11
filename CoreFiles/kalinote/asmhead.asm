; kalinote-os
; TAB=4

VBEMODE	EQU		0x118			; 1024x768x32bitɫ��
BOTPAK	EQU		0x00280000		; bootpack��װλ��
DSKCAC	EQU		0x00100000		; ���̻���λ��
DSKCAC0	EQU		0x00008000		; ���̻���λ�ã�ʵʱģʽ��

; ɫ�ʴ��룺
;	0x100 :  640 x  400 x 8bit
;	0x101 :  640 x  480 x 8bit
;	0x103 :  800 x  600 x 8bit
;	0x105 : 1024 x  768 x 8bit
;	0x107 : 1280 x 1024 x 8bit(qemu�޷�ʹ��,VMware����)
;	0x0112	:	640x480	x32bit	;��������32λɫģʽ
;	0x0115	:	800x600	x32bit
;	0x0118	:	1024x768x32bit
;	����ɫ��ģʽ����ȥVESA��

; BOOT_INFO
CYLS	EQU		0x0ff0			; �趨������
LEDS	EQU		0x0ff1
VMODE	EQU		0x0ff2			; ��ɫ��λ��
SCRNX	EQU		0x0ff4			; �ֱ���X
SCRNY	EQU		0x0ff6			; �ֱ���Y
VRAM	EQU		0x0ff8			; ͼ�񻺳�����ʼ��ַ

; VBE_INFO
VBESIGNATURE		EQU		0x1000
VBEVERSION			EQU		VBESIGNATURE + 4
OEMSTRINGPTR		EQU		VBESIGNATURE + 6
CAPABILITIES		EQU		VBESIGNATURE + 10
VIDEOMODEPTR		EQU		VBESIGNATURE + 14
TOTALMEMORY			EQU		VBESIGNATURE + 18
OEMSOFTWAREREV		EQU		VBESIGNATURE + 20
OEMVERDORNAMEPTR	EQU		VBESIGNATURE + 22
OEMPRODUCTNAMEPTR	EQU		VBESIGNATURE + 26
OEMPRODUCTREVPTR	EQU		VBESIGNATURE + 30
RESERVED			EQU		VBESIGNATURE + 34

		ORG		0xc200			; ����װ�ص��ڴ�0xc200��λ��

; -----------------------------------��������￪ʼ-----------------------------------
; ȷ��VBE�Ƿ����
		MOV		AX,0x9000
		MOV		ES,AX
		MOV		DI,0
		MOV		AX,0x4f00
		INT		0x10
		CMP		AX,0x004f
		JNE		scrn320

; ���VBE�汾
		MOV		AX,[ES:DI+4]
		CMP		AX,0x0200
		JB		scrn320			; if (AX < 0x0200) goto scrn320
		; ��¼VBE��Ϣ
		MOV		EAX, [ES:DI]
		MOV		[VBESIGNATURE], EAX
		; TODO�����VBE��Ϣ��ȡ
		

; ȡ�û���ģʽ��Ϣ
		MOV		CX,VBEMODE
		MOV		AX,0x4f01
		INT		0x10
;		CMP		AX,0x004f
;		JNE		scrn320

; ����ģʽ��Ϣȷ��
;		CMP		BYTE [ES:DI+0x19],8
;		JNE		scrn320
;		CMP		BYTE [ES:DI+0x1b],4
;		JNE		scrn320
;		MOV		AX,[ES:DI+0x00]
;		AND		AX,0x0080
;		JZ		scrn320			; ģʽ���Ե�bit7��0�����Է���

; ��ȡSVGA��Ϣ


; ����ģʽ���л�
		MOV		BX,VBEMODE+0x4000
		MOV		AX,0x4f02
		INT		0x10
		MOV		BYTE [VMODE],32	; ��¼����ģʽ
		MOV		AX,[ES:DI+0x12]
		MOV		[SCRNX],AX
		MOV		AX,[ES:DI+0x14]
		MOV		[SCRNY],AX
		MOV		EAX,[ES:DI+0x28]
		MOV		[VRAM],EAX
		JMP		keystatus

scrn320:
		MOV		AL,0x13			; VGAͼ�Ρ�1024x768x32bitɫ��
		MOV		AH,0x00
		INT		0x10
		MOV		BYTE [VMODE],32	; ��¼����ģʽ
		MOV		WORD [SCRNX],1024
		MOV		WORD [SCRNY],768
		MOV		DWORD [VRAM],0x000a0000

; ��BIOSȡ�ü����ϸ���LED��״̬
keystatus:
		MOV		AH,0x02
		INT		0x16 			; keyboard BIOS
		MOV		[LEDS],AL

; -----------------------------------�л���32λ-----------------------------------
; �����￪ʼ����32λ
; ��osdev.org�У������л����ܱ���ģʽ(Protected Mode)֮ǰ�������²��裺
; �����жϣ�����NMI����Ӣ�ض�������Ա�ֲὨ�飩��
; ����A20 line��
; ���������ڴ��롢���ݺͶ�ջ�Ķ�������"ȫ����������(GDT)"��

; PIC�ر������ж�
; ����AT���ݻ��Ĺ�����Ҫ��ʼ��PIC��
; ������CLI֮ǰ���У��������»����
; ������PIC��ʼ��
; ���ܵ�ͬ��(C����)
; io_out(PIC0_IMR, 0xff);	/* ��ֹ��PIC�ж� */
; io_out(PIC1_IMR, 0xff);	/* ��ֹ��PIC�ж� */
; io_cli();					/* ��ֹCPU�ж� */
		MOV		AL,0xff
		OUT		0x21,AL			; ��ֹ��PIC�ж�(������Բο�PIC�жϿ������������)
		NOP						; ͣ��һ�£���ֹBUG(�������������˵û��̫��ı�Ҫ�� )
		OUT		0xa1,AL			; ��ֹ��PIC�ж�

		CLI						; ��ֹCPU�ж�

; Ϊ���ܹ���CPU����1MB���ϵĴ洢�����趨A20GATE
; ���ܵ�ͬ��(C����)
; #define KEYCMD_WRITE_OUTPORT		0xd1
; #define KBC_OUTPORT_A20G_ENABLE	0Xdf
; 
; wait_KBC_sendready();		/* keyboard.c */
; io_out(PORT_KEYCMD, KEYCMD_WRITE_OUTPORT);
; wait_KBC_sendready();
; io_out(PORT_KEYCMD, KBC_OUTPORT_A20G_ENABLE);
; wait_KBC_sendready();		/* wait_KBC_sendready�Ƕ���ģ���������Ϊ�˵ȴ�A20GATE������� */
		CALL	waitkbdout		; ������wait_KBC_sendready��ͬ(�ȴ����̿��Ƶ�·׼����� P.140)
		MOV		AL,0xd1
		OUT		0x64,AL
		CALL	waitkbdout
		MOV		AL,0xdf			; ���� A20GATE,ʹCPU��1M���ϲ��ֵ��ڴ��Ϊ����״̬
		OUT		0x60,AL
		CALL	waitkbdout

; ת�򱣻�ģʽ
; CR0�Ĵ����к��п��ƴ���������ģʽ��״̬��ϵͳ���Ʊ�־
		LGDT	[GDTR0]			; ����ʱ32λGDT���ص�GDTR�Ĵ���
		MOV		EAX,CR0			; Ϊ������CR0�Ĵ�����ֵ
		; AND		EAX,0x7fffffff	; ��bit31(���λ)��Ϊ0��Ϊ�˽�ֹ��ҳ��
		OR		EAX,0x00000001	; ʹbit0(���λ)Ϊ1(Ϊ���л�������ģʽ)
		MOV		CR0,EAX
		JMP		pipelineflush
; λ�ڴ˴���jmp
; CPUͨ������CR0���뱣��ģʽ����Ҫ����ִ����תָ��(P.155)
pipelineflush:
		MOV		AX,1*8			; �ɶ�д�Ķ� 32bit
		MOV		DS,AX
		MOV		ES,AX
		MOV		FS,AX
		MOV		GS,AX
		MOV		SS,AX

; bootpack�Ĵ���
; ��bootpack���͵���Ӧ���ڴ�λ��
		MOV		ESI,bootpack	; ����Դ
		MOV		EDI,BOTPAK		; ����Ŀ�ĵ�
		MOV		ECX,512*1024/4	; �������ݴ�С
		CALL	memcpy
		
; ˳��������Ҳ��ԭ����λ�ô���

; ���ȴ���������
; ����һ�γ������˼�Ǵ�0x7c00����512�ֽڵ�DSKCAC(0x00100000)����˫�ֽ�Ϊ��λ�����ƴ���Ϊ512/4
; 0x00100000��1M�Ժ���ڴ�
		MOV		ESI,0x7c00		; ����Դ
		MOV		EDI,DSKCAC		; ����Ŀ�ĵ�
		MOV		ECX,512/4		; �������ݴ�С
		CALL	memcpy

; ʣ�µ�ȫ��

		MOV		ESI,DSKCAC0+512	; ����Դ
		MOV		EDI,DSKCAC+512	; ����Ŀ�ĵ�
		MOV		ECX,0
		MOV		CL,BYTE [CYLS]
		IMUL	ECX,512*18*2/4
		SUB		ECX,512/4		; ֻ��ȥIPL�Ĳ���
		CALL	memcpy

; asmhead�����������鶼������
; ʣ�µľͽ���bootpack��

; ����bootpack

		MOV		EBX,BOTPAK
		MOV		ECX,[EBX+16]
		ADD		ECX,3			; ECX += 3;
		SHR		ECX,2			; ECX /= 4;		SHR������ָ��
		JZ		skip			; û�п�ת���Ķ���
		MOV		ESI,[EBX+20]	; ����Դ
		ADD		ESI,EBX
		MOV		EDI,[EBX+12]	; ����Դ
		CALL	memcpy
skip:
		MOV		ESP,[EBX+12]	; ��ջ��ʼֵ
		JMP		DWORD 2*8:0x0000001b	;KaliMain��ڵĵ�ַ�������￪ʼ����C����		;�±����ڵ�ַΪ0x00000027

waitkbdout:
		IN		 AL,0x64
		AND		 AL,0x02
		IN		 AL,0x60		; �ն���Ϊ����ջ����е���������
		JNZ		waitkbdout		; ���AND�Ľ������0������waitkbdout
		RET

memcpy:
		; ����������ڸ����ڴ�
		; ESI�Ǹ���Դ��ַ
		; EDI�Ǹ���Ŀ���ַ
		; ECX�Ǹ������ݴ�С(�������ݴ�С����dwordΪ��λ[32λ]���������ֽ�������4)
		MOV		EAX,[ESI]
		ADD		ESI,4
		MOV		[EDI],EAX
		ADD		EDI,4
		SUB		ECX,1
		JNZ		memcpy			; ��������Ľ������0�������memcpy
		RET
; memcpy��������Ǽ����ַ���ƣ�Ҳ����д������

		ALIGNB	16, DB 0		; ����0��ֱ���ܱ�16��������ͬ
		
; -----------------------------------32λ��ʱGDT��-----------------------------------
GDT0:
; λ����������GDT��
		; RESB	8
		TIMES	8 DB 0			; ѡ����
		; д��8�ֽڵ�GDT��
		DW		0xffff,0x0000,0x9200,0x00cf	; �ɶ�����32bit					; CPUʹ��
		DW		0xffff,0x0000,0x9a28,0x0047	; ��ִ�ж�32bit(����bootpack)	; bootpack.kalʹ��
		; ��һ����ʵ���������������(kernel/dsctbl.c)��
		; set_segmdesc(gdt + 1, 0xffffffff,   0x00000000, AR_DATA32_RW_R0);
		; set_segmdesc(gdt + 2, LIMIT_BOTPAK, ADR_BOTPAK, AR_CODE32_ER_R0);

		DW		0
GDTR0:
; ������дһ����ʱ��GDT
		DW		8*3-1		; GDT�������
		DD		GDT0		; GDT��ĵ�ַ

		; ALIGNB	16
		ALIGN 16, DB 0
bootpack: