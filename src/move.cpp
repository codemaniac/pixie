#include "include/move.h"
#include "include/board.h"
#include "include/constants.h"
#include <cstdint>
#include <string>

// clang-format off
const uint16_t MOVE_SCORE_MVV_LVA[36] = {
 	10105, 10205, 10305, 10405, 10505, 10605,
	10104, 10204, 10304, 10404, 10504, 10604,
	10103, 10203, 10303, 10403, 10503, 10603,
	10102, 10202, 10302, 10402, 10502, 10602,
	10101, 10201, 10301, 10401, 10501, 10601,
	10100, 10200, 10300, 10400, 10500, 10600
};
// clang-format on

Move::Move() {
    this->from     = 0;
    this->to       = 0;
    this->flag     = 0;
    this->captured = 0;
    this->score    = 0;
}

Move::Move(const Move& move) {
    this->from     = move.get_from();
    this->to       = move.get_to();
    this->flag     = move.get_flag();
    this->captured = move.get_captured();
    this->score    = move.get_score();
}

Move::Move(const Square       from,
           const Square       to,
           const MoveFlag     flag,
           const Piece        captured,
           const unsigned int score) {
    this->from     = static_cast<unsigned int>(from);
    this->to       = static_cast<unsigned int>(to);
    this->flag     = static_cast<unsigned int>(flag);
    this->captured = static_cast<unsigned int>(captured);
    this->score    = score;
}

void Move::set_score(unsigned int score) { this->score = score; }

Square Move::get_from() const { return static_cast<Square>(this->from); }

Square Move::get_to() const { return static_cast<Square>(this->to); }

MoveFlag Move::get_flag() const { return static_cast<MoveFlag>(this->flag); }

Piece Move::get_captured() const { return static_cast<Piece>(this->captured); }

unsigned int Move::get_score() const { return this->score; }

unsigned int Move::get_id() const {
    return (this->captured << 16) | (this->flag << 12) | (this->to << 6) | (this->from);
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
