# Author;	Stephen Haffner
# makefile for meebee

CC=gcc -ansi -pedantic -lpthreads 
OF=build/meebee.bin

all: build/auth.o build/data.o build/crypto.o build/main.o
	$(CC) main.o auth.o data.o crypto.o -o $(OF)

build/main.o: main.c

build/auth.o: auth/auth.c
	$(CC) -c auth/auth.c

build/data.o: data/data.c
	$(CC) -c data/data.c

build/crypto.o: crypto/crypto.c
	$(CC) -lcrypt -lgcrypt -c crypto.c

clean:
	rm -rfv build/*.o build/meebee.bin
