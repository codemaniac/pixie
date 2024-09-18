#include "include/uci.h"
#include "include/chess.h"
#include "include/fen.h"
#include "include/perft.h"
#include "include/search.h"
#include "include/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PROGRAM_NAME "pixie"
#define VERSION "0.1.0-dev"
#define STARTPOS "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

#define INPUTBUFFER 400 * 6

static void _uci_parse_go(char* uci_line, Position** position) {
    uci_line += 3;

    char*  uci_ch_ptr;
    int8_t depth = -1;

    if (!strncmp(uci_line, "perft", 5))
    {
        uci_ch_ptr = strstr(uci_line, "depth");
        if (uci_ch_ptr != NULL)
        {
            uci_ch_ptr += 6;
            depth = atoi(uci_ch_ptr);
        }
        else
        {
            depth = 6;
        }
        divide(*position, depth);
        return;
    }

    int       movestogo = 30, movetime = -1, time = -1, inc = 0;
    Position* pos = *position;

    if ((uci_ch_ptr = strstr(uci_line, "infinite")))
    {
        // Do nothing
    }

    if ((uci_ch_ptr = strstr(uci_line, "wtime")) && pos->active_color == WHITE)
    {
        time = atoi(uci_ch_ptr + 6);
    }

    if ((uci_ch_ptr = strstr(uci_line, "btime")) && pos->active_color == BLACK)
    {
        time = atoi(uci_ch_ptr + 6);
    }

    if ((uci_ch_ptr = strstr(uci_line, "winc")) && pos->active_color == WHITE)
    {
        inc = atoi(uci_ch_ptr + 5);
    }

    if ((uci_ch_ptr = strstr(uci_line, "binc")) && pos->active_color == BLACK)
    {
        inc = atoi(uci_ch_ptr + 5);
    }

    if ((uci_ch_ptr = strstr(uci_line, "movestogo")))
    {
        movestogo = atoi(uci_ch_ptr + 10);
    }

    if ((uci_ch_ptr = strstr(uci_line, "movetime")))
    {
        movetime = atoi(uci_ch_ptr + 9);
    }

    if ((uci_ch_ptr = strstr(uci_line, "depth")))
    {
        depth = atoi(uci_ch_ptr + 6);
    }

    if (movetime != -1)
    {
        time      = movetime;
        movestogo = 1;
    }

    if (depth == -1)
        depth = SEARCH_MAX_DEPTH;

    SearchInfo* info = (SearchInfo*) malloc(sizeof(SearchInfo));

    info->depth     = (uint8_t) depth;
    info->timeset   = false;
    info->starttime = utils_time_curr_time_ms();

    if (time > 0)
    {
        info->timeset = true;
        time /= movestogo;
        time -= 50;
        info->stoptime = info->starttime + time + inc;
    }
    info->nodes = 0ULL;

    printf("time:%d start:%llu stop:%llu depth:%d timeset:%d\n", time, info->starttime,
           info->stoptime, info->depth, info->timeset);

    Move    best_move;
    char    best_move_str[10];
    int32_t eval = search(pos, info, &best_move);
    move_to_str(best_move_str, best_move);

    printf("info score cp %d nodes %llu time %llu\n", eval, info->nodes,
           utils_time_curr_time_ms() - info->starttime);
    printf("bestmove %s\n", best_move_str);

    free(info);
    info = NULL;
}

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
        else if (!strncmp(line, "go", 2))
        {
            _uci_parse_go(line, &position);
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
