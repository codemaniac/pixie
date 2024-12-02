#include "include/board.h"
#include "include/constants.h"
#include "include/utils.h"
#include <cstdint>
#include <iostream>
#include <string>

static const uint64_t MAGIC_BISHOP[64] = {
  0x89a1121896040240ULL, 0x2004844802002010ULL, 0x2068080051921000ULL, 0x62880a0220200808ULL,
  0x4042004000000ULL,    0x100822020200011ULL,  0xc00444222012000aULL, 0x28808801216001ULL,
  0x400492088408100ULL,  0x201c401040c0084ULL,  0x840800910a0010ULL,   0x82080240060ULL,
  0x2000840504006000ULL, 0x30010c4108405004ULL, 0x1008005410080802ULL, 0x8144042209100900ULL,
  0x208081020014400ULL,  0x4800201208ca00ULL,   0xf18140408012008ULL,  0x1004002802102001ULL,
  0x841000820080811ULL,  0x40200200a42008ULL,   0x800054042000ULL,     0x88010400410c9000ULL,
  0x520040470104290ULL,  0x1004040051500081ULL, 0x2002081833080021ULL, 0x400c00c010142ULL,
  0x941408200c002000ULL, 0x658810000806011ULL,  0x188071040440a00ULL,  0x4800404002011c00ULL,
  0x104442040404200ULL,  0x511080202091021ULL,  0x4022401120400ULL,    0x80c0040400080120ULL,
  0x8040010040820802ULL, 0x480810700020090ULL,  0x102008e00040242ULL,  0x809005202050100ULL,
  0x8002024220104080ULL, 0x431008804142000ULL,  0x19001802081400ULL,   0x200014208040080ULL,
  0x3308082008200100ULL, 0x41010500040c020ULL,  0x4012020c04210308ULL, 0x208220a202004080ULL,
  0x111040120082000ULL,  0x6803040141280a00ULL, 0x2101004202410000ULL, 0x8200000041108022ULL,
  0x21082088000ULL,      0x2410204010040ULL,    0x40100400809000ULL,   0x822088220820214ULL,
  0x40808090012004ULL,   0x910224040218c9ULL,   0x402814422015008ULL,  0x90014004842410ULL,
  0x1000042304105ULL,    0x10008830412a00ULL,   0x2520081090008908ULL, 0x40102000a0a60140ULL,
};

static const uint64_t MAGIC_ROOK[64] = {
  0xa8002c000108020ULL,  0x6c00049b0002001ULL,  0x100200010090040ULL,  0x2480041000800801ULL,
  0x280028004000800ULL,  0x900410008040022ULL,  0x280020001001080ULL,  0x2880002041000080ULL,
  0xa000800080400034ULL, 0x4808020004000ULL,    0x2290802004801000ULL, 0x411000d00100020ULL,
  0x402800800040080ULL,  0xb000401004208ULL,    0x2409000100040200ULL, 0x1002100004082ULL,
  0x22878001e24000ULL,   0x1090810021004010ULL, 0x801030040200012ULL,  0x500808008001000ULL,
  0xa08018014000880ULL,  0x8000808004000200ULL, 0x201008080010200ULL,  0x801020000441091ULL,
  0x800080204005ULL,     0x1040200040100048ULL, 0x120200402082ULL,     0xd14880480100080ULL,
  0x12040280080080ULL,   0x100040080020080ULL,  0x9020010080800200ULL, 0x813241200148449ULL,
  0x491604001800080ULL,  0x100401000402001ULL,  0x4820010021001040ULL, 0x400402202000812ULL,
  0x209009005000802ULL,  0x810800601800400ULL,  0x4301083214000150ULL, 0x204026458e001401ULL,
  0x40204000808000ULL,   0x8001008040010020ULL, 0x8410820820420010ULL, 0x1003001000090020ULL,
  0x804040008008080ULL,  0x12000810020004ULL,   0x1000100200040208ULL, 0x430000a044020001ULL,
  0x280009023410300ULL,  0xe0100040002240ULL,   0x200100401700ULL,     0x2244100408008080ULL,
  0x8000400801980ULL,    0x2000810040200ULL,    0x8010100228810400ULL, 0x2000009044210200ULL,
  0x4080008040102101ULL, 0x40002080411d01ULL,   0x2005524060000901ULL, 0x502001008400422ULL,
  0x489a000810200402ULL, 0x1004400080a13ULL,    0x4000011008020084ULL, 0x26002114058042ULL,
};

struct AttackMask {
    BitBoard mask;
    uint8_t  mask_bits;
};

static AttackMask ATTACK_MASK_TABLE_BISHOP[64];
static AttackMask ATTACK_MASK_TABLE_ROOK[64];

