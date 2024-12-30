#include "engine.h"
#include "fen.h"
#include "movegen.h"

namespace tejas {

    Engine::Engine() {
        board::Board::initialize();
        movegen::initialize();
    }

    Engine::~Engine() {}

    void Engine::setStartpos() { board.setStartpos(); }

    void Engine::setPositionFromFEN(std::string& fen) { fen::parseFEN(&board, fen); }

    void Engine::displayBoard() { board.display(); }

}
