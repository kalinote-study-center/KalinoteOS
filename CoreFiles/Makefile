TOOLPATH = ../z_tools/
INCPATH  = ../z_tools/kalinote/

MAKE     = $(TOOLPATH)make.exe -r
EDIMG    = $(TOOLPATH)edimg.exe
IMGTOL   = $(TOOLPATH)imgtol.com
COPY     = copy
DEL      = del

# 默??作

default :
	$(MAKE) kalinote.img

# 文件生成??

kalinote.img : kalinote/ipl10.bin kalinote/kalinote.sys Makefile \
		a/a.kal hello3/hello3.kal hello4/hello4.kal color/color.kal \
		winhelo/winhelo.kal star/star.kal counter/counter.kal \
		lines/lines.kal walk/walk.kal noodle/noodle.kal typeipl/typeipl.kal \
		beepdown/beepdown.kal beepup/beepup.kal sosu/sosu.kal
	$(EDIMG)   imgin:../z_tools/fdimg0at.tek \
		wbinimg src:kalinote/ipl10.bin len:512 from:0 to:0 \
		copy from:kalinote/kalinote.sys to:@: \
		copy from:kalinote/ipl10.nas to:@: \
		copy from:make.bat to:@: \
		copy from:a/a.kal to:@: \
		copy from:hello3/hello3.kal to:@: \
		copy from:hello4/hello4.kal to:@: \
		copy from:winhelo/winhelo.kal to:@: \
		copy from:star/star.kal to:@: \
		copy from:lines/lines.kal to:@: \
		copy from:walk/walk.kal to:@: \
		copy from:noodle/noodle.kal to:@: \
		copy from:beepdown/beepdown.kal to:@: \
		copy from:beepup/beepup.kal to:@: \
		copy from:color/color.kal to:@: \
		copy from:counter/counter.kal to:@: \
		copy from:sosu/sosu.kal to:@: \
		copy from:typeipl/typeipl.kal to:@: \
		imgout:kalinote.img

# 命令

run :
	$(MAKE) kalinote.img
	$(COPY) kalinote.img ..\z_tools\qemu\kalinoteos.bin
	$(MAKE) -C ../z_tools/qemu

install :
	$(MAKE) kalinote.img
	$(IMGTOL) w a: kalinote.img

full :
	$(MAKE) -C kalinote
	$(MAKE) -C apilib
	$(MAKE) -C a
	$(MAKE) -C hello3
	$(MAKE) -C hello4
	$(MAKE) -C winhelo
	$(MAKE) -C star
	$(MAKE) -C lines
	$(MAKE) -C walk
	$(MAKE) -C noodle
	$(MAKE) -C beepdown
	$(MAKE) -C beepup
	$(MAKE) -C color
	$(MAKE) -C counter
	$(MAKE) -C sosu
	$(MAKE) -C typeipl
	$(MAKE) kalinote.img

run_full :
	$(MAKE) full
	$(COPY) kalinote.img ..\z_tools\qemu\kalinoteos.bin
	$(MAKE) -C ../z_tools/qemu

install_full :
	$(MAKE) full
	$(IMGTOL) w a: kalinote.img

run_os :
	$(MAKE) -C kalinote
	$(MAKE) run

clean :
# 何もしない

src_only :
	$(MAKE) clean
	-$(DEL) kalinote.img

clean_full :
	$(MAKE) -C kalinote		clean
	$(MAKE) -C apilib		clean
	$(MAKE) -C a			clean
	$(MAKE) -C hello3		clean
	$(MAKE) -C hello4		clean
	$(MAKE) -C winhelo		clean
	$(MAKE) -C star			clean
	$(MAKE) -C lines		clean
	$(MAKE) -C walk			clean
	$(MAKE) -C noodle		clean
	$(MAKE) -C beepdown		clean
	$(MAKE) -C beepup		clean
	$(MAKE) -C color		clean
	$(MAKE) -C counter		clean
	$(MAKE) -C sosu			clean
	$(MAKE) -C typeipl		clean

src_only_full :
	$(MAKE) -C kalinote		src_only
	$(MAKE) -C apilib		src_only
	$(MAKE) -C a			src_only
	$(MAKE) -C hello3		src_only
	$(MAKE) -C hello4		src_only
	$(MAKE) -C winhelo		src_only
	$(MAKE) -C star			src_only
	$(MAKE) -C lines		src_only
	$(MAKE) -C walk			src_only
	$(MAKE) -C noodle		src_only
	$(MAKE) -C beepdown		src_only
	$(MAKE) -C beepup		src_only
	$(MAKE) -C color		src_only
	$(MAKE) -C counter		src_only
	$(MAKE) -C sosu			src_only
	$(MAKE) -C typeipl		src_only
	-$(DEL) kalinote.img

refresh :
	$(MAKE) full
	$(MAKE) clean_full
	-$(DEL) kalinote.img
