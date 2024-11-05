#ifndef POSITION_H
#define POSITION_H

#include "board.h"
#include "constants.h"
#include "containers.h"
#include "move.h"
#include <cstdint>

struct MoveHistoryEntry {
    Move     move;
    uint8_t  prev_casteling_rights;
    Square   prev_enpassant_target;
    uint32_t prev_half_move_clock;
    uint32_t prev_full_move_number;
    uint64_t prev_hash;

    MoveHistoryEntry() {
        this->move                  = Move();
        this->prev_casteling_rights = NOCA;
        this->prev_enpassant_target = NO_SQ;
        this->prev_half_move_clock  = 0;
        this->prev_full_move_number = 0;
        this->prev_hash             = 0ULL;
    }

    MoveHistoryEntry(const Move     move,
                     const uint8_t  prev_casteling_rights,
                     const Square   prev_enpassant_target,
                     const uint32_t prev_half_move_clock,
                     const uint32_t prev_full_move_number,
                     const uint64_t prev_hash) {
        this->move                  = move;
        this->prev_casteling_rights = prev_casteling_rights;
        this->prev_enpassant_target = prev_enpassant_target;
        this->prev_half_move_clock  = prev_half_move_clock;
        this->prev_full_move_number = prev_full_move_number;
        this->prev_hash             = prev_hash;
    }
};

class Position {
   private:
    Board                              board;
    Color                              active_color;
    uint8_t                            casteling_rights;
    Square                             enpassant_target;
    uint32_t                           half_move_clock;
    uint32_t                           full_move_number;
    int32_t                            ply_count;
    uint64_t                           hash;
    ArrayStack<MoveHistoryEntry, 2048> history;

   private:
    bool move_do_on_complete();

   public:
    Position();
    Position(const Position& rhs);
    void     set_piece(const Piece piece, const Square square);
    void     clear_piece(const Piece piece, const Square square);
    void     set_active_color(const Color active_color);
    void     add_casteling_rights(const CastleFlag castle_flag);
    void     set_enpassant_target(const Square enpassant_target);
    void     set_half_move_clock(const std::uint32_t half_move_clock);
    void     set_full_move_number(const std::uint32_t full_move_number);
    void     reset_ply_count();
    void     reset_hash();
    void     set_start_pos();
    bool     move_do(const Move move);
    bool     move_do(const std::string move_str);
    void     move_undo();
    Piece    get_piece(const Square square) const;
    uint8_t  get_piece_count(const Piece piece) const;
    Color    get_active_color() const;
    uint32_t get_half_move_clock() const;
    uint32_t get_ply_count() const;
    uint64_t get_hash() const;
    bool     is_valid() const;
    bool     is_in_check() const;
    bool     is_repeated() const;
    void     generate_pseudolegal_moves(ArrayList<Move>* move_list, const bool only_captures) const;
    void     display() const;
};

void position_init();

#endif
