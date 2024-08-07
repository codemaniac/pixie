#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

#define __SQUARE_INVALID 15
#define __SQUARE_EMPTY 0
#define __RF_TO_SQ(r, f) ((21 + f) + (r * 10))  // a1 = (0, 0); h8 = (7, 7)
#define __SQ_TO_RANK(sq) ((sq / 10) - 2)
#define __SQ_TO_FILE(sq) ((sq % 10) - 1)
#define __IS_SQ_ON_BOARD(sq) ((sq >= 0) && (sq < 120))

typedef struct {
    uint8_t squares[120];
    uint8_t wK_sq;
    uint8_t bK_sq;
} Board;

Board* create_empty_board(void);

void print_board(const Board* board);

#endif
