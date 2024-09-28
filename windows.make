NAME := pixie

CC := gcc
CFLAGS := -std=gnu17 -Wall -Wextra -pedantic -O3 -flto
BIN_DIR := .\bin
BUILD_DIR := .\build
SRC_DIR := .\src
INCLUDE_DIR := $(SRC_DIR)\include

all: $(NAME)

$(NAME): main.o chess.o eval.o fen.o hashkey.o perft.o search.o transpositiontable.o uci.o utils.o
	@$(CC) $(CFLAGS) -o $(BIN_DIR)\$(NAME) $(BUILD_DIR)\main.o \
		$(BUILD_DIR)\chess.o \
		$(BUILD_DIR)\eval.o \
		$(BUILD_DIR)\fen.o \
		$(BUILD_DIR)\hashkey.o \
		$(BUILD_DIR)\perft.o \
		$(BUILD_DIR)\search.o \
		$(BUILD_DIR)\transpositiontable.o \
		$(BUILD_DIR)\uci.o \
		$(BUILD_DIR)\utils.o \

main.o: $(SRC_DIR)\main.c
	@$(CC) $(CFLAGS) -c $(SRC_DIR)\main.c -o $(BUILD_DIR)\main.o

chess.o: $(SRC_DIR)\chess.c $(INCLUDE_DIR)\chess.h
	@$(CC) $(CFLAGS) -c $(SRC_DIR)\chess.c -o $(BUILD_DIR)\chess.o

eval.o: $(SRC_DIR)\eval.c $(INCLUDE_DIR)\eval.h
	@$(CC) $(CFLAGS) -c $(SRC_DIR)\eval.c -o $(BUILD_DIR)\eval.o

fen.o: $(SRC_DIR)\fen.c $(INCLUDE_DIR)\fen.h
	@$(CC) $(CFLAGS) -c $(SRC_DIR)\fen.c -o $(BUILD_DIR)\fen.o

hashkey.o: $(SRC_DIR)\hashkey.c $(INCLUDE_DIR)\hashkey.h
	@$(CC) $(CFLAGS) -c $(SRC_DIR)\hashkey.c -o $(BUILD_DIR)\hashkey.o

perft.o: $(SRC_DIR)\perft.c $(INCLUDE_DIR)\perft.h
	@$(CC) $(CFLAGS) -c $(SRC_DIR)\perft.c -o $(BUILD_DIR)\perft.o

search.o: $(SRC_DIR)\search.c $(INCLUDE_DIR)\search.h
	@$(CC) $(CFLAGS) -c $(SRC_DIR)\search.c -o $(BUILD_DIR)\search.o

transpositiontable.o: $(SRC_DIR)\transpositiontable.c $(INCLUDE_DIR)\transpositiontable.h
	@$(CC) $(CFLAGS) -c $(SRC_DIR)\transpositiontable.c -o $(BUILD_DIR)\transpositiontable.o

uci.o: $(SRC_DIR)\uci.c $(INCLUDE_DIR)\uci.h
	@$(CC) $(CFLAGS) -c $(SRC_DIR)\uci.c -o $(BUILD_DIR)\uci.o

utils.o: $(SRC_DIR)\utils.c $(INCLUDE_DIR)\utils.h
	@$(CC) $(CFLAGS) -c $(SRC_DIR)\utils.c -o $(BUILD_DIR)\utils.o
