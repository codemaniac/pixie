#include "include/chess.h"
#include "include/hashkey.h"
#include "include/utils.h"
#include <assert.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define USE_32_BIT_MULTIPLICATIONS
#define MOVEGEN_SINGLE_THREADED

#define MOVE_SCORE_MVV_LVA_IDX(a, v) (((a - 1) * 6) + (v - 1))

static const unsigned long long MAGIC_BISHOP[64] = {
  0x100420000431024ULL,  0x280800101073404ULL,  0x42000a00840802ULL,   0xca800c0410c2ULL,
  0x81004290941c20ULL,   0x400200450020250ULL,  0x444a019204022084ULL, 0x88610802202109aULL,
  0x11210a0800086008ULL, 0x400a08c08802801ULL,  0x1301a0500111c808ULL, 0x1280100480180404ULL,
  0x720009020028445ULL,  0x91880a9000010a01ULL, 0x31200940150802b2ULL, 0x5119080c20000602ULL,
  0x242400a002448023ULL, 0x4819006001200008ULL, 0x222c10400020090ULL,  0x302008420409004ULL,
  0x504200070009045ULL,  0x210071240c02046ULL,  0x1182219000022611ULL, 0x400c50000005801ULL,
  0x4004010000113100ULL, 0x2008121604819400ULL, 0xc4a4010000290101ULL, 0x404a000888004802ULL,
  0x8820c004105010ULL,   0x28280100908300ULL,   0x4c013189c0320a80ULL, 0x42008080042080ULL,
  0x90803000c080840ULL,  0x2180001028220ULL,    0x1084002a040036ULL,   0x212009200401ULL,
  0x128110040c84a84ULL,  0x81488020022802ULL,   0x8c0014100181ULL,     0x2222013020082ULL,
  0xa00100002382c03ULL,  0x1000280001005c02ULL, 0x84801010000114cULL,  0x480410048000084ULL,
  0x21204420080020aULL,  0x2020010000424a10ULL, 0x240041021d500141ULL, 0x420844000280214ULL,
  0x29084a280042108ULL,  0x84102a8080a20a49ULL, 0x104204908010212ULL,  0x40a20280081860c1ULL,
  0x3044000200121004ULL, 0x1001008807081122ULL, 0x50066c000210811ULL,  0xe3001240f8a106ULL,
  0x940c0204030020d4ULL, 0x619204000210826aULL, 0x2010438002b00a2ULL,  0x884042004005802ULL,
  0xa90240000006404ULL,  0x500d082244010008ULL, 0x28190d00040014e0ULL, 0x825201600c082444ULL,
};

static const unsigned long long MAGIC_ROOK[64] = {
  0x2080020500400f0ULL,  0x28444000400010ULL,   0x20000a1004100014ULL, 0x20010c090202006ULL,
  0x8408008200810004ULL, 0x1746000808002ULL,    0x2200098000808201ULL, 0x12c0002080200041ULL,
  0x104000208e480804ULL, 0x8084014008281008ULL, 0x4200810910500410ULL, 0x100014481c20400cULL,
  0x4014a4040020808ULL,  0x401002001010a4ULL,   0x202000500010001ULL,  0x8112808005810081ULL,
  0x40902108802020ULL,   0x42002101008101ULL,   0x459442200810c202ULL, 0x81001103309808ULL,
  0x8110000080102ULL,    0x8812806008080404ULL, 0x104020000800101ULL,  0x40a1048000028201ULL,
  0x4100ba0000004081ULL, 0x44803a4003400109ULL, 0xa010a00000030443ULL, 0x91021a000100409ULL,
  0x4201e8040880a012ULL, 0x22a000440201802ULL,  0x30890a72000204ULL,   0x10411402a0c482ULL,
  0x40004841102088ULL,   0x40230000100040ULL,   0x40100010000a0488ULL, 0x1410100200050844ULL,
  0x100090808508411ULL,  0x1410040024001142ULL, 0x8840018001214002ULL, 0x410201000098001ULL,
  0x8400802120088848ULL, 0x2060080000021004ULL, 0x82101002000d0022ULL, 0x1001101001008241ULL,
  0x9040411808040102ULL, 0x600800480009042ULL,  0x1a020000040205ULL,   0x4200404040505199ULL,
  0x2020081040080080ULL, 0x40a3002000544108ULL, 0x4501100800148402ULL, 0x81440280100224ULL,
  0x88008000000804ULL,   0x8084060000002812ULL, 0x1840201000108312ULL, 0x5080202000000141ULL,
  0x1042a180880281ULL,   0x900802900c01040ULL,  0x8205104104120ULL,    0x9004220000440aULL,
  0x8029510200708ULL,    0x8008440100404241ULL, 0x2420001111000bdULL,  0x4000882304000041ULL,
};

// clang-format off
static const uint16_t MOVE_SCORE_MVV_LVA[36] = {
 	105, 205, 305, 405, 505, 605,
	104, 204, 304, 404, 504, 604,
	103, 203, 303, 403, 503, 603,
	102, 202, 302, 402, 502, 602,
	101, 201, 301, 401, 501, 601,
	100, 200, 300, 400, 500, 600
};
// clang-format on

typedef struct {
    Position pos;
    MoveList moves;
} MovegenThreadData;

typedef struct {
    unsigned long long mask;
    uint8_t            mask_bits;
} AttackMask;

static AttackMask ATTACK_MASK_TABLE_BISHOP[64];
static AttackMask ATTACK_MASK_TABLE_ROOK[64];

static unsigned long long ATTACK_TABLE_PAWN[2][64];
static unsigned long long ATTACK_TABLE_KNIGHT[64];
static unsigned long long ATTACK_TABLE_BISHOP[64][512];
static unsigned long long ATTACK_TABLE_ROOK[64][4096];
static unsigned long long ATTACK_TABLE_KING[64];

/*
* Bitboard utility functions
*/

static unsigned long long _bitboard_north_one(const unsigned long long b) { return b << 8; }

static unsigned long long _bitboard_south_one(const unsigned long long b) { return b >> 8; }

static unsigned long long _bitboard_east_one(const unsigned long long b) {
    return (b & BOARD_MASK_NOT_H_FILE) << 1;
}

static unsigned long long _bitboard_west_one(const unsigned long long b) {
    return (b & BOARD_MASK_NOT_A_FILE) >> 1;
}

static unsigned long long _bitboard_north_east_one(const unsigned long long b) {
    return (b & BOARD_MASK_NOT_H_FILE) << 9;
}

static unsigned long long _bitboard_south_east_one(const unsigned long long b) {
    return (b & BOARD_MASK_NOT_H_FILE) >> 7;
}

static unsigned long long _bitboard_south_west_one(const unsigned long long b) {
    return (b & BOARD_MASK_NOT_A_FILE) >> 9;
}

static unsigned long long _bitboard_north_west_one(const unsigned long long b) {
    return (b & BOARD_MASK_NOT_A_FILE) << 7;
}

/*
* Initialize Magic Bitboard functions
*/

static unsigned long long _init_get_attack_mask_bishop(const uint8_t sq) {
    int8_t r, f;

    uint8_t tr = BOARD_SQ_TO_RANK(sq);
    uint8_t tf = BOARD_SQ_TO_FILE(sq);

    unsigned long long attack_mask = 0ULL;

    for (r = tr + 1, f = tf + 1; r <= 6 && f <= 6; r++, f++)
    {
        attack_mask |= (1ULL << BOARD_RF_TO_SQ(r, f));
    }
    for (r = tr + 1, f = tf - 1; r <= 6 && f >= 1; r++, f--)
    {
        attack_mask |= (1ULL << BOARD_RF_TO_SQ(r, f));
    }
    for (r = tr - 1, f = tf + 1; r >= 1 && f <= 6; r--, f++)
    {
        attack_mask |= (1ULL << BOARD_RF_TO_SQ(r, f));
    }
    for (r = tr - 1, f = tf - 1; r >= 1 && f >= 1; r--, f--)
    {
        attack_mask |= (1ULL << BOARD_RF_TO_SQ(r, f));
    }

    return attack_mask;
}

static unsigned long long _init_get_attack_vector_bishop(const uint8_t            sq,
                                                         const unsigned long long blockers) {
    int8_t r, f;

    uint8_t tr = BOARD_SQ_TO_RANK(sq);
    uint8_t tf = BOARD_SQ_TO_FILE(sq);

    unsigned long long attack_mask = 0ULL;
    unsigned long long sqb;

    for (r = tr + 1, f = tf + 1; r <= 7 && f <= 7; r++, f++)
    {
        sqb = 1ULL << BOARD_RF_TO_SQ(r, f);
        attack_mask |= sqb;
        if (blockers & sqb)
        {
            break;
        }
    }
    for (r = tr + 1, f = tf - 1; r <= 7 && f >= 0; r++, f--)
    {
        sqb = 1ULL << BOARD_RF_TO_SQ(r, f);
        attack_mask |= sqb;
        if (blockers & sqb)
        {
            break;
        }
    }
    for (r = tr - 1, f = tf + 1; r >= 0 && f <= 7; r--, f++)
    {
        sqb = 1ULL << BOARD_RF_TO_SQ(r, f);
        attack_mask |= sqb;
        if (blockers & sqb)
        {
            break;
        }
    }
    for (r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--)
    {
        sqb = 1ULL << BOARD_RF_TO_SQ(r, f);
        attack_mask |= sqb;
        if (blockers & sqb)
        {
            break;
        }
    }

    return attack_mask;
}

