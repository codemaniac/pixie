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
            void  Reset();
            void  SetPiece(const Piece, const Square);
            void  ClearPiece(const Piece, const Square);
            void  SetStartpos();
            void  MovePiece(const Piece  piece,
                            const Square from,
                            const Square to,
                            const bool   is_capture,
                            const bool   is_promotion,
                            const Piece  promoted);
            void  UndoMovePiece(const Piece  piece,
                                const Square from,
                                const Square to,
                                const bool   is_capture,
                                const Piece  captured,
                                const bool   is_promotion,
                                const Piece  promoted);
            Piece GetPiece(const Square) const;
            u8    GetPieceCount(const Piece) const;
            bool  IsValid() const;
#ifdef DEBUG
            void Display() const;
#endif
        };

    }

}
