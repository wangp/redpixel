# -*- makefile -*-
#
# Makefile for Red Pixel itself.
#


CC := gcc
INCLUDES :=	-Isrc -Isrc/include -Isrc/sk			\
		-Ilibnet/include -Ilibcda -Ijgmod/src -Iagup
CFLAGS :=	-Wall -O2 -mcpu=pentium -ffast-math		\
		-fomit-frame-pointer -funroll-loops $(INCLUDES)

ifdef DEBUG
CFLAGS += -g
endif


ifeq "$(PLATFORM)" "LINUX"
CFLAGS += -DTARGET_LINUX
GAME := redpixel
LIBS := `allegro-config --libs` $(LIBNET) -lpthread $(LIBCDA) $(JGMOD) $(AGUP)
PLATFORM_MODULES := sklinux
OBJDIR := obj/linux
endif

ifeq "$(PLATFORM)" "MINGW"
CFLAGS += -DTARGET_WINDOWS
GAME := redwin.exe
LDFLAGS := -mwindows
LIBS := $(LIBNET) -lwsock32 $(LIBCDA) -lwinmm $(JGMOD) $(AGUP) -lalleg
PLATFORM_MODULES := getopt skdummy
OBJDIR := obj/win
endif

ifeq "$(PLATFORM)" "DJGPP"
CFLAGS += -DTARGET_DJGPP
GAME := reddos.exe
LIBS := -lalleg $(LIBNET) $(LIBCDA) $(JGMOD) $(AGUP)
PLATFORM_MODULES := skdos
OBJDIR := obj/djgpp
endif


thegame: $(GAME)


COMMON_MODULES := \
	backpack	\
	blast		\
	blod		\
	bullet		\
	corpse		\
	credits		\
	demintro	\
	demo		\
	engine		\
	explo		\
	fastsqrt	\
	fblit		\
	gameloop	\
	globals		\
	inp_demo	\
	inp_peer	\
	intro		\
	launch		\
	main		\
	map		\
	mapper		\
	menu		\
	message		\
	mine		\
	mousespr	\
	music		\
	options		\
	particle	\
	player		\
	plupdate	\
	resource	\
	rg_rand		\
	rnd		\
	rpagup		\
	rpcd		\
	rpjgmod		\
	setweaps	\
	sk		\
	sklibnet	\
	skhelp		\
	sound		\
	stats		\
	statlist	\
	suicide		\
	tiles		\
	vector		\
	vidmode		\
	weapon

MODULES := $(COMMON_MODULES) $(PLATFORM_MODULES)

OBJS := $(addprefix $(OBJDIR)/,$(addsuffix .o,$(MODULES)))


vpath %.c src src/engine src/sk src/fastsqrt

$(GAME): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

$(OBJDIR)/%.o: %.c
	$(COMPILE.c) -o $@ $<

# compile without optimisations for buggy gccs (at least, RedHat's gcc 2.96)
$(OBJDIR)/fastsqrt.o: src/fastsqrt/fastsqrt.c
	$(CC) -Wall $(INCLUDES) -c -o $@ $<

obj/depend:
	gcc $(CFLAGS) -MM src/*.c src/sk/*.c		\
		| sed -e 's,^\(.*[.]o:\),obj/*/\1,'	\
		| sed -e 's,/usr/[^ ]\+,,g'		\
		| sed -e '/^ *\\$$/d' > $@


.PHONY := depend compress suidroot strip clean cleaner 

depend:
	-rm obj/depend
	$(MAKE) obj/depend

compress: $(GAME)
	upx $<

suidroot:
	chown root.allegro $(GAME)
	chmod 4750 $(GAME)

strip:
	strip $(GAME)

clean: 
	rm -f $(OBJS) core

cleaner: clean
	rm -f $(GAME) obj/depend 


-include obj/depend

