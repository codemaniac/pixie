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

    info->nodes++;

    if ((info->nodes & 2047) == 0)
        _search_check_up(info);

    int32_t alpha_orig = alpha;

    if (position->ply_count > 0)
    {
        TTEntry entry;
        bool    hashtable_probe_status = hashtable_probe(table, position, &entry);

        if (hashtable_probe_status && entry.is_valid && entry.depth >= depth)
        {
            int32_t ttentry_value = entry.value;

            if (ttentry_value < -SEARCH_MATE_SCORE)
                ttentry_value += position->ply_count;
            else if (ttentry_value > SEARCH_MATE_SCORE)
                ttentry_value -= position->ply_count;

            if (entry.flag == EXACT)
                return ttentry_value;
            else if (entry.flag == LOWERBOUND)
                alpha = alpha > ttentry_value ? alpha : ttentry_value;
            else if (entry.flag == UPPERBOUND)
                beta = beta < ttentry_value ? beta : ttentry_value;

            if (alpha >= beta)
                return ttentry_value;
        }
    }

    if (position->ply_count >= SEARCH_DEPTH_MAX - 1)
        return eval_position(position);
    if (position_is_repeated(position) || position->half_move_clock >= 100)
        return 0;
    if (position_is_in_check(position))
        depth++;
    if (depth == 0)
        return _search_quiescence(position, alpha, beta, info);

    int32_t best_value = -SEARCH_SCORE_MAX;
    Move    best_move;

    MoveList moves;
    Move     move;
    uint32_t legal_moves_count = 0;

    movegen_pseudo_legal_all(position, &moves);

    while (movegen_dequeue_move(&moves, &move))
    {
        bool is_valid_move = move_do(position, move);
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

        if (score > best_value)
        {
            best_value = score;
            best_move  = move;
        }

        alpha = (best_value > alpha) ? best_value : alpha;

        if (alpha >= beta)
            break;
    }

    if (legal_moves_count == 0)
    {
        if (position_is_in_check(position))
            return -SEARCH_MATE_VALUE + position->ply_count;
        else
            return 0;
    }

    TTFlag flag;
    if (best_value <= alpha_orig)
        flag = UPPERBOUND;
    else if (best_value >= beta)
        flag = LOWERBOUND;
    else
        flag = EXACT;
    hashtable_store(table, position, depth, flag, best_value, best_move);

    return best_value;
}

int32_t search(Position*           position,
               TranspositionTable* table,
               SearchInfo*         info,
               const bool          iterative,
               const bool          is_uci) {

    int32_t score = -SEARCH_SCORE_MAX;
    Move    bestmove;

    if (iterative)
    {
        for (uint8_t currdepth = 1; currdepth <= info->depth; currdepth++)
        {
            if (info->stopped)
                break;

            score = _search_negamax(position, currdepth, -SEARCH_SCORE_MAX, SEARCH_SCORE_MAX, info,
                                    table);

            if (!info->stopped)
            {
                Move pv[SEARCH_DEPTH_MAX];
                char pv_move_str[10];

                uint8_t pv_count = 0;
                TTEntry entry;
                while (pv_count < currdepth && hashtable_probe(table, position, &entry))
                {
                    if (entry.flag != EXACT)
                        break;

                    if (entry.move.move_id == 0)
                        break;

                    Move pv_move   = entry.move;
                    pv[pv_count++] = pv_move;

                    move_do(position, pv_move);
                }

                while (position->ply_count > 0)
                    move_undo(position);

                bestmove = pv[0];

                if (is_uci)
                {
                    if (score > -SEARCH_MATE_VALUE && score < -SEARCH_MATE_SCORE)
                        printf("info score mate %d depth %d nodes %llu pv ",
                               -(score + SEARCH_MATE_VALUE) / 2 - 1, currdepth, info->nodes);
                    else if (score > SEARCH_MATE_SCORE && score < SEARCH_MATE_VALUE)
                        printf("info score mate %d depth %d nodes %llu pv ",
                               (SEARCH_MATE_VALUE - score) / 2 + 1, currdepth, info->nodes);
                    else
                        printf("info score cp %d depth %d nodes %llu pv ", score, currdepth,
                               info->nodes);

                    char pv_move_str[10];
                    for (uint8_t i = 0; i < pv_count; i++)
                    {
                        move_to_str(pv_move_str, pv[i]);
                        printf("%s ", pv_move_str);
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
        TTEntry entry;
        hashtable_probe(table, position, &entry);
        bestmove = entry.move;
    }

    if (is_uci)
    {
        char best_move_str[10];
        move_to_str(best_move_str, bestmove);
        printf("bestmove %s\n", best_move_str);
    }

    return score;
}
