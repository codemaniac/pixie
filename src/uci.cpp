#include "uci.h"

namespace tejas {

    namespace uci {

        UCIHandler::UCIHandler(const Engine& engine) :
            engine(engine) {}

        void UCIHandler::handleUCI() {
            std::stringstream ss;
            ss << "id name " << engine.getName() << " " << engine.getVersion() << "\n";
            ss << "id author the pixie developers (see AUTHORS file)\n";
            ss << "option name Hash type spin default 16 min 1 max 256\n";
            ss << "option name Threads type spin default 1 min 1 max 8\n";
            ss << "uciok";
            std::cout << ss.str() << std::endl;
        }

        void UCIHandler::handleIsReady() { std::cout << "readyok" << std::endl; }

        void UCIHandler::handleUCINewgame() { engine.reset(); }

        void UCIHandler::handleDisplay() { engine.displayBoard(); }

        void UCIHandler::handlePosition(std::string& input) {
            std::stringstream ss(input);
            std::string       segment;
            const std::size_t moves_pos      = input.find("moves");
            bool              is_valid_input = true;

            // Discard command
            ss >> segment;

            ss >> segment;
            if (segment == "startpos")
            {
                engine.setStartpos();
            }
            else if (segment == "fen")
            {
                const std::size_t len =
                  (moves_pos == std::string::npos) ? input.length() : moves_pos;
                segment = input.substr(13, len - 13);
                // clang-format off
                try
                {
                    engine.setPositionFromFEN(segment);
                }
                catch (const std::invalid_argument& e)
                {
                    is_valid_input = false;
                    std::cerr << e.what() << std::endl;
                }
                // clang-format on
            }

            if (is_valid_input && (moves_pos != std::string::npos))
            {
                segment = input.substr(moves_pos + 6);
                std::stringstream movesss(segment);
                std::string       move;
                while (movesss >> move)
                {
                    std::cout << move << std::endl;
                }
            }
        }

        void UCIHandler::start() {
            std::string input;
            while (std::getline(std::cin, input))
            {
                if (input == "uci")
                {
                    handleUCI();
                }
                else if (input == "isready")
                {
                    handleIsReady();
                }
                else if (input == "ucinewgame")
                {
                    handleUCINewgame();
                }
                else if (input.rfind("position", 0) == 0)
                {
                    handlePosition(input);
                }
                else if (input == "d")
                {
                    handleDisplay();
                }
                else if (input == "quit")
                {
                    break;
                }
            }
        }

    }

}
