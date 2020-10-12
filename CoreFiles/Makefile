# KalinoteOS生成批处理文件
OBJS_BOOTPACK = bootpack.obj naskfunc.obj fonts.obj graphic.obj dsctbl.obj \
		int.obj fifo.obj keyboard.obj mouse.obj memory.obj sheet.obj timer.obj

TOOLPATH = ../z_tools/
INCPATH  = ../z_tools/kalinote/

MAKE     = $(TOOLPATH)make.exe -r
NASK     = $(TOOLPATH)nask.exe
CC1      = $(TOOLPATH)cc1.exe -I$(INCPATH) -Os -Wall -quiet
GAS2NASK = $(TOOLPATH)gas2nask.exe -a
OBJ2BIM  = $(TOOLPATH)obj2bim.exe
BIM2KAL  = $(TOOLPATH)bim2kal.exe
MAKEFONT = $(TOOLPATH)makefont.exe
BIN2OBJ  = $(TOOLPATH)bin2obj.exe
RULEFILE = $(TOOLPATH)kalinote/kalinote.rul
EDIMG    = $(TOOLPATH)edimg.exe
IMGTOL   = $(TOOLPATH)imgtol.com
COPY     = copy
DEL      = del

# 默认

default :
	$(MAKE) img

# 文件生成规则

ipl10.bin : ipl10.nas Makefile
	$(NASK) ipl10.nas ipl10.bin ipl10.lst

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

kalinote.img : ipl10.bin kalinote.sys Makefile
	$(EDIMG)   imgin:../z_tools/fdimg0at.tek \
		wbinimg src:ipl10.bin len:512 from:0 to:0 \
		copy from:kalinote.sys to:@: \
		imgout:kalinote.img

# 一般规则

%.gas : %.c bootpack.h Makefile
	$(CC1) -o $*.gas $*.c

%.nas : %.gas Makefile
	$(GAS2NASK) $*.gas $*.nas

%.obj : %.nas Makefile
	$(NASK) $*.nas $*.obj $*.lst

# 命令

img :
	$(MAKE) kalinote.img

run :
	$(MAKE) img
	$(COPY) kalinote.img ..\z_tools\qemu\fdimage0.bin
	$(MAKE) -C ../z_tools/qemu

install :
	$(MAKE) img
	$(IMGTOL) w a: kalinote.img

clean :
	-$(DEL) *.bin
	-$(DEL) *.lst
	-$(DEL) *.obj
	-$(DEL) bootpack.map
	-$(DEL) bootpack.bim
	-$(DEL) bootpack.kal
	-$(DEL) kalinote.sys

src_only :
	$(MAKE) clean
	-$(DEL) kalinote.img
