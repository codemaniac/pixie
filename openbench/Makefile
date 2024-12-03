EXE ?= pixie
CXX ?= clang++
CFLAGS := -MD -MP -DNDEBUG -m64 -Werror -flto -O3 -std=c++20
LDFLAGS := -m64 -static -flto

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

pixie: main.o bench.o board.o eval.o fen.o move.o perft.o position.o search.o threadpool.o tt.o uci.o utils.o
	@echo "==== Building pixie ===="
	@$(CXX) $(LDFLAGS) -o $(BIN_DIR)/$(EXE) $(OBJ_DIR)/main.o \
		$(OBJ_DIR)/bench.o \
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
	@$(CXX) $(CFLAGS) -c $(SRC_DIR)/main.cpp -o $(OBJ_DIR)/main.o -MF $(OBJ_DIR)/main.d

bench.o: $(SRC_DIR)/bench.cpp $(INCLUDE_DIR)/bench.h
	@$(CXX) $(CFLAGS) -c $(SRC_DIR)/bench.cpp -o $(OBJ_DIR)/bench.o -MF $(OBJ_DIR)/bench.d

board.o: $(SRC_DIR)/board.cpp $(INCLUDE_DIR)/board.h
	@$(CXX) $(CFLAGS) -c $(SRC_DIR)/board.cpp -o $(OBJ_DIR)/board.o -MF $(OBJ_DIR)/board.d

eval.o: $(SRC_DIR)/eval.cpp $(INCLUDE_DIR)/eval.h
	@$(CXX) $(CFLAGS) -c $(SRC_DIR)/eval.cpp -o $(OBJ_DIR)/eval.o -MF $(OBJ_DIR)/eval.d

fen.o: $(SRC_DIR)/fen.cpp $(INCLUDE_DIR)/fen.h
	@$(CXX) $(CFLAGS) -c $(SRC_DIR)/fen.cpp -o $(OBJ_DIR)/fen.o -MF $(OBJ_DIR)/fen.d

move.o: $(SRC_DIR)/move.cpp $(INCLUDE_DIR)/move.h
	@$(CXX) $(CFLAGS) -c $(SRC_DIR)/move.cpp -o $(OBJ_DIR)/move.o -MF $(OBJ_DIR)/move.d

perft.o: $(SRC_DIR)/perft.cpp $(INCLUDE_DIR)/perft.h
	@$(CXX) $(CFLAGS) -c $(SRC_DIR)/perft.cpp -o $(OBJ_DIR)/perft.o -MF $(OBJ_DIR)/perft.d

position.o: $(SRC_DIR)/position.cpp $(INCLUDE_DIR)/position.h
	@$(CXX) $(CFLAGS) -c $(SRC_DIR)/position.cpp -o $(OBJ_DIR)/position.o -MF $(OBJ_DIR)/position.d

search.o: $(SRC_DIR)/search.cpp $(INCLUDE_DIR)/search.h
	@$(CXX) $(CFLAGS) -c $(SRC_DIR)/search.cpp -o $(OBJ_DIR)/search.o -MF $(OBJ_DIR)/search.d

threadpool.o: $(SRC_DIR)/threadpool.cpp $(INCLUDE_DIR)/threadpool.h
	@$(CXX) $(CFLAGS) -c $(SRC_DIR)/threadpool.cpp -o $(OBJ_DIR)/threadpool.o -MF $(OBJ_DIR)/threadpool.d

tt.o: $(SRC_DIR)/tt.cpp $(INCLUDE_DIR)/tt.h
	@$(CXX) $(CFLAGS) -c $(SRC_DIR)/tt.cpp -o $(OBJ_DIR)/tt.o -MF $(OBJ_DIR)/tt.d

uci.o: $(SRC_DIR)/uci.cpp $(INCLUDE_DIR)/uci.h
	@$(CXX) $(CFLAGS) -c $(SRC_DIR)/uci.cpp -o $(OBJ_DIR)/uci.o -MF $(OBJ_DIR)/uci.d

utils.o: $(SRC_DIR)/utils.cpp $(INCLUDE_DIR)/utils.h
	@$(CXX) $(CFLAGS) -c $(SRC_DIR)/utils.cpp -o $(OBJ_DIR)/utils.o -MF $(OBJ_DIR)/utils.d

.PHONY: clean dir
