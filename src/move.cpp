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

        void Move::SetScore(const u32 s) { score = s; }

        Piece Move::GetPiece() const { return piece; }

        Square Move::GetFrom() const { return from; }

        Square Move::GetTo() const { return to; }

        Piece Move::GetCaptured() const { return captured; }

        MoveFlag Move::GetFlag() const { return flag; }

        u32 Move::GetScore() const { return score; }

        u32 Move::Id() const { return (flag << 16) | (captured << 12) | (to << 6) | from; }

        bool Move::operator==(Move const& rhs) { return Id() == rhs.Id(); }
    }
}
