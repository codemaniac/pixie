#ifndef MOVE_H
#define MOVE_H

#include "constants.h"
#include <cstddef>
#include <cstdint>

extern const uint16_t MOVE_SCORE_MVV_LVA[36];

enum MoveFlag : uint8_t {
    MOVE_QUIET,
    MOVE_QUIET_PAWN_DBL_PUSH,
    MOVE_CASTLE_KING_SIDE,
    MOVE_CASTLE_QUEEN_SIDE,
    MOVE_CAPTURE,
    MOVE_CAPTURE_EP,
    MOVE_PROMOTION_KNIGHT = 8,
    MOVE_PROMOTION_BISHOP,
    MOVE_PROMOTION_ROOK,
    MOVE_PROMOTION_QUEEN,
    MOVE_CAPTURE_PROMOTION_KNIGHT,
    MOVE_CAPTURE_PROMOTION_BISHOP,
    MOVE_CAPTURE_PROMOTION_ROOK,
    MOVE_CAPTURE_PROMOTION_QUEEN,
};

class Move {
   private:
    Square   from;      // 6 bits
    Square   to;        // 6 bits
    MoveFlag flag;      // 4 bits
    Piece    captured;  // 4 bits
    uint32_t score;     // 16 bits

   public:
    Move();
    Move(const Move& move);
    Move(const Square   from,
         const Square   to,
         const MoveFlag flag,
         const Piece    captured,
         uint32_t       score);
    Move(const uint32_t id);
    void     set_score(const uint32_t score);
    Square   get_from() const;
    Square   get_to() const;
    MoveFlag get_flag() const;
    Piece    get_captured() const;
    uint32_t get_score() const;
    uint32_t get_id() const;
    uint64_t get_full_id() const;
    Move&    operator=(const Move& move);
    bool     operator==(const Move& other) const;
    void     display() const;
};

constexpr uint8_t MOVE_SCORE_MVV_LVA_IDX(const PieceType a, const PieceType v) {
    return (((a - 1) * 6) + (v - 1));
}
constexpr bool MOVE_IS_CAPTURE(const MoveFlag m) { return (m & 0x4); }
constexpr bool MOVE_IS_PROMOTION(const MoveFlag m) { return (m & 0x8); }

#endif
