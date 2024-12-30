#pragma once

#include "board.h"
#include <string>

namespace tejas {

    class Engine {
       private:
        board::Board board;

       public:
        Engine();
        ~Engine();
        void setStartpos();
        void setPositionFromFEN(std::string&);
        void displayBoard();
    };

}
