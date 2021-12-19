; Kalinote-ipl
; TAB=4

CYLS				EQU		40				; ��ȡ40������
BASEOFSTACK			EQU		0x7c00			; ΪSP(ջָ��Ĵ���)�ṩջ��ַ

		ORG		0x7c00			; ������ʼװ�ص�ַ

; �����Ǳ�׼FAT12��ʽ�����ô��룬�ο���ַ��https://blog.csdn.net/m0_37329910/article/details/88927673

		JMP SHORT entry
		DB		0x90
		DB		"KALIBOOT"		; ����������(8�ֽ�)
		DW		512				; ÿ��������СΪ512
		DB		1				; ÿ���ش�СΪһ������
		DW		1				; FAT��ʼλ��(boot��¼ռ��������)
		DB		2				; FAT����
		DW		224				; ��Ŀ¼����ļ���
		DW		2880			; ���̴�С(�����������������������Ϊ0�������������)
		DB		0xf0			; ��������
		DW		9				; FAT����(ÿ��FAT��ռ��������)
		DW		18				; ÿ���ŵ���������
		DW		2				; ��ͷ��(����)
		DD		0				; ��ʹ�÷���(����������)
		DD		2880			; ��дһ����̴�С(��������������Ϊ0�������������)
		DB		0,0,0x29		; ����λ�ֱ��ʾ��INT 13H���������š���������չ�������(29h)
		DD		0xffffffff		; �����к�
		DB		"KALINOTE-OS"	; ��������(11�ֽ�)
		DB		"FAT12   "		; ���̸�ʽ����(8�ֽ�)
		RESB	18				; ��18�ֽ�

; �������壬�����￪ʼִ��
entry:
		; ��ʼ���Ĵ���
		MOV		AX,CS
		MOV		SS,AX
		MOV		ES,AX
		MOV		DS,AX
		MOV		SP,BASEOFSTACK
		
; ��ʾ��Ϣ
		; ����
		MOV		AX, 0x0600			; 06�Ź���(AH)������(AL)
		MOV		BX, 0x0700			; ʹ����������ʱBX,CX,DX����ֵ��������
		MOV		CX, 0
		MOV		DX, 0x184f
		INT		0x10				; INT 10��AH 0x06�Ź��ܣ�ALΪ0ʱ����
		
		; �趨���λ��
		MOV		AX, 0x0200			; 02�Ź���(AH)���趨���λ��
		MOV		BX, 0x0000			; BHҳ��(0ҳ)
		MOV		DX, 0x0000			; DH������DL����(0��0��)
		INT		0x10				; INT 10��AH 0x02�Ź���Ϊ������Ļ���λ��
		
		; ������ʾ
		MOV		AX, 0x1301			; 12�Ź���(AH)����ʾ�ַ������ַ���������BL�ṩ��������CX�ṩ������ƶ���ĩβ(AL 01)
		MOV		BX, 0x000f			; BHΪҳ��(0ҳ)��BLΪ�ַ�����(0x0f=0b00001111,��ɫ,����,��ɫ����,����˸)
		MOV		DX, 0x0000			; �α�����
		MOV		CX, 0x0d			; �ַ�������Ϊ13
		PUSH	AX
		MOV		AX, DS
		MOV		ES, AX
		POP		AX
		MOV		BP, StartBootMsg
		INT		0x10				; INT 10��AH 0x12�Ź���Ϊ��ʾһ���ַ���
		
; ��ȡ����(�����������ļ������ڴ�)
read:	
		MOV		AX,0x0820		; װ�ص��ڴ�0x0820��
		MOV		ES,AX
		MOV		CH,0			; ����0
		MOV		DH,0			; ��ͷ0
		MOV		CL,2			; ����2
		MOV		BX,18*2*CYLS-1	; Ҫ��ȡ�ĺϼ�������
		CALL	readfast		; ���ٶ�ȡ

; ��ȡ����������kalinote.sys

		MOV		BYTE [0x0ff0],CYLS	; ��¼IPLʵ�ʶ�ȡ�˶�������
		JMP		0xc200


