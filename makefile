#
# Makefile for gcc (linux; djgpp; mingw32)
#
# Special options:
#   make with mingw32:   make MINGW32=1
#   make without Libnet: make WITHOUT_LIBNET=1
#

CC = gcc
CFLAGS = -Wall -O3 -m486 -Isrc -Isrc/include -Isrc/sk

ifdef MINGW32
	PLATFORM = WINDOWS
	GAME = redwin.exe
	LDFLAGS = -mwindows
	ALLEGRO = -lalleg
	LIBNET = -lnet -lwsock32
	PLATFORM_MODULES = getopt skdummy
	OBJDIR = obj/win
else
ifdef DJDIR
	PLATFORM = DJGPP
	GAME = reddos.exe
	ALLEGRO = -lalleg
	LIBNET = -lnet
	PLATFORM_MODULES = skdos
	OBJDIR = obj/djgpp
else
	PLATFORM = LINUX
	GAME = redpixel
	ifndef STATIC
		ALLEGRO = `allegro-config --libs`
	else
	    	# Personal use only.
		ALLEGRO = $(HOME)/static/liballeg.a \
			-L/usr/X11R6/lib -lXxf86dga -lXxf86vm -lXext -lX11
	endif
	LIBNET = -lnet
	PLATFORM_MODULES = sklinux
	OBJDIR = obj/linux
endif
endif

CFLAGS += -DTARGET_$(PLATFORM)
LIBS = $(ALLEGRO) 

ifndef WITHOUT_LIBNET
	CFLAGS += -DLIBNET_CODE
	LIBS += $(LIBNET)
endif

COMMON_MODULES = \
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
	particle	\
	player		\
	plupdate	\
	resource	\
	rg_rand		\
	rnd		\
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
	
MODULES = $(COMMON_MODULES) $(PLATFORM_MODULES)

OBJS = $(addprefix $(OBJDIR)/,$(addsuffix .o,$(MODULES)))


vpath %.c src src/engine src/sk src/fastsqrt

$(OBJDIR)/%.o: %.c
	$(COMPILE.c) -o $@ $<

$(GAME): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

obj/depend:
	gcc $(CFLAGS) -MM src/*.c src/sk/*.c | sed 's,^\(.*[.]o:\),obj/\1,' > $@


.PHONY = depend compress suidroot clean cleaner

depend: obj/depend

compress: $(GAME)
	upx $<

suidroot:
	chown root.allegro $(GAME)
	chmod 4750 $(GAME)

clean: 
	rm -f $(OBJS) core

cleaner: 
	rm -f $(GAME) obj/depend 


-include obj/depend

