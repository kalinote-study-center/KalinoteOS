OBJS_BOOTPACK = kernel/bootpack.obj naskfunc.obj fonts.obj gui/graphic.obj kernel/dsctbl.obj \
		kernel/int.obj kernel/fifo.obj driver/keyboard.obj driver/mouse.obj kernel/memory.obj kernel/sheet.obj driver/timer.obj \
		kernel/mtask.obj gui/window.obj console/console.obj filesys/file.obj filesys/kca.obj gui/jpeg.obj driver/cmos.obj \
		kernel/list.obj kernel/lock.obj gui/taskbar.obj gui/menu.obj gui/button.obj \
		kernel/util.obj gui/textbox.obj $(OBJS_CMD)

OBJS_CMD = console/cmd_mem.obj console/cmd_cls.obj console/cmd_dir.obj console/cmd_type.obj console/cmd_exit.obj \
		console/cmd_start.obj console/cmd_run.obj console/cmd_langmode.obj console/cmd_shutdown.obj console/cmd_sysmode.obj \
		console/cmd_echo.obj console/cmd_getruntime.obj

TOOLPATH = ../../z_tools/
INCPATH  = ../../z_tools/kalinote/

MAKE     = $(TOOLPATH)make.exe -r
NASK     = $(TOOLPATH)nask.exe
CC1      = $(TOOLPATH)cc1.exe -I$(INCPATH) -Os -Wall -quiet
GAS2NASK = $(TOOLPATH)gas2nask.exe -a
OBJ2BIM  = $(TOOLPATH)obj2bim.exe
MAKEFONT = $(TOOLPATH)makefont.exe
BIN2OBJ  = $(TOOLPATH)bin2obj.exe
BIM2KAL  = $(TOOLPATH)bim2kal.exe
RULEFILE = ../kalinote.rul
EDIMG    = $(TOOLPATH)edimg.exe
IMGTOL   = $(TOOLPATH)imgtol.com
GOLIB    = $(TOOLPATH)golib00.exe 
COPY     = copy
DEL      = del

# 默认动作

default :
	$(MAKE) ipl40.bin
	$(MAKE) kalinote.sys

# 文件生成规则

ipl40.bin : ipl40.nas Makefile
	$(NASK) ipl40.nas ipl40.bin ipl40.lst

asmhead.bin : asmhead.nas Makefile
	$(NASK) asmhead.nas asmhead.bin asmhead.lst

fonts.bin : fonts.txt Makefile
	$(MAKEFONT) fonts.txt fonts.bin

fonts.obj : fonts.bin Makefile
	$(BIN2OBJ) fonts.bin fonts.obj _fonts

bootpack.bim : $(OBJS_BOOTPACK) Makefile
	$(OBJ2BIM) @$(RULEFILE) out:bootpack.bim stack:3136k map:bootpack.map \
		$(OBJS_BOOTPACK)
# 3MB+64KB=3136KB

bootpack.kal : bootpack.bim Makefile
	$(BIM2KAL) bootpack.bim bootpack.kal 0

kalinote.sys : asmhead.bin bootpack.kal Makefile
	copy /B asmhead.bin+bootpack.kal kalinote.sys

# 一般规则

%.gas : %.c bootpack.h Makefile
	$(CC1) -o $*.gas $*.c

%.nas : %.gas Makefile
	$(GAS2NASK) $*.gas $*.nas

%.obj : %.nas Makefile
	$(NASK) $*.nas $*.obj $*.lst

# 命令

clean :
	$(MAKE) -C kernel		clean
	$(MAKE) -C driver		clean
	$(MAKE) -C filesys		clean
	$(MAKE) -C gui			clean
	$(MAKE) -C console		clean
	-$(DEL) asmhead.bin
	-$(DEL) fonts.bin
	-$(DEL) *.lst
	-$(DEL) *.obj
	-$(DEL) *.map
	-$(DEL) *.bim
	-$(DEL) *.kal
	-$(DEL) *.gas

src_only :
	$(MAKE) clean
	-$(DEL) ipl40.bin
	-$(DEL) kalinote.sys
