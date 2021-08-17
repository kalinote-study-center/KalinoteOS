TARGET     = golibc
STACKSIZE  =
MALLOCSIZE =
MMAREA     =
OBJS       = abs.obj atof.obj atoi.obj errno.obj frexp.obj ldexp.obj \
	memchr.obj memcmp.obj memmove.obj memset.obj qsort.obj rand.obj \
	sprintf.obj strcat.obj strcmp.obj strcpy.obj strcspn.obj strdup.obj \
	strlen.obj strncat.obj strncmp.obj strncpy.obj strpbrk.obj strrchr.obj \
	strspn.obj strstr.obj strtol.obj strtoul.obj strtoul0.obj vsprintf.obj \
	malloc.obj free.obj printf.obj putchar.obj scanf.obj getchar.obj \
	puts.obj gets.obj isdigit.obj isspace.obj isalnum.obj isalpha.obj \
	iscntrl.obj isgraph.obj islower.obj isprint.obj ispunct.obj isupper.obj \
	isxdigit.obj tolower.obj toupper.obj atol.obj calloc.obj system.obj \
	fabs.obj

# 有空把makefile整理以下，不然标准函数太多太乱

KALIPATH	= ..\\..\\z_tools\\kalinote\\
APILIBPATH	= ../apilib/
 
TOOLPATH =
INCPATH  = .
RULEFILE =
MAKE     = $(TOOLPATH)make -r
SJISCONV = $(TOOLPATH)sjisconv -s
CC1      = $(TOOLPATH)cc1 -I$(INCPATH) -Os -quiet -W -Wall -Werror
GAS2NASK = $(TOOLPATH)gas2nask -a
NASK     = $(TOOLPATH)nask
OBJ2BIM  = $(TOOLPATH)obj2bim3
BIM2BIN  = $(TOOLPATH)bim2bin3
CPP0     = $(TOOLPATH)cpp0 -P -I$(INCPATH)
ASKA     = $(TOOLPATH)aska
NASKCNV  = $(TOOLPATH)naskcnv0 -l -s -w
GOLIB    = $(TOOLPATH)golib00
COPY     = copy
DELE     = del


ALL :
	$(MAKE) $(TARGET).lib
	$(COPY)	$(TARGET).lib $(KALIPATH)$(TARGET).lib
	$(COPY) *.h $(KALIPATH)

%.ca : %.c Makefile
	$(SJISCONV) $*.c $*.ca

%.gas : %.ca Makefile
	$(CC1) -o $*.gas $*.ca

%.nas : %.gas Makefile
	$(GAS2NASK) $*.gas $*.nas

%.obj : %.nas Makefile
	$(NASK) $*.nas $*.obj

%.ias : %.ask Makefile
	$(CPP0) -o $*.ias $*.ask

%.3as : %.ias Makefile
	$(ASKA) $*.ias $*.3as

%.nas : %.3as Makefile
	$(NASKCNV) $*.3as $*.nas

%.lst : %.nas Makefile
	$(NASK) $*.nas $*.obj $*.lst

$(TARGET).bim : $(OBJS) Makefile
	$(OBJ2BIM) @$(RULEFILE) out:$(TARGET).bim stack:$(STACKSIZE) map:$(TARGET).map $(OBJS) \
		$(APILIBPATH)apilib.lib

%.bin : %.bim Makefile
	$(BIM2BIN) in:$*.bim out:$*.org malloc:$(MALLOCSIZE) mmarea:$(MMAREA)
	$(BIM2BIN) in:$*.org out:$*.bin -osacmp -tek0

$(TARGET).lib : $(OBJS) Makefile
	$(GOLIB) out:$@ $(OBJS)

clean :
	-$(DELE) *.obj
	-$(DELE) $(TARGET).bim
	-$(DELE) $(TARGET).map
	-$(DELE) $(TARGET).org
	-$(DELE) $(TARGET).lib
