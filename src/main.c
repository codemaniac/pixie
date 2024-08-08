#include "include/board.h"
#include "include/game.h"
#include "include/move.h"
#include "include/movegen.h"
#include "lib/logc/log.h"
#include <stdio.h>

#define VERSION "0.1.0-dev"

#ifndef LOG_QUIET
    #define LOG_QUIET_VAL 0
#else
    #define LOG_QUIET_VAL 1
#endif

int main(void) {

#ifdef DEBUG
    log_set_quiet(LOG_QUIET_VAL);
    log_set_level(LOG_TRACE);
#else
    log_set_quiet(LOG_QUIET_VAL);
    log_set_level(LOG_INFO);
#endif

    log_info("Pixie %s", VERSION);

#ifdef DEBUG
    log_warn("Pixie is running in DEBUG mode!");
#endif

    const char* fen =
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    Game* game = initialize_game(fen);

    print_board(game->position->board);

    MovesListNode* candidate_moves =
      generate_pseudo_legal_moves(game->position);
    printf("\nCandidate Moves:\n");
    print_moves_list(candidate_moves);

    MovesListNode* test_move = (MovesListNode*) candidate_moves->next;
    Move           m         = test_move->move;

    printf("\nSelected move:\n");
    print_move(m);

    printf("\nPerforming Move:\n");
    do_move(game->position, m);
    print_board(game->position->board);

    printf("\nUndo move:\n");
    undo_move(game->position, m);
    print_board(game->position->board);

    delete_moves_list(candidate_moves);
    return 0;
}
