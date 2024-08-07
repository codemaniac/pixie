NAME := pixie
VERSION := 0.1.0-dev

CC := gcc
CFLAGS := -Wall -pedantic -O2
FORMATTER := clang-format

BIN_DIR := ./bin
BUILD_DIR := ./build
SRC_DIR := ./src
INCLUDE_DIR := $(SRC_DIR)/include
LIB_DIR := $(SRC_DIR)/lib

all: clean dir $(NAME)

clean:
	@rm -rf $(BUILD_DIR) $(BIN_DIR)

dir:
	@mkdir -p $(BUILD_DIR) $(BIN_DIR)

format:
	@$(FORMATTER) -style=file -i $(SRC_DIR)/*.c $(INCLUDE_DIR)/*.h

$(NAME): main.o board.o fen.o game.o move.o movegen.o piece.o position.o utils.o
	@$(CC) $(CFLAGS) -o $(BIN_DIR)/$(NAME)-$(VERSION) $(BUILD_DIR)/main.o \
		$(BUILD_DIR)/board.o \
		$(BUILD_DIR)/fen.o \
		$(BUILD_DIR)/game.o \
		$(BUILD_DIR)/move.o \
		$(BUILD_DIR)/movegen.o \
		$(BUILD_DIR)/piece.o \
		$(BUILD_DIR)/position.o \
		$(BUILD_DIR)/utils.o

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

piece.o: $(SRC_DIR)/piece.c $(INCLUDE_DIR)/piece.h
	@$(CC) $(CFLAGS) -c $(SRC_DIR)/piece.c -o $(BUILD_DIR)/piece.o

position.o: $(SRC_DIR)/position.c $(INCLUDE_DIR)/position.h
	@$(CC) $(CFLAGS) -c $(SRC_DIR)/position.c -o $(BUILD_DIR)/position.o

utils.o: $(SRC_DIR)/utils.c $(INCLUDE_DIR)/utils.h
	@$(CC) $(CFLAGS) -c $(SRC_DIR)/utils.c -o $(BUILD_DIR)/utils.o

.PHONY: clean dir format
