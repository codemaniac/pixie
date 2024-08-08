debug ?= 1
NAME := pixie

CC := gcc
CFLAGS := -std=gnu17 -Wall -Wextra -pedantic
FORMATTER := clang-format

ifeq ($(debug), 1)
	CFLAGS := $(CFLAGS) -DDEBUG -g -O0
else
	CFLAGS := $(CFLAGS) -DLOG_QUIET -O2
endif

BIN_DIR := ./bin
BUILD_DIR := ./build
SRC_DIR := ./src
INCLUDE_DIR := $(SRC_DIR)/include
LIB_DIR := $(SRC_DIR)/lib

all: clean dir $(NAME)

clean:
	@rm -rf $(BUILD_DIR) $(BIN_DIR)
	@rm -f compile_commands.json

dir:
	@mkdir -p $(BUILD_DIR) $(BIN_DIR)

format:
	@$(FORMATTER) -style=file -i $(SRC_DIR)/*.c $(INCLUDE_DIR)/*.h

bear:
	@bear -- make

$(NAME): main.o board.o fen.o game.o move.o movegen.o perft.o piece.o position.o utils.o logc.o argtable3.o
	@$(CC) $(CFLAGS) -o $(BIN_DIR)/$(NAME) $(BUILD_DIR)/main.o \
		$(BUILD_DIR)/board.o \
		$(BUILD_DIR)/fen.o \
		$(BUILD_DIR)/game.o \
		$(BUILD_DIR)/move.o \
		$(BUILD_DIR)/movegen.o \
		$(BUILD_DIR)/perft.o \
		$(BUILD_DIR)/piece.o \
		$(BUILD_DIR)/position.o \
		$(BUILD_DIR)/utils.o \
		$(BUILD_DIR)/logc.o \
		$(BUILD_DIR)/argtable3.o

main.o: $(SRC_DIR)/main.c
	@$(CC) $(CFLAGS) -c $(SRC_DIR)/main.c -o $(BUILD_DIR)/main.o

board.o: $(SRC_DIR)/board.c $(INCLUDE_DIR)/board.h
	@$(CC) $(CFLAGS) -c $(SRC_DIR)/board.c -o $(BUILD_DIR)/board.o

fen.o: $(SRC_DIR)/fen.c $(INCLUDE_DIR)/fen.h
	@$(CC) $(CFLAGS) -c $(SRC_DIR)/fen.c -o $(BUILD_DIR)/fen.o

game.o: $(SRC_DIR)/game.c $(INCLUDE_DIR)/game.h
	@$(CC) $(CFLAGS) -c $(SRC_DIR)/game.c -o $(BUILD_DIR)/game.o

move.o: $(SRC_DIR)/move.c $(INCLUDE_DIR)/move.h
	@$(CC) $(CFLAGS) -c $(SRC_DIR)/move.c -o $(BUILD_DIR)/move.o

movegen.o: $(SRC_DIR)/movegen.c $(INCLUDE_DIR)/movegen.h
	@$(CC) $(CFLAGS) -c $(SRC_DIR)/movegen.c -o $(BUILD_DIR)/movegen.o

perft.o: $(SRC_DIR)/perft.c $(INCLUDE_DIR)/perft.h
	@$(CC) $(CFLAGS) -c $(SRC_DIR)/perft.c -o $(BUILD_DIR)/perft.o

piece.o: $(SRC_DIR)/piece.c $(INCLUDE_DIR)/piece.h
	@$(CC) $(CFLAGS) -c $(SRC_DIR)/piece.c -o $(BUILD_DIR)/piece.o

position.o: $(SRC_DIR)/position.c $(INCLUDE_DIR)/position.h
	@$(CC) $(CFLAGS) -c $(SRC_DIR)/position.c -o $(BUILD_DIR)/position.o

utils.o: $(SRC_DIR)/utils.c $(INCLUDE_DIR)/utils.h
	@$(CC) $(CFLAGS) -c $(SRC_DIR)/utils.c -o $(BUILD_DIR)/utils.o

# LIBS

logc.o: $(LIB_DIR)/logc/log.c $(LIB_DIR)/logc/log.h
	@$(CC) $(CFLAGS) -c -DLOG_USE_COLOR $(LIB_DIR)/logc/log.c -o $(BUILD_DIR)/logc.o

argtable3.o: $(LIB_DIR)/argtable3/argtable3.c $(LIB_DIR)/argtable3/argtable3.h
	@$(CC) $(CFLAGS) -c $(LIB_DIR)/argtable3/argtable3.c -o $(BUILD_DIR)/argtable3.o

.PHONY: clean dir format bear
