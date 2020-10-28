# KalinoteOS生成批处理文件
OBJS_BOOTPACK = bootpack.obj naskfunc.obj fonts.obj graphic.obj dsctbl.obj \
		int.obj fifo.obj keyboard.obj mouse.obj memory.obj sheet.obj timer.obj \
		mtask.obj window.obj console.obj file.obj

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
	$(COPY) kalinote.img D:\VMFile\kalinote.img
	$(COPY) kalinote.img D:\Projects\OSProject\kalinoteOS\z_tools\qemu\kalinoteos.vfd

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

hello.kal : hello.nas Makefile
	$(NASK) hello.nas hello.kal hello.lst

hello2.kal : hello2.nas Makefile
	$(NASK) hello2.nas hello2.kal hello2.lst

a.bim : a.obj a_nask.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:a.bim map:a.map a.obj a_nask.obj

a.kal : a.bim Makefile
	$(BIM2KAL) a.bim a.kal 0

hello3.bim : hello3.obj a_nask.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:hello3.bim map:hello3.map hello3.obj a_nask.obj

hello3.kal : hello3.bim Makefile
	$(BIM2KAL) hello3.bim hello3.kal 0

hello4.bim : hello4.obj a_nask.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:hello4.bim map:hello4.map hello4.obj a_nask.obj

hello4.kal : hello4.bim Makefile
	$(BIM2KAL) hello4.bim hello4.kal 0

winhelo.bim : winhelo.obj a_nask.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:winhelo.bim map:winhelo.map winhelo.obj a_nask.obj

winhelo.kal : winhelo.bim Makefile
	$(BIM2KAL) winhelo.bim winhelo.kal 0

star.bim : star.obj a_nask.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:star.bim map:star.map star.obj a_nask.obj

star.kal : star.bim Makefile
	$(BIM2KAL) star.bim star.kal 0

lines.bim : lines.obj a_nask.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:lines.bim map:lines.map lines.obj a_nask.obj

lines.kal : lines.bim Makefile
	$(BIM2KAL) lines.bim lines.kal 0

walk.bim : walk.obj a_nask.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:walk.bim map:walk.map walk.obj a_nask.obj

walk.kal : walk.bim Makefile
	$(BIM2KAL) walk.bim walk.kal 0

counter.bim : counter.obj a_nask.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:counter.bim map:counter.map counter.obj a_nask.obj

counter.kal : counter.bim Makefile
	$(BIM2KAL) counter.bim counter.kal 0

noodle.bim : noodle.obj a_nask.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:noodle.bim map:noodle.map noodle.obj a_nask.obj

noodle.kal : noodle.bim Makefile
	$(BIM2KAL) noodle.bim noodle.kal 0

kalinote.img : ipl10.bin kalinote.sys Makefile \
		hello.kal hello2.kal hello3.kal hello4.kal \
		winhelo.kal star.kal lines.kal walk.kal \
		counter.kal noodle.kal
	$(EDIMG)   imgin:../z_tools/fdimg0at.tek \
		wbinimg src:ipl10.bin len:512 from:0 to:0 \
		copy from:kalinote.sys to:@: \
		copy from:hello.kal to:@: \
		copy from:hello2.kal to:@: \
		copy from:hello3.kal to:@: \
		copy from:hello4.kal to:@: \
		copy from:winhelo.kal to:@: \
		copy from:star.kal to:@: \
		copy from:lines.kal to:@: \
		copy from:walk.kal to:@: \
		copy from:counter.kal to:@: \
		copy from:noodle.kal to:@: \
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
	$(COPY) kalinote.img D:\VMFile\kalinote.img
	$(COPY) kalinote.img D:\Projects\OSProject\kalinoteOS\z_tools\qemu\kalinoteos.vfd

run :
	$(MAKE) img
	$(COPY) kalinote.img ..\z_tools\qemu\kalinoteos.bin
	$(COPY) kalinote.img D:\VMFile\kalinote.img
	$(COPY) kalinote.img D:\Projects\OSProject\kalinoteOS\z_tools\qemu\kalinoteos.vfd
	$(MAKE) -C ../z_tools/qemu

install :
	$(MAKE) img
	$(IMGTOL) w a: kalinote.img

clean :
	-$(DEL) *.bin
	-$(DEL) *.lst
	-$(DEL) *.obj
	-$(DEL) *.map
	-$(DEL) *.bim
	-$(DEL) *.kal
	-$(DEL) kalinote.sys

src_only :
	$(MAKE) clean
	-$(DEL) kalinote.img
