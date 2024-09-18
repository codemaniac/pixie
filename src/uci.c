#include "include/uci.h"
#include "include/chess.h"
#include "include/fen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PROGRAM_NAME "pixie"
#define VERSION "0.1.0-dev"
#define STARTPOS "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

#define INPUTBUFFER 400 * 6

static void _uci_parse_position(char* uci_line, Position** position) {
    uci_line += 9;

    char* uci_ch_ptr;

    if (!strncmp(uci_line, "startpos", 8))
    {
        *position = fen_to_position(STARTPOS);
    }
    else if (!strncmp(uci_line, "fen", 3))
    {
        uci_ch_ptr = strstr(uci_line, "fen");
        uci_ch_ptr += 4;
        *position = fen_to_position(uci_ch_ptr);
    }

    uci_ch_ptr = strstr(uci_line, "moves");
    Move move;

    if (uci_ch_ptr != NULL)
    {
        uci_ch_ptr += 6;
        while (*uci_ch_ptr)
        {
            move = move_from_str(uci_ch_ptr, *position);
            if (move.move_id == 0)
                break;
            move_do(*position, move);
            while (*uci_ch_ptr && *uci_ch_ptr != ' ')
            {
                uci_ch_ptr++;
            }
            uci_ch_ptr++;
        }
    }
}

void uci_loop(void) {
    chess_initialize();
    Position* position;

    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    char line[INPUTBUFFER];

    printf("id name %s-%s\n", PROGRAM_NAME, VERSION);
    puts("uciok");

    while (true)
    {
        memset(&line[0], 0, sizeof(line));
        fflush(stdout);

        if (!fgets(line, INPUTBUFFER, stdin))
            continue;

        if (line[0] == '\n')
            continue;

        if (!strncmp(line, "isready", 7))
        {
            puts("readyok");
            continue;
        }
        else if (!strncmp(line, "ucinewgame", 10))
        {
            _uci_parse_position("position startpos\n", &position);
            position->ply_count = 0;
        }
        else if (!strncmp(line, "position", 8))
        {
            _uci_parse_position(line, &position);
            position->ply_count = 0;
        }
        else if (!strncmp(line, "display", 7))
        {
            position_display(position);
        }
        else if (!strncmp(line, "uci", 3))
        {
            printf("id name %s-%s\n", PROGRAM_NAME, VERSION);
            puts("uciok");
        }
        else if (!strncmp(line, "quit", 4))
            break;
    }
}
