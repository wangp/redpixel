CC = gcc
CFLAGS = -Wall -O3 -m486 -Isrc -Isrc/sk 

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

MODULES = \
	common		\
	creds		\
	demintro	\
	demo		\
	fastsqrt	\
	intro		\
	mapper		\
	menu		\
	rg_rand		\
	run		\
	setweaps	\
	skdos		\
	sklinux		\
	stats		\
	statlist

OBJS = $(addprefix obj/,$(addsuffix .o,$(MODULES)))

vpath %.c src src/sk src/fastsqrt

obj/%.o: %.c
	$(COMPILE.c) -o $@ $<

$(GAME): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)


.PHONY = compress clean

compress: $(GAME)
	upx $<
	
suidroot:
	chown root.allegro $(GAME)
	chmod 4750 $(GAME)

clean: 
	rm -f $(GAME) $(OBJS)

