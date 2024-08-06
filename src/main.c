#include "include/board.h"
#include "include/fen.h"
#include "include/movegen.h"
#include "include/position.h"
#include <stdio.h>

int main() {
  char *starting_fen =
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  Position *position = fen_to_position(starting_fen);
  print_board(position->board);
  MovesListNode *candidate_moves = generate_moves_list(position);
  printf("\nCandidate Moves:\n");
  print_moves_list(candidate_moves);
  delete_moves_list(candidate_moves);
  return 0;
}