static BitBoard ATTACK_TABLE_PAWN[2][64];
static BitBoard ATTACK_TABLE_KNIGHT[64];
static BitBoard ATTACK_TABLE_BISHOP[64][512];
static BitBoard ATTACK_TABLE_ROOK[64][4096];
static BitBoard ATTACK_TABLE_KING[64];

/*
* Bitboard utility functions
*/

constexpr static BitBoard bitboard_north_one(const BitBoard b) { return b << 8; }

constexpr static BitBoard bitboard_south_one(const BitBoard b) { return b >> 8; }

constexpr static BitBoard bitboard_east_one(const BitBoard b) {
    return (b & BOARD_MASK_NOT_H_FILE) << 1;
}

constexpr static BitBoard bitboard_west_one(const BitBoard b) {
    return (b & BOARD_MASK_NOT_A_FILE) >> 1;
}

constexpr static BitBoard bitboard_north_east_one(const BitBoard b) {
    return (b & BOARD_MASK_NOT_H_FILE) << 9;
}

constexpr static BitBoard bitboard_south_east_one(const BitBoard b) {
    return (b & BOARD_MASK_NOT_H_FILE) >> 7;
}

constexpr static BitBoard bitboard_south_west_one(const BitBoard b) {
    return (b & BOARD_MASK_NOT_A_FILE) >> 9;
}

constexpr static BitBoard bitboard_north_west_one(const BitBoard b) {
    return (b & BOARD_MASK_NOT_A_FILE) << 7;
}

/*
* Initialize Magic Bitboard attack tables
*/

static BitBoard magicbb_get_attack_mask_bishop(const Square sq) {
    int8_t r, f;

    const uint8_t tr = static_cast<uint8_t>(BOARD_SQ_TO_RANK(sq));
    const uint8_t tf = static_cast<uint8_t>(BOARD_SQ_TO_FILE(sq));

    BitBoard attack_mask = 0ULL;

    for (r = tr + 1, f = tf + 1; r <= 6 && f <= 6; r++, f++)
    {
        attack_mask |= (1ULL << static_cast<uint8_t>(
                          BOARD_RF_TO_SQ(static_cast<Rank>(r), static_cast<File>(f))));
    }
    for (r = tr + 1, f = tf - 1; r <= 6 && f >= 1; r++, f--)
    {
        attack_mask |= (1ULL << static_cast<uint8_t>(
                          BOARD_RF_TO_SQ(static_cast<Rank>(r), static_cast<File>(f))));
    }
    for (r = tr - 1, f = tf + 1; r >= 1 && f <= 6; r--, f++)
    {
        attack_mask |= (1ULL << static_cast<uint8_t>(
                          BOARD_RF_TO_SQ(static_cast<Rank>(r), static_cast<File>(f))));
    }
    for (r = tr - 1, f = tf - 1; r >= 1 && f >= 1; r--, f--)
    {
        attack_mask |= (1ULL << static_cast<uint8_t>(
                          BOARD_RF_TO_SQ(static_cast<Rank>(r), static_cast<File>(f))));
    }

    return attack_mask;
}

static BitBoard magicbb_get_attack_vector_bishop(const Square sq, const BitBoard blockers) {
    int8_t r, f;

    const uint8_t tr = static_cast<uint8_t>(BOARD_SQ_TO_RANK(sq));
    const uint8_t tf = static_cast<uint8_t>(BOARD_SQ_TO_FILE(sq));

    BitBoard attack_mask = 0ULL;
    BitBoard sqb;

    for (r = tr + 1, f = tf + 1; r <= 7 && f <= 7; r++, f++)
    {
        sqb =
          1ULL << static_cast<uint8_t>(BOARD_RF_TO_SQ(static_cast<Rank>(r), static_cast<File>(f)));
        attack_mask |= sqb;
        if (blockers & sqb)
            break;
    }
    for (r = tr + 1, f = tf - 1; r <= 7 && f >= 0; r++, f--)
    {
        sqb =
          1ULL << static_cast<uint8_t>(BOARD_RF_TO_SQ(static_cast<Rank>(r), static_cast<File>(f)));
        attack_mask |= sqb;
        if (blockers & sqb)
            break;
    }
    for (r = tr - 1, f = tf + 1; r >= 0 && f <= 7; r--, f++)
    {
        sqb =
          1ULL << static_cast<uint8_t>(BOARD_RF_TO_SQ(static_cast<Rank>(r), static_cast<File>(f)));
        attack_mask |= sqb;
        if (blockers & sqb)
            break;
    }
    for (r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--)
    {
        sqb =
          1ULL << static_cast<uint8_t>(BOARD_RF_TO_SQ(static_cast<Rank>(r), static_cast<File>(f)));
        attack_mask |= sqb;
        if (blockers & sqb)
            break;
    }

    return attack_mask;
}

