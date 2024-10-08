#include "include/search.h"
#include "include/chess.h"
#include "include/eval.h"
#include "include/transpositiontable.h"
#include "include/utils.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifdef WIN32
    #include "windows.h"
#endif

int32_t pv_length[SEARCH_DEPTH_MAX];
int32_t pv_table[SEARCH_DEPTH_MAX][SEARCH_DEPTH_MAX];

// http://home.arcor.de/dreamlike/chess/
int InputWaiting() {
#ifndef WIN32
    fd_set         readfds;
    struct timeval tv;
    FD_ZERO(&readfds);
    FD_SET(fileno(stdin), &readfds);
    tv.tv_sec  = 0;
    tv.tv_usec = 0;
    select(16, &readfds, 0, 0, &tv);

    return (FD_ISSET(fileno(stdin), &readfds));
#else
    static int    init = 0, pipe;
    static HANDLE inh;
    DWORD         dw;

    if (!init)
    {
        init = 1;
        inh  = GetStdHandle(STD_INPUT_HANDLE);
        pipe = !GetConsoleMode(inh, &dw);
        if (!pipe)
        {
            SetConsoleMode(inh, dw & ~(ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT));
            FlushConsoleInputBuffer(inh);
        }
    }
    if (pipe)
    {
        if (!PeekNamedPipe(inh, NULL, 0, NULL, &dw, NULL))
            return 1;
        return dw;
    }
    else
    {
        GetNumberOfConsoleInputEvents(inh, &dw);
        return dw <= 1 ? 0 : dw;
    }
#endif
}

void ReadInput(SearchInfo* info) {
    int  bytes;
    char input[256] = "", *endc;

    if (InputWaiting())
    {
        info->stopped = true;
        do
        {
            bytes = read(fileno(stdin), input, 256);
        } while (bytes < 0);
        endc = strchr(input, '\n');
        if (endc)
            *endc = 0;

        if (strlen(input) > 0)
        {
            if (!strncmp(input, "quit", 4))
            {
                info->quit = true;
                exit(EXIT_SUCCESS);
            }
        }
        return;
    }
}

static void _search_check_up(SearchInfo* info) {
    if (info->timeset && utils_time_curr_time_ms() >= info->stoptime)
        info->stopped = true;

    ReadInput(info);
}

static int32_t
_search_quiescence(Position* position, int32_t alpha, int32_t beta, SearchInfo* info) {

    if ((info->nodes & 2047) == 0)
        _search_check_up(info);

    info->nodes++;

    int32_t evaluation = eval_position(position);

    if (position->ply_count >= SEARCH_DEPTH_MAX - 1)
        return evaluation;

    if (position_is_repeated(position) || position->half_move_clock >= 100)
        return 0;

    if (evaluation >= beta)
        return beta;
    if (evaluation > alpha)
        alpha = evaluation;

    MoveList moves;
    Move     move;
    bool     is_valid_move = false;

    movegen_pseudo_legal_captures(position, &moves);

    while (movegen_dequeue_move(&moves, &move))
    {
        is_valid_move = move_do(position, move);
        if (!is_valid_move || !position_is_valid(position))
        {
            move_undo(position);
            continue;
        }

        int32_t score = -_search_quiescence(position, -beta, -alpha, info);

        move_undo(position);

        if (info->stopped)
            return 0;

        if (score >= beta)
            return beta;

        if (score > alpha)
            alpha = score;
    }
    return alpha;
}

