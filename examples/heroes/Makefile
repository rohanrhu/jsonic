#
# jsonic is a json parser for C
#
# https://github.com/rohanrhu/jsonic
# https://oguzhaneroglu.com/projects/jsonic/
#
# Licensed under MIT
# Copyright (C) 2018, Oğuzhan Eroğlu (https://oguzhaneroglu.com/) <rohanrhu2@gmail.com>
#

CC = gcc
CFLAGS = -std=c99 -I../../ -g -O3

ifeq ($(OS), Windows_NT)
	RM = rm -rf
else
	RM = rm -rf
endif

heroes.exe: heroes.c ../../jsonic.o
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	$(RM) $(EXECUTABLE)