static void init_attack_mask_table_bishop(void) {
    BitBoard mask;
    uint8_t  mask_bits;

    for (uint8_t sq = 0; sq < 64; sq++)
    {
        mask      = magicbb_get_attack_mask_bishop(static_cast<Square>(sq));
        mask_bits = utils_bit_count1s(mask);

        ATTACK_MASK_TABLE_BISHOP[sq].mask      = mask;
        ATTACK_MASK_TABLE_BISHOP[sq].mask_bits = mask_bits;
    }
}

static BitBoard magicbb_get_attack_mask_rook(const Square sq) {
    int8_t r, f;

    const uint8_t tr = static_cast<uint8_t>(BOARD_SQ_TO_RANK(sq));
    const uint8_t tf = static_cast<uint8_t>(BOARD_SQ_TO_FILE(sq));

    BitBoard attack_mask = 0ULL;

    for (r = tr + 1; r <= 6; r++)
    {
        attack_mask |= (1ULL << static_cast<uint8_t>(
                          BOARD_RF_TO_SQ(static_cast<Rank>(r), static_cast<File>(tf))));
    }
    for (r = tr - 1; r >= 1; r--)
    {
        attack_mask |= (1ULL << static_cast<uint8_t>(
                          BOARD_RF_TO_SQ(static_cast<Rank>(r), static_cast<File>(tf))));
    }
    for (f = tf + 1; f <= 6; f++)
    {
        attack_mask |= (1ULL << static_cast<uint8_t>(
                          BOARD_RF_TO_SQ(static_cast<Rank>(tr), static_cast<File>(f))));
    }
    for (f = tf - 1; f >= 1; f--)
    {
        attack_mask |= (1ULL << static_cast<uint8_t>(
                          BOARD_RF_TO_SQ(static_cast<Rank>(tr), static_cast<File>(f))));
    }

    return attack_mask;
}

static BitBoard magicbb_get_attack_vector_rook(const Square sq, const BitBoard blockers) {
    int8_t r, f;

    const uint8_t tr = static_cast<uint8_t>(BOARD_SQ_TO_RANK(sq));
    const uint8_t tf = static_cast<uint8_t>(BOARD_SQ_TO_FILE(sq));

    BitBoard attack_mask = 0ULL;
    BitBoard sqb;

    for (r = tr + 1; r <= 7; r++)
    {
        sqb =
          1ULL << static_cast<uint8_t>(BOARD_RF_TO_SQ(static_cast<Rank>(r), static_cast<File>(tf)));
        attack_mask |= sqb;
        if (blockers & sqb)
            break;
    }
    for (r = tr - 1; r >= 0; r--)
    {
        sqb =
          1ULL << static_cast<uint8_t>(BOARD_RF_TO_SQ(static_cast<Rank>(r), static_cast<File>(tf)));
        attack_mask |= sqb;
        if (blockers & sqb)
            break;
    }
    for (f = tf + 1; f <= 7; f++)
    {
        sqb =
          1ULL << static_cast<uint8_t>(BOARD_RF_TO_SQ(static_cast<Rank>(tr), static_cast<File>(f)));
        attack_mask |= sqb;
        if (blockers & sqb)
            break;
    }
    for (f = tf - 1; f >= 0; f--)
    {
        sqb =
          1ULL << static_cast<uint8_t>(BOARD_RF_TO_SQ(static_cast<Rank>(tr), static_cast<File>(f)));
        attack_mask |= sqb;
        if (blockers & sqb)
            break;
    }

    return attack_mask;
}

static void init_attack_mask_table_rook(void) {
    BitBoard mask;
    uint8_t  mask_bits;

    for (uint8_t sq = 0; sq < 64; sq++)
    {
        mask      = magicbb_get_attack_mask_rook(static_cast<Square>(sq));
        mask_bits = utils_bit_count1s(mask);

        ATTACK_MASK_TABLE_ROOK[sq].mask      = mask;
        ATTACK_MASK_TABLE_ROOK[sq].mask_bits = mask_bits;
    }
}

static BitBoard magicbb_create_variant(int index, int bits, BitBoard m) {
    int      i, j;
    BitBoard result = 0ULL;
    for (i = 0; i < bits; i++)
    {
        j = utils_bit_bitscan_forward(&m);
        if (index & (1 << i))
            result |= (1ULL << j);
    }
    return result;
}

static int magicbb_get_magic_index(BitBoard b, BitBoard magic, int bits) {
    return (int) ((b * magic) >> (64 - bits));
}

