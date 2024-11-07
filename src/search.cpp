#include "include/search.h"
#include "include/containers.h"
#include "include/eval.h"
#include "include/move.h"
#include "include/tt.h"
#include "include/utils.h"
#include <cassert>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <memory>
#include <unistd.h>
#ifdef WIN32
    #include "windows.h"
#endif

static const int LMR_FULL_DEPTH_MOVES = 4;
static const int LMR_REDUCTION_LIMIT  = 3;

constexpr uint8_t HISTORY_MOVES_PIECE_IDX(Piece p) { return ((p > 8) ? p - 3 : p - 1); }

struct SearchData {
    Move killer_moves[2][SEARCH_DEPTH_MAX];
    int  history_moves[12][SEARCH_DEPTH_MAX];
#ifdef DEBUG
    int   tt_hit_success;
    int   tt_hit_fail;
    int   tt_hit_lowerbound;
    int   tt_hit_upperbound;
    int   tt_hit_exact;
    int   tt_hit_cut;
    float fh;
    float fhf;
    int   lmr_cnt;
#endif

    SearchData() {
        for (int i = 0; i < SEARCH_DEPTH_MAX; i++)
        {
            this->killer_moves[0][i] = Move();
            this->killer_moves[1][i] = Move();
            for (int j = 0; j < 12; j++)
                this->history_moves[j][i] = 0;
        }
#ifdef DEBUG
        this->tt_hit_success    = 0;
        this->tt_hit_fail       = 0;
        this->tt_hit_lowerbound = 0;
        this->tt_hit_upperbound = 0;
        this->tt_hit_exact      = 0;
        this->tt_hit_cut        = 0;
        this->fh                = 0;
        this->fhf               = 0;
        this->lmr_cnt           = 0;
#endif
    }
};

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

static void search_check_up(SearchInfo* info) {
    if (info->timeset && utils_get_current_time_in_milliseconds() >= info->stoptime)
        info->stopped = true;

    ReadInput(info);
}

static void search_score_moves(ArrayList<Move>*           move_list,
                               std::unique_ptr<Position>& position,
                               SearchData*                data) {
    for (uint32_t i = 0; i < move_list->size(); i++)
    {
        const Move move = move_list->at(i);

        if (MOVE_IS_CAPTURE(move.get_flag()))
        {
            const uint32_t raw_score = move.get_score();
            move_list->at(i).set_score(raw_score + 10000);
        }
        else
        {
            if (move == data->killer_moves[0][position->get_ply_count()])
                move_list->at(i).set_score(9000);
            else if (move == data->killer_moves[1][position->get_ply_count()])
                move_list->at(i).set_score(8000);
            else
            {
                const Piece    move_piece = position->get_piece(move.get_from());
                const uint32_t score      = std::min(
                  7000, data->history_moves[HISTORY_MOVES_PIECE_IDX(move_piece)][move.get_to()]);
                move_list->at(i).set_score(score);
            }
        }
    }
}

static void search_sort_moves(ArrayList<Move>* move_list, const size_t index) {
    for (uint32_t i = index + 1; i < move_list->size(); i++)
    {
        if (move_list->at(i).get_score() > move_list->at(index).get_score())
            std::swap(move_list->at(index), move_list->at(i));
    }
}

static bool search_lmr_ok_to_reduce(std::unique_ptr<Position>& position, const Move& move) {
    // Capture move
    if (MOVE_IS_CAPTURE(move.get_flag()))
        return false;
    // Killer moves
    if (move.get_score() == 9000)
        return false;
    if (move.get_score() == 8000)
        return false;
    // Pawn move
    if (PIECE_GET_TYPE(position->get_piece(move.get_from())) == PAWN)
        return false;
    // TODO: Bad Captures (SSE < 0)
    // Moves which give check
    if (position->is_in_check())
        return false;
    // Promotion move
    if (MOVE_IS_PROMOTION(move.get_flag()))
        return false;

    return true;
}

