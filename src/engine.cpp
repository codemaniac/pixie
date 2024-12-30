#include "engine.h"
#include "movegen.h"

namespace tejas {

    Engine::Engine() { movegen::Initialize(); }

    Engine::~Engine() {}

    void Engine::SetStartpos() { board.SetStartpos(); }

#ifdef DEBUG
    void Engine::DisplayBoard() { board.Display(); }
#endif

}
