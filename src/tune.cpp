#include "include/tune.h"
#include "include/constants.h"
#include "include/containers.h"
#include "include/fen.h"
#include "include/position.h"
#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <math.h>
#include <sstream>
#include <string>
#include <vector>

#define N_PARAMS 390
#define KPRECISION 10

using Parameter      = double;
using ParameterTuple = std::array<Parameter, 2>;

enum Phase {
    MG,
    EG
};

struct Entry {
    ArrayList<double, N_PARAMS> coefficients;
    double                      wdl, phase;
};

struct EvalTrace {
    int32_t pieces[6][2];
    int32_t psqt[6][64][2];
};

constexpr int32_t S(const int32_t mg, const int32_t eg) {
    return static_cast<int32_t>(static_cast<uint32_t>(eg) << 16) + mg;
}

static constexpr int32_t mg_score(const int32_t score) { return static_cast<int16_t>(score); }

static constexpr int32_t eg_score(const int32_t score) {
    return static_cast<int16_t>((score + 0x8000) >> 16);
}

constexpr double sigmoid(const double K, const double E) {
    return 1.0 / (1.0 + exp(-K * E / 400.0));
}

// clang-format off
static const int SQUARES_MIRRORED[64] = {
    A8, B8, C8, D8, E8, F7, G8, H8,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A1, B1, C1, D1, E1, F1, G1, H1,
};

constexpr std::array<int32_t, 6> pieces = {
    S(100, 100),
    S(320, 320),
    S(330, 330),
    S(500, 500),
    S(900, 900),
    S(0, 0)
};

