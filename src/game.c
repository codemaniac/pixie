#include "include/game.h"
#include "include/fen.h"
#include "include/position.h"
#include "lib/logc/log.h"
#include <stdlib.h>

Game* initialize_game(const char* fen) {
    Game* game = malloc(sizeof(Game));
    if (game == NULL)
    {
        // TODO: Raise exception - Memory Error!

        log_fatal("Memory allocation failed!");
        exit(1);
    }

    log_trace("Initializing game with FEN: %s", fen);
    game->position = fen_to_position(fen);

    return game;
}
