debug ?= 0
NAME := pixie

CC := gcc
CFLAGS := -std=gnu18 -Wall -Wextra -pedantic
FORMATTER := clang-format

ifeq ($(debug), 1)
	CFLAGS := $(CFLAGS) -DDEBUG -g -O0
else
	CFLAGS := $(CFLAGS) -DLOG_QUIET -O3 -funroll-loops -flto
endif

BIN_DIR := ./bin
BUILD_DIR := ./build
SRC_DIR := ./src
INCLUDE_DIR := $(SRC_DIR)/include
LIB_DIR := $(SRC_DIR)/lib

all: clean dir $(NAME)

clean:
	@rm -rf $(BUILD_DIR) $(BIN_DIR)
	@rm -rf pypixie.egg-info
	@rm -f *.so
	@rm -f compile_commands.json

dir:
	@mkdir -p $(BUILD_DIR) $(BIN_DIR)

format:
	@$(FORMATTER) -style=file -i $(SRC_DIR)/*.c $(INCLUDE_DIR)/*.h

bear:
	@bear -- make

$(NAME): main.o chess.o eval.o fen.o hashkey.o perft.o search.o utils.o logc.o
	@$(CC) $(CFLAGS) -o $(BIN_DIR)/$(NAME) $(BUILD_DIR)/main.o \
		$(BUILD_DIR)/chess.o \
		$(BUILD_DIR)/eval.o \
		$(BUILD_DIR)/fen.o \
		$(BUILD_DIR)/hashkey.o \
		$(BUILD_DIR)/perft.o \
		$(BUILD_DIR)/search.o \
		$(BUILD_DIR)/utils.o \
		$(BUILD_DIR)/logc.o \

test:
	python setup.py develop

main.o: $(SRC_DIR)/main.c
	@$(CC) $(CFLAGS) -c $(SRC_DIR)/main.c -o $(BUILD_DIR)/main.o

chess.o: $(SRC_DIR)/chess.c $(INCLUDE_DIR)/chess.h
	@$(CC) $(CFLAGS) -c $(SRC_DIR)/chess.c -o $(BUILD_DIR)/chess.o

eval.o: $(SRC_DIR)/eval.c $(INCLUDE_DIR)/eval.h
	@$(CC) $(CFLAGS) -c $(SRC_DIR)/eval.c -o $(BUILD_DIR)/eval.o

fen.o: $(SRC_DIR)/fen.c $(INCLUDE_DIR)/fen.h
	@$(CC) $(CFLAGS) -c $(SRC_DIR)/fen.c -o $(BUILD_DIR)/fen.o

hashkey.o: $(SRC_DIR)/hashkey.c $(INCLUDE_DIR)/hashkey.h
	@$(CC) $(CFLAGS) -c $(SRC_DIR)/hashkey.c -o $(BUILD_DIR)/hashkey.o

perft.o: $(SRC_DIR)/perft.c $(INCLUDE_DIR)/perft.h
	@$(CC) $(CFLAGS) -c $(SRC_DIR)/perft.c -o $(BUILD_DIR)/perft.o

search.o: $(SRC_DIR)/search.c $(INCLUDE_DIR)/search.h
	@$(CC) $(CFLAGS) -c $(SRC_DIR)/search.c -o $(BUILD_DIR)/search.o

utils.o: $(SRC_DIR)/utils.c $(INCLUDE_DIR)/utils.h
	@$(CC) $(CFLAGS) -c $(SRC_DIR)/utils.c -o $(BUILD_DIR)/utils.o

# LIBS

logc.o: $(LIB_DIR)/logc/log.c $(LIB_DIR)/logc/log.h
	@$(CC) $(CFLAGS) -c -DLOG_USE_COLOR $(LIB_DIR)/logc/log.c -o $(BUILD_DIR)/logc.o

.PHONY: clean dir format bear