static const int32_t psqt[6][64] = {
    {
        S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),
        S(50, 50),   S(50, 50),   S(50, 50),   S(50, 50),   S(50, 50),   S(50, 50),   S(50, 50),   S(50, 50),
        S(10, 10),   S(10, 10),   S(20, 20),   S(30, 30),   S(30, 30),   S(20, 20),   S(10, 10),   S(10, 10),
        S(5, 5),     S(5, 5),     S(10, 10),   S(25, 25),   S(25, 25),   S(10, 10),   S(5, 5),     S(5, 5),
        S(0, 0),     S(0, 0),     S(0, 0),     S(20, 20),   S(20, 20),   S(0, 0),     S(0, 0),     S(0, 0),
        S(5, 5),     S(-5, -5),   S(-10, -10), S(0, 0),     S(0, 0),     S(-10, -10), S(-5, -5),   S(5, 5),
        S(5, 5),     S(10, 10),   S(10, 10),   S(-20, -20), S(-20, -20), S(10, 10),   S(10, 10),   S(5, 5),
        S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0)
    }, {
        S(-50, -50), S(-40, -40), S(-30, -30), S(-30, -30), S(-30, -30), S(-30, -30), S(-40, -40), S(-50, -50),
        S(-40, -40), S(-20, -20), S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),     S(-20, -20), S(-40, -40),
        S(-30, -30), S(0, 0),     S(10, 10),   S(15, 15),   S(15, 15),   S(10, 10),   S(0, 0),     S(-30, -30),
        S(-30, -30), S(5, 5),     S(15, 15),   S(20, 20),   S(20, 20),   S(15, 15),   S(5, 5),     S(-30, -30),
        S(-30, -30), S(0, 0),     S(15, 15),   S(20, 20),   S(20, 20),   S(15, 15),   S(0, 0),     S(-30, -30),
        S(-30, -30), S(5, 5),     S(10, 10),   S(15, 15),   S(15, 15),   S(10, 10),   S(5, 5),     S(-30, -30),
        S(-40, -40), S(-20, -20), S(0, 0),     S(5, 5),     S(5, 5),     S(0, 0),     S(-20, -20), S(-40, -40),
        S(-50, -50), S(-40, -40), S(-30, -30), S(-30, -30), S(-30, -30), S(-30, -30), S(-40, -40), S(-50, -50)
    }, {
        S(-20, -20), S(-10, -10), S(-10, -10), S(-10, -10), S(-10, -10), S(-10, -10), S(-10, -10), S(-20, -20),
        S(-10, -10), S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),     S(-10, -10),
        S(-10, -10), S(0, 0),     S(5, 5),     S(10, 10),   S(10, 10),   S(5, 5),     S(0, 0),     S(-10, -10),
        S(-10, -10), S(5, 5),     S(5, 5),     S(10, 10),   S(10, 10),   S(5, 5),     S(5, 5),     S(-10, -10),
        S(-10, -10), S(0, 0),     S(10, 10),   S(10, 10),   S(10, 10),   S(10, 10),   S(0, 0),     S(-10, -10),
        S(-10, -10), S(10, 10),   S(10, 10),   S(10, 10),   S(10, 10),   S(10, 10),   S(10, 10),   S(-10, -10),
        S(-10, -10), S(5, 5),     S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),     S(5, 5),     S(-10, -10),
        S(-20, -20), S(-10, -10), S(-10, -10), S(-10, -10), S(-10, -10), S(-10, -10), S(-10, -10), S(-20, -20)
    }, {
        S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),
        S(5, 5),     S(10, 10),   S(10, 10),   S(10, 10),   S(10, 10),   S(10, 10),   S(10, 10),   S(5, 5),
        S(-5, -5),   S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),     S(-5, -5),
        S(-5, -5),   S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),     S(-5, -5),
        S(-5, -5),   S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),     S(-5, -5),
        S(-5, -5),   S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),     S(-5, -5),
        S(-5, -5),   S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),     S(-5, -5),
        S(0, 0),     S(0, 0),     S(0, 0),     S(5, 5),     S(5, 5),     S(0, 0),     S(0, 0),     S(0, 0)
    }, {
        S(-20, -20), S(-10, -10), S(-10, -10), S(-5, -5),   S(-5, -5),   S(-10, -10), S(-10, -10), S(-20, -20),
        S(-10, -10), S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),     S(-10, -10),
        S(-10, -10), S(0, 0),     S(5, 5),     S(5, 5),     S(5, 5),     S(5, 5),     S(0, 0),     S(-10, -10),
        S(-5, -5),   S(0, 0),     S(5, 5),     S(5, 5),     S(5, 5),     S(5, 5),     S(0, 0),     S(-5, -5),
        S(0, 0),     S(0, 0),     S(5, 5),     S(5, 5),     S(5, 5),     S(5, 5),     S(0, 0),     S(-5, -5),
        S(-10, -10), S(5, 5),     S(5, 5),     S(5, 5),     S(5, 5),     S(5, 5),     S(0, 0),     S(-10, -10),
        S(-10, -10), S(0, 0),     S(5, 5),     S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),     S(-10, -10),
        S(-20, -20), S(-10, -10), S(-10, -10), S(-5, -5),   S(-5, -5),   S(-10, -10), S(-10, -10), S(-20, -20)
    }, {
        S(-30, -50), S(-40, -40), S(-40, -30), S(-50, -20), S(-50, -20), S(-40, -30), S(-40, -40), S(-30, -50),
        S(-30, -30), S(-40, -20), S(-40, -10), S(-50, 0),   S(-50, 0),   S(-40, -10), S(-40, -20), S(-30, -30),
        S(-30, -30), S(-40, -10), S(-40, 20),  S(-50, 30),  S(-50, 30),  S(-40, 20),  S(-40, -10), S(-30, -30),
        S(-30, -30), S(-40, -10), S(-40, 30),  S(-50, 40),  S(-50, 40),  S(-40, 30),  S(-40, -10), S(-30, -30),
        S(-20, -30), S(-30, -10), S(-30, 30),  S(-40, 40),  S(-40, 40),  S(-30, 30),  S(-30, -10), S(-20, -30),
        S(-10, -30), S(-20, -10), S(-20, 20),  S(-20, 30),  S(-20, 30),  S(-20, 20),  S(-20, -10), S(-10, -30),
        S(20, -30),  S(20, -30),  S(0, 0),     S(0, 0),     S(0, 0),     S(0, 0),     S(20, -30),  S(20, -30),
        S(20, -50),  S(30, -30),  S(10, -30),  S(0, -30),   S(0, -30),   S(10, -30),  S(30, -30),  S(20, -50)
    }
};
// clang-format on

static double get_fen_wdl(const std::string& fen) {
    double wdl = 0.5;

    if (fen.find("1-0") != (size_t) std::string::npos)
        wdl = 1.0;
    else if (fen.find("0-1") != (size_t) std::string::npos)
        wdl = 0.0;

    return wdl;
}

static double get_phase(const Position& position) {
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

    double phase = 24 - (4 * q) - (2 * r) - (1 * b) - (1 * n);

    return phase;
}

static void init_eval_trace(EvalTrace* trace, const Position& position) {
    for (int sq = A1; sq <= H8; sq++)
    {
        const Piece p = position.get_piece(static_cast<Square>(sq));
        if (p == NO_PIECE)
        {
            continue;
        }
        const PieceType ptype  = PIECE_GET_TYPE(p);
        const Color     pcolor = PIECE_GET_COLOR(p);
        trace->pieces[ptype - 1][pcolor]++;
        if (pcolor == WHITE)
        {
            trace->psqt[ptype - 1][sq][pcolor]++;
        }
        else
        {
            trace->psqt[ptype - 1][SQUARES_MIRRORED[sq]][pcolor]++;
        }
    }
}

