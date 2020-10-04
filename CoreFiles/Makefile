# KalinoteOS生成批处理文件

# 默认

default :
	../Tools/maketools/z_tools/make.exe img

# 文件生成规则

ipl.bin : ipl.nas Makefile
	../Tools/maketools/z_tools/nask.exe ipl.nas ipl.bin ipl.lst

kalinoteos.img : ipl.bin Makefile
	../Tools/maketools/z_tools/edimg.exe   imgin:../Tools/maketools/z_tools/fdimg0at.tek \
		wbinimg src:ipl.bin len:512 from:0 to:0   imgout:kalinoteos.img

# 命令

asm :
	../Tools/maketools/z_tools/make.exe -r ipl.bin

img :
	../Tools/maketools/z_tools/make.exe -r kalinoteos.img

run :
	../Tools/maketools/z_tools/make.exe img
	copy kalinoteos.img ..\Tools\maketools\z_tools\qemu\fdimage0.bin
	../Tools/maketools/z_tools/make.exe -C ../Tools/maketools/z_tools/qemu

install :
	../Tools/maketools/z_tools/make.exe img
	../Tools/maketools/z_tools/imgtol.com w a: kalinoteos.img

clean :
	-del ipl.bin
	-del ipl.lst

src_only :
	../Tools/maketools/z_tools/make.exe clean
	-del kalinoteos.img
