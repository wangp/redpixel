all : ff red.exe stats.cs

OPT=-Wall -O2 -m486 -fomit-frame-pointer -ffast-math

%.o : %.c blood.h
	gcc $(OPT) -c -o $@ $<

red.exe : run.o common.o sk.o rnd.o menu.o mapper.o creds.o intro.o
	gcc $(OPT) -o $@ run.o common.o sk.o rnd.o menu.o mapper.o creds.o intro.o -lalleg -lseer

stats.cs : stats.sc
	seerc -a tjaden -t "red pixel stats" $<

dist : red.exe
	djp red.exe

ff : 
	@dump /q

clean : 
	rm -f *.exe *.o *.cs