static void init_attack_table_bishop(void) {
    BitBoard mask, b;
    uint8_t  n;
    int      magic_index;

    for (uint8_t sq = 0; sq < 64; sq++)
    {
        mask = ATTACK_MASK_TABLE_BISHOP[sq].mask;
        n    = ATTACK_MASK_TABLE_BISHOP[sq].mask_bits;

        for (uint16_t i = 0; i < (1 << n); i++)
        {
            b           = magicbb_create_variant(i, n, mask);
            magic_index = magicbb_get_magic_index(b, MAGIC_BISHOP[sq], n);
            ATTACK_TABLE_BISHOP[sq][magic_index] =
              magicbb_get_attack_vector_bishop(static_cast<Square>(sq), b);
        }
    }
}

static void init_attack_table_rook(void) {
    BitBoard mask, b;
    uint8_t  n;
    int      magic_index;

    for (uint8_t sq = 0; sq < 64; sq++)
    {
        mask = ATTACK_MASK_TABLE_ROOK[sq].mask;
        n    = ATTACK_MASK_TABLE_ROOK[sq].mask_bits;

        for (uint16_t i = 0; i < (1 << n); i++)
        {
            b           = magicbb_create_variant(i, n, mask);
            magic_index = magicbb_get_magic_index(b, MAGIC_ROOK[sq], n);
            ATTACK_TABLE_ROOK[sq][magic_index] =
              magicbb_get_attack_vector_rook(static_cast<Square>(sq), b);
        }
    }
}

static void init_attack_table_pawn(void) {
    Square   sq;
    BitBoard b;
    BitBoard attacks;

    // White pawn attacks
    for (uint8_t r = RANK_1; r <= RANK_8; r++)
    {
        for (uint8_t f = FILE_A; f <= FILE_H; f++)
        {
            sq      = BOARD_RF_TO_SQ(static_cast<Rank>(r), static_cast<File>(f));
            b       = 1ULL << sq;
            attacks = bitboard_north_east_one(b) | bitboard_north_west_one(b);
            ATTACK_TABLE_PAWN[WHITE][sq] = attacks;
        }
    }

    // Black pawn attacks
    for (int8_t r = RANK_8; r >= RANK_1; r--)
    {
        for (uint8_t f = FILE_A; f <= FILE_H; f++)
        {
            sq      = BOARD_RF_TO_SQ(static_cast<Rank>(r), static_cast<File>(f));
            b       = 1ULL << sq;
            attacks = bitboard_south_east_one(b) | bitboard_south_west_one(b);
            ATTACK_TABLE_PAWN[BLACK][sq] = attacks;
        }
    }
}

static void init_attack_table_knight(void) {
    BitBoard attacks = 0ULL;
    BitBoard b;

    for (uint8_t sq = 0; sq < 64; sq++)
    {
        b = 1ULL << sq;

        attacks = 0ULL;
        attacks |= (b & BOARD_MASK_NOT_H_FILE) << 17;
        attacks |= (b & BOARD_MASK_NOT_GH_FILE) << 10;
        attacks |= (b & BOARD_MASK_NOT_GH_FILE) >> 6;
        attacks |= (b & BOARD_MASK_NOT_H_FILE) >> 15;
        attacks |= (b & BOARD_MASK_NOT_A_FILE) << 15;
        attacks |= (b & BOARD_MASK_NOT_AB_FILE) << 6;
        attacks |= (b & BOARD_MASK_NOT_AB_FILE) >> 10;
        attacks |= (b & BOARD_MASK_NOT_A_FILE) >> 17;

        ATTACK_TABLE_KNIGHT[sq] = attacks;
    }
}

static void init_attack_table_king(void) {
    BitBoard attacks = 0ULL;
    BitBoard b;

    for (uint8_t sq = 0; sq < 64; sq++)
    {
        b = 1ULL << sq;

        attacks = 0ULL;
        attacks |= bitboard_north_one(b);
        attacks |= bitboard_south_one(b);
        attacks |= bitboard_east_one(b);
        attacks |= bitboard_west_one(b);
        attacks |= bitboard_north_east_one(b);
        attacks |= bitboard_south_east_one(b);
        attacks |= bitboard_south_west_one(b);
        attacks |= bitboard_north_west_one(b);

        ATTACK_TABLE_KING[sq] = attacks;
    }
}

/*
* Movegen attack functions
*/

static BitBoard movegen_get_pawn_attacks(const Square sq, const Color c, const BitBoard occupancy) {
    return ATTACK_TABLE_PAWN[c][sq] & occupancy;
}

static BitBoard movegen_get_knight_attacks(const Square sq, const BitBoard occupancy) {
    return ATTACK_TABLE_KNIGHT[sq] & occupancy;
}

