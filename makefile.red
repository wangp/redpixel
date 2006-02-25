# -*- makefile -*-
#
# Makefile for Red Pixel itself.
#


CC := gcc
INCLUDES :=	-Isrc -Isrc/include -Isrc/sk			\
		-Ilibnet/include -Ilibcda -Ijgmod/src -Iagup
CFLAGS :=	-W -Wall -Wno-deprecated-declarations -O2 	\
		-ffast-math -funroll-loops $(INCLUDES)

ifdef DEBUG
CFLAGS += -g
endif


ifeq "$(PLATFORM)" "LINUX"
CFLAGS += -DTARGET_LINUX
GAME := redpixel
ifdef DEBUG
ALLEGRO := `allegro-config --libs debug` 
else
ALLEGRO := `allegro-config --libs` 
endif
LIBS := $(ALLEGRO) $(LIBNET) -lpthread $(LIBCDA) $(JGMOD) $(AGUP)
PLATFORM_MODULES := sklinux
OBJDIR := obj/linux
endif

ifeq "$(PLATFORM)" "MINGW"
CFLAGS += -DTARGET_WINDOWS
GAME := redwin.exe
LDFLAGS := -Wl,--subsystem,windows
ALLEGRO := -lalleg
LIBS := $(LIBNET) -lwsock32 $(LIBCDA) -lwinmm $(JGMOD) $(AGUP) $(ALLEGRO)
PLATFORM_MODULES := getopt skdummy
OBJDIR := obj/win
endif

ifeq "$(PLATFORM)" "DJGPP"
CFLAGS += -DTARGET_DJGPP
GAME := reddos.exe
ALLEGRO := -lalleg
LIBS := $(LIBNET) $(LIBCDA) $(JGMOD) $(AGUP) $(ALLEGRO)
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
	cpu		\
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


# icon stuff for Windows

ifeq "$(PLATFORM)" "MINGW"

WFIXICON := $(OBJDIR)/wfixicon.exe
RES := $(OBJDIR)/tmp.res

$(WFIXICON): misc/wfixicon.c
	$(CC) -o $@ $< $(ALLEGRO)

$(RES): $(WFIXICON)
	$(WFIXICON) $(OBJDIR)/tmp.ico -ro misc/icon.bmp

OBJS += $(RES)

endif


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
	chown root.games $(GAME)
	chmod 4750 $(GAME)

strip:
	strip $(GAME)

# don't delete obj/depend because it requires sed to remake
# however, make depend before making a distribution

clean: 
	rm -f $(OBJS) src/TAGS core.*

cleaner: clean
	rm -f $(GAME) $(WFIXICON) $(RES) $(OBJDIR)/tmp.ico

distclean: cleaner
	make -C agup clean
	make -C jgmod/src -f $(JGMOD_MAKEFILE) clean
	make -C libcda clean
	make -C libnet cleaner


-include obj/depend


# (un)installation for Linux

VERSION := 1.0

ifeq "$(PLATFORM)" "LINUX"

prefix := /usr/local
bindir := $(prefix)/games
docdir := $(prefix)/doc/redpixel-$(VERSION)
sharedir := $(prefix)/share/redpixel

INSTALL := install -p

install: $(GAME)
	$(INSTALL) -m 755 -D -s $(GAME) $(bindir)/$(GAME)
	$(INSTALL) -m 644 -D blood.dat $(sharedir)/blood.dat
	for thedir in demos maps music stats; do					\
		files=`find $${thedir} -type f -print`;					\
		for thefile in $${files}; do						\
			$(INSTALL) -m 644 -D $${thefile} $(sharedir)/$${thefile};	\
		done;									\
	done

uninstall:
	rm -f $(bindir)/$(GAME)
	rm -fr $(docdir) $(sharedir)

endif


# vi: syntax=make
