# -*- makefile -*-
#
# Makefile for Red Pixel itself.
#


CC := gcc
INCLUDES := -Isrc -Isrc/include -Isrc/sk -Ilibnet/include -Ilibcda -Ijgmod/src
CFLAGS := -Wall -O2 -mcpu=pentium -ffast-math -fomit-frame-pointer \
	-funroll-loops $(INCLUDES)

ifdef DEBUG
CFLAGS += -g
endif


ifeq "$(PLATFORM)" "LINUX"
CFLAGS += -DTARGET_LINUX
GAME := redpixel
LIBS := `allegro-config --libs` $(LIBNET) -lpthread $(LIBCDA) $(JGMOD)
PLATFORM_MODULES := sklinux
OBJDIR := obj/linux
endif

ifeq "$(PLATFORM)" "MINGW"
CFLAGS += -DTARGET_WINDOWS
GAME := redwin.exe
LDFLAGS := -mwindows
LIBS := -lalleg $(LIBNET) -lwsock32 $(LIBCDA) $(JGMOD)
PLATFORM_MODULES := getopt skdummy
OBJDIR := obj/win
endif

ifeq "$(PLATFORM)" "DJGPP"
CFLAGS += -DTARGET_DJGPP
GAME := reddos.exe
LIBS := -lalleg $(LIBNET) $(LIBCDA) $(JGMOD)
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
	music		\
	particle	\
	player		\
	plupdate	\
	resource	\
	rg_rand		\
	rnd		\
	rpcd		\
	rpjgmod		\
	scrblit		\
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
	gcc $(CFLAGS) -MM src/*.c src/sk/*.c | sed 's,^\(.*[.]o:\),obj/*/\1,' > $@


.PHONY := depend compress suidroot strip clean cleaner 

depend: obj/depend

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

