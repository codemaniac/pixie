#pragma once

#include "board.h"
#include "pch.h"
#include "types.h"

namespace tejas {

    namespace perft {

        u64 perft(board::Board& board, const u8 depth);
        u64 divide(board::Board& board, const u8 depth);

    }

}
