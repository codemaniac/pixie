#ifndef CHESS_H
#define CHESS_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    WHITE,
    BLACK
} Color;

typedef enum {
    PAWN = 1,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
} PieceType;

typedef enum {
    NO_PIECE,
    WHITE_PAWN,
    WHITE_KNIGHT,
    WHITE_BISHOP,
    WHITE_ROOK,
    WHITE_QUEEN,
    WHITE_KING,
    BLACK_PAWN = 9,
    BLACK_KNIGHT,
    BLACK_BISHOP,
    BLACK_ROOK,
    BLACK_QUEEN,
    BLACK_KING
} Piece;

typedef enum {
    RANK_1,
    RANK_2,
    RANK_3,
    RANK_4,
    RANK_5,
    RANK_6,
    RANK_7,
    RANK_8
} Rank;

typedef enum {
    FILE_A,
    FILE_B,
    FILE_C,
    FILE_D,
    FILE_E,
    FILE_F,
    FILE_G,
    FILE_H
} File;

// Little-Endian Rank-File Mapping
// clang-format off
typedef enum {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8,
} Square;
// clang-format on

enum {
    NOCA = 0,
    WKCA = 1,
    WQCA = 2,
    BKCA = 4,
    BQCA = 8
};

typedef enum {
    MOVE_TYPE_QUITE,
    MOVE_TYPE_NON_QUIET
} MoveType;

#define PIECE_GET_TYPE(p) (p & 0x7)
#define PIECE_GET_COLOR(p) ((p & 0x8) / 8)
#define PIECE_CREATE(t, c) ((c << 3) | t)

#define BOARD_RF_TO_SQ(r, f) (8 * r + f)
#define BOARD_SQ_TO_RANK(sq) (sq >> 3)
#define BOARD_SQ_TO_FILE(sq) (sq & 7)
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

#define NO_EP_TARGET 99

#define MOVE_PIECE(move_id) ((move_id) & 0xF)
#define MOVE_FROM_SQ(move_id) ((move_id >> 4) & 0x7F)
#define MOVE_TO_SQ(move_id) ((move_id >> 11) & 0x7F)
#define MOVE_CAPTURED(move_id) ((move_id >> 18) & 0xF)
#define MOVE_PROMOTED(move_id) ((move_id >> 22) & 0xF)
#define MOVE_FLAG_PS 0x4000000
#define MOVE_FLAG_EP 0x8000000
#define MOVE_FLAG_WKCA 0x10000000
#define MOVE_FLAG_WQCA 0x20000000
#define MOVE_FLAG_BKCA 0x40000000
#define MOVE_FLAG_BQCA 0x80000000

typedef struct {
    uint32_t move_id;
    MoveType type;
} Move;

typedef struct {
    Move                  move;
    struct MovesListNode* next;
} MovesListNode;

typedef struct {
    MovesListNode* head;
    MovesListNode* tail;
    uint32_t       size;
} MoveList;

typedef struct {
    Move     move;
    uint8_t  prev_casteling_rights;
    uint8_t  prev_enpassant_target;
    uint8_t  prev_half_move_clock;
    uint8_t  prev_full_move_number;
    uint64_t prev_hash;
} MoveHistoryEntry;

typedef struct {
    int8_t            top;
    uint16_t          size;
    MoveHistoryEntry* contents;
} MoveHistory;

typedef struct {
    uint64_t bitboards[15];
    uint8_t  pieces[64];
    uint8_t  piece_count[15];
} Board;

typedef struct {
    Board*       board;
    Color        active_color;
    uint8_t      casteling_rights;
    uint8_t      enpassant_target;
    uint8_t      half_move_clock;
    uint8_t      full_move_number;
    uint8_t      ply_count;
    uint64_t     hash;
    MoveHistory* move_history;
} Position;

void chess_initialize(void);

Position* position_create(void);
Position* position_clone(const Position* position);
void      position_set_piece(Position* position, const Piece piece, const Square square);
void      position_clear_piece(Position* position, const Piece piece, const Square square);
bool      position_is_valid(const Position* position);
bool      position_is_in_check(const Position* position);
bool      position_is_repeated(const Position* position);
bool      position_has_legal_move(Position* position);
bool      position_is_in_checkmate(Position* position);
bool      position_is_in_stalemate(Position* position);
void      position_display(const Position* position);

MoveList* movegen_pseudo_legal(const Position* position);
void      movegen_display_moves(MoveList* move_list);

bool move_do(Position* position, const Move move);
void move_undo(Position* position);
Move move_from_str(const char* move_str, const Position* position);
void move_to_str(char* move_str, const Move move);

#endif
