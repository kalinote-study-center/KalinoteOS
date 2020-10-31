; Kalinote-ipl
; TAB=4
; ֻ�ܶ���10������

CYLS	EQU		20				; ��ȡ20������

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
		
readloop:
		MOV		SI,0			; ��¼ʧ�ܴ���
		
retry:
; ����ʧ�ܣ�������������
		MOV		AH,0x02			; ����
		MOV		AL,1			; 1������
		MOV		BX,0
		MOV		DL,0x00			; A������
		INT		0x13			; ���ô���BIOS
		JNC		next			; û�г���Ļ���ת��next
		ADD		SI,1			; SI��1
		CMP		SI,5			; SI��5�Ƚ�
		JAE		error			; ����5�����ԣ�����
		MOV		AH,0x00
		MOV		DL,0x00			; A������
		INT		0x13			; ����������
		JMP		retry
		
next:
		MOV		AX,ES			; �ڴ��ַ����0x200
		ADD		AX,0x0020
		MOV		ES,AX
		ADD		CL,1			; CL��1
		CMP		CL,18			; CL��18�Ƚ�
		JBE		readloop		; ���CLС��18������readloop�ظ���ȡ
		MOV		CL,1
		ADD		DH,1
		CMP		DH,2
		JB		readloop		; DHС��2��ת��readloop
		MOV		DH,0
		ADD		CH,1
		CMP		CH,CYLS
		JB		readloop		; CHС��CYLS��ת��readloop
		
; ��ȡ���ִ��kalinote.sys
		
		MOV		[0x0ff0],CH		; ��¼IPL��ȡλ��
		JMP		0xc200
		
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
		DB		"KalinoteOS load failed"
		DB		0x0a			; ����
		DB		"maybe your device have some error"
		DB		0

		RESB	0x7dfe-$		; ���0

		DB		0x55, 0xaa