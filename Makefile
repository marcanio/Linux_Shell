all: shell.exe

shell.exe: shell.o
	gcc -o shell.exe shell.o

shell.o: shell.c
	gcc -c shell.c

clean:
	rm shell.o shell.exe
