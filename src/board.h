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

        constexpr BitBoard north(const BitBoard b) { return b << 8; }
        constexpr BitBoard south(const BitBoard b) { return b >> 8; }
        constexpr BitBoard east(const BitBoard b) { return (b & MASK_NOT_H_FILE) << 1; }
        constexpr BitBoard west(const BitBoard b) { return (b & MASK_NOT_A_FILE) >> 1; }
        constexpr BitBoard northEast(const BitBoard b) { return (b & MASK_NOT_H_FILE) << 9; }
        constexpr BitBoard southEast(const BitBoard b) { return (b & MASK_NOT_H_FILE) >> 7; }
        constexpr BitBoard southWest(const BitBoard b) { return (b & MASK_NOT_A_FILE) >> 9; }
        constexpr BitBoard northWest(const BitBoard b) { return (b & MASK_NOT_A_FILE) << 7; }

        constexpr u8 bitboardColorSlot(const Piece p) { return (7 + pieceColorOf(p)); }

        class Board {
           private:
            BitBoard bitboards[15];
            Piece    pieces[64];

           public:
            Board();
            ~Board();
            void     reset();
            void     setPiece(const Piece, const Square);
            void     clearPiece(const Piece, const Square);
            void     setStartpos();
            void     movePiece(const Piece  piece,
                               const Square from,
                               const Square to,
                               const bool   is_capture   = false,
                               const bool   is_promotion = false,
                               const Piece  promoted     = Piece::NO_PIECE);
            void     undoMovePiece(const Piece  piece,
                                   const Square from,
                                   const Square to,
                                   const bool   is_capture   = false,
                                   const Piece  captured     = Piece::NO_PIECE,
                                   const bool   is_promotion = false,
                                   const Piece  promoted     = Piece::NO_PIECE);
            BitBoard getBitboard(const u8 index) const;
            Piece    getPiece(const Square) const;
            u8       getPieceCount(const Piece) const;
            bool     isValid() const;
#ifdef DEBUG
            void display() const;
#endif
        };

    }

}
