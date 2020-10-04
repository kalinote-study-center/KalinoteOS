; KalinoteOS
; TAB=4

		ORG		0x7c00			; �����N�n��?�n��

; �ꉺ��?�yFAT32�i��?��?�p��?

		JMP		entry
		DB		0x90
		DB		"KALINIPL"		; ??�於��(8��?)
		DW		512				; ?�����召?512
		DB		1				; ?���Ƒ召?�꘢���
		DW		1				; FAT�N�n�ʒu
		DB		2				; FAT����
		DW		224				; ����?�召
		DW		2880			; ��?�召
		DB		0xf0			; ��???
		DW		9				; FAT?�x
		DW		18				; ?�������I��搔
		DW		2				; ��?��
		DD		0				; �s�g�p����
		DD		2880			; �d�ʈ�Վ�?�召
		DB		0,0,0x29		; ���m*
		DD		0xffffffff		; ���m*
		DB		"KALINOTE-OS"	; ��?����(11��?)
		DB		"FAT12   "		; ��?�i������(8��?)
		RESB	18				; �Ƃ肠����18�o�C�g�����Ă���

; �������

entry:
		MOV		AX,0			; ���n���񑶊�
		MOV		SS,AX
		MOV		SP,0x7c00
		MOV		DS,AX
		MOV		ES,AX

		MOV		SI,msg
putloop:
		MOV		AL,[SI]
		ADD		SI,1			; SI��1
		CMP		AL,0
		JE		fin
		MOV		AH,0x0e			; �w��꘢����
		MOV		BX,15			; �w�莚��?�F
		INT		0x10			; ?�p??
		JMP		putloop
fin:
		HLT						; ��~CPU�C���Ҏw��
		JMP		fin				; �ٌ��z?

msg:
		DB		0x0a, 0x0a		; ?��?�s
		DB		"KalinoteOS test message"
		DB		0x0a			; ?�s
		DB		"this is a test msg"
		DB		0

		RESB	0x7dfe-$		; �U�[0

		DB		0x55, 0xaa
