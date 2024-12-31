#pragma once

#include "move.h"
#include "types.h"
#include <vector>

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

        enum CastleFlag : u8 {
            NOCA = 0,
            WKCA = 1,
            WQCA = 2,
            BKCA = 4,
            BQCA = 8
        };

        struct MoveHistoryEntry {
            move::Move move;
            u8         casteling_rights;
            Square     enpassant_target;
            u8         half_move_clock;
            u8         full_move_number;
            u64        hash;

            MoveHistoryEntry() :
                move(move::Move()),
                casteling_rights(CastleFlag::NOCA),
                enpassant_target(Square::NO_SQ),
                half_move_clock(0),
                full_move_number(0),
                hash(0ULL) {}

            MoveHistoryEntry(const move::Move move,
                             u8               casteling_rights,
                             Square           enpassant_target,
                             u8               half_move_clock,
                             u8               full_move_number,
                             u64              hash) :
                move(move),
                casteling_rights(casteling_rights),
                enpassant_target(enpassant_target),
                half_move_clock(half_move_clock),
                full_move_number(full_move_number),
                hash(hash) {}
        };

        class Board {
           private:
            BitBoard                      bitboards[15];
            Piece                         pieces[64];
            Color                         active_color;
            u8                            casteling_rights;
            Square                        enpassant_target;
            u8                            half_move_clock;
            u8                            full_move_number;
            i8                            ply_count;
            u64                           hash;
            std::vector<MoveHistoryEntry> history;

            bool doMoveComplete();

           public:
            static void initialize();
            Board();
            ~Board();
            void               reset();
            void               resetHash();
            void               setPiece(const Piece, const Square);
            void               clearPiece(const Piece, const Square);
            void               setStartpos();
            void               movePiece(const Piece  piece,
                                         const Square from,
                                         const Square to,
                                         const bool   is_capture   = false,
                                         const bool   is_promotion = false,
                                         const Piece  promoted     = Piece::NO_PIECE);
            void               undoMovePiece(const Piece  piece,
                                             const Square from,
                                             const Square to,
                                             const bool   is_capture   = false,
                                             const Piece  captured     = Piece::NO_PIECE,
                                             const bool   is_promotion = false,
                                             const Piece  promoted     = Piece::NO_PIECE);
            void               setActiveColor(const Color);
            void               addCastelingRights(const CastleFlag);
            void               setEnpassantTarget(const Square);
            void               setHalfmoveClock(const u8);
            void               setFullmoveNumber(const u8);
            void               resetPlyCount();
            [[nodiscard]] bool doMove(const move::Move);
            void               undoMove();
            BitBoard           getBitboard(const u8 index) const;
            Piece              getPiece(const Square) const;
            u8                 getPieceCount(const Piece) const;
            Color              getActiveColor() const;
            u8                 getCastelingRights() const;
            Square             getEnpassantTarget() const;
            u8                 getHalfmoveClock() const;
            u8                 getFullmoveNumber() const;
            i8                 getPlyCount() const;
            u64                getHash() const;
            bool               isValid() const;
            bool               operator==(Board const& rhs) const;
            void               display() const;
        };

    }

}
