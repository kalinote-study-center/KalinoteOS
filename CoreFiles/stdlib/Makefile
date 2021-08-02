OBJS_API =	stdlib.obj malloc.obj stdio.obj ctype.obj string.obj graphics.obj

TOOLPATH = ../../z_tools/
INCPATH  = ../../z_tools/kalinote/

MAKE     = $(TOOLPATH)make.exe -r
NASK     = $(TOOLPATH)nask.exe
CC1      = $(TOOLPATH)cc1.exe -I$(INCPATH) -Os -Wall -quiet
GAS2NASK = $(TOOLPATH)gas2nask.exe -a
BIN2OBJ  = $(TOOLPATH)bin2obj.exe
GOLIB    = $(TOOLPATH)golib00.exe 
COPY     = copy
DEL      = del

#默认动作

default :
	$(MAKE) stdlib.lib

#库生成规则

stdlib.lib : Makefile $(OBJS_API)
	$(GOLIB) $(OBJS_API) out:stdlib.lib

#文件生成规则

%.gas : %.c ../stdlib.h Makefile
	$(CC1) -o $*.gas $*.c

%.nas : %.gas Makefile
	$(GAS2NASK) $*.gas $*.nas

%.obj : %.nas Makefile
	$(NASK) $*.nas $*.obj $*.lst

#命令

clean :
	-$(DEL) *.lst
	-$(DEL) *.obj

src_only :
	$(MAKE) clean
	-$(DEL) stdlib.lib