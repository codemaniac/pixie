#include "include/eval.h"
#include "include/constants.h"
#include "include/containers.h"
#include "include/position.h"

static ArrayList<ParameterTuple, N_PARAMS> parameters;

void init_eval() {

    for (const auto& p : pieces_scores)
    {
        ParameterTuple tuple;
        tuple[MG] = mg_score(p);
        tuple[EG] = eg_score(p);

        parameters.push(tuple);
    }

    for (const auto& x : pst_rank_scores)
    {
        ParameterTuple tuple;
        tuple[MG] = mg_score(x);
        tuple[EG] = eg_score(x);

        parameters.push(tuple);
    }

    for (const auto& x : pst_file_scores)
    {
        ParameterTuple tuple;
        tuple[MG] = mg_score(x);
        tuple[EG] = eg_score(x);

        parameters.push(tuple);
    }
}

void init_eval_trace(EvalTrace* trace, const Position& position) {
    for (int sq = A1; sq <= H8; sq++)
    {
        const Square square = static_cast<Square>(sq);
        const Piece  p      = position.get_piece(square);

        if (p == NO_PIECE)
        {
            continue;
        }

        const Rank rank = BOARD_SQ_TO_RANK(square);
        const File file = BOARD_SQ_TO_FILE(square);

        const PieceType ptype  = PIECE_GET_TYPE(p);
        const Color     pcolor = PIECE_GET_COLOR(p);

        trace->pieces[ptype - 1][pcolor]++;

        if (pcolor == WHITE)
        {
            trace->pst_rank[((ptype - 1) * 8) + rank][pcolor]++;
        }
        else
        {
            const Square msq   = static_cast<Square>(SQUARES_MIRRORED[sq]);
            const Rank   mrank = BOARD_SQ_TO_RANK(msq);
            trace->pst_rank[((ptype - 1) * 8) + mrank][pcolor]++;
        }

        trace->pst_file[((ptype - 1) * 8) + file][pcolor]++;
    }
}

void init_coefficients(ArrayList<double, N_PARAMS>* coefficients, const EvalTrace& trace) {
    for (int32_t p = 0; p < 6; p++)
    {
        coefficients->push(trace.pieces[p][WHITE] - trace.pieces[p][BLACK]);
    }

    for (int32_t i = 0; i < 48; i++)
    {
        coefficients->push(trace.pst_rank[i][WHITE] - trace.pst_rank[i][BLACK]);
    }

    for (int32_t i = 0; i < 48; i++)
    {
        coefficients->push(trace.pst_file[i][WHITE] - trace.pst_file[i][BLACK]);
    }
}

int32_t get_phase(const Position& position) {
    const uint8_t wQ = position.get_piece_count(WHITE_QUEEN);
    const uint8_t bQ = position.get_piece_count(BLACK_QUEEN);

    const uint8_t wR = position.get_piece_count(WHITE_ROOK);
    const uint8_t bR = position.get_piece_count(BLACK_ROOK);

    const uint8_t wB = position.get_piece_count(WHITE_BISHOP);
    const uint8_t bB = position.get_piece_count(BLACK_BISHOP);

    const uint8_t wN = position.get_piece_count(WHITE_KNIGHT);
    const uint8_t bN = position.get_piece_count(BLACK_KNIGHT);

    const uint8_t q = wQ + bQ;
    const uint8_t r = wR + bR;
    const uint8_t b = wB + bB;
    const uint8_t n = wN + bN;

    int32_t phase = 24 - (4 * q) - (2 * r) - (1 * b) - (1 * n);

    return phase;
}

int32_t eval_position(Position& position) {
    EvalTrace trace = {};
    init_eval_trace(&trace, position);

    ArrayList<double, N_PARAMS> coefficients;
    init_coefficients(&coefficients, trace);

    int32_t phase = get_phase(position);

    int32_t midgame = 0;
    int32_t endgame = 0;

    for (int i = 0; i < N_PARAMS; i++)
    {
        midgame += coefficients[i] * parameters[i][MG];
        endgame += coefficients[i] * parameters[i][EG];
    }

    // Linear interpolation for Tapered Evaluation
    int32_t eval = (midgame * phase + endgame * (24 - phase)) / 24;

    const int8_t side_to_move = 1 - (2 * position.get_active_color());
    eval *= side_to_move;

    return eval;
}

bool eval_is_end_game(Position& position) {
    const uint8_t wQ = position.get_piece_count(WHITE_QUEEN);
    const uint8_t bQ = position.get_piece_count(BLACK_QUEEN);

    const uint8_t wR = position.get_piece_count(WHITE_ROOK);
    const uint8_t bR = position.get_piece_count(BLACK_ROOK);

    const uint8_t wB = position.get_piece_count(WHITE_BISHOP);
    const uint8_t bB = position.get_piece_count(BLACK_BISHOP);

    const uint8_t wN = position.get_piece_count(WHITE_KNIGHT);
    const uint8_t bN = position.get_piece_count(BLACK_KNIGHT);

    bool is_end_game = false;

    if (wQ == 0 && bQ == 0)
        is_end_game = true;
    else if (wQ == 1 && bQ == 1 && wR == 0 && bR == 0)
    {
        if ((wN + wB) <= 1 && (bN + bB) <= 1)
            is_end_game = true;
    }

    return is_end_game;
}
