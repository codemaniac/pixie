#pragma once

#include "board.h"

namespace tejas {

    class Engine {
       private:
        board::Board board;

       public:
        Engine();
        ~Engine();
        void SetStartpos();
#ifdef DEBUG
        void DisplayBoard();
#endif
    };

}