static int32_t _search_negamax(Position*           position,
                               uint8_t             depth,
                               int32_t             alpha,
                               int32_t             beta,
                               SearchInfo*         info,
                               TranspositionTable* table) {

    if ((info->nodes & 2047) == 0)
        _search_check_up(info);

    pv_length[position->ply_count] = position->ply_count;

    int32_t alpha_orig = alpha;

    if (position->ply_count > 0)
    {
        TTEntry entry;
        bool    hashtable_probe_status = hashtable_probe(table, position, &entry);

        if (hashtable_probe_status && entry.is_valid && entry.depth >= depth)
        {
            int32_t value = entry.value;

            if (value > SEARCH_IS_MATE)
                value -= position->ply_count;
            else if (value < -SEARCH_IS_MATE)
                value += position->ply_count;

            if (entry.flag == EXACT)
                return value;
            else if (entry.flag == LOWERBOUND)
                alpha = alpha > value ? alpha : value;
            else if (entry.flag == UPPERBOUND)
                beta = beta < value ? beta : value;

            if (alpha >= beta)
                return value;
        }
    }

    info->nodes++;

    if (position->ply_count >= SEARCH_DEPTH_MAX - 1)
        return eval_position(position);
    if (position_is_repeated(position) || position->half_move_clock >= 100)
        return 0;
    if (position_is_in_check(position))
        depth++;
    if (depth == 0)
        return _search_quiescence(position, alpha, beta, info);

    int32_t value = -SEARCH_SCORE_MAX;

    MoveList moves;
    Move     move;
    uint32_t legal_moves_count = 0;
    bool     is_valid_move     = false;

    movegen_pseudo_legal_all(position, &moves);

    while (movegen_dequeue_move(&moves, &move))
    {
        is_valid_move = move_do(position, move);
        if (!is_valid_move || !position_is_valid(position))
        {
            move_undo(position);
            continue;
        }

        legal_moves_count++;

        int32_t score = -_search_negamax(position, depth - 1, -beta, -alpha, info, table);

        move_undo(position);

        if (info->stopped)
            return 0;

        value = score > value ? score : value;

        if (value > alpha)
        {
            alpha = value;
            // Set PV
            const uint8_t ply  = position->ply_count;
            pv_table[ply][ply] = move.move_id;
            for (int next_ply = ply + 1; next_ply < pv_length[ply + 1]; next_ply++)
                pv_table[ply][next_ply] = pv_table[ply + 1][next_ply];
            pv_length[ply] = pv_length[ply + 1];
        }

        if (alpha >= beta)
            break;
    }

    if (legal_moves_count == 0)
    {
        if (position_is_in_check(position))
            return -SEARCH_SCORE_MAX + position->ply_count;
        else
            return 0;
    }

    TTFlag flag;
    if (value <= alpha_orig)
        flag = UPPERBOUND;
    else if (value >= beta)
        flag = LOWERBOUND;
    else
        flag = EXACT;
    hashtable_store(table, position, depth, flag, value);

    return value;
}

int32_t search(Position*           position,
               TranspositionTable* table,
               SearchInfo*         info,
               const bool          iterative,
               const bool          is_uci) {
    memset(pv_length, 0, sizeof(pv_length));
    memset(pv_table, 0, sizeof(pv_table));

    int32_t score;
    Move    pv_move, bestmove;
    char    move_str[10];

    uint8_t currdepth = 1;

    if (iterative)
    {
        for (currdepth = 1; currdepth <= info->depth; currdepth++)
        {
            if (info->stopped)
                break;

            score = _search_negamax(position, currdepth, -SEARCH_SCORE_MAX, SEARCH_SCORE_MAX, info,
                                    table);
            if (!info->stopped)
            {
                bestmove.move_id = pv_table[0][0];

                if (is_uci)
                {
                    if (score < -SEARCH_IS_MATE)
                        printf("info score mate %d depth %d nodes %llu pv ",
                               -(((score + SEARCH_SCORE_MAX) / 2) - 1), currdepth, info->nodes);
                    else if (score > SEARCH_IS_MATE)
                        printf("info score mate %d depth %d nodes %llu pv ",
                               ((SEARCH_SCORE_MAX - score) / 2) + 1, currdepth, info->nodes);
                    else
                        printf("info score cp %d depth %d nodes %llu pv ", score, currdepth,
                               info->nodes);

                    for (uint8_t count = 0; count < pv_length[0]; count++)
                    {
                        pv_move.move_id = pv_table[0][count];
                        move_to_str(move_str, pv_move);
                        printf("%s ", move_str);
                    }
                    printf("\n");
                }
            }
        }
    }
    else
    {
        score =
          _search_negamax(position, info->depth, -SEARCH_SCORE_MAX, SEARCH_SCORE_MAX, info, table);
        bestmove.move_id = pv_table[0][0];
    }

    if (is_uci)
    {
        move_to_str(move_str, bestmove);
        printf("bestmove %s\n", move_str);
    }

    return score;
}
