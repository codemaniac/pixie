#include "move.h"

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

        void Move::setScore(const u32 s) { score = s; }

        Piece Move::getPiece() const { return piece; }

        Square Move::getFrom() const { return from; }

        Square Move::getTo() const { return to; }

        Piece Move::getCaptured() const { return captured; }

        MoveFlag Move::getFlag() const { return flag; }

        u32 Move::getScore() const { return score; }

        u32 Move::id() const { return (flag << 16) | (captured << 12) | (to << 6) | from; }

        bool Move::operator==(Move const& rhs) { return id() == rhs.id(); }
    }
}
