EXE ?= pixie
CXX ?= clang++
CFLAGS := -DNDEBUG -m64 -Werror -flto -O3 -std=c++20
LDFLAGS := -m64 -flto

BIN_DIR := .
OBJ_DIR := ../obj
SRC_DIR := ../src
INCLUDE_DIR := $(SRC_DIR)/include

all: clean dir pixie

clean:
	@rm -rf $(OBJ_DIR)
	@rm -f $(EXE)

dir:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(BIN_DIR)

pixie: main.o board.o eval.o fen.o move.o perft.o position.o search.o threadpool.o tt.o uci.o utils.o
	@echo "==== Building pixie ===="
	@$(CXX) $(LDFLAGS) -o $(BIN_DIR)/$(EXE) $(OBJ_DIR)/main.o \
		$(OBJ_DIR)/board.o \
		$(OBJ_DIR)/eval.o \
		$(OBJ_DIR)/fen.o \
		$(OBJ_DIR)/move.o \
		$(OBJ_DIR)/perft.o \
		$(OBJ_DIR)/position.o \
		$(OBJ_DIR)/search.o \
		$(OBJ_DIR)/threadpool.o \
		$(OBJ_DIR)/tt.o \
		$(OBJ_DIR)/uci.o \
		$(OBJ_DIR)/utils.o

main.o: $(SRC_DIR)/main.cpp
	@$(CXX) $(CFLAGS) -c $(SRC_DIR)/main.cpp -o $(OBJ_DIR)/main.o

board.o: $(SRC_DIR)/board.cpp $(INCLUDE_DIR)/board.h
	@$(CXX) $(CFLAGS) -c $(SRC_DIR)/board.cpp -o $(OBJ_DIR)/board.o

eval.o: $(SRC_DIR)/eval.cpp $(INCLUDE_DIR)/eval.h
	@$(CXX) $(CFLAGS) -c $(SRC_DIR)/eval.cpp -o $(OBJ_DIR)/eval.o

fen.o: $(SRC_DIR)/fen.cpp $(INCLUDE_DIR)/fen.h
	@$(CXX) $(CFLAGS) -c $(SRC_DIR)/fen.cpp -o $(OBJ_DIR)/fen.o

move.o: $(SRC_DIR)/move.cpp $(INCLUDE_DIR)/move.h
	@$(CXX) $(CFLAGS) -c $(SRC_DIR)/move.cpp -o $(OBJ_DIR)/move.o

perft.o: $(SRC_DIR)/perft.cpp $(INCLUDE_DIR)/perft.h
	@$(CXX) $(CFLAGS) -c $(SRC_DIR)/perft.cpp -o $(OBJ_DIR)/perft.o

position.o: $(SRC_DIR)/position.cpp $(INCLUDE_DIR)/position.h
	@$(CXX) $(CFLAGS) -c $(SRC_DIR)/position.cpp -o $(OBJ_DIR)/position.o

search.o: $(SRC_DIR)/search.cpp $(INCLUDE_DIR)/search.h
	@$(CXX) $(CFLAGS) -c $(SRC_DIR)/search.cpp -o $(OBJ_DIR)/search.o

threadpool.o: $(SRC_DIR)/threadpool.cpp $(INCLUDE_DIR)/threadpool.h
	@$(CXX) $(CFLAGS) -c $(SRC_DIR)/threadpool.cpp -o $(OBJ_DIR)/threadpool.o

tt.o: $(SRC_DIR)/tt.cpp $(INCLUDE_DIR)/tt.h
	@$(CXX) $(CFLAGS) -c $(SRC_DIR)/tt.cpp -o $(OBJ_DIR)/tt.o

uci.o: $(SRC_DIR)/uci.cpp $(INCLUDE_DIR)/uci.h
	@$(CXX) $(CFLAGS) -c $(SRC_DIR)/uci.cpp -o $(OBJ_DIR)/uci.o

utils.o: $(SRC_DIR)/utils.cpp $(INCLUDE_DIR)/utils.h
	@$(CXX) $(CFLAGS) -c $(SRC_DIR)/utils.cpp -o $(OBJ_DIR)/utils.o

.PHONY: clean dir
