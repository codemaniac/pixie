#ifndef GAME_H
#define GAME_H

#include "move.h"
#include "position.h"
#include <stdbool.h>

#define MAXGAMEMOVES 2048

typedef struct {
    Position*         position;
    MoveHistoryEntry* history[MAXGAMEMOVES];
    uint16_t          ply_count;
} Game;

Game* initialize_game(const char* fen);

bool do_move(Game* game, const Move move, bool auto_undo_invalid);

void undo_move(Game* game, const Move move);

#endif
