CC = gcc
CFLAGS = -Wall -O3 -m486 -Isrc -Isrc/sk

# Even though this game is DOS-specific...
EXE = .exe

all: red$(EXE)

OBJS =  obj/run.o obj/common.o obj/sk.o obj/menu.o obj/mapper.o \
	obj/stats.o obj/statlist.o obj/intro.o obj/creds.o \
	obj/rnd.o obj/fastsqrt.o

obj/%.o: src/%.c
	$(COMPILE.c) -o $@ $<

obj/%.o: src/sk/%.c
	$(COMPILE.c) -o $@ $<

obj/%.o: src/fastsqrt/%.c
	$(COMPILE.c) -o $@ $<

red.exe: $(OBJS)
	$(CC) -o $@ $(OBJS) -lalleg
	
compress: red.exe
	upx $<

clean: 
	rm -f red.exe obj/*.o
	
