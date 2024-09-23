#include "include/chess.h"
#include "include/fen.h"
#include "include/uci.h"

int main(void) {

    // uci_loop();
    chess_initialize();
    Position pos = position_create();
    fen_to_position("4k3/4p3/8/3n1q2/4P3/4N3/8/4K3 w - - 0 1", &pos);

    MoveList m;
    movegen_pseudo_legal_all(&pos, &m);
    movegen_display_moves(&m);

    return 0;
}
