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
    ArrayList<int32_t, N_PARAMS> coefficients[2];  // 0 = WHITE, 1 = BLACK
    double                       wdl, phase, pfactors[2];
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

    entry->coefficients[WHITE][param_index]   = position.get_piece_count(WHITE_PAWN);
    entry->coefficients[BLACK][param_index++] = position.get_piece_count(BLACK_PAWN);

    entry->coefficients[WHITE][param_index]   = position.get_piece_count(WHITE_KNIGHT);
    entry->coefficients[BLACK][param_index++] = position.get_piece_count(BLACK_KNIGHT);

    entry->coefficients[WHITE][param_index]   = position.get_piece_count(WHITE_BISHOP);
    entry->coefficients[BLACK][param_index++] = position.get_piece_count(BLACK_BISHOP);

    entry->coefficients[WHITE][param_index]   = position.get_piece_count(WHITE_ROOK);
    entry->coefficients[BLACK][param_index++] = position.get_piece_count(BLACK_ROOK);

    entry->coefficients[WHITE][param_index]   = position.get_piece_count(WHITE_QUEEN);
    entry->coefficients[BLACK][param_index++] = position.get_piece_count(BLACK_QUEEN);

    entry->coefficients[WHITE][param_index]   = position.get_piece_count(WHITE_KING);
    entry->coefficients[BLACK][param_index++] = position.get_piece_count(BLACK_KING);
}

static void init_parameters(ArrayList<ParameterTuple, N_PARAMS>* parameters) {

    for (auto p : material)
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
        fen_to_position(fen, &position);

        if (position.is_in_check())
            continue;

        Entry entry;
        entry.wdl          = get_fen_wdl(fen, position.get_active_color());
        const double phase = get_phase(position);
        entry.phase        = phase;
        entry.pfactors[MG] = 1 - phase / 24.0;
        entry.pfactors[EG] = 0 + phase / 24.0;
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
        midgame += (entry.coefficients[WHITE][i] * parameters[i][MG]);
        midgame -= (entry.coefficients[BLACK][i] * parameters[i][MG]);

        endgame += (entry.coefficients[WHITE][i] * parameters[i][EG]);
        endgame -= (entry.coefficients[BLACK][i] * parameters[i][EG]);
    }

    // Linear interpolation for Tapered Evaluation
    score = ((midgame * (256 - entry.phase)) + (endgame * entry.phase)) / 256;

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

void tune() {
    std::cout << "Tuning" << std::endl;

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

    const double K = 2.5;

    const double avg_error = get_average_error(entries, parameters, K);

    std::cout << "Avg Error = " << avg_error << std::endl;
}
