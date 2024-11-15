SHELL = /bin/sh
.SUFFIXES:
.SUFFIXES: .c .o
CC = gcc
CFLAG = -I. -pthread -lrt

%.o: %.c %.h
	$(CC) -c -o $@ $< $(CFLAG)

stairs: stairs.h stairs.o
	$(CC) -Wall -o $@ stairs.c $(CFLAG) 
	./stairs > output.txt

clean:
	rm stairs.o stairs output.txt