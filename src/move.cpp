#include "include/move.h"
#include "include/board.h"
#include "include/constants.h"
#include <cstdint>
#include <iostream>
#include <string>

// clang-format off

/*

    (Victims) Pawn Knight Bishop   Rook  Queen   King
  (Attackers)
        Pawn   105    205    305    405    505    605
      Knight   104    204    304    404    504    604
      Bishop   103    203    303    403    503    603
        Rook   102    202    302    402    502    602
       Queen   101    201    301    401    501    601
        King   100    200    300    400    500    600

*/

const uint16_t MOVE_SCORE_MVV_LVA[36] = {
 	105, 205, 305, 405, 505, 605,
	104, 204, 304, 404, 504, 604,
	103, 203, 303, 403, 503, 603,
	102, 202, 302, 402, 502, 602,
	101, 201, 301, 401, 501, 601,
	100, 200, 300, 400, 500, 600
};
// clang-format on

Move::Move() {
    this->from     = NO_SQ;
    this->to       = NO_SQ;
    this->flag     = MOVE_QUIET;
    this->captured = NO_PIECE;
    this->score    = 0;
}

Move::Move(const Move& move) {
    this->from     = move.get_from();
    this->to       = move.get_to();
    this->flag     = move.get_flag();
    this->captured = move.get_captured();
    this->score    = move.get_score();
}

Move::Move(const Square   from,
           const Square   to,
           const MoveFlag flag,
           const Piece    captured,
           const uint32_t score) {
    this->from     = from;
    this->to       = to;
    this->flag     = flag;
    this->captured = captured;
    this->score    = score;
}

Move::Move(const uint32_t id) {
    this->from     = static_cast<Square>(id & 0x3F);
    this->to       = static_cast<Square>((id >> 6) & 0x3F);
    this->flag     = static_cast<MoveFlag>((id >> 12) & 0xF);
    this->captured = static_cast<Piece>((id >> 16) & 0xF);
    this->score    = (id >> 20);
}

void Move::set_score(const uint32_t score) { this->score = score; }

Square Move::get_from() const { return this->from; }

Square Move::get_to() const { return this->to; }

MoveFlag Move::get_flag() const { return this->flag; }

Piece Move::get_captured() const { return this->captured; }

uint32_t Move::get_score() const { return this->score; }

uint32_t Move::get_id() const {
    return (this->captured << 16) | (this->flag << 12) | (this->to << 6) | (this->from);
}

uint64_t Move::get_full_id() const {
    return ((uint64_t) this->score << 20) | (this->captured << 16) | (this->flag << 12)
         | (this->to << 6) | (this->from);
}

Move& Move::operator=(const Move& move) {
    this->from     = move.get_from();
    this->to       = move.get_to();
    this->flag     = move.get_flag();
    this->captured = move.get_captured();
    this->score    = move.get_score();
    return *this;
}

bool Move::operator==(const Move& other) const { return this->get_id() == other.get_id(); }

void Move::display() const {
    const std::string file_str  = "abcdefgh";
    const File        from_file = BOARD_SQ_TO_FILE(this->get_from());
    const Rank        from_rank = BOARD_SQ_TO_RANK(this->get_from());
    const File        to_file   = BOARD_SQ_TO_FILE(this->get_to());
    const Rank        to_rank   = BOARD_SQ_TO_RANK(this->get_to());
    if (this->get_flag() >= MOVE_PROMOTION_KNIGHT)
    {
        const std::string promotion_piece_str = "xxxxxxxxnbrqnbrq";
        std::cout << (char) file_str[from_file];
        std::cout << (int) (static_cast<int>(from_rank) + 1);
        std::cout << (char) file_str[to_file];
        std::cout << (int) (static_cast<int>(to_rank) + 1);
        std::cout << (char) promotion_piece_str[this->get_flag()] << std::flush;
    }
    else
    {
        std::cout << (char) file_str[from_file];
        std::cout << (int) (static_cast<int>(from_rank) + 1);
        std::cout << (char) file_str[to_file];
        std::cout << (int) (static_cast<int>(to_rank) + 1) << std::flush;
    }
}
