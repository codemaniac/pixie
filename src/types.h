#pragma once

namespace tejas {

    using u8  = unsigned char;
    using i8  = char;
    using u32 = unsigned int;
    using i32 = int;
    using u64 = unsigned long long;

    enum Color : u8 {
        WHITE,
        BLACK
    };

    enum PieceType : u8 {
        PAWN = 1,
        KNIGHT,
        BISHOP,
        ROOK,
        QUEEN,
        KING
    };

    enum Piece : u8 {
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
    };

    enum Rank : u8 {
        RANK_1,
        RANK_2,
        RANK_3,
        RANK_4,
        RANK_5,
        RANK_6,
        RANK_7,
        RANK_8
    };

    enum File : u8 {
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
    enum Square : u8 {
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

    const u8 SQUARES_MIRRORED[64] = {
        A8, B8, C8, D8, E8, F7, G8, H8,
        A7, B7, C7, D7, E7, F7, G7, H7,
        A6, B6, C6, D6, E6, F6, G6, H6,
        A5, B5, C5, D5, E5, F5, G5, H5,
        A4, B4, C4, D4, E4, F4, G4, H4,
        A3, B3, C3, D3, E3, F3, G3, H3,
        A2, B2, C2, D2, E2, F2, G2, H2,
        A1, B1, C1, D1, E1, F1, G1, H1,
    };
    // clang-format on

    constexpr PieceType pieceTypeOf(const Piece p) { return static_cast<PieceType>(p & 0x7); }

    constexpr Color pieceColorOf(const Piece p) { return static_cast<Color>((p & 0x8) / 8); }

    constexpr Piece pieceCreate(const PieceType t, Color c) {
        return static_cast<Piece>((c << 3) | t);
    }

    constexpr Color colorFlip(const Color c) { return static_cast<Color>(c ^ 1); }

    constexpr Square rf2sq(const Rank r, const File f) { return static_cast<Square>(8 * r + f); }
    constexpr Square rf2sq(const u8 r, const u8 f) { return static_cast<Square>(8 * r + f); }

    constexpr Rank sq2rank(const Square sq) { return static_cast<Rank>(sq >> 3); }
    constexpr Rank sq2rank(const u8 sq) { return static_cast<Rank>(sq >> 3); }

    constexpr File sq2file(const Square sq) { return static_cast<File>(sq & 7); }
    constexpr File sq2file(const u8 sq) { return static_cast<File>(sq & 7); }
}