static BitBoard movegen_get_bishop_attacks(const Square sq, BitBoard occupancy) {
    occupancy = occupancy & ATTACK_MASK_TABLE_BISHOP[sq].mask;
    const int magic_index =
      magicbb_get_magic_index(occupancy, MAGIC_BISHOP[sq], ATTACK_MASK_TABLE_BISHOP[sq].mask_bits);
    return ATTACK_TABLE_BISHOP[sq][magic_index];
}

static BitBoard movegen_get_rook_attacks(const Square sq, BitBoard occupancy) {
    occupancy = occupancy & ATTACK_MASK_TABLE_ROOK[sq].mask;
    const int magic_index =
      magicbb_get_magic_index(occupancy, MAGIC_ROOK[sq], ATTACK_MASK_TABLE_ROOK[sq].mask_bits);
    return ATTACK_TABLE_ROOK[sq][magic_index];
}

static BitBoard movegen_get_queen_attacks(const Square sq, BitBoard occupancy) {
    return movegen_get_bishop_attacks(sq, occupancy) | movegen_get_rook_attacks(sq, occupancy);
}

static BitBoard movegen_get_king_attacks(const Square sq, const BitBoard occupancy) {
    return ATTACK_TABLE_KING[sq] & occupancy;
}

/*
* Board functions
*/

void board_init() {
    init_attack_table_pawn();
    init_attack_table_knight();
    init_attack_mask_table_bishop();
    init_attack_table_bishop();
    init_attack_mask_table_rook();
    init_attack_table_rook();
    init_attack_table_king();
}

Board::Board() { this->reset(); }

void Board::reset() {
    for (std::uint8_t p = 0; p < 15; p++)
    {
        this->bitboards[p]   = 0ULL;
        this->piece_count[p] = 0ULL;
    }
    for (std::uint8_t sq = 0; sq < 64; sq++)
        this->pieces[sq] = NO_PIECE;

    this->bitboards[NO_PIECE] = 0xFFFFFFFFFFFFFFFF;
}

void Board::set_piece(const Piece piece, const Square square) {
    const BitBoard bit          = 1ULL << square;
    const BitBoard bit_inverted = ~(bit);
    this->bitboards[piece] |= bit;
    this->bitboards[NO_PIECE] &= bit_inverted;
    this->bitboards[BOARD_WHITE_PIECES_IDX + PIECE_GET_COLOR(piece)] |= bit;
    this->pieces[square] = piece;
    this->piece_count[piece]++;
    this->piece_count[NO_PIECE]--;
    this->piece_count[BOARD_WHITE_PIECES_IDX + PIECE_GET_COLOR(piece)]++;
}

void Board::clear_piece(const Piece piece, const Square square) {
    const BitBoard bit          = 1ULL << square;
    const BitBoard bit_inverted = ~(bit);
    this->bitboards[piece] &= bit_inverted;
    this->bitboards[NO_PIECE] |= bit;
    this->bitboards[BOARD_WHITE_PIECES_IDX + PIECE_GET_COLOR(piece)] &= bit_inverted;
    this->pieces[square] = NO_PIECE;
    this->piece_count[piece]--;
    this->piece_count[NO_PIECE]++;
    this->piece_count[BOARD_WHITE_PIECES_IDX + PIECE_GET_COLOR(piece)]--;
}

void Board::set_start_pos() {
    this->reset();
    this->set_piece(WHITE_ROOK, A1);
    this->set_piece(WHITE_KNIGHT, B1);
    this->set_piece(WHITE_BISHOP, C1);
    this->set_piece(WHITE_QUEEN, D1);
    this->set_piece(WHITE_KING, E1);
    this->set_piece(WHITE_BISHOP, F1);
    this->set_piece(WHITE_KNIGHT, G1);
    this->set_piece(WHITE_ROOK, H1);

    this->set_piece(WHITE_PAWN, A2);
    this->set_piece(WHITE_PAWN, B2);
    this->set_piece(WHITE_PAWN, C2);
    this->set_piece(WHITE_PAWN, D2);
    this->set_piece(WHITE_PAWN, E2);
    this->set_piece(WHITE_PAWN, F2);
    this->set_piece(WHITE_PAWN, G2);
    this->set_piece(WHITE_PAWN, H2);

    this->set_piece(BLACK_ROOK, A8);
    this->set_piece(BLACK_KNIGHT, B8);
    this->set_piece(BLACK_BISHOP, C8);
    this->set_piece(BLACK_QUEEN, D8);
    this->set_piece(BLACK_KING, E8);
    this->set_piece(BLACK_BISHOP, F8);
    this->set_piece(BLACK_KNIGHT, G8);
    this->set_piece(BLACK_ROOK, H8);

    this->set_piece(BLACK_PAWN, A7);
    this->set_piece(BLACK_PAWN, B7);
    this->set_piece(BLACK_PAWN, C7);
    this->set_piece(BLACK_PAWN, D7);
    this->set_piece(BLACK_PAWN, E7);
    this->set_piece(BLACK_PAWN, F7);
    this->set_piece(BLACK_PAWN, G7);
    this->set_piece(BLACK_PAWN, H7);
}

