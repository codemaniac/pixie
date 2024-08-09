#include "include/move.h"
#include "include/piece.h"
#include "include/position.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

Move encode_move(const Piece    piece,
                 const uint8_t  from_sq,
                 const uint8_t  to_sq,
                 const Piece    captured_piece,
                 const Piece    promoted_piece,
                 const bool     flag_ps,
                 const bool     flag_ep,
                 const uint32_t flag_ca) {
    uint32_t move_id = (piece) | (from_sq << 4) | (to_sq << 11)
                     | (captured_piece << 18) | (promoted_piece << 23);
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

    const Move move = {.move_id = move_id};
    return move;
}

Move encode_quite_move(const Piece   piece,
                       const uint8_t from_sq,
                       const uint8_t to_sq) {
    return encode_move(piece, from_sq, to_sq, NO_PIECE, NO_PIECE, false, false,
                       0);
}

Move encode_capture_move(const Piece   piece,
                         const uint8_t from_sq,
                         const uint8_t to_sq,
                         const Piece   captured_piece) {
    return encode_move(piece, from_sq, to_sq, captured_piece, NO_PIECE, false,
                       false, 0);
}

Move encode_pawn_start_move(const Piece   pawn,
                            const uint8_t from_sq,
                            const uint8_t to_sq) {
    return encode_move(pawn, from_sq, to_sq, NO_PIECE, NO_PIECE, true, false,
                       0);
}

Move encode_pawn_promotion_move(const Piece   pawn,
                                const uint8_t from_sq,
                                const uint8_t to_sq,
                                const Piece   captured_piece,
                                const Piece   promoted_piece) {
    return encode_move(pawn, from_sq, to_sq, captured_piece, promoted_piece,
                       false, false, 0);
}

Move encode_pawn_enpassant_move(const Piece   pawn,
                                const uint8_t from_sq,
                                const uint8_t to_sq,
                                const Piece   captured_pawn) {
    return encode_move(pawn, from_sq, to_sq, captured_pawn, NO_PIECE, false,
                       true, 0);
}

Move encode_king_castle_move(const Piece    king,
                             const uint8_t  from_sq,
                             const uint8_t  to_sq,
                             const uint32_t flag_ca) {
    return encode_move(king, from_sq, to_sq, NO_PIECE, NO_PIECE, false, false,
                       flag_ca);
}

bool do_pawn_move(Position* position, const Move move) {
    if (MOVE_PROMOTED(move.move_id))
    {
        // Pawn promotion move
        POS_SET_PIECE(position, MOVE_TO_SQ(move.move_id),
                      MOVE_PROMOTED(move.move_id));
    }
    else if (move.move_id & MOVE_FLAG_EP)
    {
        // En Passant move
        POS_SET_PIECE(position, MOVE_TO_SQ(move.move_id),
                      MOVE_PIECE(move.move_id));
        // Remove EP captured pawn
        const int8_t orientation =
          (POS_ACTIVE_COLOR(position) == WHITE) ? 1 : -1;
        POS_SET_PIECE(position, MOVE_TO_SQ(move.move_id) - 10 * orientation,
                      SQUARE_EMPTY);
    }
    else if (move.move_id & MOVE_FLAG_PS)
    {
        // Pawn start move
        POS_SET_PIECE(position, MOVE_TO_SQ(move.move_id),
                      MOVE_PIECE(move.move_id));
        // Set En passant target square in position
        const int8_t orientation =
          (POS_ACTIVE_COLOR(position) == WHITE) ? 1 : -1;
        position->enpassant_target =
          MOVE_TO_SQ(move.move_id) - 10 * orientation;
    }
    else
    {
        // Quite or capture move
        POS_SET_PIECE(position, MOVE_TO_SQ(move.move_id),
                      MOVE_PIECE(move.move_id));
    }

    if (!is_position_in_check(position))
    {
        // Reset half move clock
        position->half_move_clock = 0;

        return true;
    }

    return false;
}

