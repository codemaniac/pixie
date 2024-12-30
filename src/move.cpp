#include "move.h"
#include "types.h"

namespace tejas {

    namespace move {

        Move::Move() :
            piece(Piece::NO_PIECE),
            from(Square::NO_SQ),
            to(Square::NO_SQ),
            captured(Piece::NO_PIECE),
            flag(MOVE_QUIET),
            score(0) {}

        Move::Move(const Piece    piece,
                   const Square   from,
                   const Square   to,
                   const Piece    captured,
                   const MoveFlag flag) :
            piece(piece),
            from(from),
            to(to),
            captured(captured),
            flag(flag),
            score(0) {}

        void Move::set_score(const u32 s) { score = s; }

        Piece Move::get_piece() const { return piece; }

        Square Move::get_from() const { return from; }

        Square Move::get_to() const { return to; }

        Piece Move::get_captured() const { return captured; }

        MoveFlag Move::get_flag() const { return flag; }

        u32 Move::get_score() const { return score; }

        u32 Move::id() const { return (flag << 16) | (captured << 12) | (to << 6) | from; }

        bool Move::operator==(Move const& rhs) { return id() == rhs.id(); }
    }
}