Piece Board::get_piece(const Square square) const {
    return static_cast<Piece>(this->pieces[square]);
}

uint8_t Board::get_piece_count(const Piece piece) const { return this->piece_count[piece]; }

bool Board::is_valid() const {
    if (this->piece_count[WHITE_KING] != 1)
        return false;
    if (this->piece_count[BLACK_KING] != 1)
        return false;
    if (this->bitboards[WHITE_PAWN] & BOARD_MASK_RANK_1)
        return false;
    if (this->bitboards[WHITE_PAWN] & BOARD_MASK_RANK_8)
        return false;
    if (this->bitboards[BLACK_PAWN] & BOARD_MASK_RANK_1)
        return false;
    if (this->bitboards[BLACK_PAWN] & BOARD_MASK_RANK_8)
        return false;
    return true;
}

bool Board::is_square_attacked(const Square square, const Color attacked_by_color) const {
    const Piece queen = PIECE_CREATE(QUEEN, attacked_by_color);
    if (movegen_get_queen_attacks(square, ~this->bitboards[NO_PIECE]) & this->bitboards[queen])
        return true;

    const Piece rook = PIECE_CREATE(ROOK, attacked_by_color);
    if (movegen_get_rook_attacks(square, ~this->bitboards[NO_PIECE]) & this->bitboards[rook])
        return true;

    const Piece bishop = PIECE_CREATE(BISHOP, attacked_by_color);
    if (movegen_get_bishop_attacks(square, ~this->bitboards[NO_PIECE]) & this->bitboards[bishop])
        return true;

    const Piece knight = PIECE_CREATE(KNIGHT, attacked_by_color);
    if (ATTACK_TABLE_KNIGHT[square] & this->bitboards[knight])
        return true;

    const Piece pawn = PIECE_CREATE(PAWN, attacked_by_color);
    if (ATTACK_TABLE_PAWN[!attacked_by_color][square] & this->bitboards[pawn])
        return true;

    const Piece king = PIECE_CREATE(KING, attacked_by_color);
    if (ATTACK_TABLE_KING[square] & this->bitboards[king])
        return true;

    return false;
}

bool Board::is_in_check(const Color active_color) const {
    const Piece  king           = PIECE_CREATE(KING, active_color);
    BitBoard     king_bb        = this->bitboards[king];
    const Square king_pos       = static_cast<Square>(utils_bit_bitscan_forward(&king_bb));
    const Color  opponent_color = static_cast<Color>(active_color ^ 1);
    return this->is_square_attacked(king_pos, opponent_color);
}

