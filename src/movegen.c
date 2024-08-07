#include "include/movegen.h"
#include "include/move.h"
#include "include/piece.h"
#include "include/position.h"
#include <stdbool.h>
#include <stdlib.h>

MovesListNode* __create_move_list_node(Move move) {
    MovesListNode* node = (MovesListNode*) malloc(sizeof(MovesListNode));
    if (node == NULL)
    {
        // TODO: Raise exception - Memory Error!
        exit(1);
    }
    node->move = move;
    node->next = NULL;

    return node;
}

MovesListNode* __insert_move_to_list(MovesListNode* moves_list_tail,
                                     Move           move) {
    if (moves_list_tail == NULL)
    {
        // TODO: Raise exception - Invalid Move List!
        exit(1);
    }
    MovesListNode* new_tail_node = __create_move_list_node(move);
    moves_list_tail->next        = (struct MovesListNode*) new_tail_node;
    return new_tail_node;
}

MovesListNode* __generate_pawn_moves(Position*      position,
                                     uint8_t        src,
                                     MovesListNode* moves_list_tail) {
    Piece   pawn        = POS_PIECE(position, src);
    int8_t  orientation = (POS_ACTIVE_COLOR(position) == WHITE) ? 1 : -1;
    uint8_t starting_rank =
      (POS_ACTIVE_COLOR(position) == WHITE) ? RANK_2 : RANK_7;
    uint8_t penultimate_rank =
      (POS_ACTIVE_COLOR(position) == WHITE) ? RANK_7 : RANK_2;

    Move           move;
    MovesListNode* new_moves_list_tail = moves_list_tail;
    Piece          captured_piece;

    uint8_t dest = src + 10 * orientation;
    if (POS_IS_SQ_ON_BOARD(dest) && POS_IS_SQ_EMPTY(position, dest))
    {
        if (POS_SQ_TO_RANK(src) != penultimate_rank)
        {
            // Generate quite move
            move = encode_quite_move(pawn, src, dest);
            new_moves_list_tail =
              __insert_move_to_list(new_moves_list_tail, move);
        }
        else
        {
            // Promote to knight
            move = encode_pawn_promotion_move(
              pawn, src, dest, NO_PIECE,
              PIECE_CREATE(KNIGHT, POS_ACTIVE_COLOR(position)));
            new_moves_list_tail =
              __insert_move_to_list(new_moves_list_tail, move);
            // Promote to bishop
            move = encode_pawn_promotion_move(
              pawn, src, dest, NO_PIECE,
              PIECE_CREATE(BISHOP, POS_ACTIVE_COLOR(position)));
            new_moves_list_tail =
              __insert_move_to_list(new_moves_list_tail, move);
            // Promote to rook
            move = encode_pawn_promotion_move(
              pawn, src, dest, NO_PIECE,
              PIECE_CREATE(ROOK, POS_ACTIVE_COLOR(position)));
            new_moves_list_tail =
              __insert_move_to_list(new_moves_list_tail, move);
            // Promote to queen
            move = encode_pawn_promotion_move(
              pawn, src, dest, NO_PIECE,
              PIECE_CREATE(QUEEN, POS_ACTIVE_COLOR(position)));
            new_moves_list_tail =
              __insert_move_to_list(new_moves_list_tail, move);
        }
    }

    // Generate pawn start move
    dest                       = src + 20 * orientation;
    uint8_t intermediate_sq_id = src + 10 * orientation;
    if (POS_SQ_TO_RANK(src) == starting_rank
        && POS_IS_SQ_EMPTY(position, intermediate_sq_id)
        && POS_IS_SQ_EMPTY(position, dest))
    {
        move                = encode_pawn_start_move(pawn, src, dest);
        new_moves_list_tail = __insert_move_to_list(new_moves_list_tail, move);
    }

    // Generate pawn capture moves
    for (uint8_t i = 0; i < 2; i++)
    {
        dest = src + PAWN_CAPTURE_DIRECTIONS[i] * orientation;

        if (POS_IS_SQ_ON_BOARD(dest) && !POS_IS_SQ_INVALID(position, dest)
            && !POS_IS_SQ_EMPTY(position, dest)
            && PIECE_GET_COLOR(POS_PIECE(position, dest))
                 != POS_ACTIVE_COLOR(position))
        {
            captured_piece = POS_PIECE(position, dest);

            if (POS_SQ_TO_RANK(src) == penultimate_rank)
            {
                // Capture with promotion
                // Promote to knight
                move = encode_pawn_promotion_move(
                  pawn, src, dest, captured_piece,
                  PIECE_CREATE(KNIGHT, POS_ACTIVE_COLOR(position)));
                new_moves_list_tail =
                  __insert_move_to_list(new_moves_list_tail, move);
                // Promote to bishop
                move = encode_pawn_promotion_move(
                  pawn, src, dest, captured_piece,
                  PIECE_CREATE(BISHOP, POS_ACTIVE_COLOR(position)));
                new_moves_list_tail =
                  __insert_move_to_list(new_moves_list_tail, move);
                // Promote to rook
                move = encode_pawn_promotion_move(
                  pawn, src, dest, captured_piece,
                  PIECE_CREATE(ROOK, POS_ACTIVE_COLOR(position)));
                new_moves_list_tail =
                  __insert_move_to_list(new_moves_list_tail, move);
                // Promote to queen
                move = encode_pawn_promotion_move(
                  pawn, src, dest, captured_piece,
                  PIECE_CREATE(QUEEN, POS_ACTIVE_COLOR(position)));
                new_moves_list_tail =
                  __insert_move_to_list(new_moves_list_tail, move);
            }
            else
            {
                // Capture
                move = encode_capture_move(pawn, src, dest, captured_piece);
                new_moves_list_tail =
                  __insert_move_to_list(new_moves_list_tail, move);
            }
        }

        // En passant capture
        if (POS_IS_SQ_ON_BOARD(dest)
            && !POS_IS_SQ_INVALID(position, position->enpassant_target)
            && dest == position->enpassant_target
            && POS_IS_SQ_EMPTY(position, dest))
        {
            captured_piece = POS_PIECE(position, dest - 10 * orientation);
            move = encode_pawn_enpassant_move(pawn, src, dest, captured_piece);
            new_moves_list_tail =
              __insert_move_to_list(new_moves_list_tail, move);
        }
    }

    return new_moves_list_tail;
}

