OBJS_API =	api_putchar.obj api_putstr0.obj api_putstr1.obj api_end.obj api_openwin.obj api_putstrwin.obj \
			api_boxfilwin.obj api_initmalloc.obj api_malloc.obj api_free.obj api_point.obj api_refreshwin.obj \
			api_linewin.obj api_closewin.obj api_getkey.obj api_alloctimer.obj api_inittimer.obj api_settimer.obj \
			api_freetimer.obj api_beep.obj api_fopen.obj api_fclose.obj api_fseek.obj api_fsize.obj \
			api_fread.obj api_cmdline.obj api_getlang.obj api_cls.obj api_get_year.obj api_get_mon_hex.obj \
			api_get_day_of_week.obj api_get_day_of_month.obj api_get_hour_hex.obj api_get_min_hex.obj api_get_sec_hex.obj api_get_system_runtime.obj \
			api_showjpeg.obj api_send_key_to_cmd.obj \
			alloca.obj

TOOLPATH = ../../../z_tools/
INCPATH  = ../include/

MAKE     = $(TOOLPATH)make.exe -r
NASK     = $(TOOLPATH)nask.exe
GOLIB    = $(TOOLPATH)golib00.exe 
DEL      = del

default :
	$(MAKE) apilib.lib

apilib.lib : Makefile $(OBJS_API)
	$(GOLIB) $(OBJS_API) out:apilib.lib

%.obj : %.nas Makefile
	$(NASK) $*.nas $*.obj $*.lst

clean :
	-$(DEL) *.lst
	-$(DEL) *.obj

src_only :
	$(MAKE) clean
	-$(DEL) apilib.lib