; KalinoteOS
; TAB=4

		ORG		0x7c00			; 程序起始装?地址

; 一下是?准FAT32格式?体?用代?

		JMP		entry
		DB		0x90
		DB		"KALINIPL"		; ??区名称(8字?)
		DW		512				; ?个扇区大小?512
		DB		1				; ?个簇大小?一个扇区
		DW		1				; FAT起始位置
		DB		2				; FAT个数
		DW		224				; 根目?大小
		DW		2880			; 磁?大小
		DB		0xf0			; 磁???
		DW		9				; FAT?度
		DW		18				; ?个磁道的扇区数
		DW		2				; 磁?数
		DD		0				; 不使用分区
		DD		2880			; 重写一遍磁?大小
		DB		0,0,0x29		; 未知*
		DD		0xffffffff		; 未知*
		DB		"KALINOTE-OS"	; 磁?名称(11字?)
		DB		"FAT12   "		; 磁?格式名称(8字?)
		RESB	18				; とりあえず18バイトあけておく

; 程序主体

entry:
		MOV		AX,0			; 初始化寄存器
		MOV		SS,AX
		MOV		SP,0x7c00
		MOV		DS,AX
		MOV		ES,AX

		MOV		SI,msg
putloop:
		MOV		AL,[SI]
		ADD		SI,1			; SI加1
		CMP		AL,0
		JE		fin
		MOV		AH,0x0e			; 指定一个文字
		MOV		BX,15			; 指定字符?色
		INT		0x10			; ?用??
		JMP		putloop
fin:
		HLT						; 停止CPU，等待指令
		JMP		fin				; 无限循?

msg:
		DB		0x0a, 0x0a		; ?次?行
		DB		"KalinoteOS test message"
		DB		0x0a			; ?行
		DB		"this is a test msg"
		DB		0

		RESB	0x7dfe-$		; 填充0

		DB		0x55, 0xaa
