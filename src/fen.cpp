#include "fen.h"
#include "types.h"
#include <sstream>

namespace tejas {

    namespace fen {

        void parseFEN(board::Board* board, std::string fen, const bool full) {
            std::string        segment;
            std::istringstream ss(fen);

            // Reset board
            board->reset();

            // Parse Piece placement data
            u8 rank = Rank::RANK_8;
            u8 file = File::FILE_A;
            ss >> segment;
            for (const char& ch : segment)
            {
                if (ch == '/')
                {
                    rank--;
                    file = File::FILE_A;
                }
                else if (isdigit(ch))
                {
                    const u8 empty_squares = ch - '0';
                    if (empty_squares < 1 || empty_squares > 8)
                    {
                        throw std::invalid_argument("Invalid FEN!");
                    }
                    file += empty_squares;
                }
                else if (isalpha(ch))
                {
                    std::size_t piece_id = PIECES_STR.find(ch);
                    if (piece_id == std::string::npos)
                    {
                        throw std::invalid_argument("Invalid FEN!");
                    }
                    board->setPiece(static_cast<Piece>(piece_id), rf2sq(rank, file++));
                }
            }

            // Parse Active color
            ss >> segment;
            if (segment == "w")
            {
                board->setActiveColor(Color::WHITE);
            }
            else if (segment == "b")
            {
                board->setActiveColor(Color::BLACK);
            }
            else
            {
                throw std::invalid_argument("Invalid FEN!");
            }

            // Parse Castling availability
            ss >> segment;
            if (segment == "-")
            {
                board->addCastelingRights(board::CastleFlag::NOCA);
            }
            else
            {
                if (segment.find('K') != std::string::npos)
                {
                    board->addCastelingRights(board::CastleFlag::WKCA);
                }
                if (segment.find('Q') != std::string::npos)
                {
                    board->addCastelingRights(board::CastleFlag::WQCA);
                }
                if (segment.find('k') != std::string::npos)
                {
                    board->addCastelingRights(board::CastleFlag::BKCA);
                }
                if (segment.find('q') != std::string::npos)
                {
                    board->addCastelingRights(board::CastleFlag::BQCA);
                }
            }

            // Parse En passant target square
            ss >> segment;
            if (segment == "-")
            {
                board->setEnpassantTarget(Square::NO_SQ);
            }
            else
            {
                const Rank rank = static_cast<Rank>((segment[1] - '0') - 1);
                if (board->getActiveColor() == Color::WHITE)
                {
                    if (rank != Rank::RANK_6)
                    {
                        throw std::invalid_argument("Invalid FEN!");
                    }
                }
                else
                {
                    if (rank != Rank::RANK_3)
                    {
                        throw std::invalid_argument("Invalid FEN!");
                    }
                }
                const File   file         = static_cast<File>(segment[0] - 'a');
                const Square ep_target_sq = rf2sq(rank, file);
                board->setEnpassantTarget(ep_target_sq);
            }

            // Parse Halfmove clock
            ss >> segment;
            if (!full || segment.empty() || segment == "-")
            {
                board->setHalfmoveClock(0);
            }
            else
            {
                board->setHalfmoveClock(std::stoi(segment));
            }

            // Parse Fullmove number
            ss >> segment;
            if (!full || segment.empty() || segment == "-")
            {
                board->setFullmoveNumber(1);
            }
            else
            {
                board->setFullmoveNumber(std::stoi(segment));
            }

            // Reset Hash
            board->resetHash();
        }

    }

}
