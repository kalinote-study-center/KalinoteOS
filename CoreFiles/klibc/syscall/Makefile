OBJS_API =	api_putchar.obj api_putstr0.obj api_putstr1.obj api004.obj api005.obj api006.obj \
			api007.obj api008.obj api009.obj api010.obj api011.obj api012.obj \
			api013.obj api014.obj api015.obj api016.obj api017.obj api018.obj \
			api019.obj api020.obj api021.obj api022.obj api023.obj api024.obj \
			api025.obj api026.obj api027.obj api029.obj api030.obj api031.obj \
			api032.obj api033.obj api034.obj api035.obj api036.obj api037.obj \
			api038.obj api039.obj \
			alloca.obj

TOOLPATH = ../../../z_tools/
INCPATH  = ../include/

MAKE     = $(TOOLPATH)make.exe -r
NASK     = $(TOOLPATH)nask.exe
GOLIB    = $(TOOLPATH)golib00.exe 
DEL      = del

default :
	$(MAKE) apilib.lib
	$(MAKE) -C entrypoint

apilib.lib : Makefile $(OBJS_API)
	$(GOLIB) $(OBJS_API) out:apilib.lib

%.obj : %.nas Makefile
	$(NASK) $*.nas $*.obj $*.lst

clean :
	-$(DEL) *.lst
	-$(DEL) *.obj
	$(MAKE) -C entrypoint		clean

src_only :
	$(MAKE) clean
	-$(DEL) apilib.lib
	$(MAKE) -C entrypoint		src_only