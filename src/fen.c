#include "include/fen.h"
#include "include/chess.h"
#include "include/hashkey.h"
#include "include/utils.h"
#include <ctype.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

void fen_to_position(const char* fen, Position* position) {
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
        exit(EXIT_FAILURE);
    }

    // Validate the active color part
    utils_str_trimwhitespace(parts[1]);
    if (strcmp(parts[1], "w") != 0 && strcmp(parts[1], "b") != 0)
    {
        exit(EXIT_FAILURE);
    }

    // Validate the castling availability part
    utils_str_trimwhitespace(parts[2]);
    if (strcmp(parts[2], "-") != 0)
    {
        for (uint8_t i = 0; i < strlen(parts[2]); i++)
        {
            if (!strchr("KQkq", parts[2][i]))
            {
                exit(EXIT_FAILURE);
            }
        }
    }

    // Validate the en passant target square part
    utils_str_trimwhitespace(parts[3]);
    if (strcmp(parts[3], "-") != 0)
    {
        if (strlen(parts[3]) != 2 || parts[3][0] < 'a' || parts[3][0] > 'h' || parts[3][1] < '1'
            || parts[3][1] > '8')
        {
            exit(EXIT_FAILURE);
        }
    }

    // Validate the halfmove clock part
    utils_str_trimwhitespace(parts[4]);
    for (uint8_t i = 0; i < strlen(parts[4]); i++)
    {
        if (!isdigit(parts[4][i]))
        {
            exit(EXIT_FAILURE);
        }
    }

    // Validate the fullmove number part
    utils_str_trimwhitespace(parts[5]);
    for (uint8_t i = 0; i < strlen(parts[5]); i++)
    {
        if (!isdigit(parts[5][i]))
        {
            exit(EXIT_FAILURE);
        }
    }

    utils_str_trimwhitespace(parts[0]);

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
            ch++;
            continue;
        }
        else if (isdigit(*ch))
        {
            uint8_t empty_squares = *ch - '0';
            if (empty_squares < 1 || empty_squares > 8)
            {
                exit(EXIT_FAILURE);
            }
            else
            {
                file += empty_squares;
            }
        }
        else if (isalpha(*ch))
        {
            int8_t piece_id;
            if (islower(*ch))
            {
                piece_id = utils_str_indexof(black_pieces, *ch);
                if (piece_id == -1)
                {
                    exit(EXIT_FAILURE);
                }
                piece_id += 8;
            }
            else
            {
                piece_id = utils_str_indexof(white_pieces, *ch);
                if (piece_id == -1)
                {
                    exit(EXIT_FAILURE);
                }
            }
            position_set_piece(position, piece_id, BOARD_RF_TO_SQ(rank, file++));
        }
        else
        {
            exit(EXIT_FAILURE);
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

        if (strcmp(parts[1], "w") == 0)
        {
            if (ep_target_rank != RANK_6)
            {
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            if (ep_target_rank != RANK_3)
            {
                exit(EXIT_FAILURE);
            }
        }

        position->enpassant_target = BOARD_RF_TO_SQ(ep_target_rank, ep_target_file);
    }
    else
    {
        position->enpassant_target = NO_EP_TARGET;
    }

    position->half_move_clock = atoi(parts[4]);

    position->full_move_number = atoi(parts[5]);

    position->hash = hashkey_position(position);
}
