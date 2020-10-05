; kalinote-os
; TAB=4

BOTPAK	EQU		0x00280000		; bootpack��װλ��
DSKCAC	EQU		0x00100000		; ���̻���λ��
DSKCAC0	EQU		0x00008000		; ���̻���λ�ã�ʵʱģʽ��

; BOOT_INFO
CYLS	EQU		0x0ff0			; �趨������
LEDS	EQU		0x0ff1
VMODE	EQU		0x0ff2			; ��ɫ��λ��
SCRNX	EQU		0x0ff4			; �ֱ���X
SCRNY	EQU		0x0ff6			; �ֱ���Y
VRAM	EQU		0x0ff8			; ͼ�񻺳�����ʼ��ַ

		ORG		0xc200			; ����װ�ص��ڴ�0xc200��λ��
		
; �趨����ģʽ
		
		MOV		AL,0x13			; VGA�Կ�,320x200x8λ��ɫ
		MOV		AH,0x00
		INT		0x10
		MOV		BYTE [VMODE],8	; ��¼����ģʽ
		MOV		WORD [SCRNX],320
		MOV		WORD [SCRNY],200
		MOV		DWORD [VRAM],0x000a0000
		
; ��BIOSȡ�ü����ϸ���LED��״̬

		MOV		AH,0x02
		INT		0x16 			; keyboard BIOS
		MOV		[LEDS],AL
		
����ֹPIC�����κ��ж�
����AT���ݻ��Ĺ���У����Ҫ��ʼ��PIC
���������CLI֮ǰ���Ļ���ż��������
��PIC�ĳ�ʼ���Ժ����

		MOV		AL,0xff
		OUT		0x21,AL
		NOP						; ͣ��һ�£���ֹBUG(����˵ĳЩ�����������UOT������)
		OUT		0xa1,AL

		CLI						; ����CPU����Ҳ��ֹ
		
; Ϊ���ܹ���CPU����1MB���ϵĴ洢�����趨A20GATE

		CALL	waitkbdout
		MOV		AL,0xd1
		OUT		0x64,AL
		CALL	waitkbdout
		MOV		AL,0xdf			; enable A20
		OUT		0x60,AL
		CALL	waitkbdout
		
; ת�򱣻�ģʽ

[INSTRSET "i486p"]

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
		IMUL	ECX,512*18*2/4	; ������������Х�����/4�ˉ�Q
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
