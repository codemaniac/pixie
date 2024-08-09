#include "include/game.h"
#include "include/fen.h"
#include "include/hashkey.h"
#include "include/move.h"
#include "include/piece.h"
#include "lib/logc/log.h"
#include <stdlib.h>

Game* initialize_game(const char* fen) {

    Game* game = (Game*) malloc(sizeof(Game));
    if (game == NULL)
    {
        // TODO: Raise exception - Memory Error!

        log_fatal("Memory allocation failed!");
        exit(1);
    }

    log_trace("Initializing game with FEN: %s", fen);
    game->position       = fen_to_position(fen);
    game->position->hash = hashkey_position(game->position);
    game->ply_count      = 0;

    return game;
}

void static __do_move_prepare_handler(Game* game, const Move move) {
    // Save position to history
    MoveHistoryEntry* history_entry =
      (MoveHistoryEntry*) malloc(sizeof(MoveHistoryEntry));
    if (history_entry == NULL)
    {
        // TODO: Raise exception - Memory Error!

        log_fatal("Memory allocation failed!");
        exit(1);
    }
    history_entry->move                  = move;
    history_entry->prev_casteling_rights = game->position->casteling_rights;
    history_entry->prev_enpassant_target = game->position->enpassant_target;
    history_entry->prev_half_move_clock  = game->position->half_move_clock;
    history_entry->prev_full_move_number = game->position->full_move_number;
    history_entry->prev_hash             = game->position->hash;
    game->history[game->ply_count]       = history_entry;

    // Increment game ply count
    game->ply_count++;

    // Start making the move
    POS_SET_PIECE(game->position, MOVE_FROM_SQ(move.move_id), SQUARE_EMPTY);
}

void static __do_move_success_handler(Game* game) {
    // Generate position hashkey
    game->position->hash = hashkey_position(game->position);

    // Increment full move number
    if (POS_ACTIVE_COLOR(game->position) == BLACK)
    {
        game->position->full_move_number++;
    }

    // Toggle active color
    game->position->active_color = !game->position->active_color;
}

void static __do_move_failure_handler(Game* game, const Move move) {
    undo_move(game, move);
}

void static __undo_move_complete_handler(Game* game) {
    // Decrement game ply count
    game->ply_count--;

    // Restore active color
    game->position->active_color = !game->position->active_color;

    MoveHistoryEntry* history_entry = game->history[game->ply_count];
    // Restore casteling rights
    game->position->casteling_rights = history_entry->prev_casteling_rights;
    // Restore enpassant target
    game->position->enpassant_target = history_entry->prev_enpassant_target;
    // Restore half move clock
    game->position->half_move_clock = history_entry->prev_half_move_clock;
    // Restore full move number
    game->position->full_move_number = history_entry->prev_full_move_number;
    // Restore hash
    // TODO: Fix bug
    const uint64_t hash = hashkey_position(game->position);
    if (hash == history_entry->prev_hash)
    {
        game->position->hash = history_entry->prev_hash;
    }
    else
    {
        game->position->hash = history_entry->prev_hash;
        log_error(
          "Found incorrect hash during move undo\nNEW HASH = %llu\nPREV HASH = %llu",
          hash, history_entry->prev_hash);
        // exit(1);
    }
}

bool do_move(Game* game, const Move move, bool auto_undo_invalid) {

    Color move_piece_color = PIECE_GET_COLOR(MOVE_PIECE(move.move_id));
    if (move_piece_color != game->position->active_color)
    {
        log_error("Found invalid move!");
        exit(1);
    }

    __do_move_prepare_handler(game, move);

    PieceType move_piece_type = PIECE_GET_TYPE(MOVE_PIECE(move.move_id));
    bool      is_legal_move   = false;

    switch (move_piece_type)
    {
        case PAWN:
            is_legal_move = do_pawn_move(game->position, move);
            break;
        case ROOK:
            is_legal_move = do_rook_move(game->position, move);
            break;
        case KING:
            is_legal_move = do_king_move(game->position, move);
            break;
        case KNIGHT:
        case BISHOP:
        case QUEEN:
            is_legal_move = do_knight_bishop_queen_move(game->position, move);
            break;
    }

    if (is_legal_move)
    {
        __do_move_success_handler(game);
        return true;
    }

    if (auto_undo_invalid)
    {
        __do_move_failure_handler(game, move);
    }
    return false;
}

void undo_move(Game* game, const Move move) {

    PieceType move_piece_type = PIECE_GET_TYPE(MOVE_PIECE(move.move_id));

    switch (move_piece_type)
    {
        case PAWN:
            undo_pawn_move(game->position, move);
            break;
        case KING:
            undo_king_move(game->position, move);
            break;
        case ROOK:
        case KNIGHT:
        case BISHOP:
        case QUEEN:
            undo_piece_move(game->position, move);
            break;
    }

    __undo_move_complete_handler(game);
}