bool do_rook_move(Position* position, const Move move) {
    // Quite or capture move
    POS_SET_PIECE(position, MOVE_TO_SQ(move.move_id), MOVE_PIECE(move.move_id));

    if (!is_position_in_check(position))
    {
        // Remove castle permissions
        const uint8_t ca_perm_nullifier =
          (POS_ACTIVE_COLOR(position) == WHITE) ? 0xC : 0x3;
        position->casteling_rights &= ca_perm_nullifier;
        // Reset half move clock
        if (MOVE_CAPTURED(move.move_id))
        {
            // Reset half move clock
            position->half_move_clock = 0;
        }

        return true;
    }

    return false;
}

bool do_king_move(Position* position, const Move move) {
    if (move.move_id & MOVE_FLAG_WKCA)
    {
        // WKCA
        POS_SET_PIECE(position, E1, SQUARE_EMPTY);
        POS_SET_PIECE(position, F1, WHITE_ROOK);
        POS_SET_PIECE(position, G1, WHITE_KING);
        POS_SET_PIECE(position, H1, SQUARE_EMPTY);
        position->board->wK_sq = G1;
    }
    else if (move.move_id & MOVE_FLAG_WQCA)
    {
        // WQCA
        POS_SET_PIECE(position, E1, SQUARE_EMPTY);
        POS_SET_PIECE(position, D1, WHITE_ROOK);
        POS_SET_PIECE(position, C1, WHITE_KING);
        POS_SET_PIECE(position, B1, SQUARE_EMPTY);
        POS_SET_PIECE(position, A1, SQUARE_EMPTY);
        position->board->wK_sq = C1;
    }
    else if (move.move_id & MOVE_FLAG_BKCA)
    {
        // BKCA
        POS_SET_PIECE(position, E8, SQUARE_EMPTY);
        POS_SET_PIECE(position, F8, WHITE_ROOK);
        POS_SET_PIECE(position, G8, WHITE_KING);
        POS_SET_PIECE(position, H8, SQUARE_EMPTY);
        position->board->bK_sq = G8;
    }
    else if (move.move_id & MOVE_FLAG_BQCA)
    {
        // BQCA
        POS_SET_PIECE(position, E8, SQUARE_EMPTY);
        POS_SET_PIECE(position, D8, WHITE_ROOK);
        POS_SET_PIECE(position, C8, WHITE_KING);
        POS_SET_PIECE(position, B8, SQUARE_EMPTY);
        POS_SET_PIECE(position, A8, SQUARE_EMPTY);
        position->board->bK_sq = C8;
    }
    else
    {
        // Quite or capture move
        POS_SET_PIECE(position, MOVE_TO_SQ(move.move_id),
                      MOVE_PIECE(move.move_id));
        if (POS_ACTIVE_COLOR(position) == WHITE)
        {
            position->board->wK_sq = MOVE_TO_SQ(move.move_id);
        }
        else
        {
            position->board->bK_sq = MOVE_TO_SQ(move.move_id);
        }
    }

    if (!is_position_in_check(position))
    {
        // Remove castle permissions
        const uint8_t ca_perm_nullifier =
          (POS_ACTIVE_COLOR(position) == WHITE) ? 0xC : 0x3;
        position->casteling_rights &= ca_perm_nullifier;
        // Reset half move clock
        if (MOVE_CAPTURED(move.move_id))
        {
            // Reset half move clock
            position->half_move_clock = 0;
        }
        return true;
    }

    return false;
}

bool do_knight_bishop_queen_move(Position* position, const Move move) {
    // Quite or capture move
    POS_SET_PIECE(position, MOVE_TO_SQ(move.move_id), MOVE_PIECE(move.move_id));

    if (!is_position_in_check(position))
    {
        // Reset half move clock
        if (MOVE_CAPTURED(move.move_id))
        {
            // Reset half move clock
            position->half_move_clock = 0;
        }
        return true;
    }

    return false;
}