static void init_entry_coefficients(Entry* entry, const EvalTrace& trace) {
    for (int32_t p = 0; p < 6; p++)
    {
        entry->coefficients.push(trace.pieces[p][WHITE] - trace.pieces[p][BLACK]);
    }

    for (int32_t p = 0; p < 6; p++)
    {
        for (int32_t sq = A1; sq <= H8; sq++)
        {
            entry->coefficients.push(trace.psqt[p][sq][WHITE] - trace.psqt[p][sq][BLACK]);
        }
    }
}

static void init_parameters(ArrayList<ParameterTuple, N_PARAMS>* parameters) {

    for (const auto& p : pieces)
    {
        double p_mg = mg_score(p);
        double p_eg = eg_score(p);

        ParameterTuple tuple;
        tuple[MG] = p_mg;
        tuple[EG] = p_eg;

        parameters->push(tuple);
    }

    for (int p = 0; p < 6; p++)
    {
        for (int sq = A1; sq <= H8; sq++)
        {
            double psqt_mg = mg_score(psqt[p][SQUARES_MIRRORED[sq]]);
            double psqt_eg = eg_score(psqt[p][SQUARES_MIRRORED[sq]]);

            ParameterTuple tuple;
            tuple[MG] = psqt_mg;
            tuple[EG] = psqt_eg;

            parameters->push(tuple);
        }
    }
}

static void init_entries(std::vector<Entry>* entries, std::vector<std::string>& data) {

    for (const auto& fen : data)
    {
        Position position;
        fen_to_position(fen, &position, false);

        if (position.is_in_check())
            continue;

        Entry entry;
        entry.wdl          = get_fen_wdl(fen);
        const double phase = get_phase(position);
        entry.phase        = phase;
        EvalTrace trace    = {};
        init_eval_trace(&trace, position);
        init_entry_coefficients(&entry, trace);

        entries->push_back(entry);
    }
}

static double linear_eval(const Entry&                               entry,
                          const ArrayList<ParameterTuple, N_PARAMS>& parameters) {
    double score   = 0;
    double midgame = 0;
    double endgame = 0;

    for (int i = 0; i < N_PARAMS; i++)
    {
        midgame += entry.coefficients[i] * parameters[i][MG];
        endgame += entry.coefficients[i] * parameters[i][EG];
    }

    // Linear interpolation for Tapered Evaluation
    score = (midgame * entry.phase + endgame * (24 - entry.phase)) / 24;

    return score;
}

static double get_average_error(const std::vector<Entry>&                  entries,
                                const ArrayList<ParameterTuple, N_PARAMS>& parameters,
                                const double                               K) {
    double total_error = 0;
    for (const Entry& entry : entries)
    {
        const double eval        = linear_eval(entry, parameters);
        const double sig         = sigmoid(K, eval);
        const double diff        = entry.wdl - sig;
        const double entry_error = pow(diff, 2);
        total_error += entry_error;
    }
    const double avg_error = total_error / static_cast<double>(entries.size());
    return avg_error;
}

static void update_single_gradient(ArrayList<std::array<double, 2>, N_PARAMS>* gradient,
                                   const Entry&                                entry,
                                   const ArrayList<ParameterTuple, N_PARAMS>&  parameters,
                                   const double                                K) {
    const double eval = linear_eval(entry, parameters);
    const double sig  = sigmoid(K, eval);
    const double res  = (entry.wdl - sig) * sig * (1 - sig);

    const double mg_base = res * (entry.phase / 24.0);
    const double eg_base = res - mg_base;

    for (int i = 0; i < N_PARAMS; i++)
    {
        gradient->at(i)[MG] += mg_base * entry.coefficients.at(i);
        gradient->at(i)[EG] += eg_base * entry.coefficients.at(i);
    }
}

static void compute_gradient(ArrayList<std::array<double, 2>, N_PARAMS>* gradient,
                             const std::vector<Entry>&                   entries,
                             const ArrayList<ParameterTuple, N_PARAMS>&  parameters,
                             const double                                K) {
    for (const Entry& entry : entries)
    {
        update_single_gradient(gradient, entry, parameters, K);
    }
}

static double find_optimal_k(const std::vector<Entry>&                  entries,
                             const ArrayList<ParameterTuple, N_PARAMS>& parameters) {

    double start = 0.0, end = 10, step = 1.0;
    double curr = start, error;
    double best = get_average_error(entries, parameters, start);

    for (int i = 0; i < KPRECISION; i++)
    {
        curr = start - step;
        while (curr < end)
        {
            curr  = curr + step;
            error = get_average_error(entries, parameters, curr);
            if (error <= best)
                best = error, start = curr;
        }
        end   = start + step;
        start = start - step;
        step  = step / 10.0;
    }
    return start;
}

