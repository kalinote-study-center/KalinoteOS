; KalinoteOS
; TAB=4

		ORG		0x7c00			; 程序起始装载地址

; 以下是标准FAT32格式载体用代码

		JMP		entry
		DB		0x90
		DB		"KALINIPL"		; 启动区名称(8字?)
		DW		512				; 每个扇区大小为512
		DB		1				; 每个簇大小为一个扇区
		DW		1				; FAT起始位置
		DB		2				; FAT个数
		DW		224				; 根目的大小
		DW		2880			; 磁盘大小
		DB		0xf0			; 磁盘种类
		DW		9				; FAT长度
		DW		18				; 每个磁道的扇区数
		DW		2				; 磁头数
		DD		0				; 不使用分区
		DD		2880			; 重写一遍磁盘大小
		DB		0,0,0x29		; 未知*
		DD		0xffffffff		; 未知*
		DB		"KALINOTE-OS"	; 磁盘名称(11字节)
		DB		"FAT12   "		; 磁盘格式名称(8字节)
		RESB	18				; 空18字节

; 程序主体

entry:
		MOV		AX,0			; 初始化寄存器
		MOV		SS,AX
		MOV		SP,0x7c00
		MOV		DS,AX
		
; 读取磁盘
		
		MOV		AX,0x0820
		MOV		ES,AX
		MOV		CH,0			; 柱面0
		MOV		DH,0			; 磁头0
		MOV		CL,2			; 扇区2
		
		MOV		AH,0x02			; 读盘
		MOV		AL,1			; 1个扇区
		MOV		BX,0
		MOV		DL,0x00			; A驱动器
		INT		0x13			; 调用磁盘BIOS
		JC		error
		
; 读取完成后停止
		
fin:
; 启动完成
		HLT						; 停止CPU，等待指令
		JMP		fin				; 无限循环

error:
; 启动错误，显示错误信息
		MOV		SI,msg

putloop:
		MOV		AL,[SI]
		ADD		SI,1			; SI加1
		CMP		AL,0
		JE		fin
		MOV		AH,0x0e			; 显示一个文字
		MOV		BX,15			; 指定一个颜色
		INT		0x10			; 调用显卡
		JMP		putloop

msg:
		DB		0x0a, 0x0a		; 换行两次
		DB		"KalinoteOS test message"
		DB		0x0a			; 换行
		DB		"this is a test msg"
		DB		0

		RESB	0x7dfe-$		; 填充0

		DB		0x55, 0xaa