GAME_SOURCE=src
GAME_LIB=lib
EXECUTABLE=shmup

CC=gcc
LIBS=-lm -lglfw -lenet
LDFLAGS=-g
CFLAGS=-c -std=c99

UNAME_S=$(shell uname)

ifeq ($(UNAME_S), Linux)
LIBS += -lGL
endif

ifeq ($(UNAME_S), Darwin)
CC=clang
LDFLAGS += -framework OpenGL -framework Cocoa
endif

#MMM, Libraries
CFLAGS+=-I$(GAME_LIB)
SOURCES=$(wildcard $(GAME_LIB)/soil/*.c)

SOURCES+=$(wildcard $(GAME_SOURCE)/*.c)

OBJECTS=$(SOURCES:.c=.o)

.PHONY : all clean

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(LIBS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