constexpr int32_t round_value(const Parameter value) { return static_cast<int32_t>(round(value)); }

static void print_parameter(std::stringstream& ss, const ParameterTuple parameter) {
    const auto mg = round_value(parameter[MG]);
    const auto eg = round_value(parameter[EG]);
    ss << "S(" << mg << ", " << eg << ")";
}

static void print_array(std::stringstream&                         ss,
                        const ArrayList<ParameterTuple, N_PARAMS>& parameters,
                        const int                                  start,
                        const int                                  count,
                        const std::string&                         name) {
    ss << "static const int32_t " << name << "[] = {";
    for (int i = start; i < start + count; i++)
    {
        print_parameter(ss, parameters[i]);

        if (i != count - 1)
        {
            ss << ", ";
        }
    }
    ss << "};" << std::endl;
}

static void print_psqt(std::stringstream&                         ss,
                       const ArrayList<ParameterTuple, N_PARAMS>& parameters,
                       const int                                  start) {
    ss << "static const int32_t psqt[6][64] = {";
    int ptr = start;
    for (int p = 0; p < 6; p++)
    {
        ParameterTuple psqr_params[64];
        for (int sq = A1; sq <= H8; sq++)
        {
            psqr_params[sq] = parameters[ptr++];
        }

        ss << "{";
        for (int sq = A1; sq <= H8; sq++)
        {
            print_parameter(ss, psqr_params[SQUARES_MIRRORED[sq]]);
            if (sq != H8)
            {
                ss << ", ";
            }
            if (sq % 8 == 7)
            {
                ss << std::endl;
            }
        }
        ss << "}";
        if (p != 5)
        {
            ss << ",";
        }
    }
    ss << "};" << std::endl;
}

static void print_parameters(const ArrayList<ParameterTuple, N_PARAMS>& parameters) {
    std::stringstream ss1;
    print_array(ss1, parameters, 0, 6, "pieces");
    std::cout << ss1.str() << std::endl;

    std::stringstream ss2;
    print_psqt(ss2, parameters, 6);
    std::cout << ss2.str() << std::endl;
}

void tune(const std::string path) {
    std::cout << "Starting tuning" << std::endl;

    std::filesystem::path filepath = std::filesystem::canonical(path);
    std::ifstream         file(filepath);

    std::vector<std::string> data;

    for (std::string line; std::getline(file, line);)
    {
        data.push_back(line);
    }

    ArrayList<ParameterTuple, N_PARAMS> parameters;
    std::vector<Entry>                  entries;

    init_parameters(&parameters);
    init_entries(&entries, data);

    const double K = find_optimal_k(entries, parameters);

    std::cout << "Using K = " << K << std::endl;

    const double avg_error = get_average_error(entries, parameters, K);

    std::cout << "Initial error = " << avg_error << std::endl;

    double        learning_rate               = 0.1;
    const int32_t max_tune_epoch              = 1000;
    const int32_t learning_rate_drop_interval = 100;
    const double  learning_rate_drop_ratio    = 0.5;

    const double beta1 = 0.9;
    const double beta2 = 0.999;

    ArrayList<std::array<double, 2>, N_PARAMS> momentum;
    ArrayList<std::array<double, 2>, N_PARAMS> velocity;

    for (int32_t epoch = 1; epoch < max_tune_epoch; epoch++)
    {
        ArrayList<std::array<double, 2>, N_PARAMS> gradient;

        compute_gradient(&gradient, entries, parameters, K);

        for (int parameter_index = 0; parameter_index < N_PARAMS; parameter_index++)
        {
            for (int phase_stage = 0; phase_stage < 2; phase_stage++)
            {
                const double grad =
                  -K / 400.0 * gradient[parameter_index][phase_stage] / entries.size();
                momentum[parameter_index][phase_stage] =
                  beta1 * momentum[parameter_index][phase_stage] + (1 - beta1) * grad;
                velocity[parameter_index][phase_stage] =
                  beta2 * velocity[parameter_index][phase_stage] + (1 - beta2) * pow(grad, 2);
                parameters[parameter_index][phase_stage] -=
                  learning_rate * momentum[parameter_index][phase_stage]
                  / (static_cast<double>(1e-8) + sqrt(velocity[parameter_index][phase_stage]));
            }
        }

        if (epoch % 100 == 0)
        {
            const double error = get_average_error(entries, parameters, K);
            std::cout << "Error = " << error << std::endl;
        }

        if (epoch % learning_rate_drop_interval == 0)
        {
            learning_rate *= learning_rate_drop_ratio;
        }
    }

    print_parameters(parameters);
}
