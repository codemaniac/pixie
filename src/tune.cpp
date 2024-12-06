#include "include/tune.h"
#include "include/constants.h"
#include "include/fen.h"
#include "include/position.h"
#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <math.h>
#include <sstream>
#include <string>
#include <vector>

using tune_t         = double;
using pair_t         = std::array<tune_t, 2>;
using parameters_t   = std::vector<pair_t>;
using coefficients_t = std::vector<int16_t>;

enum Phase {
    MG,
    EG
};

struct EvalResult {
    coefficients_t coefficients;
    tune_t         score;
    tune_t         endgame_scale = 1;
};

struct CoefficientEntry {
    int16_t value;
    int16_t index;
};

struct Entry {
    std::vector<CoefficientEntry> coefficients;
    tune_t                        wdl;
    bool                          white_to_move;
    int32_t                       phase;
    tune_t                        endgame_scale;
};

struct Trace {
    int32_t material[6][2]{};
};

static constexpr int32_t S(const int32_t mg, const int32_t eg) {
    return static_cast<int32_t>(static_cast<uint32_t>(eg) << 16) + mg;
}

static constexpr int32_t mg_score(const int32_t score) { return score & 0xFFFF; }

static constexpr int32_t eg_score(const int32_t score) { return score >> 16; }

constexpr double sigmoid(const double K, const double E) {
    return 1.0 / (1.0 + exp(-K * E / 400.0));
}

void get_initial_parameter_single(parameters_t* parameters, const int32_t parameter) {
    const auto   mg   = mg_score(parameter);
    const auto   eg   = eg_score(parameter);
    const pair_t pair = {(double) mg, (double) eg};
    parameters->push_back(pair);
}

template<size_t N>
void get_initial_parameter_array(parameters_t*                 parameters,
                                 const std::array<int32_t, N>& parameter) {
    for (int i = 0; i < (int) parameter.size(); i++)
    {
        get_initial_parameter_single(parameters, parameter[i]);
    }
}

static tune_t get_fen_wdl(const std::string& fen, const Color active_color) {
    tune_t wdl = 0.5;

    if (fen.find("[1-0]") != (size_t) std::string::npos)
        wdl = 1.0 - static_cast<int>(active_color);
    else if (fen.find("[0-1]") != (size_t) std::string::npos)
        wdl = static_cast<int>(active_color);

    return wdl;
}

static void get_coefficient_entries(std::vector<CoefficientEntry>* coefficient_entries,
                                    const coefficients_t&          coefficients,
                                    const size_t                   parameter_count) {
    if (coefficients.size() != parameter_count)
    {
        throw std::runtime_error("Parameter count mismatch");
    }

    for (int16_t i = 0; i < (int16_t) coefficients.size(); i++)
    {
        if (coefficients[i] == 0)
            continue;

        const auto coefficient_entry = CoefficientEntry{coefficients[i], i};
        coefficient_entries->push_back(coefficient_entry);
    }
}

static tune_t linear_eval(const Entry& entry, const parameters_t& parameters) {
    tune_t score   = 0;
    tune_t midgame = 0;
    tune_t endgame = 0;
    for (const auto& coefficient : entry.coefficients)
    {
        midgame += coefficient.value * parameters[coefficient.index][MG];
        endgame += coefficient.value * parameters[coefficient.index][EG] * entry.endgame_scale;
    }
    score += (midgame * entry.phase + endgame * (24 - entry.phase)) / 24;

    return score;
}

static int get_phase(const Position& position) {
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

    const int phase = 24 - (4 * q) - (2 * r) - (1 * b) - (1 * n);

    return phase;
}

static tune_t get_average_error(const std::vector<Entry>& entries,
                                const parameters_t&       parameters,
                                const tune_t              K) {
    tune_t error = 0;
    for (const auto& entry : entries)
    {
        const auto eval        = linear_eval(entry, parameters);
        const auto sig         = sigmoid(K, eval);
        const auto diff        = entry.wdl - sig;
        const auto entry_error = pow(diff, 2);
        error += entry_error;
    }
    const tune_t avg_error = error / static_cast<tune_t>(entries.size());
    return avg_error;
}

static void update_single_gradient(parameters_t&       gradient,
                                   const Entry&        entry,
                                   const parameters_t& params,
                                   const tune_t        K) {
    const tune_t eval = linear_eval(entry, params);
    const tune_t sig  = sigmoid(K, eval);
    const tune_t res  = (entry.wdl - sig) * sig * (1 - sig);

    const auto mg_base = res * (entry.phase / static_cast<tune_t>(24));
    const auto eg_base = res - mg_base;

    for (const auto& coefficient : entry.coefficients)
    {
        gradient[coefficient.index][MG] += mg_base * coefficient.value;
        gradient[coefficient.index][EG] += eg_base * coefficient.value * entry.endgame_scale;
    }
}

static void compute_gradient(parameters_t&             gradient,
                             const std::vector<Entry>& entries,
                             const parameters_t&       params,
                             tune_t                    K) {
    parameters_t local = parameters_t(params.size(), pair_t{});
    for (const auto& entry : entries)
        update_single_gradient(local, entry, params, K);

    for (auto parameter_index = 0; parameter_index < (int) params.size(); parameter_index++)
    {
        gradient[parameter_index][MG] += local[parameter_index][MG];
        gradient[parameter_index][EG] += local[parameter_index][EG];
    }
}

