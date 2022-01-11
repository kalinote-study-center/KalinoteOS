; kalinote-os
; TAB=4

VBEMODE	EQU		0x118			; 1024x768x32bit色彩
BOTPAK	EQU		0x00280000		; bootpack安装位置
DSKCAC	EQU		0x00100000		; 磁盘缓存位置
DSKCAC0	EQU		0x00008000		; 磁盘缓存位置（实时模式）

; 色彩代码：
;	0x100 :  640 x  400 x 8bit
;	0x101 :  640 x  480 x 8bit
;	0x103 :  800 x  600 x 8bit
;	0x105 : 1024 x  768 x 8bit
;	0x107 : 1280 x 1024 x 8bit(qemu无法使用,VMware可以)
;	0x0112	:	640x480	x32bit	;这三个是32位色模式
;	0x0115	:	800x600	x32bit
;	0x0118	:	1024x768x32bit
;	更多色彩模式代码去VESA查

; BOOT_INFO
CYLS	EQU		0x0ff0			; 设定启动区
LEDS	EQU		0x0ff1
VMODE	EQU		0x0ff2			; 颜色的位数
SCRNX	EQU		0x0ff4			; 分辨率X
SCRNY	EQU		0x0ff6			; 分辨率Y
VRAM	EQU		0x0ff8			; 图像缓冲区开始地址

; VBE_INFO
VBESIGNATURE		EQU		0x1000
VBEVERSION			EQU		VBESIGNATURE + 4
OEMSTRINGPTR		EQU		VBESIGNATURE + 6
CAPABILITIES		EQU		VBESIGNATURE + 10
VIDEOMODEPTR		EQU		VBESIGNATURE + 14
TOTALMEMORY			EQU		VBESIGNATURE + 18
OEMSOFTWAREREV		EQU		VBESIGNATURE + 20
OEMVERDORNAMEPTR	EQU		VBESIGNATURE + 22
OEMPRODUCTNAMEPTR	EQU		VBESIGNATURE + 26
OEMPRODUCTREVPTR	EQU		VBESIGNATURE + 30
RESERVED			EQU		VBESIGNATURE + 34

		ORG		0xc200			; 程序被装载到内存0xc200的位置

; -----------------------------------代码从这里开始-----------------------------------
; 确认VBE是否存在
		MOV		AX,0x9000
		MOV		ES,AX
		MOV		DI,0
		MOV		AX,0x4f00
		INT		0x10
		CMP		AX,0x004f
		JNE		scrn320

; 检查VBE版本
		MOV		AX,[ES:DI+4]
		CMP		AX,0x0200
		JB		scrn320			; if (AX < 0x0200) goto scrn320
		; 记录VBE信息
		MOV		EAX, [ES:DI]
		MOV		[VBESIGNATURE], EAX
		; TODO：完成VBE信息获取
		

; 取得画面模式信息
		MOV		CX,VBEMODE
		MOV		AX,0x4f01
		INT		0x10
;		CMP		AX,0x004f
;		JNE		scrn320

; 画面模式信息确认
;		CMP		BYTE [ES:DI+0x19],8
;		JNE		scrn320
;		CMP		BYTE [ES:DI+0x1b],4
;		JNE		scrn320
;		MOV		AX,[ES:DI+0x00]
;		AND		AX,0x0080
;		JZ		scrn320			; 模式属性的bit7是0，所以放弃

; 获取SVGA信息


; 画面模式的切换
		MOV		BX,VBEMODE+0x4000
		MOV		AX,0x4f02
		INT		0x10
		MOV		BYTE [VMODE],32	; 记录画面模式
		MOV		AX,[ES:DI+0x12]
		MOV		[SCRNX],AX
		MOV		AX,[ES:DI+0x14]
		MOV		[SCRNY],AX
		MOV		EAX,[ES:DI+0x28]
		MOV		[VRAM],EAX
		JMP		keystatus

scrn320:
		MOV		AL,0x13			; VGA图形、1024x768x32bit色彩
		MOV		AH,0x00
		INT		0x10
		MOV		BYTE [VMODE],32	; 记录画面模式
		MOV		WORD [SCRNX],1024
		MOV		WORD [SCRNY],768
		MOV		DWORD [VRAM],0x000a0000

; 用BIOS取得键盘上各种LED的状态
keystatus:
		MOV		AH,0x02
		INT		0x16 			; keyboard BIOS
		MOV		[LEDS],AL

; -----------------------------------切换到32位-----------------------------------
; 从这里开始进入32位
; 在osdev.org中，对于切换到受保护模式(Protected Mode)之前，有以下步骤：
; 禁用中断，包括NMI（如英特尔开发人员手册建议）。
; 启用A20 line。
; 加载适用于代码、数据和堆栈的段描述符"全球描述器表(GDT)"。

; PIC关闭所有中断
; 根据AT兼容机的规格，如果要初始化PIC，
; 必须在CLI之前进行，否则有事会挂起
; 随后进行PIC初始化
; 功能等同于(C语言)
; io_out(PIC0_IMR, 0xff);	/* 禁止主PIC中断 */
; io_out(PIC1_IMR, 0xff);	/* 禁止从PIC中断 */
; io_cli();					/* 禁止CPU中断 */
		MOV		AL,0xff
		OUT		0x21,AL			; 禁止主PIC中断(这里可以参考PIC中断控制器相关资料)
		NOP						; 停顿一下，防止BUG(这个对于现在来说没有太大的必要性 )
		OUT		0xa1,AL			; 禁止从PIC中断

		CLI						; 禁止CPU中断

