# Author;	Stephen Haffner
# makefile for meebee

CC=gcc -lpthread -lcrypt -lssl -lcrypto
OF=build/meebee.bin

all: main.c socket/socket.c data/data.c auth/auth.c crypto/encryptutils.c
	$(CC)  socket/socket.c data/data.c auth/auth.c main.c -o $(OF)


clean:
	rm -rfv build/*.o build/meebee.bin
