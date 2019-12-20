CC = gcc
CFLAGS = -std=c99 -I../../ -g -O3
JSONIC_SOURCES = $(filter-out $(shell find ../../ -path "*/examples/*"), $(shell find ../../ -name "*.c"))
JSONIC_HEADERS = $(filter-out $(shell find ../../ -path "*/examples/*"), $(shell find ../../ -name "*.h"))
JSONIC_OBJ = $(JSONIC_SOURCES:.c=.o)
SOURCE = heroes.c
EXECUTABLE = $(SOURCE:.c=.exe)

ifeq ($(OS), Windows_NT)
	RM = rm -rf
else
	RM = rm -rf
endif

$(EXECUTABLE): $(JSONIC_OBJ)
	$(CC) -o $@ $(SOURCE) $< $(CFLAGS)

clean:
	$(RM) $(EXECUTABLE)