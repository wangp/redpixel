all : mapper.exe run.exe

%.o : %.c *.h
	gcc -Wall -O3 -c -o $@ $<

mapper.exe : mapper.o common.o
	gcc -Wall -O3 -o $@ mapper.o common.o -lalleg

run.exe : run.o common.o
	gcc -Wall -O3 -o $@ run.o common.o -lalleg

clean : 
	rm *.exe *.o