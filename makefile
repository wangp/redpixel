CC = gcc
CFLAGS = -Wall -O3 -m486 -Isrc -Isrc/include -Isrc/sk

ifdef DJDIR
	# djgpp.
	GAME = red.exe
	LIBS = -lalleg
	CFLAGS += -DTARGET_DJGPP
else
	# Assume Linux.
	GAME = redpixel
	LIBS = `allegro-config --libs`
	CFLAGS += -DTARGET_LINUX
endif

ifndef WITHOUT_LIBNET
	CFLAGS += -DLIBNET_CODE
	LIBS += -lnet
endif

MODULES = \
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
	skdos		\
	sklibnet	\
	sklinux		\
	skhelp		\
	sound		\
	stats		\
	statlist	\
	suicide		\
	tiles		\
	vector		\
	weapon		

OBJS = $(addprefix obj/,$(addsuffix .o,$(MODULES)))

vpath %.c src src/engine src/sk src/fastsqrt

obj/%.o: %.c
	$(COMPILE.c) -o $@ $<

$(GAME): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)
	
obj/depend:
	gcc $(CFLAGS) -MM src/*.c src/sk/*.c | sed 's,^\(.*[.]o:\),obj/\1,' > $@
	

.PHONY = depend compress suidroot clean 

depend: obj/depend

compress: $(GAME)
	upx $<

suidroot:
	chown root.allegro $(GAME)
	chmod 4750 $(GAME)

clean: 
	rm -f $(GAME) $(OBJS) obj/depend core
	
	
include obj/depend