static void _init_attack_mask_table_bishop(void) {
    unsigned long long mask;
    uint8_t            mask_bits;

    for (uint8_t sq = 0; sq < 64; sq++)
    {
        mask      = _init_get_attack_mask_bishop(sq);
        mask_bits = utils_bit_count1s(mask);

        ATTACK_MASK_TABLE_BISHOP[sq].mask      = mask;
        ATTACK_MASK_TABLE_BISHOP[sq].mask_bits = mask_bits;
    }
}

static unsigned long long _init_get_attack_mask_rook(const uint8_t sq) {
    int8_t r, f;

    uint8_t tr = BOARD_SQ_TO_RANK(sq);
    uint8_t tf = BOARD_SQ_TO_FILE(sq);

    unsigned long long attack_mask = 0ULL;

    for (r = tr + 1; r <= 6; r++)
    {
        attack_mask |= (1ULL << BOARD_RF_TO_SQ(r, tf));
    }
    for (r = tr - 1; r >= 1; r--)
    {
        attack_mask |= (1ULL << BOARD_RF_TO_SQ(r, tf));
    }
    for (f = tf + 1; f <= 6; f++)
    {
        attack_mask |= (1ULL << BOARD_RF_TO_SQ(tr, f));
    }
    for (f = tf - 1; f >= 1; f--)
    {
        attack_mask |= (1ULL << BOARD_RF_TO_SQ(tr, f));
    }

    return attack_mask;
}

static unsigned long long _init_get_attack_vector_rook(const uint8_t            sq,
                                                       const unsigned long long blockers) {
    int8_t r, f;

    uint8_t tr = BOARD_SQ_TO_RANK(sq);
    uint8_t tf = BOARD_SQ_TO_FILE(sq);

    unsigned long long attack_mask = 0ULL;
    unsigned long long sqb;

    for (r = tr + 1; r <= 7; r++)
    {
        sqb = 1ULL << BOARD_RF_TO_SQ(r, tf);
        attack_mask |= sqb;
        if (blockers & sqb)
        {
            break;
        }
    }
    for (r = tr - 1; r >= 0; r--)
    {
        sqb = 1ULL << BOARD_RF_TO_SQ(r, tf);
        attack_mask |= sqb;
        if (blockers & sqb)
        {
            break;
        }
    }
    for (f = tf + 1; f <= 7; f++)
    {
        sqb = 1ULL << BOARD_RF_TO_SQ(tr, f);
        attack_mask |= sqb;
        if (blockers & sqb)
        {
            break;
        }
    }
    for (f = tf - 1; f >= 0; f--)
    {
        sqb = 1ULL << BOARD_RF_TO_SQ(tr, f);
        attack_mask |= sqb;
        if (blockers & sqb)
        {
            break;
        }
    }

    return attack_mask;
}

static void _init_attack_mask_table_rook(void) {
    unsigned long long mask;
    uint8_t            mask_bits;

    for (uint8_t sq = 0; sq < 64; sq++)
    {
        mask      = _init_get_attack_mask_rook(sq);
        mask_bits = utils_bit_count1s(mask);

        ATTACK_MASK_TABLE_ROOK[sq].mask      = mask;
        ATTACK_MASK_TABLE_ROOK[sq].mask_bits = mask_bits;
    }
}

static unsigned long long _init_create_variant(int index, int bits, unsigned long long m) {
    int                i, j;
    unsigned long long result = 0ULL;
    for (i = 0; i < bits; i++)
    {
        j = utils_bit_bitscan_forward(&m);
        if (index & (1 << i))
            result |= (1ULL << j);
    }
    return result;
}

static int _init_get_magic_index(unsigned long long b, unsigned long long magic, int bits) {
#if defined(USE_32_BIT_MULTIPLICATIONS)
    return (unsigned) ((int) b * (int) magic ^ (int) (b >> 32) * (int) (magic >> 32))
        >> (32 - bits);
#else
    return (int) ((b * magic) >> (64 - bits));
#endif
}

static void _init_attack_table_bishop(void) {
    unsigned long long mask, b;
    uint8_t            n;
    int                magic_index;

    for (unsigned long long sq = 0; sq < 64; sq++)
    {
        mask = ATTACK_MASK_TABLE_BISHOP[sq].mask;
        n    = ATTACK_MASK_TABLE_BISHOP[sq].mask_bits;

        for (uint16_t i = 0; i < (1 << n); i++)
        {
            b                                    = _init_create_variant(i, n, mask);
            magic_index                          = _init_get_magic_index(b, MAGIC_BISHOP[sq], n);
            ATTACK_TABLE_BISHOP[sq][magic_index] = _init_get_attack_vector_bishop(sq, b);
        }
    }
}

static void _init_attack_table_rook(void) {
    unsigned long long mask, b;
    uint8_t            n;
    int                magic_index;

    for (unsigned long long sq = 0; sq < 64; sq++)
    {
        mask = ATTACK_MASK_TABLE_ROOK[sq].mask;
        n    = ATTACK_MASK_TABLE_ROOK[sq].mask_bits;

        for (uint16_t i = 0; i < (1 << n); i++)
        {
            b                                  = _init_create_variant(i, n, mask);
            magic_index                        = _init_get_magic_index(b, MAGIC_ROOK[sq], n);
            ATTACK_TABLE_ROOK[sq][magic_index] = _init_get_attack_vector_rook(sq, b);
        }
    }
}

static void _init_attack_table_pawn(void) {
    Square             sq;
    unsigned long long b;
    unsigned long long attacks;

    // White pawn attacks
    for (uint8_t r = RANK_1; r <= RANK_8; r++)
    {
        for (uint8_t f = FILE_A; f <= FILE_H; f++)
        {
            sq      = BOARD_RF_TO_SQ(r, f);
            b       = 1ULL << sq;
            attacks = _bitboard_north_east_one(b) | _bitboard_north_west_one(b);
            ATTACK_TABLE_PAWN[WHITE][sq] = attacks;
        }
    }

    // Black pawn attacks
    for (int8_t r = RANK_8; r >= RANK_1; r--)
    {
        for (uint8_t f = FILE_A; f <= FILE_H; f++)
        {
            sq      = BOARD_RF_TO_SQ(r, f);
            b       = 1ULL << sq;
            attacks = _bitboard_south_east_one(b) | _bitboard_south_west_one(b);
            ATTACK_TABLE_PAWN[BLACK][sq] = attacks;
        }
    }
}

