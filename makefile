all: flushsmartdrv red.exe

OPT = -Wall -O2 -m486 -fomit-frame-pointer -ffast-math -g

%.o: %.c blood.h
	gcc $(OPT) -c -o $@ $<

OBJ = run.o common.o sk.o rnd.o menu.o mapper.o creds.o intro.o

red.exe: $(OBJ)
	gcc $(OPT) -o $@ $(OBJ) -lalleg

flushsmartdrv: 
	@dump /q

clean: 
	rm -f *.exe *.o