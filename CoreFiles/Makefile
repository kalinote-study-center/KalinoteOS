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

ipl.bin : ipl.nas Makefile
	$(NASK) ipl.nas ipl.bin ipl.lst

kalinoteos.img : ipl.bin Makefile
	$(EDIMG)   imgin:../Tools/maketools/z_tools/fdimg0at.tek \
		wbinimg src:ipl.bin len:512 from:0 to:0   imgout:kalinoteos.img

# 命令

asm :
	$(MAKE) ipl.bin

img :
	$(MAKE) kalinoteos.img

run :
	$(MAKE) img
	$(COPY) kalinoteos.img ..\Tools\maketools\z_tools\qemu\fdimage0.bin
	$(MAKE) -C ../Tools/maketools/z_tools/qemu

install :
	$(MAKE) img
	$(IMGTOL) w a: kalinoteos.img

clean :
	-$(DEL) ipl.bin
	-$(DEL) ipl.lst

src_only :
	$(MAKE) clean
	-$(DEL) kalinoteos.img