void Board::generate_pseudolegal_moves_pawn(const Color      active_color,
                                            const Square     ep_target,
                                            const bool       only_captures,
                                            ArrayList<Move>* move_list) const {
    const Piece pawn = PIECE_CREATE(PAWN, active_color);
    const Rank  pawn_promotion_rank =
      static_cast<Rank>(7 * (active_color ^ 1));  // White = 7; Black = 0
    BitBoard bb = this->bitboards[pawn];
    while (bb)
    {
        const Square from_sq = static_cast<Square>(utils_bit_bitscan_forward(&bb));

        // Capture Moves
        BitBoard occupancy = this->bitboards[BOARD_WHITE_PIECES_IDX + (active_color ^ 1)];
        if (ep_target != NO_SQ)
            occupancy |= (1ULL << ep_target);
        BitBoard attacks = movegen_get_pawn_attacks(from_sq, active_color, occupancy);
        while (attacks)
        {
            const Square to_sq  = static_cast<Square>(utils_bit_bitscan_forward(&attacks));
            const Piece  victim = this->pieces[to_sq];
            if (BOARD_SQ_TO_RANK(to_sq) == pawn_promotion_rank)
            {
                // Pawn Captures + Promotion
                move_list->emplace_back(from_sq, to_sq, MOVE_CAPTURE_PROMOTION_QUEEN, victim,
                                        MOVE_SCORE_MVV_LVA[MOVE_SCORE_MVV_LVA_IDX(
                                          PIECE_GET_TYPE(pawn), PIECE_GET_TYPE(victim))]);
                move_list->emplace_back(from_sq, to_sq, MOVE_CAPTURE_PROMOTION_ROOK, victim,
                                        MOVE_SCORE_MVV_LVA[MOVE_SCORE_MVV_LVA_IDX(
                                          PIECE_GET_TYPE(pawn), PIECE_GET_TYPE(victim))]);
                move_list->emplace_back(from_sq, to_sq, MOVE_CAPTURE_PROMOTION_BISHOP, victim,
                                        MOVE_SCORE_MVV_LVA[MOVE_SCORE_MVV_LVA_IDX(
                                          PIECE_GET_TYPE(pawn), PIECE_GET_TYPE(victim))]);
                move_list->emplace_back(from_sq, to_sq, MOVE_CAPTURE_PROMOTION_KNIGHT, victim,
                                        MOVE_SCORE_MVV_LVA[MOVE_SCORE_MVV_LVA_IDX(
                                          PIECE_GET_TYPE(pawn), PIECE_GET_TYPE(victim))]);
            }
            else if (to_sq == ep_target)
            {
                // En Passant Capture
                const Piece ep_victim = PIECE_CREATE(PAWN, static_cast<Color>(active_color ^ 1));
                move_list->emplace_back(from_sq, to_sq, MOVE_CAPTURE_EP, ep_victim,
                                        MOVE_SCORE_MVV_LVA[MOVE_SCORE_MVV_LVA_IDX(
                                          PIECE_GET_TYPE(pawn), PIECE_GET_TYPE(ep_victim))]);
            }
            else
            {
                // Pawn Captures
                move_list->emplace_back(from_sq, to_sq, MOVE_CAPTURE, victim,
                                        MOVE_SCORE_MVV_LVA[MOVE_SCORE_MVV_LVA_IDX(
                                          PIECE_GET_TYPE(pawn), PIECE_GET_TYPE(victim))]);
            }
        }

        // Quite Moves
        if (only_captures)
            continue;
        occupancy = this->bitboards[NO_PIECE];
        if (active_color == WHITE)
            attacks = bitboard_north_one((1ULL << from_sq)) & occupancy;
        else
            attacks = bitboard_south_one((1ULL << from_sq)) & occupancy;
        if (attacks)
        {
            const Square to_sq = static_cast<Square>(utils_bit_bitscan_forward(&attacks));
            if (BOARD_SQ_TO_RANK(to_sq) == pawn_promotion_rank)
            {
                // Pawn Promotion
                move_list->emplace_back(from_sq, to_sq, MOVE_PROMOTION_QUEEN, NO_PIECE, 0);
                move_list->emplace_back(from_sq, to_sq, MOVE_PROMOTION_ROOK, NO_PIECE, 0);
                move_list->emplace_back(from_sq, to_sq, MOVE_PROMOTION_BISHOP, NO_PIECE, 0);
                move_list->emplace_back(from_sq, to_sq, MOVE_PROMOTION_KNIGHT, NO_PIECE, 0);
            }
            else
                move_list->emplace_back(from_sq, to_sq, MOVE_QUIET, NO_PIECE, 0);
        }
        // Pawn Double push
        if (active_color == WHITE)
        {
            attacks = bitboard_north_one((1ULL << from_sq)) & occupancy;
            attacks = bitboard_north_one(attacks) & occupancy & BOARD_MASK_RANK_4;
        }
        else
        {
            attacks = bitboard_south_one((1ULL << from_sq)) & occupancy;
            attacks = bitboard_south_one(attacks) & occupancy & BOARD_MASK_RANK_5;
        }
        if (attacks)
        {
            const Square to_sq = static_cast<Square>(utils_bit_bitscan_forward(&attacks));
            move_list->emplace_back(from_sq, to_sq, MOVE_QUIET_PAWN_DBL_PUSH, NO_PIECE, 0);
        }
    }
}

void Board::generate_pseudolegal_moves_piece(const Piece      attacker,
                                             const Color      active_color,
                                             const bool       only_captures,
                                             ArrayList<Move>* move_list) const {
    BitBoard bb = this->bitboards[attacker];
    while (bb)
    {
        const Square from_sq = static_cast<Square>(utils_bit_bitscan_forward(&bb));

        // Capture Moves
        BitBoard occupancy = 0ULL;
        BitBoard attacks   = 0ULL;
        switch (PIECE_GET_TYPE(attacker))
        {
            case KNIGHT :
                occupancy = this->bitboards[BOARD_WHITE_PIECES_IDX + (active_color ^ 1)]
                          | this->bitboards[NO_PIECE];
                attacks = movegen_get_knight_attacks(from_sq, occupancy);
                break;
            case BISHOP :
                occupancy = ~this->bitboards[NO_PIECE];
                attacks   = movegen_get_bishop_attacks(from_sq, occupancy);
                break;
            case ROOK :
                occupancy = ~this->bitboards[NO_PIECE];
                attacks   = movegen_get_rook_attacks(from_sq, occupancy);
                break;
            case QUEEN :
                occupancy = ~this->bitboards[NO_PIECE];
                attacks   = movegen_get_queen_attacks(from_sq, occupancy);
                break;
            case KING :
                occupancy = this->bitboards[BOARD_WHITE_PIECES_IDX + (active_color ^ 1)]
                          | this->bitboards[NO_PIECE];
                attacks = movegen_get_king_attacks(from_sq, occupancy);
                break;
            default :
                break;
        }

        while (attacks)
        {
            const Square to_sq  = static_cast<Square>(utils_bit_bitscan_forward(&attacks));
            const Piece  victim = this->pieces[to_sq];

            if (!only_captures && victim == NO_PIECE)
                move_list->emplace_back(from_sq, to_sq, MOVE_QUIET, NO_PIECE, 0);
            else if (victim != NO_PIECE && PIECE_GET_COLOR(victim) != active_color)
                move_list->emplace_back(from_sq, to_sq, MOVE_CAPTURE, victim,
                                        MOVE_SCORE_MVV_LVA[MOVE_SCORE_MVV_LVA_IDX(
                                          PIECE_GET_TYPE(attacker), PIECE_GET_TYPE(victim))]);
        }
    }
}

