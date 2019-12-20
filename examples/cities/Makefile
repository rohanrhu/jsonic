CC = gcc
CFLAGS = -std=c99 -I../../ -g -O3
JSONIC_SOURCES = $(filter-out $(shell find ../../ -path "*/examples/*"), $(shell find ../../ -name "*.c"))
JSONIC_HEADERS = $(filter-out $(shell find ../../ -path "*/examples/*"), $(shell find ../../ -name "*.h"))
JSONIC_OBJ = $(JSONIC_SOURCES:.c=.o)
SOURCE = cities.c
EXECUTABLE = $(SOURCE:.c=.exe)

ifeq ($(OS), Windows_NT)
	RM = rm -rf
else
	RM = rm -rf
endif

$(EXECUTABLE): $(JSONIC_OBJ) cities.json
	$(CC) -o $@ $(SOURCE) $< $(CFLAGS)

cities.json:
	wget -O cities.json https://raw.githubusercontent.com/zemirco/sf-city-lots-json/master/citylots.json

clean:
	$(RM) $(EXECUTABLE)

cleanall:
	$(RM) $(EXECUTABLE)
	$(RM) cities.json