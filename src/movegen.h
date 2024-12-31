#pragma once

#include "board.h"
#include "move.h"
#include "types.h"
#include <vector>

namespace tejas {

    namespace movegen {

        enum class MovegenType {
            ALL,
            CAPTURES
        };

        void initialize();

        bool isSquareAttacked(const board::Board& board, const Square sq, const Color attacked_by);

        bool isInCheck(const board::Board& board);

        void generatePseudolegalMoves(std::vector<move::Move>* moves,
                                      const board::Board&      board,
                                      const MovegenType        type);

    }

}
