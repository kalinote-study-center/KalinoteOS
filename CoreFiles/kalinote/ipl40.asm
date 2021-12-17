; Kalinote-ipl
; TAB=4

CYLS	EQU		40				; 读取40个柱面

		ORG		0x7c00			; 程序起始装载地址

; 以下是标准FAT12格式载体用代码，参考网址：https://blog.csdn.net/m0_37329910/article/details/88927673

		JMP SHORT entry
		DB		0x90
		DB		"KALIIPL "		; 启动区名称(8字节)
		DW		512				; 每个扇区大小为512
		DB		1				; 每个簇大小为一个扇区
		DW		1				; FAT起始位置(boot记录占用扇区数)
		DB		2				; FAT表数
		DW		224				; 根目录最大文件数
		DW		2880			; 磁盘大小(总扇区数，如果这里扇区数为0，则由下面给出)
		DB		0xf0			; 磁盘种类
		DW		9				; FAT长度(每个FAT表占用扇区数)
		DW		18				; 每个磁道的扇区数
		DW		2				; 磁头数(面数)
		DD		0				; 不使用分区(隐藏扇区数)
		DD		2880			; 重写一遍磁盘大小(如果上面的扇区数为0，则由这里给出)
		DB		0,0,0x29		; 这三位分别表示：INT 13H的驱动器号、保留、扩展引导标记(29h)
		DD		0xffffffff		; 卷序列号
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
		MOV		BX,18*2*CYLS-1	; 要读取的合计扇区数
		CALL	readfast		; 高速读取

; 读取结束，运行kalinote.sys

		MOV		BYTE [0x0ff0],CYLS	; 记录IPL实际读取了多少内容
		JMP		0xc200

error:
		MOV		AX,0
		MOV		ES,AX
		MOV		SI,msg
putloop:
		MOV		AL,[SI]
		ADD		SI,1			; SI加1
		CMP		AL,0
		JE		fin
		MOV		AH,0x0e			; 显示一个字符的函数
		MOV		BX,15			; 颜色代码
		INT		0x10			; 调用显示BIOS
		JMP		putloop
fin:
		HLT						; 暂时让CPU停止运行
		JMP		fin				; 无限循环
msg:
		DB		0x0a, 0x0a		; 两个换行
		DB		"KalinoteOS load error!"
		DB		0x0a			; 换行
		DB		0

readfast:	; 使用AL尽量一次性读取数据
;	ES:读取地址, CH:柱面, DH:磁头, CL:扇区, BX:读取扇区数

		MOV		AX,ES			; < 通过ES计算AL最大值 >
		SHL		AX,3			; 将AX除以32，将结果存入AH(SHL是左位移指令)
		AND		AH,0x7f			; AH是AH除以128所得的余数（512*128=64K）
		MOV		AL,128			; AL = 128 - AH; AH是AH除以128所得的余数
		SUB		AL,AH

		MOV		AH,BL			; < 通过BX计算AL的最大值并存入AH >
		CMP		BH,0			; if (BH != 0) { AH = 18; }
		JE		.skip1
		MOV		AH,18
.skip1:
		CMP		AL,AH			; if (AL > AH) { AL = AH; }
		JBE		.skip2
		MOV		AL,AH
.skip2:

		MOV		AH,19			; < 通过CL计算AL的最大值并存入AH >
		SUB		AH,CL			; AH = 19 - CL;
		CMP		AL,AH			; if (AL > AH) { AL = AH; }
		JBE		.skip3
		MOV		AL,AH
.skip3:

		PUSH	BX
		MOV		SI,0			; 计算失败次数的寄存器
retry:
		MOV		AH,0x02			; AH=0x02 : 读取磁盘
		MOV		BX,0
		MOV		DL,0x00			; A盘
		PUSH	ES
		PUSH	DX
		PUSH	CX
		PUSH	AX
		INT		0x13			; 调用磁盘BIOS
		JNC		next			; 没有出错的话转至next
		ADD		SI,1			; SI+1
		CMP		SI,5			; SI和5比较
		JAE		error			; SI >= 5 跳转至error
		MOV		AH,0x00
		MOV		DL,0x00			; A盘
		INT		0x13			; 驱动器重置
		POP		AX
		POP		CX
		POP		DX
		POP		ES
		JMP		retry
next:
		POP		AX
		POP		CX
		POP		DX
		POP		BX				; 将ES的内容存至BX
		SHR		BX,5			; 将BX由16字节单位转换为512字节单位
		MOV		AH,0
		ADD		BX,AX			; BX += AL;
		SHL		BX,5			; 将BX由512字节单位转换为16字节单位
		MOV		ES,BX			; 相当于ES += AL * 0x20;
		POP		BX
		SUB		BX,AX
		JZ		.ret
		ADD		CL,AL			; 将CL加上AL
		CMP		CL,18			; 将CL与18比较
		JBE		readfast		; CL <= 18 跳转至readfast
		MOV		CL,1
		ADD		DH,1
		CMP		DH,2
		JB		readfast		; DH < 2 跳转至readfast
		MOV		DH,0
		ADD		CH,1
		JMP		readfast
.ret:
		RET

		TIMES 0x1fe-($-$$) DB 0	; 填充0

		DB		0x55, 0xaa