static int32_t search_quiescence(std::unique_ptr<Position>& position,
                                 int32_t                    alpha,
                                 int32_t                    beta,
                                 SearchInfo*                info,
                                 SearchData*                data) {

    if ((info->nodes & 2047) == 0)
        search_check_up(info);

    if (position->get_ply_count() >= SEARCH_DEPTH_MAX - 1)
        return eval_position(position);
    if (position->is_repeated() || position->get_half_move_clock() >= 100)
        return 0;

    int32_t stand_pat = eval_position(position);
    if (stand_pat >= beta)
        return beta;
    if (alpha < stand_pat)
        alpha = stand_pat;
    // Generate capture moves
    ArrayList<Move> capture_moves;
    position->generate_pseudolegal_moves(&capture_moves, true);
    // Score moves for move ordering
    search_score_moves(&capture_moves, position, data);
    for (uint32_t i = 0; i < capture_moves.size(); i++)
    {
        // Sort moves on the fly to have the top scoring move at i-th position
        search_sort_moves(&capture_moves, i);
        // Pick top scoring move at i-th position
        const Move move          = capture_moves.at(i);
        const bool is_valid_move = position->move_do(move);
        if (!is_valid_move)
        {
            position->move_undo();
            continue;
        }
        info->nodes++;
        const int32_t score = -search_quiescence(position, -beta, -alpha, info, data);
        position->move_undo();
        if (info->stopped)
            return 0;
        if (score > alpha)
        {
            alpha = score;
            if (score >= beta)
                return beta;
        }
    }
    return alpha;
}

