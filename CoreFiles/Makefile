# KalinoteOS生成批处理文件

TOOLPATH = ../Tools/maketools/z_tools/
MAKE     = $(TOOLPATH)make.exe -r
NASK     = $(TOOLPATH)nask.exe
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

kalinote.sys : kalinote.nas Makefile
	$(NASK) kalinote.nas kalinote.sys kalinote.lst

kalinote.img : ipl10.bin kalinote.sys Makefile
	$(EDIMG)   imgin:../Tools/maketools/z_tools/fdimg0at.tek \
		wbinimg src:ipl10.bin len:512 from:0 to:0 \
		copy from:kalinote.sys to:@: \
		imgout:kalinote.img

# 命令

img :
	$(MAKE) kalinote.img

run :
	$(MAKE) img
	$(COPY) kalinote.img ..\Tools\maketools\z_tools\qemu\fdimage0.bin
	$(MAKE) -C ../Tools/maketools/z_tools/qemu

install :
	$(MAKE) img
	$(IMGTOL) w a: kalinote.img

clean :
	-$(DEL) ipl10.bin
	-$(DEL) ipl10.lst
	-$(DEL) kalinote.sys
	-$(DEL) kalinote.lst

src_only :
	$(MAKE) clean
	-$(DEL) kalinote.img
