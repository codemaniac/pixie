#pragma once

#include "board.h"
#include "pch.h"

namespace tejas {

    namespace fen {

        void parseFEN(board::Board*, std::string, const bool full = true);

    }

}
