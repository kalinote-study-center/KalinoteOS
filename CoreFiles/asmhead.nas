; kalinote-os
; TAB=4

BOTPAK	EQU		0x00280000		; bootpack安装位置
DSKCAC	EQU		0x00100000		; 磁盘缓存位置
DSKCAC0	EQU		0x00008000		; 磁盘缓存位置（实时模式）

; BOOT_INFO
CYLS	EQU		0x0ff0			; 设定启动区
LEDS	EQU		0x0ff1
VMODE	EQU		0x0ff2			; 颜色的位数
SCRNX	EQU		0x0ff4			; 分辨率X
SCRNY	EQU		0x0ff6			; 分辨率Y
VRAM	EQU		0x0ff8			; 图像缓冲区开始地址

		ORG		0xc200			; 程序被装载到内存0xc200的位置
		
; 设定画面模式
		
		MOV		AL,0x13			; VGA显卡,320x200x8位彩色
		MOV		AH,0x00
		INT		0x10
		MOV		BYTE [VMODE],8	; 记录画面模式
		MOV		WORD [SCRNX],320
		MOV		WORD [SCRNY],200
		MOV		DWORD [VRAM],0x000a0000
		
; 用BIOS取得键盘上各种LED的状态

		MOV		AH,0x02
		INT		0x16 			; keyboard BIOS
		MOV		[LEDS],AL
		
；防止PIC接受任何中断
；在AT兼容机的规格中，如果要初始化PIC
；如果不在CLI之前做的话，偶尔会死机
；PIC的初始化稍后进行

		MOV		AL,0xff
		OUT		0x21,AL
		NOP						; 停顿一下，防止BUG(作者说某些机型如果连续UOT会死掉)
		OUT		0xa1,AL

		CLI						; 甚至CPU级别也禁止
		
; 为了能够从CPU访问1MB以上的存储器，设定A20GATE

		CALL	waitkbdout
		MOV		AL,0xd1
		OUT		0x64,AL
		CALL	waitkbdout
		MOV		AL,0xdf			; enable A20
		OUT		0x60,AL
		CALL	waitkbdout
		
; 转向保护模式

[INSTRSET "i486p"]

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
		IMUL	ECX,512*18*2/4	; シリンダ数からバイト数/4に変換
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
