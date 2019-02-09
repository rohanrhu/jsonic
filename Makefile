CC = gcc
CFLAGS = -std=c99 -I. -g
ifeq ($(OS), Windows_NT)
	RM = del
else
	RM = rm
endif

build:
	$(CC) -o test.exe test.c jsonic.c $(CFLAGS)

clean:
	$(RM) test.exe