TOOLPATH = ../z_tools/

MAKE     = $(TOOLPATH)make.exe -r
EDIMG    = $(TOOLPATH)edimg.exe
IMGTOL   = $(TOOLPATH)imgtol.com
COPY     = copy
DEL      = del

# 默认操作

default :
	$(MAKE) kalinote.img

# 文件生成规则

kalinote.img : kalinote/mbr.bin kalinote/kalinote.sys Makefile \
		hello4/hello4.kal color/color.kal \
		winhelo/winhelo.kal star/star.kal counter/counter.kal \
		lines/lines.kal noodle/noodle.kal \
		beepdown/beepdown.kal beepup/beepup.kal sosu/sosu.kal \
		cat/cat.kal chklang/chklang.kal \
		notrec/notrec.kal bball/bball.kal calc/calc.kal \
		a2048/a2048.kal light/light.kal timeapp/timeapp.kal \
		pi/pi.kal sincurve/sincurve.kal runtime/runtime.kal \
		divzero/divzero.kal amiya/amiya.kal randint/randint.kal \
		aobench/aobench.kal shape/shape.kal
	$(EDIMG)   imgin:../z_tools/fdimg0at.tek \
		wbinimg src:kalinote/mbr.bin len:512 from:0 to:0 \
		copy from:kalinote/kalinote.sys to:@: \
		copy from:hello4/hello4.kal to:@: \
		copy from:winhelo/winhelo.kal to:@: \
		copy from:star/star.kal to:@: \
		copy from:lines/lines.kal to:@: \
		copy from:noodle/noodle.kal to:@: \
		copy from:beepdown/beepdown.kal to:@: \
		copy from:beepup/beepup.kal to:@: \
		copy from:color/color.kal to:@: \
		copy from:counter/counter.kal to:@: \
		copy from:sosu/sosu.kal to:@: \
		copy from:cat/cat.kal to:@: \
		copy from:chklang/chklang.kal to:@: \
		copy from:langs/HZK16.fnt to:@: \
		copy from:langs/nihongo.fnt to:@: \
		copy from:notrec/notrec.kal to:@: \
		copy from:bball/bball.kal to:@: \
		copy from:calc/calc.kal to:@: \
		copy from:resources/back.jpg to:@: \
		copy from:resources/startup.kbs to:@: \
		copy from:amiya/amiya.jpg to:@: \
		copy from:a2048/a2048.kal to:@: \
		copy from:light/light.kal to:@: \
		copy from:timeapp/timeapp.kal to:@: \
		copy from:pi/pi.kal to:@: \
		copy from:sincurve/sincurve.kal to:@: \
		copy from:runtime/runtime.kal to:@: \
		copy from:divzero/divzero.kal to:@: \
		copy from:amiya/amiya.kal to:@: \
		copy from:randint/randint.kal to:@: \
		copy from:aobench/aobench.kal to:@: \
		copy from:shape/shape.kal to:@: \
		imgout:kalinote.img

# 命令

run :
	$(MAKE) kalinote.img
	$(COPY) kalinote.img ..\z_tools\qemu\kalinoteos.bin
	$(COPY) kalinote.img ..\z_tools\qemu\kalinoteos.vfd
	$(MAKE) -C ../z_tools/qemu

install :
	$(MAKE) kalinote.img
	$(IMGTOL) w a: kalinote.img

full :
	$(MAKE) -C klibc
	$(MAKE) -C kalinote
	$(MAKE) -C hello4
	$(MAKE) -C winhelo
	$(MAKE) -C star
	$(MAKE) -C lines
	$(MAKE) -C noodle
	$(MAKE) -C beepdown
	$(MAKE) -C beepup
	$(MAKE) -C color
	$(MAKE) -C counter
	$(MAKE) -C sosu
	$(MAKE) -C cat
	$(MAKE) -C chklang
	$(MAKE) -C notrec
	$(MAKE) -C bball
	$(MAKE) -C calc
	$(MAKE) -C a2048
	$(MAKE) -C light
	$(MAKE) -C timeapp
	$(MAKE) -C pi
	$(MAKE) -C sincurve
	$(MAKE) -C runtime
	$(MAKE) -C divzero
	$(MAKE) -C amiya
	$(MAKE) -C randint
	$(MAKE) -C aobench
	$(MAKE) -C shape
	$(MAKE) kalinote.img
	$(COPY) kalinote.img ..\z_tools\qemu\kalinoteos.bin
	$(COPY) kalinote.img ..\z_tools\qemu\kalinoteos.vfd

