; kalinote-os
; TAB=4

[INSTRSET "i486p"]

VBEMODE	EQU		0x105			; 1024x768x8bitɫ��
BOTPAK	EQU		0x00280000		; bootpack��װλ��
DSKCAC	EQU		0x00100000		; ���̻���λ��
DSKCAC0	EQU		0x00008000		; ���̻���λ�ã�ʵʱģʽ��

; ɫ�ʴ��룺
;	0x100 :  640 x  400 x 8bit
;	0x101 :  640 x  480 x 8bit
;	0x103 :  800 x  600 x 8bit
;	0x105 : 1024 x  768 x 8bit
;	0x107 : 1280 x 1024 x 8bit(qemu�޷�ʹ��,VMware����)
;	����ɫ��ģʽ����ȥVESA��

; BOOT_INFO
CYLS	EQU		0x0ff0			; �趨������
LEDS	EQU		0x0ff1
VMODE	EQU		0x0ff2			; ��ɫ��λ��
SCRNX	EQU		0x0ff4			; �ֱ���X
SCRNY	EQU		0x0ff6			; �ֱ���Y
VRAM	EQU		0x0ff8			; ͼ�񻺳�����ʼ��ַ

		ORG		0xc200			; ����װ�ص��ڴ�0xc200��λ��

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

; ȡ�û���ģʽ��Ϣ
		MOV		CX,VBEMODE
		MOV		AX,0x4f01
		INT		0x10
		CMP		AX,0x004f
		JNE		scrn320

; ����ģʽ��Ϣȷ��
		CMP		BYTE [ES:DI+0x19],8
		JNE		scrn320
		CMP		BYTE [ES:DI+0x1b],4
		JNE		scrn320
		MOV		AX,[ES:DI+0x00]
		AND		AX,0x0080
		JZ		scrn320			; ģʽ���Ե�bit7��0�����Է���

; ����ģʽ���л�
		MOV		BX,VBEMODE+0x4000
		MOV		AX,0x4f02
		INT		0x10
		MOV		BYTE [VMODE],8	; ��¼����ģʽ
		MOV		AX,[ES:DI+0x12]
		MOV		[SCRNX],AX
		MOV		AX,[ES:DI+0x14]
		MOV		[SCRNY],AX
		MOV		EAX,[ES:DI+0x28]
		MOV		[VRAM],EAX
		JMP		keystatus

scrn320:
		MOV		AL,0x13			; VGAͼ�Ρ�1024x768x8bitɫ��
		MOV		AH,0x00
		INT		0x10
		MOV		BYTE [VMODE],8	; ��¼����ģʽ
		MOV		WORD [SCRNX],1024
		MOV		WORD [SCRNY],768
		MOV		DWORD [VRAM],0x000a0000

; ��BIOSȡ�ü����ϸ���LED��״̬
keystatus:
		MOV		AH,0x02
		INT		0x16 			; keyboard BIOS
		MOV		[LEDS],AL

��PIC�ر������ж�
������AT���ݻ��Ĺ�����Ҫ��ʼ��PIC��
��������CLI֮ǰ���У��������»����
��������PIC��ʼ��

		MOV		AL,0xff
		OUT		0x21,AL
		NOP						; ͣ��һ�£���ֹBUG(����˵ĳЩ�����������UOT������)
		OUT		0xa1,AL

		CLI						; ��ֹCPU�����ж�

; Ϊ���ܹ���CPU����1MB���ϵĴ洢�����趨A20GATE

		CALL	waitkbdout
		MOV		AL,0xd1
		OUT		0x64,AL
		CALL	waitkbdout
		MOV		AL,0xdf			; enable A20
		OUT		0x60,AL
		CALL	waitkbdout

; ת�򱣻�ģʽ

		LGDT	[GDTR0]			; �趨�ݶ�GDT
		MOV		EAX,CR0
		AND		EAX,0x7fffffff	; ��bit31��Ϊ0��Ϊ�˽�ֹѰ����
		OR		EAX,0x00000001	; ʹbit0Ϊ1(Ϊ�˱���ģʽת��)
		MOV		CR0,EAX
		JMP		pipelineflush
pipelineflush:
		MOV		AX,1*8			; �ɶ�����32bit
		MOV		DS,AX
		MOV		ES,AX
		MOV		FS,AX
		MOV		GS,AX
		MOV		SS,AX
		
; bootpack�Ĵ���

		MOV		ESI,bootpack	; ����Դ
		MOV		EDI,BOTPAK		; ����Դ
		MOV		ECX,512*1024/4
		CALL	memcpy
		
; ˳��������Ҳ��ԭ����λ�ô���

; ���ȴ���������

		MOV		ESI,0x7c00		; ����Դ
		MOV		EDI,DSKCAC		; ����Դ
		MOV		ECX,512/4
		CALL	memcpy
		
; ʣ�µ�ȫ��

		MOV		ESI,DSKCAC0+512	; ����Դ
		MOV		EDI,DSKCAC+512	; ����Դ
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
		SHR		ECX,2			; ECX /= 4;
		JZ		skip			; û�п�ת���Ķ���
		MOV		ESI,[EBX+20]	; ����Դ
		ADD		ESI,EBX
		MOV		EDI,[EBX+12]	; ����Դ
		CALL	memcpy
skip:
		MOV		ESP,[EBX+12]	; ��ջ��ʼֵ
		JMP		DWORD 2*8:0x0000001b

waitkbdout:
		IN		 AL,0x64
		AND		 AL,0x02
		JNZ		waitkbdout		; ���AND�Ľ������0������waitkbdout
		RET

memcpy:
		MOV		EAX,[ESI]
		ADD		ESI,4
		MOV		[EDI],EAX
		ADD		EDI,4
		SUB		ECX,1
		JNZ		memcpy			; ��������Ľ������0�������memcpy
		RET
; memcpy��������Ǽ����ַ���ƣ�Ҳ����д������

		ALIGNB	16
GDT0:
		RESB	8				; ѡ����
		DW		0xffff,0x0000,0x9200,0x00cf	; �ɶ�����32bit
		DW		0xffff,0x0000,0x9a28,0x0047	; ��ִ�ж�32bit(����bootpack)

		DW		0
GDTR0:
		DW		8*3-1
		DD		GDT0

		ALIGNB	16
bootpack:
