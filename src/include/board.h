#ifndef BOARD_H
#define BOARD_H

#include "constants.h"
#include "containers.h"
#include "move.h"
#include <cstdint>

typedef std::uint64_t BitBoard;

#define BOARD_WHITE_PIECES_IDX 7
#define BOARD_BLACK_PIECES_IDX 8
#define BOARD_MASK_A_FILE 0x0101010101010101
#define BOARD_MASK_H_FILE 0x8080808080808080
#define BOARD_MASK_NOT_A_FILE 0xFEFEFEFEFEFEFEFE
#define BOARD_MASK_NOT_H_FILE 0x7F7F7F7F7F7F7F7F
#define BOARD_MASK_NOT_AB_FILE 0xFCFCFCFCFCFCFCFC
#define BOARD_MASK_NOT_GH_FILE 0x3F3F3F3F3F3F3F3F
#define BOARD_MASK_RANK_1 0x00000000000000FF
#define BOARD_MASK_RANK_4 0x00000000FF000000
#define BOARD_MASK_RANK_5 0x000000FF00000000
#define BOARD_MASK_RANK_8 0xFF00000000000000
#define BOARD_MASK_A1_H8_DIAGONAL 0x8040201008040201
#define BOARD_MASK_H1_A8_ANTIDIAGONAL 0x0102040810204080
#define BOARD_MASK_LIGHT_SQUARES 0x55AA55AA55AA55AA
#define BOARD_MASK_DARK_SQUARES 0xAA55AA55AA55AA55

constexpr Square BOARD_RF_TO_SQ(Rank r, File f) { return static_cast<Square>(8 * r + f); }
constexpr Rank   BOARD_SQ_TO_RANK(Square sq) { return static_cast<Rank>(sq >> 3); }
constexpr File   BOARD_SQ_TO_FILE(Square sq) { return static_cast<File>(sq & 7); }

class Board {
   private:
    BitBoard     bitboards[15];
    Piece        pieces[64];
    std::uint8_t piece_count[15];

   private:
    bool is_square_attacked(const Square square, const Color attacked_by_color) const;
    void generate_pseudolegal_moves_pawn(const Color      active_color,
                                         const Square     ep_target,
                                         const bool       only_captures,
                                         ArrayList<Move>* move_list) const;
    void generate_pseudolegal_moves_piece(const Piece      attacker,
                                          const Color      active_color,
                                          const bool       only_captures,
                                          ArrayList<Move>* move_list) const;
    void generate_pseudolegal_moves_castle(const Color      active_color,
                                           const uint8_t    casteling_rights,
                                           ArrayList<Move>* move_list) const;

   public:
    Board();
    void    reset();
    void    set_piece(const Piece piece, const Square square);
    void    clear_piece(const Piece piece, const Square square);
    void    set_start_pos();
    Piece   get_piece(const Square square) const;
    uint8_t get_piece_count(const Piece piece) const;
    bool    is_valid() const;
    bool    is_in_check(const Color active_color) const;
    void    generate_pseudolegal_moves(const Color      active_color,
                                       const uint8_t    casteling_rights,
                                       const Square     ep_target,
                                       const bool       only_captures,
                                       ArrayList<Move>* move_list) const;
    void    display() const;
};

void board_init();

#endif
