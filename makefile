all : mapper.exe run.exe

OPT=-Wall -O2 -m486 -fomit-frame-pointer -ffast-math

%.o : %.c common.h blood.h
	gcc $(OPT) -c -o $@ $<

mapper.exe : mapper.o common.o
	gcc $(OPT) -s -o $@ mapper.o common.o -lalleg

run.exe : run.o common.o sk.o rnd.o menu.o
	gcc $(OPT) -o $@ run.o common.o sk.o rnd.o menu.o -lalleg

dist : mapper.exe run.exe
	djp *.exe

clean : 
	rm -f *.exe *.o