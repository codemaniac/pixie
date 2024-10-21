#include "include/search.h"
#include "include/containers.h"
#include "include/eval.h"
#include "include/move.h"
#include "include/tt.h"
#include "include/utils.h"
#include <cassert>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
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
                info->stopped = true;
                exit(EXIT_SUCCESS);
            }
        }
        return;
    }
}

static void _search_check_up(SearchInfo* info) {
    if (info->timeset && utils_get_current_time_in_milliseconds() >= info->stoptime)
        info->stopped = true;

    ReadInput(info);
}

static int32_t _search_quiescence(std::unique_ptr<Position>& position,
                                  int32_t                    alpha,
                                  int32_t                    beta,
                                  SearchInfo*                info) {

    if ((info->nodes & 2047) == 0)
        _search_check_up(info);

    info->nodes++;

    if (position->get_ply_count() >= SEARCH_DEPTH_MAX - 1)
        return eval_position(position);
    if (position->is_repeated() || position->get_half_move_clock() >= 100)
        return 0;

    int32_t stand_pat = eval_position(position);
    if (stand_pat >= beta)
        return beta;
    if (alpha < stand_pat)
        alpha = stand_pat;
    ArrayList<Move> capture_moves;
    position->generate_pseudolegal_moves(&capture_moves, true);
    for (const Move& move : capture_moves)
    {
        const bool is_valid_move = position->move_do(move);
        if (!is_valid_move)
        {
            position->move_undo();
            continue;
        }
        const int32_t score = -_search_quiescence(position, -beta, -alpha, info);
        position->move_undo();
        if (info->stopped)
            return 0;
        if (score >= beta)
            return beta;
        if (score > alpha)
            alpha = score;
    }
    return alpha;
}

static int32_t _search_negamax(std::unique_ptr<Position>&           position,
                               uint8_t                              depth,
                               int32_t                              alpha,
                               int32_t                              beta,
                               std::unique_ptr<TranspositionTable>& table,
                               SearchInfo*                          info) {

    info->nodes++;

    if ((info->nodes & 2047) == 0)
        _search_check_up(info);

    const int32_t alpha_orig = alpha;

    if (position->get_ply_count() > 0)
    {
        TTEntry entry;
        if (table->probe(position, &entry))
        {
            if (entry.is_valid && entry.depth >= depth)
            {
                int32_t ttentry_value = entry.value;

                if (ttentry_value < -SEARCH_MATE_SCORE)
                    ttentry_value += position->get_ply_count();
                else if (ttentry_value > SEARCH_MATE_SCORE)
                    ttentry_value -= position->get_ply_count();

                if (entry.flag == EXACT)
                    return ttentry_value;
                else if (entry.flag == LOWERBOUND)
                    alpha = std::max(alpha, ttentry_value);
                else if (entry.flag == UPPERBOUND)
                    beta = std::min(beta, ttentry_value);

                if (alpha >= beta)
                    return ttentry_value;
            }
        }
    }

    if (position->get_ply_count() >= SEARCH_DEPTH_MAX - 1)
        return eval_position(position);
    if (position->is_repeated() || position->get_half_move_clock() >= 100)
        return 0;
    if (position->is_in_check())
        depth++;
    if (depth == 0)
        return _search_quiescence(position, alpha, beta, info);

    ArrayList<Move> candidate_moves;
    position->generate_pseudolegal_moves(&candidate_moves, false);
    Move     best_move_so_far;
    int32_t  best_score        = -SEARCH_SCORE_MAX;
    uint32_t legal_moves_count = 0;
    for (const Move& move : candidate_moves)
    {
#ifdef DEBUG
        assert(move.get_id() != 0);
#endif
        const bool is_valid_move = position->move_do(move);
        if (!is_valid_move)
        {
            position->move_undo();
            continue;
        }
        legal_moves_count++;
        int32_t score = -_search_negamax(position, depth - 1, -beta, -alpha, table, info);
        position->move_undo();
        if (info->stopped)
            return 0;
        if (score > best_score)
        {
            best_score       = score;
            best_move_so_far = move;
        }
        alpha = std::max(alpha, best_score);
        if (alpha >= beta)
            break;
    }

    if (legal_moves_count == 0)
    {
        if (position->is_in_check())
            return -SEARCH_MATE_VALUE + position->get_ply_count();
        else
            return 0;
    }

    TTFlag flag = NONE;
    if (best_score <= alpha_orig)
        flag = UPPERBOUND;
    else if (best_score >= beta)
        flag = LOWERBOUND;
    else
    {
        flag = EXACT;
#ifdef DEBUG
        assert(best_move_so_far.get_id() != 0);
#endif
    }

    table->store(position, depth, flag, best_score, best_move_so_far);

    return best_score;
}

int32_t search(std::unique_ptr<Position>&           position,
               std::unique_ptr<TranspositionTable>& table,
               SearchInfo*                          info) {

    int32_t score = -SEARCH_SCORE_MAX;
    Move    bestmove;

    if (info->use_iterative)
    {
        for (uint8_t currdepth = 1; currdepth <= info->depth; currdepth++)
        {
            if (info->stopped)
                break;

            score = _search_negamax(position, currdepth, -SEARCH_SCORE_MAX, SEARCH_SCORE_MAX, table,
                                    info);

            if (!info->stopped)
            {
                ArrayList<Move, SEARCH_DEPTH_MAX> pv_move_list;
                uint8_t                           pv_count = 0;
                TTEntry                           entry;
                while (pv_count++ < currdepth && table->probe(position, &entry))
                {
                    if (entry.flag != EXACT)
                        break;

                    const Move pv_move = entry.move;
#ifdef DEBUG
                    assert(pv_move.get_id() != 0);
#endif
                    try
                    {
                        if (position->move_do(pv_move))
                            pv_move_list.emplace_back(pv_move);
                        else
                            break;
                    } catch (const std::invalid_argument& e)
                    {
                        // Found an illegal move! Most likely, a hash collision.
                        break;
                    }
                }

                while (position->get_ply_count() > 0)
                    position->move_undo();

                if (pv_move_list.size() > 0)
                    bestmove = pv_move_list.at(0);

                if (info->use_uci)
                {
                    if (score > -SEARCH_MATE_VALUE && score < -SEARCH_MATE_SCORE)
                        std::cout << "info score mate "
                                  << (int) (-(score + SEARCH_MATE_VALUE) / 2 - 1);
                    else if (score > SEARCH_MATE_SCORE && score < SEARCH_MATE_VALUE)
                        std::cout << "info score mate "
                                  << (int) ((SEARCH_MATE_VALUE - score) / 2 + 1);
                    else
                        std::cout << "info score cp " << (int) score;

                    std::cout << " depth " << (unsigned int) currdepth;
                    std::cout << " nodes " << (unsigned long long) info->nodes;
                    std::cout << " pv ";

                    for (const Move& pv_move : pv_move_list)
                    {
                        pv_move.display();
                        std::cout << " ";
                    }
                    std::cout << std::endl;
                }
            }
        }
    }
    else
    {
        score =
          _search_negamax(position, info->depth, -SEARCH_SCORE_MAX, SEARCH_SCORE_MAX, table, info);
        TTEntry entry;
        table->probe(position, &entry);
        bestmove = entry.move;
    }

    if (info->use_uci)
    {
        std::cout << "bestmove ";
        bestmove.display();
        std::cout << std::endl;
    }

    return score;
}
