#include "engine.h"

namespace tejas {

    Engine::Engine() {}

    Engine::~Engine() {}

    void Engine::SetStartpos() { board.SetStartpos(); }

#ifdef DEBUG
    void Engine::DisplayBoard() { board.Display(); }
#endif

}
