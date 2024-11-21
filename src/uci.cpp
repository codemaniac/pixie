#include "include/uci.h"
#include "include/fen.h"
#include "include/perft.h"
#include "include/position.h"
#include "include/search.h"
#include "include/tt.h"
#include "include/utils.h"
#include <future>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#define PROGRAM_NAME "pixie"
#define VERSION "0.8.0"

static void uci_parse_setoption(const std::string&                   command,
                                std::unique_ptr<TranspositionTable>& table,
                                std::unique_ptr<ThreadPool>&         pool) {
    std::istringstream iss(command);
    std::string        cmd, name, id, valuename, value;

    iss >> cmd >> name >> id >> valuename >> value;

    if (id == "Hash")
    {
        const int ttsize = std::stoi(value);
        if (ttsize >= 1 && ttsize <= 256)
        {
            table = std::make_unique<TranspositionTable>(ttsize);
        }
    }
    else if (id == "Threads")
    {
        const int threadpool_size = std::stoi(value);
        if (threadpool_size >= 1 && threadpool_size <= 8)
        {
            pool = std::make_unique<ThreadPool>(threadpool_size);
        }
    }
}

static std::future<void> uci_parse_go(const std::string&  command,
                                      Position*           position,
                                      TranspositionTable* table,
                                      ThreadPool*         pool,
                                      SearchInfo*         info) {
    std::istringstream iss(command);
    std::string        token;

    int movestogo = 30, movetime = -1, time = -1, inc = 0, depth = -1;

    // Skip Go token
    iss >> token;

    while (iss >> token)
    {
        if (token == "perft")
        {
            int value;
            iss >> value;
            depth = value;

            std::future<void> f = std::async(std::launch::async, [&position, depth] {
                Position       position_clone = Position(*position);
                const uint64_t starttime      = utils_get_current_time_in_milliseconds();
                (void) divide(position_clone, depth, true);
                const uint64_t stoptime = utils_get_current_time_in_milliseconds();
                const uint64_t time     = stoptime - starttime;
                std::cout << "Execution Time (in ms) = " << (unsigned long long) time << std::endl;
            });

            return f;
        }
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

    info->depth         = depth;
    info->timeset       = false;
    info->starttime     = utils_get_current_time_in_milliseconds();
    info->stoptime      = 0;
    info->stopped       = false;
    info->use_iterative = true;
    info->use_uci       = true;

    if (time > 0)
    {
        info->timeset = true;
        time /= movestogo;
        time -= 50;
        info->stoptime = info->starttime + time + inc;
    }

    std::future<void> f = std::async(std::launch::async, [&position, &table, &pool, &info] {
        Position position_clone = Position(*position);
        (void) search(position_clone, table, pool, info);
    });

    return f;
}

static void uci_parse_position(const std::string& command, Position* position) {
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
    if (moves_pos != (int) std::string::npos)
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
    position_init();
    std::unique_ptr<Position>           position = std::make_unique<Position>();
    std::unique_ptr<TranspositionTable> table    = std::make_unique<TranspositionTable>(16);
    std::unique_ptr<ThreadPool>         pool     = std::make_unique<ThreadPool>(2);
    SearchInfo                          info;
    std::future<void>                   uci_go_future;

    std::string input;

    while (std::getline(std::cin, input))
    {
        if (input == "uci")
        {
            std::cout << "id name " << PROGRAM_NAME << " " << VERSION << std::endl;
            std::cout << "id author the pixie developers (see AUTHORS file)" << std::endl;
            std::cout << "option name Hash type spin default 16 min 1 max 256" << std::endl;
            std::cout << "option name Threads type spin default 2 min 1 max 8" << std::endl;
            std::cout << "uciok" << std::endl;
        }
        else if (input == "isready")
        {
            std::cout << "readyok" << std::endl;
        }
        else if (input == "ucinewgame")
        {
            position = std::make_unique<Position>();
            table->clear();
        }
        else if (input.rfind("position", 0) == 0)
        {
            position = std::make_unique<Position>();
            uci_parse_position(input, position.get());
            position->reset_ply_count();
            table->reset_for_search();
        }
        else if (input.rfind("go", 0) == 0)
        {
            info          = SearchInfo();
            uci_go_future = uci_parse_go(input, position.get(), table.get(), pool.get(), &info);
        }
        else if (input.rfind("setoption", 0) == 0)
        {
            uci_parse_setoption(input, table, pool);
        }
        else if (input == "display")
        {
            position->display();
        }
        else if (input == "stop")
        {
            info.stopped = true;
            if (uci_go_future.valid())
                uci_go_future.wait();
        }
        else if (input == "quit")
        {
            info.stopped = true;
            if (uci_go_future.valid())
                uci_go_future.wait();
            break;
        }
    }
}