; -------------------------���̶�ȡ���ִ���-------------------------
error:
		MOV		AX,0
		MOV		ES,AX
		MOV		SI,msg
putloop:
		MOV		AL,[SI]
		ADD		SI,1			; SI��1
		CMP		AL,0
		JE		fin
		MOV		AH,0x0e			; ��ʾһ���ַ��ĺ���
		MOV		BX,15			; ��ɫ����
		INT		0x10			; ������ʾBIOS
		JMP		putloop
fin:
		HLT						; ��ʱ��CPUֹͣ����
		JMP		fin				; ����ѭ��
msg:
		DB		0x0a, 0x0a		; ��������
		DB		"KalinoteOS load error!"
		DB		0x0a			; ����
		DB		0

; -------------------------���̶�ȡ��ش���-------------------------
readfast:	; ʹ��AL����һ���Զ�ȡ����
;	ES:��ȡ��ַ, CH:����, DH:��ͷ, CL:����, BX:��ȡ������

		MOV		AX,ES			; < ͨ��ES����AL���ֵ >
		SHL		AX,3			; ��AX����32�����������AH(SHL����λ��ָ��)
		AND		AH,0x7f			; AH��AH����128���õ�������512*128=64K��
		MOV		AL,128			; AL = 128 - AH; AH��AH����128���õ�����
		SUB		AL,AH

		MOV		AH,BL			; < ͨ��BX����AL�����ֵ������AH >
		CMP		BH,0			; if (BH != 0) { AH = 18; }
		JE		.skip1
		MOV		AH,18
.skip1:
		CMP		AL,AH			; if (AL > AH) { AL = AH; }
		JBE		.skip2
		MOV		AL,AH
.skip2:

		MOV		AH,19			; < ͨ��CL����AL�����ֵ������AH >
		SUB		AH,CL			; AH = 19 - CL;
		CMP		AL,AH			; if (AL > AH) { AL = AH; }
		JBE		.skip3
		MOV		AL,AH
.skip3:

		PUSH	BX
		MOV		SI,0			; ����ʧ�ܴ����ļĴ���
retry:
		MOV		AH,0x02			; AH=0x02 : ��ȡ����
		MOV		BX,0
		MOV		DL,0x00			; A��
		PUSH	ES
		PUSH	DX
		PUSH	CX
		PUSH	AX
		INT		0x13			; ���ô���BIOS
		JNC		next			; û�г���Ļ�ת��next
		ADD		SI,1			; SI+1
		CMP		SI,5			; SI��5�Ƚ�
		JAE		error			; SI >= 5 ��ת��error
		MOV		AH,0x00
		MOV		DL,0x00			; A��
		INT		0x13			; ����������
		POP		AX
		POP		CX
		POP		DX
		POP		ES
		JMP		retry
next:
		POP		AX
		POP		CX
		POP		DX
		POP		BX				; ��ES�����ݴ���BX
		SHR		BX,5			; ��BX��16�ֽڵ�λת��Ϊ512�ֽڵ�λ
		MOV		AH,0
		ADD		BX,AX			; BX += AL;
		SHL		BX,5			; ��BX��512�ֽڵ�λת��Ϊ16�ֽڵ�λ
		MOV		ES,BX			; �൱��ES += AL * 0x20;
		POP		BX
		SUB		BX,AX
		JZ		.ret
		ADD		CL,AL			; ��CL����AL
		CMP		CL,18			; ��CL��18�Ƚ�
		JBE		readfast		; CL <= 18 ��ת��readfast
		MOV		CL,1
		ADD		DH,1
		CMP		DH,2
		JB		readfast		; DH < 2 ��ת��readfast
		MOV		DH,0
		ADD		CH,1
		JMP		readfast
.ret:
		RET


; -------------------------��ʾ��Ϣ-------------------------
StartBootMsg:
		DB		"Start Boot..."
SysFileName:
		DB		"KALINOTESYS",0
SysFileNotFound:
		DB		"KALINOTE.SYS is not found!"

		TIMES 0x1fe-($-$$) DB 0	; ���0
		DB		0x55, 0xaa