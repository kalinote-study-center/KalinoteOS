; Kalinote-ipl
; TAB=4
; 只能读入10个柱面

CYLS	EQU		20				; 读取20个柱面

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
		
readloop:
		MOV		SI,0			; 记录失败次数
		
retry:
; 启动失败，尝试重新启动
		MOV		AH,0x02			; 读盘
		MOV		AL,1			; 1个扇区
		MOV		BX,0
		MOV		DL,0x00			; A驱动器
		INT		0x13			; 调用磁盘BIOS
		JNC		next			; 没有出错的话跳转到next
		ADD		SI,1			; SI加1
		CMP		SI,5			; SI和5比较
		JAE		error			; 超过5次重试，报错
		MOV		AH,0x00
		MOV		DL,0x00			; A驱动器
		INT		0x13			; 重置驱动器
		JMP		retry
		
next:
		MOV		AX,ES			; 内存地址后移0x200
		ADD		AX,0x0020
		MOV		ES,AX
		ADD		CL,1			; CL加1
		CMP		CL,18			; CL和18比较
		JBE		readloop		; 如果CL小于18，跳到readloop重复读取
		MOV		CL,1
		ADD		DH,1
		CMP		DH,2
		JB		readloop		; DH小于2跳转到readloop
		MOV		DH,0
		ADD		CH,1
		CMP		CH,CYLS
		JB		readloop		; CH小于CYLS跳转到readloop
		
; 读取完成执行kalinote.sys
		
		MOV		[0x0ff0],CH		; 记录IPL读取位置
		JMP		0xc200
		
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
		DB		"KalinoteOS load failed"
		DB		0x0a			; 换行
		DB		"maybe your device have some error"
		DB		0

		RESB	0x7dfe-$		; 填充0

		DB		0x55, 0xaa