MovesListNode* __generate_knight_moves(Position*      position,
                                       uint8_t        src,
                                       MovesListNode* moves_list_tail) {
    Piece knight = POS_PIECE(position, src);

    Move           move;
    MovesListNode* new_moves_list_tail = moves_list_tail;
    uint8_t        dest;
    Piece          captured_piece;

    for (uint8_t i = 0; i < 8; i++)
    {
        dest = src + KNIGHT_MOVE_DIRECTIONS[i];
        if (POS_IS_SQ_ON_BOARD(dest) && !POS_IS_SQ_INVALID(position, dest))
        {
            if (POS_IS_SQ_EMPTY(position, dest))
            {
                move = encode_quite_move(knight, src, dest);
                new_moves_list_tail =
                  __insert_move_to_list(new_moves_list_tail, move);
            }
            else if (PIECE_GET_COLOR(POS_PIECE(position, dest))
                     != POS_ACTIVE_COLOR(position))
            {
                captured_piece = POS_PIECE(position, dest);
                move = encode_capture_move(knight, src, dest, captured_piece);
                new_moves_list_tail =
                  __insert_move_to_list(new_moves_list_tail, move);
            }
        }
    }

    return new_moves_list_tail;
}

MovesListNode* __generate_bishop_moves(Position*      position,
                                       uint8_t        src,
                                       MovesListNode* moves_list_tail) {
    Piece bishop = POS_PIECE(position, src);

    Move           move;
    MovesListNode* new_moves_list_tail = moves_list_tail;
    uint8_t        dest;
    Piece          captured_piece;

    for (uint8_t i = 0; i < 4; i++)
    {
        dest = src + BISHOP_MOVE_DIRECTIONS[i];
        while (POS_IS_SQ_ON_BOARD(dest) && !POS_IS_SQ_INVALID(position, dest))
        {
            if (POS_IS_SQ_EMPTY(position, dest))
            {
                move = encode_quite_move(bishop, src, dest);
                new_moves_list_tail =
                  __insert_move_to_list(new_moves_list_tail, move);
            }
            else if (PIECE_GET_COLOR(POS_PIECE(position, dest))
                     != POS_ACTIVE_COLOR(position))
            {
                captured_piece = POS_PIECE(position, dest);
                move = encode_capture_move(bishop, src, dest, captured_piece);
                new_moves_list_tail =
                  __insert_move_to_list(new_moves_list_tail, move);
                break;
            }
            else
            {
                break;
            }
            dest = dest + BISHOP_MOVE_DIRECTIONS[i];
        }
    }

    return new_moves_list_tail;
}

