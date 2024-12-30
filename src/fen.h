#pragma once

#include "board.h"
#include <string>

namespace tejas {

    namespace fen {

        void parseFEN(board::Board*, std::string&, const bool full = true);

    }

}
