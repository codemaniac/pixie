#pragma once

#include "board.h"
#include "types.h"

namespace tejas {

    namespace movegen {

        void initialize();

        bool isSquareAttacked(const board::Board board, const Square sq, const Color attacked_by);

    }

}
