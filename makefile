all : red.exe

OPT=-Wall -O2 -m486 -fomit-frame-pointer -ffast-math -g

%.o : %.c blood.h
	gcc $(OPT) -c -o $@ $<

red.exe : run.o common.o sk.o rnd.o menu.o mapper.o creds.o intro.o
	gcc $(OPT) -o $@ run.o common.o sk.o rnd.o menu.o mapper.o creds.o intro.o -lalleg -lseer

cs : stats.sc
	seerc -a tjaden -t stats $<

dist : red.exe
	djp red.exe

clean : 
	rm -f *.exe *.o