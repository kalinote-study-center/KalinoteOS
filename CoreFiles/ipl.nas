; KalinoteOS
; TAB=4

		ORG		0x7c00			; ������ʼװ�ص�ַ

; �����Ǳ�׼FAT32��ʽ�����ô���

		JMP		entry
		DB		0x90
		DB		"KALINIPL"		; ����������(8��?)
		DW		512				; ÿ��������СΪ512
		DB		1				; ÿ���ش�СΪһ������
		DW		1				; FAT��ʼλ��
		DB		2				; FAT����
		DW		224				; ��Ŀ�Ĵ�С
		DW		2880			; ���̴�С
		DB		0xf0			; ��������
		DW		9				; FAT����
		DW		18				; ÿ���ŵ���������
		DW		2				; ��ͷ��
		DD		0				; ��ʹ�÷���
		DD		2880			; ��дһ����̴�С
		DB		0,0,0x29		; δ֪*
		DD		0xffffffff		; δ֪*
		DB		"KALINOTE-OS"	; ��������(11�ֽ�)
		DB		"FAT12   "		; ���̸�ʽ����(8�ֽ�)
		RESB	18				; ��18�ֽ�

; ��������

entry:
		MOV		AX,0			; ��ʼ���Ĵ���
		MOV		SS,AX
		MOV		SP,0x7c00
		MOV		DS,AX
		
; ��ȡ����
		
		MOV		AX,0x0820
		MOV		ES,AX
		MOV		CH,0			; ����0
		MOV		DH,0			; ��ͷ0
		MOV		CL,2			; ����2
		
		MOV		AH,0x02			; ����
		MOV		AL,1			; 1������
		MOV		BX,0
		MOV		DL,0x00			; A������
		INT		0x13			; ���ô���BIOS
		JC		error
		
; ��ȡ��ɺ�ֹͣ
		
fin:
; �������
		HLT						; ֹͣCPU���ȴ�ָ��
		JMP		fin				; ����ѭ��

error:
; ����������ʾ������Ϣ
		MOV		SI,msg

putloop:
		MOV		AL,[SI]
		ADD		SI,1			; SI��1
		CMP		AL,0
		JE		fin
		MOV		AH,0x0e			; ��ʾһ������
		MOV		BX,15			; ָ��һ����ɫ
		INT		0x10			; �����Կ�
		JMP		putloop

msg:
		DB		0x0a, 0x0a		; ��������
		DB		"KalinoteOS test message"
		DB		0x0a			; ����
		DB		"this is a test msg"
		DB		0

		RESB	0x7dfe-$		; ���0

		DB		0x55, 0xaa