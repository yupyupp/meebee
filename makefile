# Author;	Stephen Haffner
# makefile for meebee

CC=gcc -ansi -pedantic -lpthreads 
OF=build/meebee.bin
BD=build

all: auth.o data.o crypto.o
	$(CC) auth.o data.o crypto.o -o $(OF)

auth.o: auth/auth.c
	$(CC) -c auth/auth.c -o $(BD)/auth.o

data.o: data/data.c
	$(CC) -c data/data.c -o $(BD)/data.o

crypto.o: crypto/crypto.c
	$(CC) -lcrypt -lgcrypt -c crypto.c -o $(BD)/crypto.o

clean:
	rm -rfv build/*.o build/meebee.bin
