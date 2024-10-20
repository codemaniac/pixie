#include "include/fen.h"
#include <iostream>
#include <sstream>
#include <string>

void fen_to_position(std::string fen, std::unique_ptr<Position>& pos) {
    std::string piece_placement;
    std::string active_color;
    std::string castling_rights;
    std::string ep_target;
    std::string halfmove_clock;
    std::string fullmove_number;

    std::istringstream iss(fen);

    iss >> piece_placement >> active_color >> castling_rights >> ep_target >> halfmove_clock
      >> fullmove_number;

    const std::string pieces_str = ".PNBRQKXXpnbrqk";
    uint8_t           rank       = RANK_8;
    uint8_t           file       = FILE_A;
    for (char ch : piece_placement)
    {
        if (ch == '/')
        {
            rank--;
            file = 0;
        }
        else if (isdigit(ch))
        {
            const uint8_t empty_squares = ch - '0';
            if (empty_squares < 1 || empty_squares > 8)
                throw std::invalid_argument("Invalid FEN!");
            file += empty_squares;
        }
        else if (isalpha(ch))
        {
            std::size_t piece_id = pieces_str.find(ch);
            if (piece_id == std::string::npos)
                throw std::invalid_argument("Invalid FEN!");
            pos->set_piece(static_cast<Piece>(piece_id),
                           BOARD_RF_TO_SQ(static_cast<Rank>(rank), static_cast<File>(file++)));
        }
    }

    if (active_color == "w")
        pos->set_active_color(WHITE);
    else if (active_color == "b")
        pos->set_active_color(BLACK);
    else
        throw std::invalid_argument("Invalid FEN!");

    if (castling_rights == "-")
        pos->add_casteling_rights(NOCA);
    else
    {
        if (castling_rights.find('K') != std::string::npos)
            pos->add_casteling_rights(WKCA);
        if (castling_rights.find('Q') != std::string::npos)
            pos->add_casteling_rights(WQCA);
        if (castling_rights.find('k') != std::string::npos)
            pos->add_casteling_rights(BKCA);
        if (castling_rights.find('q') != std::string::npos)
            pos->add_casteling_rights(BQCA);
    }

    if (ep_target == "-")
        pos->set_enpassant_target(NO_SQ);
    else
    {
        const Rank rank = static_cast<Rank>((ep_target[1] - '0') - 1);
        if (active_color == "w")
        {
            if (rank != RANK_6)
                throw std::invalid_argument("Invalid FEN!");
        }
        else
        {
            if (rank != RANK_3)
                throw std::invalid_argument("Invalid FEN!");
        }
        const File   file         = static_cast<File>(ep_target[0] - 'a');
        const Square ep_target_sq = BOARD_RF_TO_SQ(rank, file);
        pos->set_enpassant_target(ep_target_sq);
    }

    if (halfmove_clock.empty())
        pos->set_half_move_clock(0);
    else
        pos->set_half_move_clock(std::stoi(halfmove_clock));

    if (fullmove_number.empty())
        pos->set_full_move_number(1);
    else
        pos->set_full_move_number(std::stoi(fullmove_number));

    pos->reset_hash();
}