MovesListNode* __generate_rook_moves(Position*      position,
                                     uint8_t        src,
                                     MovesListNode* moves_list_tail) {
    Piece rook = POS_PIECE(position, src);

    Move           move;
    MovesListNode* new_moves_list_tail = moves_list_tail;
    uint8_t        dest;
    Piece          captured_piece;

    for (uint8_t i = 0; i < 4; i++)
    {
        dest = src + ROOK_MOVE_DIRECTIONS[i];
        while (POS_IS_SQ_ON_BOARD(dest) && !POS_IS_SQ_INVALID(position, dest))
        {
            if (POS_IS_SQ_EMPTY(position, dest))
            {
                move = encode_quite_move(rook, src, dest);
                new_moves_list_tail =
                  __insert_move_to_list(new_moves_list_tail, move);
            }
            else if (PIECE_GET_COLOR(POS_PIECE(position, dest))
                     != POS_ACTIVE_COLOR(position))
            {
                captured_piece = POS_PIECE(position, dest);
                move = encode_capture_move(rook, src, dest, captured_piece);
                new_moves_list_tail =
                  __insert_move_to_list(new_moves_list_tail, move);
                break;
            }
            else
            {
                break;
            }
            dest = dest + ROOK_MOVE_DIRECTIONS[i];
        }
    }

    return new_moves_list_tail;
}

MovesListNode* __generate_queen_moves(Position*      position,
                                      uint8_t        src,
                                      MovesListNode* moves_list_tail) {
    Piece queen = POS_PIECE(position, src);

    Move           move;
    MovesListNode* new_moves_list_tail = moves_list_tail;
    uint8_t        dest;
    Piece          captured_piece;

    for (uint8_t i = 0; i < 8; i++)
    {
        dest = src + QUEEN_MOVE_DIRECTIONS[i];
        while (POS_IS_SQ_ON_BOARD(dest) && !POS_IS_SQ_INVALID(position, dest))
        {
            if (POS_IS_SQ_EMPTY(position, dest))
            {
                move = encode_quite_move(queen, src, dest);
                new_moves_list_tail =
                  __insert_move_to_list(new_moves_list_tail, move);
            }
            else if (PIECE_GET_COLOR(POS_PIECE(position, dest))
                     != POS_ACTIVE_COLOR(position))
            {
                captured_piece = POS_PIECE(position, dest);
                move = encode_capture_move(queen, src, dest, captured_piece);
                new_moves_list_tail =
                  __insert_move_to_list(new_moves_list_tail, move);
                break;
            }
            else
            {
                break;
            }
            dest = dest + QUEEN_MOVE_DIRECTIONS[i];
        }
    }

    return new_moves_list_tail;
}

