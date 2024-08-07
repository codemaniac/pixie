#include "include/board.h"
#include "include/fen.h"
#include "include/move.h"
#include "include/movegen.h"
#include "include/position.h"
#include <stdio.h>

int main() {
  char *fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

  Position *position = fen_to_position(fen);
  print_board(position->board);

  MovesListNode *candidate_moves = generate_moves_list(position);
  printf("\nCandidate Moves:\n");
  print_moves_list(candidate_moves);

  MovesListNode *test_move = (MovesListNode *)candidate_moves->next;
  Move m = test_move->move;

  printf("\nSelected move:\n");
  print_move(m);

  printf("\nPerforming Move:\n");
  do_move(position, m);
  print_board(position->board);

  printf("\nUndo move:\n");
  undo_move(position, m);
  print_board(position->board);

  delete_moves_list(candidate_moves);
  return 0;
}
