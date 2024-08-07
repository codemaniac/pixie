#include "include/game.h"
#include "include/fen.h"
#include "include/position.h"
#include <stdlib.h>

Game *initialize_game(char *fen) {
  Game *game = malloc(sizeof(Game));
  if (game == NULL) {
    // TODO: Raise exception - Memory Error!
    exit(1);
  }
  game->position = fen_to_position(fen);
  return game;
}