; 为了能够从CPU访问1MB以上的存储器，设定A20GATE
; 功能等同于(C语言)
; #define KEYCMD_WRITE_OUTPORT		0xd1
; #define KBC_OUTPORT_A20G_ENABLE	0Xdf
; 
; wait_KBC_sendready();		/* keyboard.c */
; io_out(PORT_KEYCMD, KEYCMD_WRITE_OUTPORT);
; wait_KBC_sendready();
; io_out(PORT_KEYCMD, KBC_OUTPORT_A20G_ENABLE);
; wait_KBC_sendready();		/* wait_KBC_sendready是多余的，在这里是为了等待A20GATE处理完毕 */
		CALL	waitkbdout		; 这里与wait_KBC_sendready相同(等待键盘控制电路准备完毕 P.140)
		MOV		AL,0xd1
		OUT		0x64,AL
		CALL	waitkbdout
		MOV		AL,0xdf			; 激活 A20GATE,使CPU将1M以上部分的内存变为可用状态
		OUT		0x60,AL
		CALL	waitkbdout

; 转向保护模式
; CR0寄存器中含有控制处理器操作模式和状态的系统控制标志
		LGDT	[GDTR0]			; 将临时32位GDT加载到GDTR寄存器
		MOV		EAX,CR0			; 为了设置CR0寄存器的值
		; AND		EAX,0x7fffffff	; 将bit31(最高位)设为0（为了禁止分页）
		OR		EAX,0x00000001	; 使bit0(最低位)为1(为了切换到保护模式)
		MOV		CR0,EAX
		JMP		pipelineflush
; 位于此处的jmp
; CPU通过代入CR0进入保护模式后，需要立即执行跳转指令(P.155)
pipelineflush:
		MOV		AX,1*8			; 可读写的段 32bit
		MOV		DS,AX
		MOV		ES,AX
		MOV		FS,AX
		MOV		GS,AX
		MOV		SS,AX

; bootpack的传送
; 将bootpack传送到相应的内存位置
		MOV		ESI,bootpack	; 传送源
		MOV		EDI,BOTPAK		; 传送目的地
		MOV		ECX,512*1024/4	; 传送数据大小
		CALL	memcpy
		
; 顺便盘数据也向原来的位置传送

; 首先从引导扇区
; 这里一段程序的意思是从0x7c00复制512字节到DSKCAC(0x00100000)，以双字节为单位，复制次数为512/4
; 0x00100000是1M以后的内存
		MOV		ESI,0x7c00		; 传送源
		MOV		EDI,DSKCAC		; 传送目的地
		MOV		ECX,512/4		; 传送数据大小
		CALL	memcpy

; 剩下的全部

		MOV		ESI,DSKCAC0+512	; 传送源
		MOV		EDI,DSKCAC+512	; 传送目的地
		MOV		ECX,0
		MOV		CL,BYTE [CYLS]
		IMUL	ECX,512*18*2/4
		SUB		ECX,512/4		; 只减去IPL的部分
		CALL	memcpy

; asmhead必须做的事情都做完了
; 剩下的就交给bootpack了

; 启动bootpack

		MOV		EBX,BOTPAK
		MOV		ECX,[EBX+16]
		ADD		ECX,3			; ECX += 3;
		SHR		ECX,2			; ECX /= 4;		SHR是右移指令
		JZ		skip			; 没有可转发的东西
		MOV		ESI,[EBX+20]	; 传送源
		ADD		ESI,EBX
		MOV		EDI,[EBX+12]	; 传送源
		CALL	memcpy
skip:
		MOV		ESP,[EBX+12]	; 堆栈初始值
		JMP		DWORD 2*8:0x0000001b	;KaliMain入口的地址，从这里开始进入C语言		;新标的入口地址为0x00000027

waitkbdout:
		IN		 AL,0x64
		AND		 AL,0x02
		IN		 AL,0x60		; 空读，为了清空缓存中的垃圾数据
		JNZ		waitkbdout		; 如果AND的结果不是0，进入waitkbdout
		RET

memcpy:
		; 这个函数用于复制内存
		; ESI是复制源地址
		; EDI是复制目标地址
		; ECX是复制数据大小(传送数据大小是以dword为单位[32位]，所以用字节数除以4)
		MOV		EAX,[ESI]
		ADD		ESI,4
		MOV		[EDI],EAX
		ADD		EDI,4
		SUB		ECX,1
		JNZ		memcpy			; 如果减法的结果不是0，则进入memcpy
		RET
; memcpy如果不忘记加入地址复制，也可以写串命令

		ALIGNB	16, DB 0		; 补充0，直到能被16整除。下同
		
; -----------------------------------32位临时GDT表-----------------------------------
GDT0:
; 位于数据区的GDT表
		; RESB	8
		TIMES	8 DB 0			; 选择器
		; 写入8字节的GDT表
		DW		0xffff,0x0000,0x9200,0x00cf	; 可读区段32bit					; CPU使用
		DW		0xffff,0x0000,0x9a28,0x0047	; 可执行段32bit(用于bootpack)	; bootpack.kal使用
		; 这一段其实跟下面两句代码差不多(kernel/dsctbl.c)：
		; set_segmdesc(gdt + 1, 0xffffffff,   0x00000000, AR_DATA32_RW_R0);
		; set_segmdesc(gdt + 2, LIMIT_BOTPAK, ADR_BOTPAK, AR_CODE32_ER_R0);

		DW		0
GDTR0:
; 这里是写一个临时的GDT
		DW		8*3-1		; GDT表的数量
		DD		GDT0		; GDT表的地址

		; ALIGNB	16
		ALIGN 16, DB 0
bootpack: