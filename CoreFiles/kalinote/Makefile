OBJS_BOOTPACK = kernel/bootpack.obj fonts.obj nasmfunc.obj gui/graphic.obj kernel/dsctbl.obj \
		kernel/int.obj kernel/fifo.obj driver/keyboard.obj driver/mouse.obj kernel/memory.obj kernel/sheet.obj driver/timer.obj \
		kernel/mtask.obj gui/window.obj console/console.obj filesys/fat12.obj filesys/kca.obj gui/jpeg.obj driver/cmos.obj \
		kernel/list.obj kernel/lock.obj gui/taskbar.obj gui/menu.obj gui/button.obj driver/acpi.obj console/exception.obj \
		kernel/util.obj gui/textbox.obj console/syscall.obj kernel/cpuid.obj driver/fdc.obj driver/io.obj filesys/buf.obj $(OBJS_CMD)

OBJS_CMD = console/cmd_mem.obj console/cmd_cls.obj console/cmd_dir.obj console/cmd_type.obj console/cmd_exit.obj \
		console/cmd_start.obj console/cmd_run.obj console/cmd_langmode.obj console/cmd_shutdown.obj console/cmd_sysmode.obj \
		console/cmd_echo.obj console/cmd_getruntime.obj console/cmd_sysinfo.obj console/cmd_pwd.obj console/cmd_cd.obj \
		console/cmd_testfunc.obj console/cmd_ps.obj
		
OBJS_GL = gui/TinyGL/api.obj gui/TinyGL/list.obj gui/TinyGL/vertex.obj gui/TinyGL/init.obj gui/TinyGL/matrix.obj gui/TinyGL/texture.obj \
      gui/TinyGL/misc.obj gui/TinyGL/clear.obj gui/TinyGL/light.obj gui/TinyGL/clip.obj gui/TinyGL/select.obj gui/TinyGL/get.obj gui/TinyGL/error.obj \
      gui/TinyGL/zbuffer.obj gui/TinyGL/zline.obj gui/TinyGL/zdither.obj gui/TinyGL/ztriangle.obj \
      gui/TinyGL/zmath.obj gui/TinyGL/image_util.obj gui/TinyGL/oscontext.obj gui/TinyGL/msghandling.obj \
      gui/TinyGL/arrays.obj gui/TinyGL/specbuf.obj gui/TinyGL/memory.obj gui/TinyGL/glx.obj
	  
TOOLPATH = ../../z_tools/
INCPATH  = ./libc/include/

MAKE     = $(TOOLPATH)make.exe -r
NASK     = $(TOOLPATH)nask.exe
NASM	 = nasm
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
	$(MAKE) -C libc
	$(MAKE) mbr.bin
	$(MAKE) kalinote.sys

# 文件生成规则

mbr.bin : mbr.asm Makefile
	$(NASM) -f bin mbr.asm -o mbr.bin -l mbr.lst

asmhead.bin : asmhead.asm Makefile
	$(NASM) -f bin asmhead.asm -o asmhead.bin -l asmhead.lst

nasmfunc.obj : nasmfunc.asm Makefile
	$(NASM) -f coff -l nasmfunc.lst nasmfunc.asm -o nasmfunc.obj

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
	$(MAKE) -C libc			clean
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
	$(MAKE) -C libc		src_only
	-$(DEL) mbr.bin
	-$(DEL) kalinote.sys
