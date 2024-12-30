#pragma once

#include "board.h"

namespace tejas {

    class Engine {
       private:
        board::Board board;

       public:
        Engine();
        ~Engine();
        void set_startpos();
#ifdef DEBUG
        void display_board();
#endif
    };

}