static int32_t search_think(std::unique_ptr<Position>&           position,
                            uint8_t                              depth,
                            int32_t                              alpha,
                            int32_t                              beta,
                            std::unique_ptr<TranspositionTable>& table,
                            SearchInfo*                          info,
                            SearchData*                          data) {
    if ((info->nodes & 2047) == 0)
        search_check_up(info);

    const int32_t alpha_orig = alpha;

    const bool is_pv_node = (beta - alpha) > 1;  // If true, then PV node

    if (position->get_ply_count() > 0 && !is_pv_node)
    {
        TTEntry entry;
        if (table->probe(position, &entry))
        {
#ifdef DEBUG
            data->tt_hit_success++;
#endif
            if (entry.is_valid && entry.depth >= depth)
            {
                int32_t ttentry_value = entry.value;

                if (ttentry_value < -SEARCH_MATE_SCORE)
                    ttentry_value += position->get_ply_count();
                else if (ttentry_value > SEARCH_MATE_SCORE)
                    ttentry_value -= position->get_ply_count();

                if (entry.flag == EXACT)
                {
#ifdef DEBUG
                    data->tt_hit_exact++;
#endif
                    return ttentry_value;
                }
                else if (entry.flag == LOWERBOUND)
                {
#ifdef DEBUG
                    data->tt_hit_lowerbound++;
#endif
                    alpha = std::max(alpha, ttentry_value);
                }
                else if (entry.flag == UPPERBOUND)
                {
#ifdef DEBUG
                    data->tt_hit_upperbound++;
#endif
                    beta = std::min(beta, ttentry_value);
                }

                if (alpha >= beta)
                {
#ifdef DEBUG
                    data->tt_hit_cut++;
#endif
                    return ttentry_value;
                }
            }
        }
#ifdef DEBUG
        else
            data->tt_hit_fail++;
#endif
    }

    const bool is_in_check = position->is_in_check();

    if (position->get_ply_count() >= SEARCH_DEPTH_MAX - 1)
        return eval_position(position);
    if (position->is_repeated() || position->get_half_move_clock() >= 100)
        return 0;
    if (is_in_check)
        depth++;
    if (depth == 0)
        return search_quiescence(position, alpha, beta, info, data);

    // Generate candidate moves
    ArrayList<Move> candidate_moves;
    position->generate_pseudolegal_moves(&candidate_moves, false);
    // Score moves for move ordering
    search_score_moves(&candidate_moves, position, data);

    Move     best_move_so_far;
    int32_t  best_score        = -SEARCH_SCORE_MAX;
    uint32_t legal_moves_count = 0;
    uint32_t moves_searched    = 0;

    for (uint32_t i = 0; i < candidate_moves.size(); i++)
    {
        // Sort moves on the fly to have the top scoring move at i-th position
        search_sort_moves(&candidate_moves, i);
        // Pick top scoring move at i-th position
        const Move move          = candidate_moves.at(i);
        const bool is_valid_move = position->move_do(move);
        if (!is_valid_move)
        {
            position->move_undo();
            continue;
        }
        info->nodes++;
        legal_moves_count++;
        int32_t score = -SEARCH_SCORE_MAX;
        if (moves_searched == 0)
            score = -search_think(position, depth - 1, -beta, -alpha, table, info, data);
        else
        {
            // clang-format off
            if (!is_pv_node &&
                !is_in_check &&
                moves_searched >= LMR_FULL_DEPTH_MOVES &&
                depth >= LMR_REDUCTION_LIMIT &&
                search_lmr_ok_to_reduce(position, move))  // clang-format on
            {
#ifdef DEBUG
                data->lmr_cnt++;
#endif
                score = -search_think(position, depth - 2, -alpha - 1, -alpha, table, info, data);
            }
            else
                score = alpha + 1;  // Hack to ensure that full-depth search is done

            if (score > alpha)
            {
                score = -search_think(position, depth - 1, -alpha - 1, -alpha, table, info, data);
                if (score > alpha && score < beta)
                    score = -search_think(position, depth - 1, -beta, -alpha, table, info, data);
            }
        }
        moves_searched++;
        position->move_undo();
        if (info->stopped)
            return 0;
        if (score > best_score)
        {
            best_score       = score;
            best_move_so_far = move;
            if (score > alpha)
            {
                if (score >= beta)
                {
#ifdef DEBUG
                    if (legal_moves_count == 1)
                        data->fhf++;
                    data->fh++;
#endif
                    // fail-hard beta-cutoff
                    // Store killer quite moves
                    if (MOVE_IS_CAPTURE(move.get_flag()) == 0)
                    {
                        const int32_t ply          = position->get_ply_count();
                        data->killer_moves[1][ply] = data->killer_moves[0][ply];
                        data->killer_moves[0][ply] = move;
                    }
                    break;
                }
                alpha = score;
                // Store history quite moves
                if (MOVE_IS_CAPTURE(move.get_flag()) == 0)
                {
                    const Piece move_piece = position->get_piece(move.get_from());
                    data->history_moves[HISTORY_MOVES_PIECE_IDX(move_piece)][move.get_to()] +=
                      depth;
                }
            }
        }
    }

    if (legal_moves_count == 0)
    {
        if (is_in_check)
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
        flag = EXACT;

    table->store(position, depth, flag, best_score, best_move_so_far);

    return best_score;
}

int32_t search(std::unique_ptr<Position>&           position,
               std::unique_ptr<TranspositionTable>& table,
               SearchInfo*                          info) {
#ifdef DEBUG
    table->reset_counters();
#endif
    SearchData data;

    int32_t score = -SEARCH_SCORE_MAX;
    Move    bestmove;

    if (info->use_iterative)
    {
        int32_t alpha = -SEARCH_SCORE_MAX;
        int32_t beta  = SEARCH_SCORE_MAX;
#ifdef DEBUG
        int researches = 0;
#endif

        const uint64_t starttime = utils_get_current_time_in_milliseconds();
        for (uint8_t currdepth = 1; currdepth <= info->depth; currdepth++)
        {
            score = search_think(position, currdepth, alpha, beta, table, info, &data);
            const uint64_t stoptime = utils_get_current_time_in_milliseconds();
            const uint64_t time     = stoptime - starttime;

            if (info->stopped)
                break;

            if ((score <= alpha) || (score >= beta))
            {
                // We fell outside the window
                // Try again with a full-width window (and the same depth).
                alpha = -SEARCH_SCORE_MAX;
                beta  = SEARCH_SCORE_MAX;
                currdepth--;
#ifdef DEBUG
                researches++;
#endif
                continue;
            }
            else
            {
                alpha = score - 100;
                beta  = score + 100;
            }

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
                    if (position->move_do(pv_move))
                        pv_move_list.push(pv_move);
                    else
                        break;
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
                    std::cout << " time " << (unsigned long long) time;
                    std::cout << " nps " << (unsigned long long) (info->nodes * 1000 / (time + 1));
#ifdef DEBUG
                    std::cout << " ord " << std::fixed << std::setprecision(4)
                              << (float) (data.fhf / data.fh);
                    std::cout << " res " << (int) researches;
#endif
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
        score = search_think(position, info->depth, -SEARCH_SCORE_MAX, SEARCH_SCORE_MAX, table,
                             info, &data);
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

#ifdef DEBUG
    std::cout << std::endl << "New Writes Empty = " << table->new_writes_empty;
    std::cout << "\n" << "New Writes Age = " << table->new_writes_age;
    std::cout << "\n" << "New Writes Depth = " << table->new_writes_depth;
    std::cout << "\n" << "TT Hit Success = " << data.tt_hit_success;
    std::cout << "\n  " << "TT Hit Exact = " << data.tt_hit_exact;
    std::cout << "\n  " << "TT Hit Lowerbound = " << data.tt_hit_lowerbound;
    std::cout << "\n  " << "TT Hit Upperbound = " << data.tt_hit_upperbound;
    std::cout << "\n  " << "TT Hit Cut = " << data.tt_hit_cut;
    std::cout << "\n" << "TT Hit Fail = " << data.tt_hit_fail;
    std::cout << "\n" << "LMR Searches = " << data.lmr_cnt << "\n" << std::endl;
#endif

    return score;
}
