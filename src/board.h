#pragma once

#include "types.h"

namespace tejas {

    namespace board {

        using BitBoard = u64;

        constexpr BitBoard BOARD_MASK_RANK_1() { return 0x00000000000000FF; }
        constexpr BitBoard BOARD_MASK_RANK_8() { return 0xFF00000000000000; }
        constexpr u8       BOARD_PIECE_COLOR_IDX(const Piece p) { return (7 + PIECE_COLOR(p)); }

        class Board {
           private:
            BitBoard bitboards[15];
            Piece    pieces[64];

           public:
            Board();
            ~Board();
            void  reset();
            void  set_piece(const Piece, const Square);
            void  clear_piece(const Piece, const Square);
            void  set_startpos();
            void  move_piece(const Piece,
                             const Square from,
                             const Square to,
                             const bool   is_capture,
                             const bool   is_promotion,
                             const Piece  promoted);
            void  undo_move_piece(const Piece,
                                  const Square from,
                                  const Square to,
                                  const bool   is_capture,
                                  const bool   is_promotion,
                                  const Piece  promoted);
            Piece get_piece(const Square) const;
            u8    get_piece_count(const Piece) const;
            bool  is_valid() const;
#ifdef DEBUG
            void display() const;
#endif
        };

    }

}
