#include "include/move.h"
#include "include/board.h"
#include "include/constants.h"
#include <cstdint>
#include <string>

// clang-format off
const uint16_t MOVE_SCORE_MVV_LVA[36] = {
 	105, 205, 305, 405, 505, 605,
	104, 204, 304, 404, 504, 604,
	103, 203, 303, 403, 503, 603,
	102, 202, 302, 402, 502, 602,
	101, 201, 301, 401, 501, 601,
	100, 200, 300, 400, 500, 600
};
// clang-format on

Move::Move() { this->move.id = 0; }

Move::Move(const Move& move) { this->move.id = move.get_id(); }

Move::Move(const Square       from,
           const Square       to,
           const MoveFlag     flag,
           const Piece        captured,
           const unsigned int score) {
    this->move.from     = static_cast<unsigned int>(from);
    this->move.to       = static_cast<unsigned int>(to);
    this->move.flag     = static_cast<unsigned int>(flag);
    this->move.captured = static_cast<unsigned int>(captured);
    this->move.score    = score;
}

void Move::set_score(unsigned int score) { this->move.score = score; }

Square Move::get_from() const { return static_cast<Square>(this->move.from); }

Square Move::get_to() const { return static_cast<Square>(this->move.to); }

MoveFlag Move::get_flag() const { return static_cast<MoveFlag>(this->move.flag); }

Piece Move::get_captured() const { return static_cast<Piece>(this->move.captured); }

unsigned int Move::get_score() const { return this->move.score; }

unsigned int Move::get_id() const { return this->move.id; }

Move& Move::operator=(const Move& move) {
    this->move.id = move.get_id();
    return *this;
}

bool Move::operator==(const Move& other) const { return this->move.id == other.get_id(); }

void Move::display() const {
    const std::string file_str  = "abcdefgh";
    const File        from_file = BOARD_SQ_TO_FILE(this->get_from());
    const Rank        from_rank = BOARD_SQ_TO_RANK(this->get_from());
    const File        to_file   = BOARD_SQ_TO_FILE(this->get_to());
    const Rank        to_rank   = BOARD_SQ_TO_RANK(this->get_to());
    if (this->get_flag() >= MOVE_PROMOTION_KNIGHT)
    {
        const std::string promotion_piece_str = "xxxxxxnbrqnbrq";
        printf("%c%d%c%d%c", file_str[from_file], static_cast<int>(from_rank) + 1,
               file_str[to_file], static_cast<int>(to_rank) + 1,
               promotion_piece_str[this->get_flag()]);
    }
    else
    {
        printf("%c%d%c%d", file_str[from_file], static_cast<int>(from_rank) + 1, file_str[to_file],
               static_cast<int>(to_rank) + 1);
    }
}
