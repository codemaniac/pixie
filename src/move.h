#pragma once

#include "types.h"

namespace tejas {

    namespace move {

        enum MoveFlag : u8 {
            MOVE_QUIET,
            MOVE_QUIET_PAWN_DBL_PUSH,
            MOVE_CASTLE_KING_SIDE,
            MOVE_CASTLE_QUEEN_SIDE,
            MOVE_CAPTURE,
            MOVE_CAPTURE_EP,
            MOVE_PROMOTION_KNIGHT = 8,
            MOVE_PROMOTION_BISHOP,
            MOVE_PROMOTION_ROOK,
            MOVE_PROMOTION_QUEEN,
            MOVE_CAPTURE_PROMOTION_KNIGHT,
            MOVE_CAPTURE_PROMOTION_BISHOP,
            MOVE_CAPTURE_PROMOTION_ROOK,
            MOVE_CAPTURE_PROMOTION_QUEEN,
        };

        class Move {
           private:
            Piece    piece;
            Square   from;
            Square   to;
            Piece    captured;
            MoveFlag flag;
            u32      score;

           public:
            Move();
            Move(const Piece    piece,
                 const Square   from,
                 const Square   to,
                 const Piece    captured,
                 const MoveFlag flag);
            void     SetScore(const u32);
            Piece    GetPiece() const;
            Square   GetFrom() const;
            Square   GetTo() const;
            Piece    GetCaptured() const;
            MoveFlag GetFlag() const;
            u32      GetScore() const;
            u32      Id() const;
            bool     operator==(Move const& rhs);
        };

        constexpr bool MOVE_IS_CAPTURE(const MoveFlag m) { return (m & 0x4); }

        constexpr bool MOVE_IS_PROMOTION(const MoveFlag m) { return (m & 0x8); }

    }

}
