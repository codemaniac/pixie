#include "engine.h"

int main() {
    tejas::Engine engine;
    std::string   fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    engine.setPositionFromFEN(fen);
    engine.displayBoard();
    return 0;
}
