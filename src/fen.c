#include "include/fen.h"
#include "include/piece.h"
#include "include/position.h"
#include "include/utils.h"
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

Position* fen_to_position(const char* fen) {
    char fen_copy[MAX_FEN_SIZE];

    strncpy(fen_copy, fen, sizeof(fen_copy));
    fen_copy[sizeof(fen_copy) - 1] = '\0';

    char*   parts[6];
    char*   token = strtok(fen_copy, " ");
    uint8_t index = 0;

    while (token && index < 6)
    {
        parts[index++] = token;
        token          = strtok(NULL, " ");
    }

    if (index != 6)
    {
        // TODO: raise exception - Invalid FEN!
        return NULL;
    }

    // Validate the active color part
    if (strcmp(parts[1], "w") != 0 && strcmp(parts[1], "b") != 0)
    {
        // TODO: raise exception - Invalid FEN!
        return NULL;
    }

    // Validate the castling availability part
    if (strcmp(parts[2], "-") != 0)
    {
        for (uint8_t i = 0; i < strlen(parts[2]); i++)
        {
            if (!strchr("KQkq", parts[2][i]))
            {
                // TODO: raise exception - Invalid FEN!
                return NULL;
            }
        }
    }

    // Validate the en passant target square part
    if (strcmp(parts[3], "-") != 0)
    {
        if (strlen(parts[3]) != 2 || parts[3][0] < 'a' || parts[3][0] > 'h'
            || parts[3][1] < '1' || parts[3][1] > '8')
        {
            // TODO: raise exception - Invalid FEN!
            return NULL;
        }
    }

    // Validate the halfmove clock part
    for (uint8_t i = 0; i < strlen(parts[4]); i++)
    {
        if (!isdigit(parts[4][i]))
        {
            // TODO: raise exception - Invalid FEN!
            return NULL;
        }
    }

    // Validate the fullmove number part
    for (uint8_t i = 0; i < strlen(parts[5]); i++)
    {
        if (!isdigit(parts[5][i]))
        {
            // TODO: raise exception - Invalid FEN!
            return NULL;
        }
    }

    Position* position = malloc(sizeof(Position));
    position->board    = create_empty_board();

    char*              ch           = parts[0];
    static const char* white_pieces = ".PNBRQK";
    static const char* black_pieces = ".pnbrqk";
    uint8_t            rank = 7, file = 0;

    while (*ch)
    {
        if (*ch == '/')
        {
            rank--;
            file = 0;
        }
        else if (isdigit(*ch))
        {
            uint8_t npawns = *ch - '0';
            if (npawns < 1 || npawns > 8)
            {
                // TODO: raise exception - Invalid FEN!
                return NULL;
            }
            for (uint8_t i = 0; i < npawns; i++)
            {
                POS_SET_PIECE(position, POS_RF_TO_SQ(rank, file++),
                              SQUARE_EMPTY);
            }
        }
        else if (isalpha(*ch))
        {
            int8_t piece_id;
            if (islower(*ch))
            {
                piece_id = str_indexof(black_pieces, *ch);
                if (piece_id == -1)
                {
                    // TODO: raise exception - Invalid FEN!
                    return NULL;
                }
                piece_id += 8;
            }
            else
            {
                piece_id = str_indexof(white_pieces, *ch);
                if (piece_id == -1)
                {
                    // TODO: raise exception - Invalid FEN!
                    return NULL;
                }
            }
            POS_SET_PIECE(position, POS_RF_TO_SQ(rank, file++), piece_id);
            if (piece_id == WHITE_KING)
            {
                position->board->wK_sq = POS_RF_TO_SQ(rank, file - 1);
            }
            else if (piece_id == BLACK_KING)
            {
                position->board->bK_sq = POS_RF_TO_SQ(rank, file - 1);
            }
        }
        else
        {
            // TODO: raise exception - Invalid FEN!
            return NULL;
        }

        ch++;
    }

    if (strcmp(parts[1], "w") == 0)
    {
        position->active_color = WHITE;
    }
    else if (strcmp(parts[1], "b") == 0)
    {
        position->active_color = BLACK;
    }

    if (strcmp(parts[2], "-") != 0)
    {
        for (uint8_t i = 0; i < strlen(parts[2]); i++)
        {
            switch (parts[2][i])
            {
                case 'K':
                    position->casteling_rights |= WKCA;
                    break;
                case 'Q':
                    position->casteling_rights |= WQCA;
                    break;
                case 'k':
                    position->casteling_rights |= BKCA;
                    break;
                case 'q':
                    position->casteling_rights |= BQCA;
                    break;
                default:
                    break;
            }
        }
    }
    else
    {
        position->casteling_rights = NOCA;
    }

    if (strcmp(parts[3], "-") != 0)
    {
        uint8_t ep_target_rank = (parts[3][1] - '0') - 1;
        uint8_t ep_target_file = parts[3][0] - 'a';
        position->enpassant_target =
          POS_RF_TO_SQ(ep_target_rank, ep_target_file);
    }
    else
    {
        position->enpassant_target = SQUARE_INVALID;
    }

    return position;
}
