#pragma once

#include "board.h"
#include "types.h"

namespace tejas {

    namespace movegen {

        void Initialize();

        bool IsSquareAttacked(const board::Board board, const Square sq, const Color attacked_by);

    }

}