run_full :
	$(MAKE) full
	$(COPY) kalinote.img ..\z_tools\qemu\kalinoteos.bin
	$(COPY) kalinote.img ..\z_tools\qemu\kalinoteos.vfd
	$(MAKE) -C ../z_tools/qemu

install_full :
	$(MAKE) full
	$(IMGTOL) w a: kalinote.img

run_os :
	$(MAKE) -C kalinote
	$(MAKE) run

os:
	$(MAKE) -C kalinote
	$(MAKE) kalinote.img
	$(COPY) kalinote.img ..\z_tools\qemu\kalinoteos.bin
	$(COPY) kalinote.img ..\z_tools\qemu\kalinoteos.vfd

clean :
# 没用

clean_os:
	$(MAKE) -C kalinote		clean

src_only :
	$(MAKE) clean
	-$(DEL) kalinote.img

clean_full :
	$(MAKE) -C klibc		clean
	$(MAKE) -C kalinote		clean
	$(MAKE) -C hello4		clean
	$(MAKE) -C winhelo		clean
	$(MAKE) -C star			clean
	$(MAKE) -C lines		clean
	$(MAKE) -C noodle		clean
	$(MAKE) -C beepdown		clean
	$(MAKE) -C beepup		clean
	$(MAKE) -C color		clean
	$(MAKE) -C counter		clean
	$(MAKE) -C sosu			clean
	$(MAKE) -C cat			clean
	$(MAKE) -C chklang		clean
	$(MAKE) -C notrec		clean
	$(MAKE) -C bball		clean
	$(MAKE) -C calc			clean
	$(MAKE) -C a2048		clean
	$(MAKE) -C light		clean
	$(MAKE) -C timeapp		clean
	$(MAKE) -C pi			clean
	$(MAKE) -C sincurve		clean
	$(MAKE) -C runtime		clean
	$(MAKE) -C divzero		clean
	$(MAKE) -C amiya		clean
	$(MAKE) -C randint		clean
	$(MAKE) -C aobench		clean
	$(MAKE) -C shape		clean

src_only_full :
	$(MAKE) -C klibc		src_only
	$(MAKE) -C kalinote		src_only
	$(MAKE) -C hello4		src_only
	$(MAKE) -C winhelo		src_only
	$(MAKE) -C star			src_only
	$(MAKE) -C lines		src_only
	$(MAKE) -C noodle		src_only
	$(MAKE) -C beepdown		src_only
	$(MAKE) -C beepup		src_only
	$(MAKE) -C color		src_only
	$(MAKE) -C counter		src_only
	$(MAKE) -C sosu			src_only
	$(MAKE) -C cat			src_only
	$(MAKE) -C chklang		src_only
	$(MAKE) -C notrec		src_only
	$(MAKE) -C bball		src_only
	$(MAKE) -C calc			src_only
	$(MAKE) -C a2048		src_only
	$(MAKE) -C light		src_only
	$(MAKE) -C timeapp		src_only
	$(MAKE) -C sincurve		src_only
	$(MAKE) -C runtime		src_only
	$(MAKE) -C divzero		src_only
	$(MAKE) -C amiya		src_only
	$(MAKE) -C randint		src_only
	$(MAKE) -C aobench		src_only
	$(MAKE) -C shape		src_only
	-$(DEL) kalinote.img

refresh :
	$(MAKE) full
	$(MAKE) clean_full
	-$(DEL) kalinote.img
