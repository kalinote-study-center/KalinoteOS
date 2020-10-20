; kalinote-os
; TAB=4

[INSTRSET "i486p"]

VBEMODE	EQU		0x105			; 1024x768x8bit色彩
BOTPAK	EQU		0x00280000		; bootpack安装位置
DSKCAC	EQU		0x00100000		; 磁盘缓存位置
DSKCAC0	EQU		0x00008000		; 磁盘缓存位置（实时模式）

; 色彩代码：
;	0x100 :  640 x  400 x 8bit
;	0x101 :  640 x  480 x 8bit
;	0x103 :  800 x  600 x 8bit
;	0x105 : 1024 x  768 x 8bit
;	0x107 : 1280 x 1024 x 8bit(qemu无法使用,VMware可以)
;	更多色彩模式代码去VESA查

; BOOT_INFO
CYLS	EQU		0x0ff0			; 设定启动区
LEDS	EQU		0x0ff1
VMODE	EQU		0x0ff2			; 颜色的位数
SCRNX	EQU		0x0ff4			; 分辨率X
SCRNY	EQU		0x0ff6			; 分辨率Y
VRAM	EQU		0x0ff8			; 图像缓冲区开始地址

		ORG		0xc200			; 程序被装载到内存0xc200的位置

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

; 取得画面模式信息
		MOV		CX,VBEMODE
		MOV		AX,0x4f01
		INT		0x10
		CMP		AX,0x004f
		JNE		scrn320

; 画面模式信息确认
		CMP		BYTE [ES:DI+0x19],8
		JNE		scrn320
		CMP		BYTE [ES:DI+0x1b],4
		JNE		scrn320
		MOV		AX,[ES:DI+0x00]
		AND		AX,0x0080
		JZ		scrn320			; 模式属性的bit7是0，所以放弃

; 画面模式的切换
		MOV		BX,VBEMODE+0x4000
		MOV		AX,0x4f02
		INT		0x10
		MOV		BYTE [VMODE],8	; 记录画面模式
		MOV		AX,[ES:DI+0x12]
		MOV		[SCRNX],AX
		MOV		AX,[ES:DI+0x14]
		MOV		[SCRNY],AX
		MOV		EAX,[ES:DI+0x28]
		MOV		[VRAM],EAX
		JMP		keystatus

scrn320:
		MOV		AL,0x13			; VGA图形、1024x768x8bit色彩
		MOV		AH,0x00
		INT		0x10
		MOV		BYTE [VMODE],8	; 记录画面模式
		MOV		WORD [SCRNX],1024
		MOV		WORD [SCRNY],768
		MOV		DWORD [VRAM],0x000a0000

; 用BIOS取得键盘上各种LED的状态
keystatus:
		MOV		AH,0x02
		INT		0x16 			; keyboard BIOS
		MOV		[LEDS],AL

；PIC关闭所有中断
；根据AT兼容机的规格，如果要初始化PIC，
；必须在CLI之前进行，否则有事会挂起
；随后进行PIC初始化

		MOV		AL,0xff
		OUT		0x21,AL
		NOP						; 停顿一下，防止BUG(作者说某些机型如果连续UOT会死掉)
		OUT		0xa1,AL

		CLI						; 禁止CPU级别中断

; 为了能够从CPU访问1MB以上的存储器，设定A20GATE

		CALL	waitkbdout
		MOV		AL,0xd1
		OUT		0x64,AL
		CALL	waitkbdout
		MOV		AL,0xdf			; enable A20
		OUT		0x60,AL
		CALL	waitkbdout

; 转向保护模式

		LGDT	[GDTR0]			; 设定暂定GDT
		MOV		EAX,CR0
		AND		EAX,0x7fffffff	; 将bit31设为0（为了禁止寻呼）
		OR		EAX,0x00000001	; 使bit0为1(为了保护模式转移)
		MOV		CR0,EAX
		JMP		pipelineflush
pipelineflush:
		MOV		AX,1*8			; 可读区段32bit
		MOV		DS,AX
		MOV		ES,AX
		MOV		FS,AX
		MOV		GS,AX
		MOV		SS,AX
		
; bootpack的传送

		MOV		ESI,bootpack	; 传送源
		MOV		EDI,BOTPAK		; 传送源
		MOV		ECX,512*1024/4
		CALL	memcpy
		
; 顺便盘数据也向原来的位置传送

; 首先从引导扇区

		MOV		ESI,0x7c00		; 传送源
		MOV		EDI,DSKCAC		; 传送源
		MOV		ECX,512/4
		CALL	memcpy
		
; 剩下的全部

		MOV		ESI,DSKCAC0+512	; 传送源
		MOV		EDI,DSKCAC+512	; 传送源
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
		SHR		ECX,2			; ECX /= 4;
		JZ		skip			; 没有可转发的东西
		MOV		ESI,[EBX+20]	; 传送源
		ADD		ESI,EBX
		MOV		EDI,[EBX+12]	; 传送源
		CALL	memcpy
skip:
		MOV		ESP,[EBX+12]	; 堆栈初始值
		JMP		DWORD 2*8:0x0000001b

waitkbdout:
		IN		 AL,0x64
		AND		 AL,0x02
		JNZ		waitkbdout		; 如果AND的结果不是0，进入waitkbdout
		RET

memcpy:
		MOV		EAX,[ESI]
		ADD		ESI,4
		MOV		[EDI],EAX
		ADD		EDI,4
		SUB		ECX,1
		JNZ		memcpy			; 如果减法的结果不是0，则进入memcpy
		RET
; memcpy如果不忘记加入地址复制，也可以写串命令

		ALIGNB	16
GDT0:
		RESB	8				; 选择器
		DW		0xffff,0x0000,0x9200,0x00cf	; 可读区段32bit
		DW		0xffff,0x0000,0x9a28,0x0047	; 可执行段32bit(用于bootpack)

		DW		0
GDTR0:
		DW		8*3-1
		DD		GDT0

		ALIGNB	16
bootpack:
