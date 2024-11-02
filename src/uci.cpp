#include "include/uci.h"
#include "include/board.h"
#include "include/constants.h"
#include "include/fen.h"
#include "include/position.h"
#include "include/search.h"
#include "include/tt.h"
#include "include/utils.h"
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#define PROGRAM_NAME "pixie"
#define VERSION "0.2.0"

static void _uci_parse_go(const std::string&                   command,
                          std::unique_ptr<Position>&           position,
                          std::unique_ptr<TranspositionTable>& table) {
    std::istringstream iss(command);
    std::string        token;

    int movestogo = 30, movetime = -1, time = -1, inc = 0, depth = -1;

    // Skip Go token
    iss >> token;

    while (iss >> token)
    {
        if (token == "infinite")
        {
            // Do nothing
        }
        else if (token == "wtime" && position->get_active_color() == WHITE)
        {
            int value;
            iss >> value;
            time = value;
        }
        else if (token == "btime" && position->get_active_color() == BLACK)
        {
            int value;
            iss >> value;
            time = value;
        }
        else if (token == "winc" && position->get_active_color() == WHITE)
        {
            int value;
            iss >> value;
            inc = value;
        }
        else if (token == "binc" && position->get_active_color() == BLACK)
        {
            int value;
            iss >> value;
            inc = value;
        }
        else if (token == "movestogo")
        {
            int value;
            iss >> value;
            movestogo = value;
        }
        else if (token == "movetime")
        {
            int value;
            iss >> value;
            movetime = value;
        }
        else if (token == "depth")
        {
            int value;
            iss >> value;
            depth = value;
        }
    }

    if (movetime != -1)
    {
        time      = movetime;
        movestogo = 1;
    }

    if (depth == -1)
        depth = 64;

    SearchInfo info;

    info.depth         = (uint8_t) depth;
    info.timeset       = false;
    info.starttime     = utils_get_current_time_in_milliseconds();
    info.stoptime      = 0;
    info.stopped       = false;
    info.nodes         = 0ULL;
    info.use_iterative = true;
    info.use_uci       = true;

    if (time > 0)
    {
        info.timeset = true;
        time /= movestogo;
        time -= 50;
        info.stoptime = info.starttime + time + inc;
    }

    (void) search(position, table, &info);
}

static void _uci_parse_position(const std::string& command, std::unique_ptr<Position>& position) {
    std::istringstream iss(command);
    std::string        token;
    iss >> token;
    iss >> token;

    if (token == "startpos")
        position->set_start_pos();
    else if (token == "fen")
    {
        std::string fen;
        std::string fen_part;
        while (iss >> fen_part)
            fen += fen_part + " ";
        fen = fen.substr(0, fen.size() - 1);
        try
        { fen_to_position(fen, position); } catch (const std::invalid_argument& e)
        { std::cerr << e.what() << std::endl; }
    }
    else
    {
        throw std::invalid_argument("Invalid command!");
    }

    int moves_pos = command.find("moves");
    if (moves_pos != std::string::npos)
    {
        std::string        moves = command.substr(moves_pos + 6);
        std::string        move;
        std::istringstream movesiss(moves);
        while (movesiss >> move)
        {
            try
            {
                const bool is_valid_move = position->move_do(move);
                if (!is_valid_move)
                    throw std::invalid_argument("Invalid move!");
            } catch (const std::invalid_argument& e)
            {
                // Gracefully handle exceptions
                std::cerr << e.what() << std::endl;
            }
        }
    }
}

void uci_loop(void) {
    board_init();
    std::unique_ptr<Position>           position = std::make_unique<Position>();
    std::unique_ptr<TranspositionTable> table    = std::make_unique<TranspositionTable>(64);

    std::string input;

    while (std::getline(std::cin, input))
    {
        if (input == "uci")
        {
            std::cout << "id name " << PROGRAM_NAME << " " << VERSION << std::endl;
            std::cout << "id author the pixie developers (see AUTHORS file)" << std::endl;
            std::cout << "uciok" << std::endl;
        }
        else if (input == "isready")
        {
            std::cout << "readyok" << std::endl;
        }
        else if (input == "ucinewgame")
        {
            position = std::make_unique<Position>();  // TODO: Check if there is a better way
            table->clear();
        }
        else if (input.rfind("position", 0) == 0)
        {
            position = std::make_unique<Position>();  // TODO: Check if there is a better way
            _uci_parse_position(input, position);
            position->reset_ply_count();
        }
        else if (input.rfind("go", 0) == 0)
        {
            _uci_parse_go(input, position, table);
        }
        else if (input == "display")
        {
            position->display();
        }
        else if (input == "quit")
        {
            break;
        }
    }
}
