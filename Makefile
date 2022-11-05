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
CFLAGS = -std=c99 -I. -g -O3
SOURCES = $(filter-out $(shell find . -path "*/examples/*"), $(shell find . -name "*.c"))
HEADERS = $(filter-out $(shell find . -path "*/examples/*"), $(shell find . -name "*.h"))
OBJ = jsonic.o
EXISTING_EXECUTABLES = $(shell find . -iname "*.exe")
TEST_EXECUTABLES =  examples/heroes/heroes.exe \
					examples/cities/cities.exe \
					examples/twitter/twitter.exe \
					examples/kviteration/kviteration.exe
TEST_SOURCES = $(shell find . -iname "*.c")

ifeq ($(OS), Windows_NT)
	RM = rm -rf
else
	RM = rm -rf
endif

all: jsonic.o
	@make -C examples/heroes
	@make -C examples/cities
	@make -C examples/twitter
	@make -C examples/kviteration

jsonic.o: jsonic.c
	$(CC) -c -o $@ $^ $(CFLAGS)

clean:
	$(RM) $(OBJ)
	$(RM) $(EXISTING_EXECUTABLES)