// clang-format off
constexpr std::array<int32_t, 6> material = {
    S(100, 100),
    S(320, 320),
    S(330, 330),
    S(500, 500),
    S(900, 900)
};
// clang-format on

static parameters_t get_initial_parameters() {
    parameters_t parameters;
    get_initial_parameter_array<6>(&parameters, material);
    return parameters;
}

static coefficients_t get_coefficients(const Trace& trace) {
    coefficients_t coefficients;

    for (const auto& m : trace.material)
        coefficients.push_back(static_cast<int16_t>(m[WHITE] - m[BLACK]));

    return coefficients;
}

static EvalResult get_fen_eval_result(const Position& position) {
    Trace trace;

    for (uint8_t sq = 0; sq < 64; sq++)
    {
        const Piece piece = position.get_piece(static_cast<Square>(sq));

        if (piece == NO_PIECE)
            continue;

        if (PIECE_GET_COLOR(piece) == WHITE)
        {
            trace.material[PIECE_GET_TYPE(piece) - 1][WHITE]++;
        }
        else
        {
            trace.material[PIECE_GET_TYPE(piece) - 1][BLACK]++;
        }
    }

    EvalResult result;
    result.coefficients = get_coefficients(trace);
    result.score        = 0;

    return result;
}

static void get_initial_entries(std::vector<Entry>*       entries,
                                std::vector<std::string>& data,
                                const size_t              parameter_count) {
    for (const auto& fen : data)
    {
        Position position;
        fen_to_position(fen, &position);

        if (position.is_in_check())
            continue;

        Entry entry;
        entry.wdl           = get_fen_wdl(fen, position.get_active_color());
        entry.white_to_move = (position.get_active_color() == WHITE);
        entry.phase         = get_phase(position);

        EvalResult eval_result;
        eval_result         = get_fen_eval_result(position);
        entry.endgame_scale = eval_result.endgame_scale;
        get_coefficient_entries(&entry.coefficients, eval_result.coefficients, parameter_count);

        entries->push_back(entry);
    }
}

static void print_parameter(std::stringstream& ss, const pair_t parameter) {
    ss << "S(" << parameter[MG] << ", " << parameter[EG] << ")";
}

static void print_array(std::stringstream&  ss,
                        const parameters_t& parameters,
                        int&                index,
                        const std::string&  name,
                        int                 count) {
    ss << "constexpr int " << name << "[] = {";
    for (auto i = 0; i < count; i++)
    {
        print_parameter(ss, parameters[index]);
        index++;

        if (i != count - 1)
        {
            ss << ", ";
        }
    }
    ss << "};" << std::endl;
}

void print_parameters(const parameters_t& parameters) {
    int               index = 0;
    std::stringstream ss;
    print_array(ss, parameters, index, "material", 6);
    std::cout << ss.str() << std::endl;
}

void tune() {
    std::vector<std::string> data;
    std::filesystem::path    filepath = std::filesystem::current_path() / "data" / "data.epd";
    filepath                          = std::filesystem::canonical(filepath);
    std::ifstream file(filepath);

    for (std::string line; std::getline(file, line);)
    {
        data.push_back(line);
    }

    std::cout << "Starting tuning" << std::endl;

    auto parameters = get_initial_parameters();

    std::vector<Entry> entries;
    get_initial_entries(&entries, data, parameters.size());

    tune_t K = 2.5;

    const auto avg_error = get_average_error(entries, parameters, K);
    std::cout << "Initial error = " << avg_error << std::endl;

    tune_t  learning_rate  = 1;
    int32_t max_tune_epoch = 500;

    parameters_t momentum(parameters.size(), pair_t{});
    parameters_t velocity(parameters.size(), pair_t{});

    for (int32_t epoch = 1; epoch < max_tune_epoch; epoch++)
    {
        parameters_t gradient(parameters.size(), pair_t{});
        compute_gradient(gradient, entries, parameters, K);

        constexpr tune_t beta1 = 0.9;
        constexpr tune_t beta2 = 0.999;

        for (int parameter_index = 0; parameter_index < (int) parameters.size(); parameter_index++)
        {
            const tune_t mg_grad =
              (-K / 200.0) * gradient[parameter_index][MG] / static_cast<tune_t>(entries.size());
            const tune_t eg_grad =
              (-K / 200.0) * gradient[parameter_index][EG] / static_cast<tune_t>(entries.size());

            momentum[parameter_index][MG] =
              beta1 * momentum[parameter_index][MG] + (1.0 - beta1) * mg_grad;
            momentum[parameter_index][EG] =
              beta1 * momentum[parameter_index][EG] + (1.0 - beta1) * eg_grad;

            velocity[parameter_index][MG] =
              beta2 * velocity[parameter_index][MG] + (1 - beta2) * pow(mg_grad, 2);
            velocity[parameter_index][EG] =
              beta2 * velocity[parameter_index][EG] + (1 - beta2) * pow(eg_grad, 2);

            parameters[parameter_index][MG] -=
              learning_rate * momentum[parameter_index][MG]
              / (static_cast<tune_t>(1e-8) + sqrt(velocity[parameter_index][MG]));
            parameters[parameter_index][EG] -=
              learning_rate * momentum[parameter_index][EG]
              / (static_cast<tune_t>(1e-8) + sqrt(velocity[parameter_index][EG]));
        }

        if (epoch % 100 == 0)
        {
            const tune_t error = get_average_error(entries, parameters, K);
            std::cout << "Epoch = " << epoch << " Error = " << error << std::endl;
        }
    }

    print_parameters(parameters);
}
