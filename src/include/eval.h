#ifndef EVAL_H
#define EVAL_H

#include "containers.h"
#include "position.h"
#include <cstdint>

#define N_PARAMS 102

using Parameter      = double;
using ParameterTuple = std::array<Parameter, 2>;

enum Phase {
    MG,
    EG
};

struct EvalTrace {
    int32_t pieces[6][2]{};
    int32_t pst_rank[48][2]{};
    int32_t pst_file[48][2]{};
};

constexpr int32_t S(const int32_t mg, const int32_t eg) {
    return static_cast<int32_t>(static_cast<uint32_t>(eg) << 16) + mg;
}

constexpr int32_t mg_score(const int32_t score) { return static_cast<int16_t>(score); }

constexpr int32_t eg_score(const int32_t score) {
    return static_cast<int16_t>((score + 0x8000) >> 16);
}

// clang-format off
static const int32_t pieces_scores[] = {
    S(293, 110),
    S(310, 419),
    S(327, 443),
    S(607, 546),
    S(1118, 1139),
    S(0, 0)
};

static const int32_t pst_rank_scores[] = {
    S(0, 0), S(-105, -32), S(-112, -31), S(-111, -22), S(-97, -12), S(-10, 1), S(114, 92), S(0, 0),
    S(-25, -14), S(0, -5), S(14, 10), S(31, 19), S(46, 29), S(13, 67), S(4, 22), S(3, -121),
    S(-3, -8), S(9, 17), S(22, 34), S(37, 30), S(36, 29), S(12, 46), S(12, 12), S(9, -37),
    S(15, 10), S(13, -14), S(13, -6), S(13, 0), S(22, 21), S(15, 60), S(18, 90), S(21, 89),
    S(14, 22), S(35, 33), S(70, 22), S(99, 15), S(118, 5), S(80, 50), S(106, 6), S(82, 41),
    S(-27, 18), S(15, -18), S(29, -40), S(37, -107), S(45, -38), S(24, 106), S(18, 47), S(-21, 62)
};

static const int32_t pst_file_scores[] = {
    S(-39, -45), S(-50, -6), S(-65, -5), S(-73, 11), S(-74, 17), S(-83, 34), S(-69, 43), S(-65, -25),
    S(-31, -18), S(-12, 8), S(20, 15), S(23, 26), S(22, 27), S(7, 33), S(3, 21), S(-28, -2),
    S(2, -9), S(3, 19), S(13, 17), S(16, 14), S(21, 18), S(6, 23), S(11, 23), S(4, -5),
    S(15, -8), S(20, 4), S(21, 19), S(18, 43), S(12, 36), S(10, 27), S(21, -11), S(8, -3),
    S(42, 24), S(65, 23), S(81, 30), S(86, 30), S(114, 25), S(99, 35), S(85, 34), S(91, 46),
    S(-24, -14), S(-8, 22), S(17, -4), S(32, -74), S(16, -10), S(27, -41), S(4, 29), S(-17, 23)
};
// clang-format on

void init_eval();

void init_eval_trace(EvalTrace* trace, const Position& position);

void init_coefficients(ArrayList<double, N_PARAMS>* coefficients, const EvalTrace& trace);

int32_t get_phase(const Position& position);

int32_t eval_position(Position& position);

bool eval_is_end_game(Position& position);

#endif
