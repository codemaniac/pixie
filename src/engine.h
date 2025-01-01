#pragma once

#include "board.h"
#include <string>

namespace tejas {

    class Engine {
       private:
        std::string  name;
        std::string  version;
        board::Board board;

       public:
        Engine();
        ~Engine();
        std::string getName() const;
        std::string getVersion() const;
        void        reset();
        void        setStartpos();
        void        setPositionFromFEN(std::string&);
        void        displayBoard() const;
    };

}