void undo_pawn_move(Position* position, Move move) {
    POS_SET_PIECE(position, MOVE_FROM_SQ(move.move_id),
                  MOVE_PIECE(move.move_id));
    if (MOVE_CAPTURED(move.move_id) != NO_PIECE)
    {
        if (move.move_id & MOVE_FLAG_EP)
        {
            const int8_t orientation =
              (POS_ACTIVE_COLOR(position) == WHITE) ? 1 : -1;
            POS_SET_PIECE(position, MOVE_TO_SQ(move.move_id) - 10 * orientation,
                          MOVE_CAPTURED(move.move_id));
        }
        else
        {
            POS_SET_PIECE(position, MOVE_TO_SQ(move.move_id),
                          MOVE_CAPTURED(move.move_id));
        }
    }
    else
    {
        // Undo quite move and quite promotion move
        POS_SET_PIECE(position, MOVE_TO_SQ(move.move_id), SQUARE_EMPTY);
    }
}

void undo_king_move(Position* position, Move move) {
    POS_SET_PIECE(position, MOVE_FROM_SQ(move.move_id),
                  MOVE_PIECE(move.move_id));
    if (MOVE_CAPTURED(move.move_id) != NO_PIECE)
    {
        POS_SET_PIECE(position, MOVE_TO_SQ(move.move_id),
                      MOVE_CAPTURED(move.move_id));
    }
    else if (move.move_id & MOVE_FLAG_WKCA)
    {
        POS_SET_PIECE(position, E1, WHITE_KING);
        POS_SET_PIECE(position, F1, SQUARE_EMPTY);
        POS_SET_PIECE(position, G1, SQUARE_EMPTY);
        POS_SET_PIECE(position, H1, WHITE_ROOK);
    }
    else if (move.move_id & MOVE_FLAG_WQCA)
    {
        POS_SET_PIECE(position, E1, WHITE_KING);
        POS_SET_PIECE(position, D1, SQUARE_EMPTY);
        POS_SET_PIECE(position, C1, SQUARE_EMPTY);
        POS_SET_PIECE(position, B1, SQUARE_EMPTY);
        POS_SET_PIECE(position, A1, WHITE_ROOK);
    }
    else if (move.move_id & MOVE_FLAG_BKCA)
    {
        POS_SET_PIECE(position, E8, WHITE_KING);
        POS_SET_PIECE(position, F8, SQUARE_EMPTY);
        POS_SET_PIECE(position, G8, SQUARE_EMPTY);
        POS_SET_PIECE(position, H8, WHITE_ROOK);
    }
    else if (move.move_id & MOVE_FLAG_BQCA)
    {
        POS_SET_PIECE(position, E8, WHITE_KING);
        POS_SET_PIECE(position, D8, SQUARE_EMPTY);
        POS_SET_PIECE(position, C8, SQUARE_EMPTY);
        POS_SET_PIECE(position, B8, SQUARE_EMPTY);
        POS_SET_PIECE(position, A8, WHITE_ROOK);
    }
    else
    {
        POS_SET_PIECE(position, MOVE_TO_SQ(move.move_id), SQUARE_EMPTY);
    }
}

void undo_piece_move(Position* position, Move move) {
    POS_SET_PIECE(position, MOVE_FROM_SQ(move.move_id),
                  MOVE_PIECE(move.move_id));

    if (MOVE_CAPTURED(move.move_id) != NO_PIECE)
    {
        // Undo capture move by piece
        POS_SET_PIECE(position, MOVE_TO_SQ(move.move_id),
                      MOVE_CAPTURED(move.move_id));
    }
    else
    {
        // Undo quite move by piece
        POS_SET_PIECE(position, MOVE_TO_SQ(move.move_id), SQUARE_EMPTY);
    }
}

void print_move(const Move move) {
    const char* pieces = ".PNBRQK";
    const char* files  = "abcdefgh";

    const char piece = pieces[(MOVE_PIECE(move.move_id) & 0x7)];

    const int  src_rank = POS_SQ_TO_RANK(MOVE_FROM_SQ(move.move_id)) + 1;
    const char src_file = files[POS_SQ_TO_FILE(MOVE_FROM_SQ(move.move_id))];

    const int  dest_rank = POS_SQ_TO_RANK(MOVE_TO_SQ(move.move_id)) + 1;
    const char dest_file = files[POS_SQ_TO_FILE(MOVE_TO_SQ(move.move_id))];

    printf("%c%c%d%c%d\n", piece, src_file, src_rank, dest_file, dest_rank);
}