MovesListNode* __generate_king_moves(Position*      position,
                                     uint8_t        src,
                                     MovesListNode* moves_list_tail) {
    Piece king = POS_PIECE(position, src);

    Move           move;
    MovesListNode* new_moves_list_tail = moves_list_tail;
    uint8_t        dest;
    Piece          captured_piece;

    for (uint8_t i = 0; i < 8; i++)
    {
        dest = src + KING_MOVE_DIRECTIONS[i];
        if (POS_IS_SQ_ON_BOARD(dest) && !POS_IS_SQ_INVALID(position, dest))
        {
            if (POS_IS_SQ_EMPTY(position, dest))
            {
                move = encode_quite_move(king, src, dest);
                new_moves_list_tail =
                  __insert_move_to_list(new_moves_list_tail, move);
            }
            else if ((PIECE_GET_COLOR(POS_PIECE(position, dest))
                      != POS_ACTIVE_COLOR(position))
                     && (PIECE_GET_TYPE(POS_PIECE(position, dest)) != KING))
            {
                captured_piece = POS_PIECE(position, dest);
                move = encode_capture_move(king, src, dest, captured_piece);
                new_moves_list_tail =
                  __insert_move_to_list(new_moves_list_tail, move);
            }
        }
        dest = dest + KING_MOVE_DIRECTIONS[i];
    }

    // Generate castling moves
    if (POS_ACTIVE_COLOR(position) == WHITE)
    {
        if (position->casteling_rights & WKCA)
        {
            if (POS_IS_SQ_EMPTY(position, POS_RF_TO_SQ(RANK_1, FILE_F))
                && POS_IS_SQ_EMPTY(position, POS_RF_TO_SQ(RANK_1, FILE_G))
                && !is_square_attacked(position, POS_RF_TO_SQ(RANK_1, FILE_F)))
            {
                move = encode_king_castle_move(
                  king, POS_RF_TO_SQ(RANK_1, FILE_E),
                  POS_RF_TO_SQ(RANK_1, FILE_G), MOVE_FLAG_WKCA);
                new_moves_list_tail =
                  __insert_move_to_list(new_moves_list_tail, move);
            }
        }
        if (position->casteling_rights & WQCA)
        {
            if (POS_IS_SQ_EMPTY(position, POS_RF_TO_SQ(RANK_1, FILE_B))
                && POS_IS_SQ_EMPTY(position, POS_RF_TO_SQ(RANK_1, FILE_C))
                && POS_IS_SQ_EMPTY(position, POS_RF_TO_SQ(RANK_1, FILE_D))
                && !is_square_attacked(position, POS_RF_TO_SQ(RANK_1, FILE_D)))
            {
                move = encode_king_castle_move(
                  king, POS_RF_TO_SQ(RANK_1, FILE_E),
                  POS_RF_TO_SQ(RANK_1, FILE_C), MOVE_FLAG_WQCA);
                new_moves_list_tail =
                  __insert_move_to_list(new_moves_list_tail, move);
            }
        }
    }
    else
    {
        if (position->casteling_rights & BKCA)
        {
            if (POS_IS_SQ_EMPTY(position, POS_RF_TO_SQ(RANK_8, FILE_F))
                && POS_IS_SQ_EMPTY(position, POS_RF_TO_SQ(RANK_8, FILE_G))
                && !is_square_attacked(position, POS_RF_TO_SQ(RANK_8, FILE_F)))
            {
                move = encode_king_castle_move(
                  king, POS_RF_TO_SQ(RANK_8, FILE_E),
                  POS_RF_TO_SQ(RANK_8, FILE_G), MOVE_FLAG_BKCA);
                new_moves_list_tail =
                  __insert_move_to_list(new_moves_list_tail, move);
            }
        }
        if (position->casteling_rights & BQCA)
        {
            if (POS_IS_SQ_EMPTY(position, POS_RF_TO_SQ(RANK_8, FILE_B))
                && POS_IS_SQ_EMPTY(position, POS_RF_TO_SQ(RANK_8, FILE_C))
                && POS_IS_SQ_EMPTY(position, POS_RF_TO_SQ(RANK_8, FILE_D))
                && !is_square_attacked(position, POS_RF_TO_SQ(RANK_8, FILE_D)))
            {
                move = encode_king_castle_move(
                  king, POS_RF_TO_SQ(RANK_8, FILE_E),
                  POS_RF_TO_SQ(RANK_8, FILE_C), MOVE_FLAG_BQCA);
                new_moves_list_tail =
                  __insert_move_to_list(new_moves_list_tail, move);
            }
        }
    }

    return new_moves_list_tail;
}

MovesListNode* generate_pseudo_legal_moves(Position* position) {
    Move           null_move       = {.move_id = 0};
    MovesListNode* moves_list      = __create_move_list_node(null_move);
    MovesListNode* moves_list_tail = moves_list;

    for (uint8_t i = 21; i < 99; i++)
    {
        if (PIECE_GET_COLOR(POS_PIECE(position, i))
            != POS_ACTIVE_COLOR(position))
        {
            continue;
        }

        if (PIECE_GET_TYPE(POS_PIECE(position, i)) == PAWN)
        {
            moves_list_tail =
              __generate_pawn_moves(position, i, moves_list_tail);
        }
        else if (PIECE_GET_TYPE(POS_PIECE(position, i)) == KNIGHT)
        {
            moves_list_tail =
              __generate_knight_moves(position, i, moves_list_tail);
        }
        else if (PIECE_GET_TYPE(POS_PIECE(position, i)) == BISHOP)
        {
            moves_list_tail =
              __generate_bishop_moves(position, i, moves_list_tail);
        }
        else if (PIECE_GET_TYPE(POS_PIECE(position, i)) == ROOK)
        {
            moves_list_tail =
              __generate_rook_moves(position, i, moves_list_tail);
        }
        else if (PIECE_GET_TYPE(POS_PIECE(position, i)) == QUEEN)
        {
            moves_list_tail =
              __generate_queen_moves(position, i, moves_list_tail);
        }
        else if (PIECE_GET_TYPE(POS_PIECE(position, i)) == KING)
        {
            moves_list_tail =
              __generate_king_moves(position, i, moves_list_tail);
        }
    }

    return moves_list;
}

void print_moves_list(MovesListNode* head) {
    MovesListNode* temp = (MovesListNode*) head->next;
    while (temp != NULL)
    {
        print_move(temp->move);
        temp = (MovesListNode*) temp->next;
    }
}

void delete_moves_list(MovesListNode* head) {
    MovesListNode* temp;
    while (head != NULL)
    {
        temp = head;
        head = (MovesListNode*) head->next;
        free(temp);
    }
}
