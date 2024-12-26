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

#define N_PARAMS 6

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
constexpr std::array<int32_t, 6> material = {
    S(100, 100),
    S(320, 320),
    S(330, 330),
    S(500, 500),
    S(900, 900),
    S(0, 0)
};
// clang-format on

static double get_fen_wdl(const std::string& fen, const Color active_color) {
    double wdl = 0.5;

    if (fen.find("[1-0]") != (size_t) std::string::npos)
        wdl = 1.0 - static_cast<int>(active_color);
    else if (fen.find("[0-1]") != (size_t) std::string::npos)
        wdl = static_cast<int>(active_color);

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

static void init_entry_coefficients(Entry* entry, const Position& position) {
    int param_index = 0;

    entry->coefficients[param_index++] =
      position.get_piece_count(WHITE_PAWN) - position.get_piece_count(BLACK_PAWN);
    entry->coefficients[param_index++] =
      position.get_piece_count(WHITE_KNIGHT) - position.get_piece_count(BLACK_KNIGHT);
    entry->coefficients[param_index++] =
      position.get_piece_count(WHITE_BISHOP) - position.get_piece_count(BLACK_BISHOP);
    entry->coefficients[param_index++] =
      position.get_piece_count(WHITE_ROOK) - position.get_piece_count(BLACK_ROOK);
    entry->coefficients[param_index++] =
      position.get_piece_count(WHITE_QUEEN) - position.get_piece_count(BLACK_QUEEN);
    entry->coefficients[param_index++] =
      position.get_piece_count(WHITE_KING) - position.get_piece_count(BLACK_KING);
}

static void init_parameters(ArrayList<ParameterTuple, N_PARAMS>* parameters) {

    for (const auto& p : material)
    {
        double p_mg = mg_score(p);
        double p_eg = eg_score(p);

        ParameterTuple tuple;
        tuple[MG] = p_mg;
        tuple[EG] = p_eg;

        parameters->push(tuple);
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
        entry.wdl          = get_fen_wdl(fen, position.get_active_color());
        const double phase = get_phase(position);
        entry.phase        = phase;
        init_entry_coefficients(&entry, position);

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

    const double rate           = 10;
    const double delta          = 1e-5;
    const double deviation_goal = 1e-6;
    double       K              = 2.5;
    double       deviation      = 1;

    while (fabs(deviation) > deviation_goal)
    {
        const double up   = get_average_error(entries, parameters, K + delta);
        const double down = get_average_error(entries, parameters, K - delta);
        deviation         = (up - down) / (2 * delta);
        std::cout << "Current K: " << K << ", up: " << up << ", down: " << down
                  << ", deviation: " << deviation << std::endl;
        K -= deviation * rate;
    }

    return K;
}

static void print_parameters(const ArrayList<ParameterTuple, N_PARAMS>& parameters) {
    for (const ParameterTuple& p : parameters)
    {
        std::cout << p[MG] << "\t" << p[EG] << std::endl;
    }
}

void tune() {
    std::cout << "Starting tuning" << std::endl;

    std::vector<std::string> data;
    std::filesystem::path    filepath = std::filesystem::current_path() / "data" / "data.epd";
    filepath                          = std::filesystem::canonical(filepath);
    std::ifstream file(filepath);

    for (std::string line; std::getline(file, line);)
    {
        data.push_back(line);
    }

    ArrayList<ParameterTuple, N_PARAMS> parameters;
    std::vector<Entry>                  entries;

    init_parameters(&parameters);
    init_entries(&entries, data);

    const double K = find_optimal_k(entries, parameters);

    const double avg_error = get_average_error(entries, parameters, K);

    std::cout << "Initial error = " << avg_error << std::endl;

    double  learning_rate  = 0.01;
    int32_t max_tune_epoch = 1000;

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

        if (epoch % 10 == 0)
        {
            const double error = get_average_error(entries, parameters, K);
            std::cout << "Error = " << error << std::endl;
        }
    }

    print_parameters(parameters);
}
