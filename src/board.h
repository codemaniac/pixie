#pragma once

#include "types.h"

namespace tejas {

    namespace board {

        using BitBoard = u64;

        constexpr BitBoard MASK_RANK_1      = 0x00000000000000FF;
        constexpr BitBoard MASK_RANK_8      = 0xFF00000000000000;
        constexpr BitBoard MASK_NOT_A_FILE  = 0xFEFEFEFEFEFEFEFE;
        constexpr BitBoard MASK_NOT_H_FILE  = 0x7F7F7F7F7F7F7F7F;
        constexpr BitBoard MASK_NOT_AB_FILE = 0xFCFCFCFCFCFCFCFC;
        constexpr BitBoard MASK_NOT_GH_FILE = 0x3F3F3F3F3F3F3F3F;

        constexpr BitBoard North(const BitBoard b) { return b << 8; }
        constexpr BitBoard South(const BitBoard b) { return b >> 8; }
        constexpr BitBoard East(const BitBoard b) { return (b & MASK_NOT_H_FILE) << 1; }
        constexpr BitBoard West(const BitBoard b) { return (b & MASK_NOT_A_FILE) >> 1; }
        constexpr BitBoard NorthEast(const BitBoard b) { return (b & MASK_NOT_H_FILE) << 9; }
        constexpr BitBoard SouthEast(const BitBoard b) { return (b & MASK_NOT_H_FILE) >> 7; }
        constexpr BitBoard SouthWest(const BitBoard b) { return (b & MASK_NOT_A_FILE) >> 9; }
        constexpr BitBoard NorthWest(const BitBoard b) { return (b & MASK_NOT_A_FILE) << 7; }

        constexpr u8 PieceColorIndex(const Piece p) { return (7 + PieceColorOf(p)); }

        class Board {
           private:
            BitBoard bitboards[15];
            Piece    pieces[64];

           public:
            Board();
            ~Board();
            void     Reset();
            void     SetPiece(const Piece, const Square);
            void     ClearPiece(const Piece, const Square);
            void     SetStartpos();
            void     MovePiece(const Piece  piece,
                               const Square from,
                               const Square to,
                               const bool   is_capture   = false,
                               const bool   is_promotion = false,
                               const Piece  promoted     = Piece::NO_PIECE);
            void     UndoMovePiece(const Piece  piece,
                                   const Square from,
                                   const Square to,
                                   const bool   is_capture   = false,
                                   const Piece  captured     = Piece::NO_PIECE,
                                   const bool   is_promotion = false,
                                   const Piece  promoted     = Piece::NO_PIECE);
            BitBoard GetBitboard(const u8 index) const;
            Piece    GetPiece(const Square) const;
            u8       GetPieceCount(const Piece) const;
            bool     IsValid() const;
#ifdef DEBUG
            void Display() const;
#endif
        };

    }

}