static void _init_attack_table_knight(void) {
    unsigned long long attacks = 0ULL;
    unsigned long long b;

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

static void _init_attack_table_king(void) {
    unsigned long long attacks = 0ULL;
    unsigned long long b;

    for (uint8_t sq = 0; sq < 64; sq++)
    {
        b = 1ULL << sq;

        attacks = 0ULL;
        attacks |= _bitboard_north_one(b);
        attacks |= _bitboard_south_one(b);
        attacks |= _bitboard_east_one(b);
        attacks |= _bitboard_west_one(b);
        attacks |= _bitboard_north_east_one(b);
        attacks |= _bitboard_south_east_one(b);
        attacks |= _bitboard_south_west_one(b);
        attacks |= _bitboard_north_west_one(b);

        ATTACK_TABLE_KING[sq] = attacks;
    }
}

void chess_initialize(void) {
    _init_attack_table_pawn();
    _init_attack_table_knight();
    _init_attack_mask_table_bishop();
    _init_attack_table_bishop();
    _init_attack_mask_table_rook();
    _init_attack_table_rook();
    _init_attack_table_king();
}

/*
* Movegen Magic Bitboard functions
*/

static unsigned long long _movegen_get_pawn_attacks(int sq, Color c, unsigned long long occupancy) {
    return ATTACK_TABLE_PAWN[c][sq] & occupancy;
}

static unsigned long long _movegen_get_knight_attacks(int sq, unsigned long long occupancy) {
    return ATTACK_TABLE_KNIGHT[sq] & occupancy;
}

static unsigned long long _movegen_get_bishop_attacks(int sq, unsigned long long occupancy) {
    occupancy = occupancy & ATTACK_MASK_TABLE_BISHOP[sq].mask;
    int magic_index =
      _init_get_magic_index(occupancy, MAGIC_BISHOP[sq], ATTACK_MASK_TABLE_BISHOP[sq].mask_bits);
    return ATTACK_TABLE_BISHOP[sq][magic_index];
}

static unsigned long long _movegen_get_rook_attacks(int sq, unsigned long long occupancy) {
    occupancy = occupancy & ATTACK_MASK_TABLE_ROOK[sq].mask;
    int magic_index =
      _init_get_magic_index(occupancy, MAGIC_ROOK[sq], ATTACK_MASK_TABLE_ROOK[sq].mask_bits);
    return ATTACK_TABLE_ROOK[sq][magic_index];
}

static unsigned long long _movegen_get_queen_attacks(int sq, unsigned long long occupancy) {
    return _movegen_get_bishop_attacks(sq, occupancy) | _movegen_get_rook_attacks(sq, occupancy);
}

static unsigned long long _movegen_get_king_attacks(int sq, unsigned long long occupancy) {
    return ATTACK_TABLE_KING[sq] & occupancy;
}

/*
* Position functions
*/

static Board _board_create(void) {
    Board b;

    for (uint8_t p = 0; p < 15; p++)
    {
        b.bitboards[p]   = 0ULL;
        b.piece_count[p] = 0;
    }

    for (uint8_t sq = 0; sq < 64; sq++)
        b.pieces[sq] = 0;

    b.bitboards[NO_PIECE] = 0xFFFFFFFFFFFFFFFF;

    return b;
}

static void _board_display(const Board board) {
    uint8_t sq;

    putchar('\n');

    for (int8_t rank = RANK_8; rank >= RANK_1; rank--)
    {
        for (int8_t file = FILE_A; file <= FILE_H; file++)
        {
            sq = BOARD_RF_TO_SQ(rank, file);
            switch (board.pieces[sq])
            {
                case NO_PIECE:
                    printf(". ");
                    break;
                case WHITE_PAWN:
                    printf("P ");
                    break;
                case WHITE_KNIGHT:
                    printf("N ");
                    break;
                case WHITE_BISHOP:
                    printf("B ");
                    break;
                case WHITE_ROOK:
                    printf("R ");
                    break;
                case WHITE_QUEEN:
                    printf("Q ");
                    break;
                case WHITE_KING:
                    printf("K ");
                    break;
                case BLACK_PAWN:
                    printf("p ");
                    break;
                case BLACK_KNIGHT:
                    printf("n ");
                    break;
                case BLACK_BISHOP:
                    printf("b ");
                    break;
                case BLACK_ROOK:
                    printf("r ");
                    break;
                case BLACK_QUEEN:
                    printf("q ");
                    break;
                case BLACK_KING:
                    printf("k ");
                    break;
                default:
                    printf("X ");
                    break;
            }
            if (sq % 8 == 7)
            {
                putchar('\n');
            }
        }
    }
}

static bool _position_history_is_full(Position* pos) {
    return (pos->move_history.top == pos->move_history.size - 1);
}

static bool _position_history_is_empty(Position* pos) { return (pos->move_history.top == -1); }

static void _position_history_push(Position* pos, MoveHistoryEntry move_history_entry) {
    if (_position_history_is_full(pos))
        return;

    pos->move_history.contents[++pos->move_history.top] = move_history_entry;
}

static MoveHistoryEntry _position_history_pop(Position* pos) {
    if (_position_history_is_empty(pos))
    {
        exit(EXIT_FAILURE);
    }

    return pos->move_history.contents[pos->move_history.top--];
}

Position position_create(void) {
    Position pos;
    pos.board            = _board_create();
    pos.active_color     = WHITE;
    pos.casteling_rights = NOCA;
    pos.enpassant_target = NO_EP_TARGET;
    pos.half_move_clock  = 0;
    pos.full_move_number = 0;
    pos.ply_count        = 0;
    pos.hash             = 0ULL;

    MoveHistory history;
    history.top  = -1;
    history.size = MAX_MOVES;

    Move             nomove         = {.move_id = 0, .score = -1};
    MoveHistoryEntry nohistoryentry = {.move                  = nomove,
                                       .prev_casteling_rights = NOCA,
                                       .prev_enpassant_target = NO_EP_TARGET,
                                       .prev_half_move_clock  = 0,
                                       .prev_full_move_number = 0,
                                       .prev_hash             = 0ULL};

    for (int i = 0; i < MAX_MOVES; i++)
    {
        history.contents[i] = nohistoryentry;
    }

    pos.move_history = history;

    return pos;
}

Position position_clone(const Position* position) {
    Position position_copy = position_create();
    for (uint8_t p = 0; p < 15; p++)
    {
        position_copy.board.bitboards[p]   = position->board.bitboards[p];
        position_copy.board.piece_count[p] = position->board.piece_count[p];
    }
    for (uint8_t sq = 0; sq < 64; sq++)
    {
        position_copy.board.pieces[sq] = position->board.pieces[sq];
    }
    position_copy.active_color     = position->active_color;
    position_copy.casteling_rights = position->casteling_rights;
    position_copy.enpassant_target = position->enpassant_target;
    position_copy.half_move_clock  = position->half_move_clock;
    position_copy.full_move_number = position->full_move_number;
    position_copy.ply_count        = position->ply_count;
    position_copy.hash             = position->hash;
    return position_copy;
}

void position_set_startpos(Position* position) {
    position_set_piece(position, WHITE_ROOK, A1);
    position_set_piece(position, WHITE_KNIGHT, B1);
    position_set_piece(position, WHITE_BISHOP, C1);
    position_set_piece(position, WHITE_QUEEN, D1);
    position_set_piece(position, WHITE_KING, E1);
    position_set_piece(position, WHITE_BISHOP, F1);
    position_set_piece(position, WHITE_KNIGHT, G1);
    position_set_piece(position, WHITE_ROOK, H1);

    position_set_piece(position, WHITE_PAWN, A2);
    position_set_piece(position, WHITE_PAWN, B2);
    position_set_piece(position, WHITE_PAWN, C2);
    position_set_piece(position, WHITE_PAWN, D2);
    position_set_piece(position, WHITE_PAWN, E2);
    position_set_piece(position, WHITE_PAWN, F2);
    position_set_piece(position, WHITE_PAWN, G2);
    position_set_piece(position, WHITE_PAWN, H2);

    position_set_piece(position, BLACK_ROOK, A8);
    position_set_piece(position, BLACK_KNIGHT, B8);
    position_set_piece(position, BLACK_BISHOP, C8);
    position_set_piece(position, BLACK_QUEEN, D8);
    position_set_piece(position, BLACK_KING, E8);
    position_set_piece(position, BLACK_BISHOP, F8);
    position_set_piece(position, BLACK_KNIGHT, G8);
    position_set_piece(position, BLACK_ROOK, H8);

    position_set_piece(position, BLACK_PAWN, A7);
    position_set_piece(position, BLACK_PAWN, B7);
    position_set_piece(position, BLACK_PAWN, C7);
    position_set_piece(position, BLACK_PAWN, D7);
    position_set_piece(position, BLACK_PAWN, E7);
    position_set_piece(position, BLACK_PAWN, F7);
    position_set_piece(position, BLACK_PAWN, G7);
    position_set_piece(position, BLACK_PAWN, H7);

    position->active_color     = WHITE;
    position->casteling_rights = WKCA | WQCA | BKCA | BQCA;
    position->enpassant_target = NO_EP_TARGET;
    position->half_move_clock  = 0;
    position->full_move_number = 1;
    position->ply_count        = 0;
}

void position_set_piece(Position* position, const Piece piece, const Square square) {
    const unsigned long long bit          = 1ULL << square;
    const unsigned long long bit_inverted = ~(bit);
    position->board.bitboards[piece] |= bit;
    position->board.bitboards[NO_PIECE] &= bit_inverted;
    position->board.bitboards[BOARD_WHITE_PIECES_IDX + PIECE_GET_COLOR(piece)] |= bit;
    position->board.pieces[square] = piece;
    position->board.piece_count[piece] += 1;
    position->board.piece_count[NO_PIECE] -= 1;
    position->board.piece_count[BOARD_WHITE_PIECES_IDX + PIECE_GET_COLOR(piece)] += 1;
    position->hash ^= HASHTABLE[HASH_IDX(piece)][square];
}

void position_clear_piece(Position* position, const Piece piece, const Square square) {
    const unsigned long long bit          = 1ULL << square;
    const unsigned long long bit_inverted = ~(bit);
    position->board.bitboards[piece] &= bit_inverted;
    position->board.bitboards[NO_PIECE] |= bit;
    position->board.bitboards[BOARD_WHITE_PIECES_IDX + PIECE_GET_COLOR(piece)] &= bit_inverted;
    position->board.pieces[square] = NO_PIECE;
    position->board.piece_count[piece] -= 1;
    position->board.piece_count[NO_PIECE] += 1;
    position->board.piece_count[BOARD_WHITE_PIECES_IDX + PIECE_GET_COLOR(piece)] -= 1;
    position->hash ^= HASHTABLE[HASH_IDX(piece)][square];
}

void position_display(const Position* position) {
    _board_display(position->board);
    printf("\nActive Color: %d\n", position->active_color);
    printf("Casteling Rights: %d\n", position->casteling_rights);
    printf("En Passant Target: %d\n", position->enpassant_target);
    printf("Half Move Clock: %d\n", position->half_move_clock);
    printf("Full Move Number: %d\n", position->full_move_number);
    printf("Ply Count: %d\n", position->ply_count);
    printf("Hash: %llu\n\n", position->hash);
}

bool position_is_square_attacked(const Position* position,
                                 const Square    sq,
                                 const Color     attacked_by_color) {

    const PieceType queen = PIECE_CREATE(QUEEN, attacked_by_color);
    if (_movegen_get_queen_attacks(sq, ~position->board.bitboards[NO_PIECE])
        & position->board.bitboards[queen])
        return true;

    const PieceType rook = PIECE_CREATE(ROOK, attacked_by_color);
    if (_movegen_get_rook_attacks(sq, ~position->board.bitboards[NO_PIECE])
        & position->board.bitboards[rook])
        return true;

    const PieceType bishop = PIECE_CREATE(BISHOP, attacked_by_color);
    if (_movegen_get_bishop_attacks(sq, ~position->board.bitboards[NO_PIECE])
        & position->board.bitboards[bishop])
        return true;

    const PieceType knight = PIECE_CREATE(KNIGHT, attacked_by_color);
    if (ATTACK_TABLE_KNIGHT[sq] & position->board.bitboards[knight])
        return true;

    const PieceType pawn = PIECE_CREATE(PAWN, attacked_by_color);
    if (ATTACK_TABLE_PAWN[!attacked_by_color][sq] & position->board.bitboards[pawn])
        return true;

    const PieceType king = PIECE_CREATE(KING, attacked_by_color);
    if (ATTACK_TABLE_KING[sq] & position->board.bitboards[king])
        return true;

    return false;
}

bool position_is_in_check(const Position* position) {
    const PieceType    king     = PIECE_CREATE(KING, position->active_color);
    unsigned long long king_bb  = position->board.bitboards[king];
    uint8_t            king_pos = utils_bit_bitscan_forward(&king_bb);
    return position_is_square_attacked(position, king_pos, !position->active_color);
}

bool position_is_valid(const Position* position) {
    if (position->board.piece_count[WHITE_KING] != 1)
        return false;
    if (position->board.piece_count[BLACK_KING] != 1)
        return false;
    if (position->board.bitboards[WHITE_PAWN] & BOARD_MASK_RANK_1)
        return false;
    if (position->board.bitboards[WHITE_PAWN] & BOARD_MASK_RANK_8)
        return false;
    if (position->board.bitboards[BLACK_PAWN] & BOARD_MASK_RANK_1)
        return false;
    if (position->board.bitboards[BLACK_PAWN] & BOARD_MASK_RANK_8)
        return false;
    return true;
}

bool position_is_repeated(const Position* position) {
    for (uint16_t i = (position->ply_count - position->half_move_clock);
         i < position->ply_count - 1; ++i)
    {
        if (position->hash == position->move_history.contents[i].prev_hash)
        {
            return true;
        }
    }
    return false;
}

bool position_has_legal_move(Position* position) {
    MoveList move_list;

    movegen_pseudo_legal_all(position, &move_list);

    bool is_valid_move = false;

    for (int32_t i = 0; i < move_list.size; i++)
    {
        is_valid_move = move_do(position, move_list.moves[i]);

        if (is_valid_move && position_is_valid(position))
        {
            move_undo(position);
            return true;
        }

        move_undo(position);
    }

    return false;
}

bool position_is_in_checkmate(Position* position) {
    return position_is_in_check(position) && !position_has_legal_move(position);
}

bool position_is_in_stalemate(Position* position) {
    return !position_is_in_check(position) && !position_has_legal_move(position);
}

/*
* Movegen and Move functions
*/

static Move _move_encode(const Piece    piece,
                         const uint8_t  from_sq,
                         const uint8_t  to_sq,
                         const Piece    captured_piece,
                         const Piece    promoted_piece,
                         const bool     flag_ps,
                         const bool     flag_ep,
                         const uint32_t flag_ca) {
    uint32_t move_id =
      (piece) | (from_sq << 4) | (to_sq << 11) | (captured_piece << 18) | (promoted_piece << 22);
    if (flag_ps)
    {
        move_id |= MOVE_FLAG_PS;
    }
    else if (flag_ep)
    {
        move_id |= MOVE_FLAG_EP;
    }
    else if (flag_ca)
    {
        move_id |= flag_ca;
    }

    uint16_t move_score = 0;

    if (captured_piece != NO_PIECE)
    {
        const PieceType attacker = PIECE_GET_TYPE(piece);
        const PieceType victim   = PIECE_GET_TYPE(captured_piece);

        move_score = MOVE_SCORE_MVV_LVA[MOVE_SCORE_MVV_LVA_IDX(attacker, victim)];
    }

    const Move move = {.move_id = move_id, .score = move_score};
    return move;
}

static Move _move_encode_quite_move(const Piece piece, const uint8_t from_sq, const uint8_t to_sq) {
    return _move_encode(piece, from_sq, to_sq, NO_PIECE, NO_PIECE, false, false, 0);
}

static Move _move_encode_capture_move(const Piece   piece,
                                      const uint8_t from_sq,
                                      const uint8_t to_sq,
                                      const Piece   captured_piece) {
    return _move_encode(piece, from_sq, to_sq, captured_piece, NO_PIECE, false, false, 0);
}

static Move
_move_encode_pawn_start_move(const Piece pawn, const uint8_t from_sq, const uint8_t to_sq) {
    return _move_encode(pawn, from_sq, to_sq, NO_PIECE, NO_PIECE, true, false, 0);
}

static Move _move_encode_pawn_promotion_move(const Piece   pawn,
                                             const uint8_t from_sq,
                                             const uint8_t to_sq,
                                             const Piece   captured_piece,
                                             const Piece   promoted_piece) {
    return _move_encode(pawn, from_sq, to_sq, captured_piece, promoted_piece, false, false, 0);
}

static Move _move_encode_pawn_enpassant_move(const Piece   pawn,
                                             const uint8_t from_sq,
                                             const uint8_t to_sq,
                                             const Piece   captured_pawn) {
    return _move_encode(pawn, from_sq, to_sq, captured_pawn, NO_PIECE, false, true, 0);
}

static Move _move_encode_king_castle_move(const Piece    king,
                                          const uint8_t  from_sq,
                                          const uint8_t  to_sq,
                                          const uint32_t flag_ca) {
    return _move_encode(king, from_sq, to_sq, NO_PIECE, NO_PIECE, false, false, flag_ca);
}

static void _movegen_enqueue_move_sort(MoveList* list) {
    int  i, j;
    Move key;

    for (i = 1; i < list->size; i++)
    {
        key = list->moves[i];
        j   = i - 1;

        while (j >= 0 && list->moves[j].score < key.score)
        {
            list->moves[j + 1] = list->moves[j];
            j                  = j - 1;
        }
        list->moves[j + 1] = key;
    }
}

static bool _movegen_enqueue_move(MoveList* list, const Move move, bool sort) {
    if (list->size == MAX_MOVES - 1)
        return false;

    list->moves[++list->size] = move;

    if (sort)
        _movegen_enqueue_move_sort(list);

    return true;
}

bool movegen_dequeue_move(MoveList* list, Move* move) {
    if (list->size == -1)
        return false;

    if (list->head == list->size)
        return false;

    *move = list->moves[++list->head];
    return true;
}

void movegen_pseudo_legal_captures(const Position* position, MoveList* move_list) {
    const Color        active_color = position->active_color;
    Piece              p;
    unsigned long long bb, attacks, occupancy;
    uint8_t            from_sq, to_sq;
    Move               m;

    const Move nomove = {.move_id = 0, .score = -1};
    for (int i = 0; i < MAX_MOVES; i++)
    {
        move_list->moves[i] = nomove;
    }
    move_list->size = -1;
    move_list->head = -1;

    if (active_color == WHITE)
    {
        bb = position->board.bitboards[WHITE_PAWN];

        while (bb)
        {
            from_sq = utils_bit_bitscan_forward(&bb);

            // Pawn Captures
            occupancy = position->board.bitboards[BOARD_BLACK_PIECES_IDX];
            attacks   = _movegen_get_pawn_attacks(from_sq, active_color, occupancy);
            while (attacks)
            {
                to_sq = utils_bit_bitscan_forward(&attacks);

                if (BOARD_SQ_TO_RANK(to_sq) == RANK_8)
                {
                    // Pawn Captures + Promotion
                    m = _move_encode_pawn_promotion_move(
                      WHITE_PAWN, from_sq, to_sq, position->board.pieces[to_sq], WHITE_KNIGHT);
                    _movegen_enqueue_move(move_list, m, true);
                    m = _move_encode_pawn_promotion_move(
                      WHITE_PAWN, from_sq, to_sq, position->board.pieces[to_sq], WHITE_BISHOP);
                    _movegen_enqueue_move(move_list, m, true);
                    m = _move_encode_pawn_promotion_move(WHITE_PAWN, from_sq, to_sq,
                                                         position->board.pieces[to_sq], WHITE_ROOK);
                    _movegen_enqueue_move(move_list, m, true);
                    m = _move_encode_pawn_promotion_move(
                      WHITE_PAWN, from_sq, to_sq, position->board.pieces[to_sq], WHITE_QUEEN);
                    _movegen_enqueue_move(move_list, m, true);
                }
                else
                {
                    // Pawn Captures
                    m = _move_encode_capture_move(WHITE_PAWN, from_sq, to_sq,
                                                  position->board.pieces[to_sq]);
                    _movegen_enqueue_move(move_list, m, true);
                }
            }

            if (position->enpassant_target != NO_EP_TARGET)
            {
                occupancy = 1ULL << position->enpassant_target;
                attacks   = _movegen_get_pawn_attacks(from_sq, active_color, occupancy);
                if (attacks)
                {
                    to_sq = utils_bit_bitscan_forward(&attacks);
                    m = _move_encode_pawn_enpassant_move(WHITE_PAWN, from_sq, to_sq, BLACK_PAWN);
                    _movegen_enqueue_move(move_list, m, true);
                }
            }
        }
    }
    else
    {
        bb = position->board.bitboards[BLACK_PAWN];

        while (bb)
        {
            from_sq = utils_bit_bitscan_forward(&bb);

            occupancy = position->board.bitboards[BOARD_WHITE_PIECES_IDX];
            attacks   = _movegen_get_pawn_attacks(from_sq, active_color, occupancy);
            while (attacks)
            {
                to_sq = utils_bit_bitscan_forward(&attacks);

                if (BOARD_SQ_TO_RANK(to_sq) == RANK_1)
                {
                    m = _move_encode_pawn_promotion_move(
                      BLACK_PAWN, from_sq, to_sq, position->board.pieces[to_sq], BLACK_KNIGHT);
                    _movegen_enqueue_move(move_list, m, true);
                    m = _move_encode_pawn_promotion_move(
                      BLACK_PAWN, from_sq, to_sq, position->board.pieces[to_sq], BLACK_BISHOP);
                    _movegen_enqueue_move(move_list, m, true);
                    m = _move_encode_pawn_promotion_move(BLACK_PAWN, from_sq, to_sq,
                                                         position->board.pieces[to_sq], BLACK_ROOK);
                    _movegen_enqueue_move(move_list, m, true);
                    m = _move_encode_pawn_promotion_move(
                      BLACK_PAWN, from_sq, to_sq, position->board.pieces[to_sq], BLACK_QUEEN);
                    _movegen_enqueue_move(move_list, m, true);
                }
                else
                {
                    m = _move_encode_capture_move(BLACK_PAWN, from_sq, to_sq,
                                                  position->board.pieces[to_sq]);
                    _movegen_enqueue_move(move_list, m, true);
                }
            }

            if (position->enpassant_target != NO_EP_TARGET)
            {
                occupancy = 1ULL << position->enpassant_target;
                attacks   = _movegen_get_pawn_attacks(from_sq, active_color, occupancy);
                if (attacks)
                {
                    to_sq = utils_bit_bitscan_forward(&attacks);
                    m = _move_encode_pawn_enpassant_move(BLACK_PAWN, from_sq, to_sq, WHITE_PAWN);
                    _movegen_enqueue_move(move_list, m, true);
                }
            }
        }
    }

    p  = PIECE_CREATE(KNIGHT, active_color);
    bb = position->board.bitboards[p];

    while (bb)
    {
        from_sq   = utils_bit_bitscan_forward(&bb);
        occupancy = position->board.bitboards[BOARD_WHITE_PIECES_IDX + (!active_color)];
        attacks   = _movegen_get_knight_attacks(from_sq, occupancy);
        while (attacks)
        {
            to_sq = utils_bit_bitscan_forward(&attacks);

            m = _move_encode_capture_move(p, from_sq, to_sq, position->board.pieces[to_sq]);
            _movegen_enqueue_move(move_list, m, true);
        }
    }

    p  = PIECE_CREATE(BISHOP, active_color);
    bb = position->board.bitboards[p];

    while (bb)
    {
        from_sq   = utils_bit_bitscan_forward(&bb);
        occupancy = ~position->board.bitboards[NO_PIECE];
        attacks   = _movegen_get_bishop_attacks(from_sq, occupancy);
        while (attacks)
        {
            to_sq = utils_bit_bitscan_forward(&attacks);

            if (position->board.pieces[to_sq] != NO_PIECE
                && PIECE_GET_COLOR(position->board.pieces[to_sq]) != active_color)
            {
                m = _move_encode_capture_move(p, from_sq, to_sq, position->board.pieces[to_sq]);
                _movegen_enqueue_move(move_list, m, true);
            }
        }
    }

    p  = PIECE_CREATE(ROOK, active_color);
    bb = position->board.bitboards[p];

    while (bb)
    {
        from_sq   = utils_bit_bitscan_forward(&bb);
        occupancy = ~position->board.bitboards[NO_PIECE];
        attacks   = _movegen_get_rook_attacks(from_sq, occupancy);
        while (attacks)
        {
            to_sq = utils_bit_bitscan_forward(&attacks);

            if (position->board.pieces[to_sq] != NO_PIECE
                && PIECE_GET_COLOR(position->board.pieces[to_sq]) != active_color)
            {
                m = _move_encode_capture_move(p, from_sq, to_sq, position->board.pieces[to_sq]);
                _movegen_enqueue_move(move_list, m, true);
            }
        }
    }

    p  = PIECE_CREATE(QUEEN, active_color);
    bb = position->board.bitboards[p];

    while (bb)
    {
        from_sq   = utils_bit_bitscan_forward(&bb);
        occupancy = ~position->board.bitboards[NO_PIECE];
        attacks   = _movegen_get_queen_attacks(from_sq, occupancy);
        while (attacks)
        {
            to_sq = utils_bit_bitscan_forward(&attacks);

            if (position->board.pieces[to_sq] != NO_PIECE
                && PIECE_GET_COLOR(position->board.pieces[to_sq]) != active_color)
            {
                m = _move_encode_capture_move(p, from_sq, to_sq, position->board.pieces[to_sq]);
                _movegen_enqueue_move(move_list, m, true);
            }
        }
    }

    p  = PIECE_CREATE(KING, active_color);
    bb = position->board.bitboards[p];

    while (bb)
    {
        from_sq   = utils_bit_bitscan_forward(&bb);
        occupancy = position->board.bitboards[BOARD_WHITE_PIECES_IDX + (!active_color)];
        attacks   = _movegen_get_king_attacks(from_sq, occupancy);
        while (attacks)
        {
            to_sq = utils_bit_bitscan_forward(&attacks);

            m = _move_encode_capture_move(p, from_sq, to_sq, position->board.pieces[to_sq]);
            _movegen_enqueue_move(move_list, m, true);
        }
    }
}

void movegen_pseudo_legal_quite(const Position* position, MoveList* move_list) {
    const Color        active_color = position->active_color;
    Piece              p;
    unsigned long long bb, attacks, occupancy;
    uint8_t            from_sq, to_sq;
    Move               m;

    const Move nomove = {.move_id = 0, .score = -1};
    for (int i = 0; i < MAX_MOVES; i++)
    {
        move_list->moves[i] = nomove;
    }
    move_list->size = -1;
    move_list->head = -1;

    if (active_color == WHITE)
    {
        bb = position->board.bitboards[WHITE_PAWN];

        while (bb)
        {
            occupancy = position->board.bitboards[NO_PIECE];
            from_sq   = utils_bit_bitscan_forward(&bb);
            attacks   = _bitboard_north_one((1ULL << from_sq)) & occupancy;
            if (attacks)
            {
                to_sq = utils_bit_bitscan_forward(&attacks);

                if (BOARD_SQ_TO_RANK(to_sq) == RANK_8)
                {
                    // Pawn Promotion
                    m = _move_encode_pawn_promotion_move(WHITE_PAWN, from_sq, to_sq, NO_PIECE,
                                                         WHITE_KNIGHT);
                    _movegen_enqueue_move(move_list, m, false);
                    m = _move_encode_pawn_promotion_move(WHITE_PAWN, from_sq, to_sq, NO_PIECE,
                                                         WHITE_BISHOP);
                    _movegen_enqueue_move(move_list, m, false);
                    m = _move_encode_pawn_promotion_move(WHITE_PAWN, from_sq, to_sq, NO_PIECE,
                                                         WHITE_ROOK);
                    _movegen_enqueue_move(move_list, m, false);
                    m = _move_encode_pawn_promotion_move(WHITE_PAWN, from_sq, to_sq, NO_PIECE,
                                                         WHITE_QUEEN);
                    _movegen_enqueue_move(move_list, m, false);
                }
                else
                {
                    // Pawn Push
                    m = _move_encode_quite_move(WHITE_PAWN, from_sq, to_sq);
                    _movegen_enqueue_move(move_list, m, false);
                }
            }

            // Pawn Start
            attacks = _bitboard_north_one((1ULL << from_sq)) & occupancy;
            attacks = _bitboard_north_one(attacks) & occupancy & BOARD_MASK_RANK_4;
            if (attacks)
            {
                to_sq = utils_bit_bitscan_forward(&attacks);
                m     = _move_encode_pawn_start_move(WHITE_PAWN, from_sq, to_sq);
                _movegen_enqueue_move(move_list, m, false);
            }
        }
    }
    else
    {
        bb = position->board.bitboards[BLACK_PAWN];

        while (bb)
        {
            occupancy = position->board.bitboards[NO_PIECE];
            from_sq   = utils_bit_bitscan_forward(&bb);
            attacks   = _bitboard_south_one((1ULL << from_sq)) & occupancy;
            if (attacks)
            {
                to_sq = utils_bit_bitscan_forward(&attacks);

                if (BOARD_SQ_TO_RANK(to_sq) == RANK_1)
                {
                    m = _move_encode_pawn_promotion_move(BLACK_PAWN, from_sq, to_sq, NO_PIECE,
                                                         BLACK_KNIGHT);
                    _movegen_enqueue_move(move_list, m, false);
                    m = _move_encode_pawn_promotion_move(BLACK_PAWN, from_sq, to_sq, NO_PIECE,
                                                         BLACK_BISHOP);
                    _movegen_enqueue_move(move_list, m, false);
                    m = _move_encode_pawn_promotion_move(BLACK_PAWN, from_sq, to_sq, NO_PIECE,
                                                         BLACK_ROOK);
                    _movegen_enqueue_move(move_list, m, false);
                    m = _move_encode_pawn_promotion_move(BLACK_PAWN, from_sq, to_sq, NO_PIECE,
                                                         BLACK_QUEEN);
                    _movegen_enqueue_move(move_list, m, false);
                }
                else
                {
                    m = _move_encode_quite_move(BLACK_PAWN, from_sq, to_sq);
                    _movegen_enqueue_move(move_list, m, false);
                }
            }

            attacks = _bitboard_south_one((1ULL << from_sq)) & occupancy;
            attacks = _bitboard_south_one(attacks) & occupancy & BOARD_MASK_RANK_5;
            if (attacks)
            {
                to_sq = utils_bit_bitscan_forward(&attacks);
                m     = _move_encode_pawn_start_move(BLACK_PAWN, from_sq, to_sq);
                _movegen_enqueue_move(move_list, m, false);
            }
        }
    }

    p  = PIECE_CREATE(KNIGHT, active_color);
    bb = position->board.bitboards[p];

    while (bb)
    {
        from_sq   = utils_bit_bitscan_forward(&bb);
        occupancy = position->board.bitboards[NO_PIECE];
        attacks   = _movegen_get_knight_attacks(from_sq, occupancy);
        while (attacks)
        {
            to_sq = utils_bit_bitscan_forward(&attacks);

            m = _move_encode_quite_move(p, from_sq, to_sq);
            _movegen_enqueue_move(move_list, m, false);
        }
    }

    p  = PIECE_CREATE(BISHOP, active_color);
    bb = position->board.bitboards[p];

    while (bb)
    {
        from_sq   = utils_bit_bitscan_forward(&bb);
        occupancy = ~position->board.bitboards[NO_PIECE];
        attacks   = _movegen_get_bishop_attacks(from_sq, occupancy);
        while (attacks)
        {
            to_sq = utils_bit_bitscan_forward(&attacks);

            if (position->board.pieces[to_sq] == NO_PIECE)
            {
                m = _move_encode_quite_move(p, from_sq, to_sq);
                _movegen_enqueue_move(move_list, m, false);
            }
        }
    }

    p  = PIECE_CREATE(ROOK, active_color);
    bb = position->board.bitboards[p];

    while (bb)
    {
        from_sq   = utils_bit_bitscan_forward(&bb);
        occupancy = ~position->board.bitboards[NO_PIECE];
        attacks   = _movegen_get_rook_attacks(from_sq, occupancy);
        while (attacks)
        {
            to_sq = utils_bit_bitscan_forward(&attacks);

            if (position->board.pieces[to_sq] == NO_PIECE)
            {
                m = _move_encode_quite_move(p, from_sq, to_sq);
                _movegen_enqueue_move(move_list, m, false);
            }
        }
    }

    p  = PIECE_CREATE(QUEEN, active_color);
    bb = position->board.bitboards[p];

    while (bb)
    {
        from_sq   = utils_bit_bitscan_forward(&bb);
        occupancy = ~position->board.bitboards[NO_PIECE];
        attacks   = _movegen_get_queen_attacks(from_sq, occupancy);
        while (attacks)
        {
            to_sq = utils_bit_bitscan_forward(&attacks);

            if (position->board.pieces[to_sq] == NO_PIECE)
            {
                m = _move_encode_quite_move(p, from_sq, to_sq);
                _movegen_enqueue_move(move_list, m, false);
            }
        }
    }

    p  = PIECE_CREATE(KING, active_color);
    bb = position->board.bitboards[p];

    while (bb)
    {
        from_sq   = utils_bit_bitscan_forward(&bb);
        occupancy = position->board.bitboards[NO_PIECE];
        attacks   = _movegen_get_king_attacks(from_sq, occupancy);
        while (attacks)
        {
            to_sq = utils_bit_bitscan_forward(&attacks);

            m = _move_encode_quite_move(p, from_sq, to_sq);
            _movegen_enqueue_move(move_list, m, false);
        }
    }

    if (!position_is_in_check(position))
    {
        if (active_color == WHITE)
        {
            if (position->casteling_rights & WKCA)
            {
                if (position->board.pieces[F1] == NO_PIECE && position->board.pieces[G1] == NO_PIECE
                    && !position_is_square_attacked(position, F1, BLACK))
                {
                    m = _move_encode_king_castle_move(WHITE_KING, E1, G1, MOVE_FLAG_WKCA);
                    _movegen_enqueue_move(move_list, m, false);
                }
            }
            if (position->casteling_rights & WQCA)
            {
                if (position->board.pieces[D1] == NO_PIECE && position->board.pieces[C1] == NO_PIECE
                    && position->board.pieces[B1] == NO_PIECE
                    && !position_is_square_attacked(position, D1, BLACK))
                {
                    m = _move_encode_king_castle_move(WHITE_KING, E1, C1, MOVE_FLAG_WQCA);
                    _movegen_enqueue_move(move_list, m, false);
                }
            }
        }
        else
        {
            if (position->casteling_rights & BKCA)
            {
                if (position->board.pieces[F8] == NO_PIECE && position->board.pieces[G8] == NO_PIECE
                    && !position_is_square_attacked(position, F8, WHITE))
                {
                    m = _move_encode_king_castle_move(BLACK_KING, E8, G8, MOVE_FLAG_BKCA);
                    _movegen_enqueue_move(move_list, m, false);
                }
            }
            if (position->casteling_rights & BQCA)
            {
                if (position->board.pieces[D8] == NO_PIECE && position->board.pieces[C8] == NO_PIECE
                    && position->board.pieces[B8] == NO_PIECE
                    && !position_is_square_attacked(position, D8, WHITE))
                {
                    m = _move_encode_king_castle_move(BLACK_KING, E8, C8, MOVE_FLAG_BQCA);
                    _movegen_enqueue_move(move_list, m, false);
                }
            }
        }
    }
}

void* _movegen_pseudo_legal_captures_async(void* arg) {

    MovegenThreadData* data      = (MovegenThreadData*) arg;
    Position*          position  = &data->pos;
    MoveList*          move_list = &data->moves;

    movegen_pseudo_legal_captures(position, move_list);
    pthread_exit(NULL);
}

void* _movegen_pseudo_legal_quiet_async(void* arg) {

    MovegenThreadData* data      = (MovegenThreadData*) arg;
    Position*          position  = &data->pos;
    MoveList*          move_list = &data->moves;

    movegen_pseudo_legal_quite(position, move_list);
    pthread_exit(NULL);
}

void movegen_pseudo_legal_all(const Position* position, MoveList* move_list) {
#ifdef MOVEGEN_SINGLE_THREADED

    const Move nomove = {.move_id = 0, .score = -1};

    for (int i = 0; i < MAX_MOVES; i++)
    {
        move_list->moves[i] = nomove;
    }
    move_list->size = -1;
    move_list->head = -1;

    MoveList moves;
    Move     move;

    movegen_pseudo_legal_captures(position, &moves);

    while (movegen_dequeue_move(&moves, &move))
        _movegen_enqueue_move(move_list, move, false);

    movegen_pseudo_legal_quite(position, &moves);

    while (movegen_dequeue_move(&moves, &move))
        _movegen_enqueue_move(move_list, move, false);

#else

    pthread_t movegen_pseudo_legal_captures_thread_id;
    pthread_t movegen_pseudo_legal_quiet_thread_id;

    MovegenThreadData movegen_pseudo_legal_captures_thread_data;
    MovegenThreadData movegen_pseudo_legal_quiet_thread_data;

    MoveList capture_moves;
    MoveList quite_moves;

    movegen_pseudo_legal_captures_thread_data.pos   = position_clone(position);
    movegen_pseudo_legal_captures_thread_data.moves = capture_moves;

    movegen_pseudo_legal_quiet_thread_data.pos   = position_clone(position);
    movegen_pseudo_legal_quiet_thread_data.moves = quite_moves;

    pthread_create(&movegen_pseudo_legal_captures_thread_id, NULL,
                   _movegen_pseudo_legal_captures_async,
                   (void*) &movegen_pseudo_legal_captures_thread_data);
    pthread_create(&movegen_pseudo_legal_quiet_thread_id, NULL, _movegen_pseudo_legal_quiet_async,
                   (void*) &movegen_pseudo_legal_quiet_thread_data);

    const Move nomove = {.move_id = 0, .score = -1};
    Move       move;

    for (int i = 0; i < MAX_MOVES; i++)
    {
        move_list->moves[i] = nomove;
    }
    move_list->size = -1;
    move_list->head = -1;

    pthread_join(movegen_pseudo_legal_captures_thread_id, NULL);
    pthread_join(movegen_pseudo_legal_quiet_thread_id, NULL);

    while (movegen_dequeue_move(&movegen_pseudo_legal_captures_thread_data.moves, &move))
        _movegen_enqueue_move(move_list, move, false);

    while (movegen_dequeue_move(&movegen_pseudo_legal_quiet_thread_data.moves, &move))
        _movegen_enqueue_move(move_list, move, false);

#endif
}

void movegen_display_moves(MoveList* move_list) {
    uint16_t i;
    Move     move;
    char     move_str[10];
    for (i = 0; i < move_list->size + 1; i++)
    {
        move = move_list->moves[i];
        move_to_str(move_str, move);
        printf("%s %d\n", move_str, move.score);
    }
}

bool move_do(Position* position, const Move move) {
    assert(PIECE_GET_COLOR(MOVE_PIECE(move.move_id)) == position->active_color);
    assert(!_position_history_is_full(position));

    bool is_legal_move = false;

    const MoveHistoryEntry mhe = {.move                  = move,
                                  .prev_casteling_rights = position->casteling_rights,
                                  .prev_enpassant_target = position->enpassant_target,
                                  .prev_full_move_number = position->full_move_number,
                                  .prev_half_move_clock  = position->half_move_clock,
                                  .prev_hash             = position->hash};

    (void) _position_history_push(position, mhe);

    position->enpassant_target = NO_EP_TARGET;
    position->half_move_clock++;
    if (position->active_color == BLACK)
    {
        position->full_move_number++;
    }
    position->ply_count++;

    position_clear_piece(position, MOVE_PIECE(move.move_id), MOVE_FROM_SQ(move.move_id));

    if (move.move_id & MOVE_FLAG_PS)
    {
        if (position->active_color == WHITE)
            position->enpassant_target = MOVE_FROM_SQ(move.move_id) + 8;
        else
            position->enpassant_target = MOVE_FROM_SQ(move.move_id) - 8;
        position_set_piece(position, MOVE_PIECE(move.move_id), MOVE_TO_SQ(move.move_id));
        position->half_move_clock = 0;
        is_legal_move             = !position_is_in_check(position);
        position->active_color    = !position->active_color;
        return is_legal_move;
    }

    if (move.move_id & MOVE_FLAG_EP)
    {
        if (position->active_color == WHITE)
            position_clear_piece(position, MOVE_CAPTURED(move.move_id),
                                 MOVE_TO_SQ(move.move_id) - 8);
        else
            position_clear_piece(position, MOVE_CAPTURED(move.move_id),
                                 MOVE_TO_SQ(move.move_id) + 8);
        position_set_piece(position, MOVE_PIECE(move.move_id), MOVE_TO_SQ(move.move_id));
        position->half_move_clock = 0;
        is_legal_move             = !position_is_in_check(position);
        position->active_color    = !position->active_color;
        return is_legal_move;
    }

    if (move.move_id & MOVE_FLAG_WKCA)
    {
        position_set_piece(position, WHITE_KING, G1);
        position_clear_piece(position, WHITE_ROOK, H1);
        position_set_piece(position, WHITE_ROOK, F1);
        position->casteling_rights &= 0xC;
        is_legal_move          = !position_is_in_check(position);
        position->active_color = !position->active_color;
        return is_legal_move;
    }

    if (move.move_id & MOVE_FLAG_WQCA)
    {
        position_set_piece(position, WHITE_KING, C1);
        position_clear_piece(position, WHITE_ROOK, A1);
        position_set_piece(position, WHITE_ROOK, D1);
        position->casteling_rights &= 0xC;
        is_legal_move          = !position_is_in_check(position);
        position->active_color = !position->active_color;
        return is_legal_move;
    }

    if (move.move_id & MOVE_FLAG_BKCA)
    {
        position_set_piece(position, BLACK_KING, G8);
        position_clear_piece(position, BLACK_ROOK, H8);
        position_set_piece(position, BLACK_ROOK, F8);
        position->casteling_rights &= 0x3;
        is_legal_move          = !position_is_in_check(position);
        position->active_color = !position->active_color;
        return is_legal_move;
    }

    if (move.move_id & MOVE_FLAG_BQCA)
    {
        position_set_piece(position, BLACK_KING, C8);
        position_clear_piece(position, BLACK_ROOK, A8);
        position_set_piece(position, BLACK_ROOK, D8);
        position->casteling_rights &= 0x3;
        is_legal_move          = !position_is_in_check(position);
        position->active_color = !position->active_color;
        return is_legal_move;
    }

    if (MOVE_PROMOTED(move.move_id))
    {
        if (MOVE_CAPTURED(move.move_id))
        {
            position_clear_piece(position, MOVE_CAPTURED(move.move_id), MOVE_TO_SQ(move.move_id));

            if (PIECE_GET_TYPE(MOVE_CAPTURED(move.move_id)) == ROOK)
            {
                if (position->active_color == WHITE)
                {
                    if (MOVE_TO_SQ(move.move_id) == H8)
                    {
                        position->casteling_rights &= 0xB;
                    }
                    else if (MOVE_TO_SQ(move.move_id) == A8)
                    {
                        position->casteling_rights &= 0x7;
                    }
                }
                else
                {
                    if (MOVE_TO_SQ(move.move_id) == H1)
                    {
                        position->casteling_rights &= 0xE;
                    }
                    else if ((position->casteling_rights & WQCA) && MOVE_TO_SQ(move.move_id) == A1)
                    {
                        position->casteling_rights &= 0xD;
                    }
                }
            }
        }

        position_set_piece(position, MOVE_PROMOTED(move.move_id), MOVE_TO_SQ(move.move_id));
        position->half_move_clock = 0;
        is_legal_move             = !position_is_in_check(position);
        position->active_color    = !position->active_color;
        return is_legal_move;
    }

    if (MOVE_CAPTURED(move.move_id))
    {
        position_clear_piece(position, MOVE_CAPTURED(move.move_id), MOVE_TO_SQ(move.move_id));
        position_set_piece(position, MOVE_PIECE(move.move_id), MOVE_TO_SQ(move.move_id));

        if (PIECE_GET_TYPE(MOVE_CAPTURED(move.move_id)) == ROOK)
        {
            if (position->active_color == WHITE)
            {
                if (MOVE_TO_SQ(move.move_id) == H8)
                {
                    position->casteling_rights &= 0xB;
                }
                else if (MOVE_TO_SQ(move.move_id) == A8)
                {
                    position->casteling_rights &= 0x7;
                }
            }
            else
            {
                if (MOVE_TO_SQ(move.move_id) == H1)
                {
                    position->casteling_rights &= 0xE;
                }
                else if ((position->casteling_rights & WQCA) && MOVE_TO_SQ(move.move_id) == A1)
                {
                    position->casteling_rights &= 0xD;
                }
            }
        }

        position->half_move_clock = 0;
    }
    else
    {
        position_set_piece(position, MOVE_PIECE(move.move_id), MOVE_TO_SQ(move.move_id));
    }

    if (PIECE_GET_TYPE(MOVE_PIECE(move.move_id)) == PAWN)
    {
        position->half_move_clock = 0;
    }
    else if (PIECE_GET_TYPE(MOVE_PIECE(move.move_id)) == KING)
    {
        if (position->active_color == WHITE)
        {
            position->casteling_rights &= 0xC;
        }
        else
        {
            position->casteling_rights &= 0x3;
        }
    }
    else if (PIECE_GET_TYPE(MOVE_PIECE(move.move_id)) == ROOK)
    {
        if (position->active_color == WHITE)
        {
            if (MOVE_FROM_SQ(move.move_id) == H1)
            {
                position->casteling_rights &= 0xE;
            }
            else if (MOVE_FROM_SQ(move.move_id) == A1)
            {
                position->casteling_rights &= 0xD;
            }
        }
        else
        {
            if (MOVE_FROM_SQ(move.move_id) == H8)
            {
                position->casteling_rights &= 0xB;
            }
            else if (MOVE_FROM_SQ(move.move_id) == A8)
            {
                position->casteling_rights &= 0x7;
            }
        }
    }

    is_legal_move          = !position_is_in_check(position);
    position->active_color = !position->active_color;
    return is_legal_move;
}

void move_undo(Position* position) {
    assert(!_position_history_is_empty(position));

    const MoveHistoryEntry mhe = _position_history_pop(position);

    const Move prev_move = mhe.move;

    if (prev_move.move_id & MOVE_FLAG_WKCA)
    {
        position_clear_piece(position, WHITE_KING, G1);
        position_set_piece(position, WHITE_KING, E1);
        position_clear_piece(position, WHITE_ROOK, F1);
        position_set_piece(position, WHITE_ROOK, H1);
    }
    else if (prev_move.move_id & MOVE_FLAG_WQCA)
    {
        position_clear_piece(position, WHITE_KING, C1);
        position_set_piece(position, WHITE_KING, E1);
        position_clear_piece(position, WHITE_ROOK, D1);
        position_set_piece(position, WHITE_ROOK, A1);
    }
    else if (prev_move.move_id & MOVE_FLAG_BKCA)
    {
        position_clear_piece(position, BLACK_KING, G8);
        position_set_piece(position, BLACK_KING, E8);
        position_clear_piece(position, BLACK_ROOK, F8);
        position_set_piece(position, BLACK_ROOK, H8);
    }
    else if (prev_move.move_id & MOVE_FLAG_BQCA)
    {
        position_clear_piece(position, BLACK_KING, C8);
        position_set_piece(position, BLACK_KING, E8);
        position_clear_piece(position, BLACK_ROOK, D8);
        position_set_piece(position, BLACK_ROOK, A8);
    }
    else if (prev_move.move_id & MOVE_FLAG_EP)
    {
        position_clear_piece(position, MOVE_PIECE(prev_move.move_id),
                             MOVE_TO_SQ(prev_move.move_id));
        position_set_piece(position, MOVE_PIECE(prev_move.move_id),
                           MOVE_FROM_SQ(prev_move.move_id));
        if (position->active_color == WHITE)
            position_set_piece(position, MOVE_CAPTURED(prev_move.move_id),
                               MOVE_TO_SQ(prev_move.move_id) + 8);
        else
            position_set_piece(position, MOVE_CAPTURED(prev_move.move_id),
                               MOVE_TO_SQ(prev_move.move_id) - 8);
    }
    else if (MOVE_PROMOTED(prev_move.move_id))
    {
        position_clear_piece(position, MOVE_PROMOTED(prev_move.move_id),
                             MOVE_TO_SQ(prev_move.move_id));
        position_set_piece(position, MOVE_PIECE(prev_move.move_id),
                           MOVE_FROM_SQ(prev_move.move_id));

        if (MOVE_CAPTURED(prev_move.move_id))
        {
            position_set_piece(position, MOVE_CAPTURED(prev_move.move_id),
                               MOVE_TO_SQ(prev_move.move_id));
        }
    }
    else if (MOVE_CAPTURED(prev_move.move_id))
    {
        position_clear_piece(position, MOVE_PIECE(prev_move.move_id),
                             MOVE_TO_SQ(prev_move.move_id));
        position_set_piece(position, MOVE_CAPTURED(prev_move.move_id),
                           MOVE_TO_SQ(prev_move.move_id));
        position_set_piece(position, MOVE_PIECE(prev_move.move_id),
                           MOVE_FROM_SQ(prev_move.move_id));
    }
    else
    {
        position_clear_piece(position, MOVE_PIECE(prev_move.move_id),
                             MOVE_TO_SQ(prev_move.move_id));
        position_set_piece(position, MOVE_PIECE(prev_move.move_id),
                           MOVE_FROM_SQ(prev_move.move_id));
    }

    assert(position->hash == mhe.prev_hash);

    position->active_color     = !position->active_color;
    position->casteling_rights = mhe.prev_casteling_rights;
    position->enpassant_target = mhe.prev_enpassant_target;
    position->half_move_clock  = mhe.prev_half_move_clock;
    position->full_move_number = mhe.prev_full_move_number;
    position->ply_count--;
}

Move move_from_str(const char* move_str, const Position* position) {

    if (strlen(move_str) < 4)
    {
        Move nomove = {.move_id = 0};
        return nomove;
    }

    const uint8_t from_file = move_str[0] - 'a';
    const uint8_t from_rank = (move_str[1] - '0') - 1;
    const Square  from_sq   = BOARD_RF_TO_SQ(from_rank, from_file);

    const uint8_t to_file = move_str[2] - 'a';
    const uint8_t to_rank = (move_str[3] - '0') - 1;
    const Square  to_sq   = BOARD_RF_TO_SQ(to_rank, to_file);

    const Piece move_piece     = position->board.pieces[from_sq];
    Piece       captured_piece = position->board.pieces[to_sq];

    Piece promoted_piece = NO_PIECE;

    if (move_str[4] == 'n' || move_str[4] == 'b' || move_str[4] == 'r' || move_str[4] == 'q')
    {
        const char promotion_piece_ch = move_str[4];
        switch (promotion_piece_ch)
        {
            case 'n':
                promoted_piece = PIECE_CREATE(KNIGHT, position->active_color);
                break;
            case 'b':
                promoted_piece = PIECE_CREATE(BISHOP, position->active_color);
                break;
            case 'r':
                promoted_piece = PIECE_CREATE(ROOK, position->active_color);
                break;
            case 'q':
                promoted_piece = PIECE_CREATE(QUEEN, position->active_color);
                break;
            default:
                promoted_piece = NO_PIECE;
                break;
        }
    }

    bool     flag_ps = false;
    bool     flag_ep = false;
    uint32_t flag_ca = NOCA;

    if (PIECE_GET_TYPE(move_piece) == PAWN)
    {
        if (to_sq == position->enpassant_target)
        {
            captured_piece = PIECE_CREATE(PAWN, !position->active_color);
            flag_ep        = true;
        }
        else if (captured_piece == NO_PIECE)
        {
            if (position->active_color == WHITE && from_rank == RANK_2 && to_rank == RANK_4)
                flag_ps = true;
            else if (position->active_color == BLACK && from_rank == RANK_7 && to_rank == RANK_5)
                flag_ps = true;
        }
    }
    else if (PIECE_GET_TYPE(move_piece) == KING)
    {
        if (position->active_color == WHITE)
        {
            if ((position->casteling_rights & WKCA) && from_sq == E1 && to_sq == G1)
                flag_ca = MOVE_FLAG_WKCA;
            else if ((position->casteling_rights & WQCA) && from_sq == E1 && to_sq == C1)
                flag_ca = MOVE_FLAG_WQCA;
        }
        else
        {
            if ((position->casteling_rights & BKCA) && from_sq == E8 && to_sq == G8)
                flag_ca = MOVE_FLAG_BKCA;
            else if ((position->casteling_rights & BQCA) && from_sq == E8 && to_sq == C8)
                flag_ca = MOVE_FLAG_BQCA;
        }
    }

    const Move move = _move_encode(move_piece, from_sq, to_sq, captured_piece, promoted_piece,
                                   flag_ps, flag_ep, flag_ca);

    return move;
}

void move_to_str(char* move_str, const Move move) {
    const char* pstr = "  nbrq";

    uint32_t move_id = move.move_id;

    char    from_file = 'a' + BOARD_SQ_TO_FILE(MOVE_FROM_SQ(move_id));
    uint8_t from_rank = BOARD_SQ_TO_RANK(MOVE_FROM_SQ(move_id)) + 1;

    char    to_file = 'a' + BOARD_SQ_TO_FILE(MOVE_TO_SQ(move_id));
    uint8_t to_rank = BOARD_SQ_TO_RANK(MOVE_TO_SQ(move_id)) + 1;

    if (MOVE_PROMOTED(move_id) != NO_PIECE)
        sprintf(move_str, "%c%d%c%d%c", from_file, from_rank, to_file, to_rank,
                pstr[PIECE_GET_TYPE(MOVE_PROMOTED(move_id))]);
    else
        sprintf(move_str, "%c%d%c%d", from_file, from_rank, to_file, to_rank);
}
