#include "include/board.h"
#include "include/game.h"
#include "include/move.h"
#include "include/movegen.h"
#include <stdio.h>

int main() {
  char *fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

  Game *game = initialize_game(fen);

  print_board(game->position->board);

  MovesListNode *candidate_moves = generate_moves_list(game->position);
  printf("\nCandidate Moves:\n");
  print_moves_list(candidate_moves);

  MovesListNode *test_move = (MovesListNode *)candidate_moves->next;
  Move m = test_move->move;

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
