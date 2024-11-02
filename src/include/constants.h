#ifndef CHESS_H
#define CHESS_H

#include <cstdint>

enum Color : uint8_t {
    WHITE,  // 0
    BLACK   // 1
};

enum PieceType : uint8_t {
    PAWN = 1,  // 001
    KNIGHT,    // 010
    BISHOP,    // 011
    ROOK,      // 100
    QUEEN,     // 101
    KING       // 110
};

enum Piece : uint8_t {
    NO_PIECE,        // 0000
    WHITE_PAWN,      // 0001
    WHITE_KNIGHT,    // 0010
    WHITE_BISHOP,    // 0011
    WHITE_ROOK,      // 0100
    WHITE_QUEEN,     // 0101
    WHITE_KING,      // 0110
    BLACK_PAWN = 9,  // 1001
    BLACK_KNIGHT,    // 1010
    BLACK_BISHOP,    // 1011
    BLACK_ROOK,      // 1100
    BLACK_QUEEN,     // 1101
    BLACK_KING       // 1110
};

enum Rank : uint8_t {
    RANK_1,
    RANK_2,
    RANK_3,
    RANK_4,
    RANK_5,
    RANK_6,
    RANK_7,
    RANK_8
};

enum File : uint8_t {
    FILE_A,
    FILE_B,
    FILE_C,
    FILE_D,
    FILE_E,
    FILE_F,
    FILE_G,
    FILE_H
};

// Little-Endian Rank-File Mapping
// clang-format off
enum Square : uint8_t {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8,
    NO_SQ = 99
};
// clang-format on

enum CastleFlag : uint8_t {
    NOCA = 0,
    WKCA = 1,
    WQCA = 2,
    BKCA = 4,
    BQCA = 8
};

constexpr PieceType PIECE_GET_TYPE(Piece p) { return static_cast<PieceType>(p & 0x7); }
constexpr Color     PIECE_GET_COLOR(Piece p) { return static_cast<Color>((p & 0x8) / 8); }
constexpr Piece     PIECE_CREATE(PieceType t, Color c) { return static_cast<Piece>((c << 3) | t); }

#endif
