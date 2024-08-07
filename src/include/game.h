#ifndef GAME_H
#define GAME_H

#include "move.h"
#include "position.h"

#define MAXGAMEMOVES 2048

typedef struct {
    Position*        position;
    MoveHistoryEntry history[MAXGAMEMOVES];
} Game;

Game* initialize_game(char* fen);

#endif
