CC = gcc
CFLAGS = -std=c99 -I. -g
SOURCES = $(filter-out $(shell find . -path "*/examples/*"), $(shell find . -name "*.c"))
HEADERS = $(filter-out $(shell find . -path "*/examples/*"), $(shell find . -name "*.h"))
EXECUTABLES = $(shell find . -name "*.exe")
OBJ = $(SOURCES:.c=.o)

ifeq ($(OS), Windows_NT)
	RM = rm -rf
else
	RM = rm -rf
endif

all: $(OBJ)

$(OBJ): %:
	$(CC) -c -o $@ $(SOURCE) $(basename $@).c $(CFLAGS)

clean:
	$(RM) $(OBJ)
	$(RM) $(EXECUTABLES)