CC=gcc
INCLUDE=-I./
LIBS=

SRC_C=common.c deque.c writer.c reader.c main.c
SRC_H=acx.h args.h common.h deque.h reader.h writer.h

BIN=./acx

ifeq ($(OS),Windows_NT)
	BIN:=$(BIN).exe
	LIBS+=-largp
endif

all : ACX

ACX : $(BIN)

$(BIN) : $(SRC_C) $(SRC_H)
	$(CC) $(SRC_C) $(INCLUDE) $(LIBS) -o $(BIN)

.PHONY : clean

clean :
	rm $(BIN) -v