void Board::generate_pseudolegal_moves_castle(const Color      active_color,
                                              const uint8_t    casteling_rights,
                                              ArrayList<Move>* move_list) const {
    if (this->is_in_check(active_color))
        return;

    if (active_color == WHITE)
    {
        if (casteling_rights & WKCA)
        {
            if (this->pieces[F1] == NO_PIECE && this->pieces[G1] == NO_PIECE
                && this->pieces[H1] == WHITE_ROOK && !this->is_square_attacked(F1, BLACK))
                move_list->emplace_back(E1, G1, MOVE_CASTLE_KING_SIDE, NO_PIECE, 0);
        }
        if (casteling_rights & WQCA)
        {
            if (this->pieces[D1] == NO_PIECE && this->pieces[C1] == NO_PIECE
                && this->pieces[B1] == NO_PIECE && this->pieces[A1] == WHITE_ROOK
                && !this->is_square_attacked(D1, BLACK))
                move_list->emplace_back(E1, C1, MOVE_CASTLE_QUEEN_SIDE, NO_PIECE, 0);
        }
    }
    else
    {
        if (casteling_rights & BKCA)
        {
            if (this->pieces[F8] == NO_PIECE && this->pieces[G8] == NO_PIECE
                && this->pieces[H8] == BLACK_ROOK && !this->is_square_attacked(F8, WHITE))
                move_list->emplace_back(E8, G8, MOVE_CASTLE_KING_SIDE, NO_PIECE, 0);
        }
        if (casteling_rights & BQCA)
        {
            if (this->pieces[D8] == NO_PIECE && this->pieces[C8] == NO_PIECE
                && this->pieces[B8] == NO_PIECE && this->pieces[A8] == BLACK_ROOK
                && !this->is_square_attacked(D8, WHITE))
                move_list->emplace_back(E8, C8, MOVE_CASTLE_QUEEN_SIDE, NO_PIECE, 0);
        }
    }
}

void Board::generate_pseudolegal_moves(const Color      active_color,
                                       const uint8_t    casteling_rights,
                                       const Square     ep_target,
                                       const bool       only_captures,
                                       ArrayList<Move>* move_list) const {
    this->generate_pseudolegal_moves_pawn(active_color, ep_target, only_captures, move_list);
    this->generate_pseudolegal_moves_piece(PIECE_CREATE(KNIGHT, active_color), active_color,
                                           only_captures, move_list);
    this->generate_pseudolegal_moves_piece(PIECE_CREATE(BISHOP, active_color), active_color,
                                           only_captures, move_list);
    this->generate_pseudolegal_moves_piece(PIECE_CREATE(ROOK, active_color), active_color,
                                           only_captures, move_list);
    this->generate_pseudolegal_moves_piece(PIECE_CREATE(QUEEN, active_color), active_color,
                                           only_captures, move_list);
    this->generate_pseudolegal_moves_piece(PIECE_CREATE(KING, active_color), active_color,
                                           only_captures, move_list);
    this->generate_pseudolegal_moves_castle(active_color, casteling_rights, move_list);
}

void Board::display() const {
    const std::string pieces_str = ".PNBRQKXXpnbrqk";

    for (std::int8_t rank = RANK_8; rank >= RANK_1; rank--)
    {
        for (std::int8_t file = FILE_A; file <= FILE_H; file++)
        {
            const Square sq = BOARD_RF_TO_SQ(static_cast<Rank>(rank), static_cast<File>(file));
            std::cout << (char) pieces_str[this->pieces[sq]] << " ";
        }
        std::cout << std::endl;
    }
}
