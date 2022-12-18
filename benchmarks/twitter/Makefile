#
# jsonic is a json parser for C
#
# https://github.com/rohanrhu/jsonic
# https://oguzhaneroglu.com/projects/jsonic/
#
# Licensed under MIT
# Copyright (C) 2018, Oğuzhan Eroğlu (https://oguzhaneroglu.com/) <rohanrhu2@gmail.com>
#

CC = g++
CFLAGS = -std=c++17 -I../../ -I/opt/homebrew/include -L/opt/homebrew/Cellar/boost/1.80.0/lib -lboost_json -Wno-writable-strings -g -O3

ifeq ($(OS), Windows_NT)
	RM = rm -rf
else
	RM = rm -rf
endif

twitter.exe: twitter.cpp ../../jsonic.o
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	$(RM) $(EXECUTABLE)