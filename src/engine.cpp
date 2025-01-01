#include "engine.h"
#include "fen.h"

namespace tejas {

    Engine::Engine() {
        name    = "Tejas";
        version = "0.1.0";
        board::Board::initialize();
    }

    Engine::~Engine() {}

    std::string Engine::getName() const { return name; }

    std::string Engine::getVersion() const { return version; }

    void Engine::reset() { board.reset(); }

    void Engine::setStartpos() { board.setStartpos(); }

    void Engine::setPositionFromFEN(std::string& fen) { fen::parseFEN(&board, fen); }

    void Engine::displayBoard() const { board.display(); }

}
