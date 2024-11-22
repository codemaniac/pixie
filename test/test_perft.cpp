#include "../src/include/fen.h"
#include "../src/include/perft.h"
#include "../src/include/position.h"
#include "../src/include/threadpool.h"
#include "lib/doctest.h"


using pair_type = std::pair<std::string, std::vector<std::uint64_t>>;

static ThreadPool pool = ThreadPool(2);

TEST_SUITE("Perft") {
    TEST_CASE("Perft - startpos") {
        Position position;
        position.set_start_pos();

        const uint64_t nodes = perft_multithreaded(position, 4, pool, false);
        CHECK(nodes == 197281);
    }

    TEST_CASE("Perft - Sanity") {
        const std::array<pair_type, 7> positions = {
          {{"8/4k3/8/8/8/8/4K3/8 w - - 0 1", {1, 8}},
           {"7k/8/8/8/8/4B3/8/7K w - - 0 1", {1, 14}},
           {"7k/8/8/8/8/4N3/8/7K w - - 0 1", {1, 11}},
           {"4k3/4r3/8/8/8/3p4/4P3/4K3 w - - 0 1", {1, 6}},
           {"4k3/4r3/8/8/8/8/3p4/4K3 w - - 0 2", {1, 4}},
           {"4k3/8/8/8/8/8/1r4K1/5b2 w - - 0 2", {1, 5}},
           {"4k3/2b3q1/3P1P2/4K3/3P1P2/2b3q1/8/8 w - - 0 1", {1, 6}}}};

        for (const auto& [fen, nodes] : positions)
        {
            Position position;
            fen_to_position(fen, &position);
            for (std::size_t i = 0; i < nodes.size(); ++i)
            {
                const uint64_t result = perft_multithreaded(position, i, pool, false);
                REQUIRE(result == nodes[i]);
            }
        }
    }

    TEST_CASE("Perft - En passant -- Diagonal pin") {
        const std::array<pair_type, 12> positions = {{
          {"4k3/b7/8/2Pp4/8/8/8/6K1 w - d6 0 2", {1, 5}},
          {"4k3/7b/8/4pP2/8/8/8/1K6 w - e6 0 2", {1, 5}},
          {"6k1/8/8/8/2pP4/8/B7/3K4 b - d3 0 2", {1, 5}},
          {"1k6/8/8/8/4Pp2/8/7B/4K3 b - e3 0 2", {1, 5}},
          {"4k3/b7/8/1pP5/8/8/8/6K1 w - b6 0 2", {1, 6}},
          {"4k3/7b/8/5Pp1/8/8/8/1K6 w - g6 0 2", {1, 6}},
          {"6k1/8/8/8/1Pp5/8/B7/4K3 b - b3 0 2", {1, 6}},
          {"1k6/8/8/8/5pP1/8/7B/4K3 b - g3 0 2", {1, 6}},
          {"4k3/K7/8/1pP5/8/8/8/6b1 w - b6 0 2", {1, 6}},
          {"4k3/7K/8/5Pp1/8/8/8/1b6 w - g6 0 2", {1, 6}},
          {"6B1/8/8/8/1Pp5/8/k7/4K3 b - b3 0 2", {1, 6}},
          {"1B6/8/8/8/5pP1/8/7k/4K3 b - g3 0 2", {1, 6}},
        }};

        for (const auto& [fen, nodes] : positions)
        {
            Position position;
            fen_to_position(fen, &position);
            for (std::size_t i = 0; i < nodes.size(); ++i)
            {
                const uint64_t result = perft_multithreaded(position, i, pool, false);
                REQUIRE(result == nodes[i]);
            }
        }
    }

    TEST_CASE("Perft - En passant -- Horizontal pin") {
        const std::array<pair_type, 4> positions = {{
          {"4k3/8/8/K2pP2r/8/8/8/8 w - d6 0 1", {1, 6}},
          {"4k3/8/8/r2pP2K/8/8/8/8 w - d6 0 1", {1, 6}},
          {"8/8/8/8/1k1Pp2R/8/8/4K3 b - d3 0 1", {1, 8}},
          {"8/8/8/8/1R1Pp2k/8/8/4K3 b - d3 0 1", {1, 6}},
        }};

        for (const auto& [fen, nodes] : positions)
        {
            Position position;
            fen_to_position(fen, &position);
            for (std::size_t i = 0; i < nodes.size(); ++i)
            {
                const uint64_t result = perft_multithreaded(position, i, pool, false);
                REQUIRE(result == nodes[i]);
            }
        }
    }

    TEST_CASE("Perft - En passant -- Vertical pin") {
        const std::array<pair_type, 2> positions = {{
          {"k7/8/4r3/3pP3/8/8/8/4K3 w - d6 0 1", {1, 5}},
          {"k3K3/8/8/3pP3/8/8/8/4r3 w - d6 0 1", {1, 6}},
        }};

        for (const auto& [fen, nodes] : positions)
        {
            Position position;
            fen_to_position(fen, &position);
            for (std::size_t i = 0; i < nodes.size(); ++i)
            {
                const uint64_t result = perft_multithreaded(position, i, pool, false);
                REQUIRE(result == nodes[i]);
            }
        }
    }

    TEST_CASE("Perft - En passant -- Legal") {
        const std::array<pair_type, 5> positions = {{
          {"8/8/8/8/1k1PpN1R/8/8/4K3 b - d3 0 1", {1, 9, 193, 1322}},
          {"8/8/8/8/1k1Ppn1R/8/8/4K3 b - d3 0 1", {1, 17, 220, 3001}},
          {"4k3/8/8/2PpP3/8/8/8/4K3 w - d6 0 1", {1, 9, 47, 376}},
          {"4k3/8/8/8/2pPp3/8/8/4K3 b - d3 0 1", {1, 9, 47, 376}},
          {"r3k2r/p2pqpb1/bn2pnp1/2pPN3/1p2P3/2N2Q1p/PPPBBPPP/R4K1R w kq c6 0 2", {1, 46}},
        }};

        for (const auto& [fen, nodes] : positions)
        {
            Position position;
            fen_to_position(fen, &position);
            for (std::size_t i = 0; i < nodes.size(); ++i)
            {
                const uint64_t result = perft_multithreaded(position, i, pool, false);
                REQUIRE(result == nodes[i]);
            }
        }
    }

    TEST_CASE("Perft - En passant -- Capture checker") {
        const std::array<pair_type, 2> positions = {{
          {"4k3/8/8/4pP2/3K4/8/8/8 w - e6 0 2", {1, 9}},
          {"8/8/8/4k3/5Pp1/8/8/3K4 b - f3 0 1", {1, 9}},
        }};

        for (const auto& [fen, nodes] : positions)
        {
            Position position;
            fen_to_position(fen, &position);
            for (std::size_t i = 0; i < nodes.size(); ++i)
            {
                const uint64_t result = perft_multithreaded(position, i, pool, false);
                REQUIRE(result == nodes[i]);
            }
        }
    }

    TEST_CASE("Perft - En passant -- In check") {
        const std::array<pair_type, 14> positions = {{
          {"2b1k3/8/8/2Pp4/8/7K/8/8 w - - 0 1", {1, 4, 52}},
          {"2b1k3/8/8/2Pp4/8/7K/8/8 w - d6 0 1", {1, 4, 52}},
          {"4k3/r6K/8/2Pp4/8/8/8/8 w - - 0 1", {1, 4, 77}},
          {"4k3/r6K/8/2Pp4/8/8/8/8 w - d6 0 1", {1, 4, 77}},
          {"2K1k3/8/8/2Pp4/8/7b/8/8 w - - 0 1", {1, 3, 37}},
          {"2K1k3/8/8/2Pp4/8/7b/8/8 w - d6 0 1", {1, 3, 37}},
          {"2K1k3/8/8/2Pp4/8/7q/8/8 w - - 0 1", {1, 3, 79}},
          {"2K1k3/8/8/2Pp4/8/7q/8/8 w - d6 0 1", {1, 3, 79}},
          {"8/8/7k/8/2pP4/8/8/2B1K3 b - - 0 1", {1, 4, 52}},
          {"8/8/7k/8/2pP4/8/8/2B1K3 b - d3 0 1", {1, 4, 52}},
          {"8/8/7k/8/2pP4/8/8/2Q1K3 b - - 0 1", {1, 4, 76}},
          {"8/8/7k/8/2pP4/8/8/2Q1K3 b - d3 0 1", {1, 4, 76}},
          {"8/8/8/8/2pP4/8/R6k/4K3 b - - 0 1", {1, 4, 77}},
          {"8/8/8/8/2pP4/8/R6k/4K3 b - d3 0 1", {1, 4, 77}},
        }};

        for (const auto& [fen, nodes] : positions)
        {
            Position position;
            fen_to_position(fen, &position);
            for (std::size_t i = 0; i < nodes.size(); ++i)
            {
                const uint64_t result = perft_multithreaded(position, i, pool, false);
                REQUIRE(result == nodes[i]);
            }
        }
    }

    TEST_CASE("Perft - En passant -- Block check") {
        const std::array<pair_type, 12> positions = {{
          {"4k3/8/K6r/3pP3/8/8/8/8 w - d6 0 1", {1, 6, 109}},
          {"4k3/8/K6q/3pP3/8/8/8/8 w - d6 0 1", {1, 6, 151}},
          {"4kb2/8/8/3pP3/8/K7/8/8 w - d6 0 1", {1, 5, 55}},
          {"4kq2/8/8/3pP3/8/K7/8/8 w - d6 0 1", {1, 5, 100}},
          {"4k3/8/r6K/3pP3/8/8/8/8 w - d6 0 1", {1, 6, 107}},
          {"4k3/8/q6K/3pP3/8/8/8/8 w - d6 0 1", {1, 6, 149}},
          {"3k1K2/8/8/3pP3/8/b7/8/8 w - d6 0 1", {1, 4, 44}},
          {"3k1K2/8/8/3pP3/8/q7/8/8 w - d6 0 1", {1, 4, 100}},
          {"8/8/8/8/3Pp3/k6R/8/4K3 b - d3 0 1", {1, 6, 109}},
          {"8/8/8/8/3Pp3/k6Q/8/4K3 b - d3 0 1", {1, 6, 151}},
          {"8/8/k7/8/3Pp3/8/8/4KB2 b - d3 0 1", {1, 5, 55}},
          {"8/8/k7/8/3Pp3/8/8/4KQ2 b - d3 0 1", {1, 5, 100}},
        }};

        for (const auto& [fen, nodes] : positions)
        {
            Position position;
            fen_to_position(fen, &position);
            for (std::size_t i = 0; i < nodes.size(); ++i)
            {
                const uint64_t result = perft_multithreaded(position, i, pool, false);
                REQUIRE(result == nodes[i]);
            }
        }
    }

    TEST_CASE("Perft - Shallow") {
        const std::array<pair_type, 25> positions = {{
          {"4k3/b7/8/2Pp4/8/8/8/6K1 w - d6 0 2", {5}},
          {"4k3/7b/8/4pP2/8/8/8/1K6 w - e6 0 2", {5}},
          {"6k1/8/8/8/2pP4/8/B7/3K4 b - d3 0 2", {5}},
          {"1k6/8/8/8/4Pp2/8/7B/4K3 b - e3 0 2", {5}},
          {"4k3/b7/8/1pP5/8/8/8/6K1 w - b6 0 2", {6}},
          {"4k3/7b/8/5Pp1/8/8/8/1K6 w - g6 0 2", {6}},
          {"6k1/8/8/8/1Pp5/8/B7/4K3 b - b3 0 2", {6}},
          {"1k6/8/8/8/5pP1/8/7B/4K3 b - g3 0 2", {6}},
          {"4k3/K7/8/1pP5/8/8/8/6b1 w - b6 0 2", {6}},
          {"4k3/7K/8/5Pp1/8/8/8/1b6 w - g6 0 2", {6}},
          {"6B1/8/8/8/1Pp5/8/k7/4K3 b - b3 0 2", {6}},
          {"1B6/8/8/8/5pP1/8/7k/4K3 b - g3 0 2", {6}},
          {"4k3/8/8/K2pP2r/8/8/8/8 w - d6 0 1", {6}},
          {"4k3/8/8/r2pP2K/8/8/8/8 w - d6 0 1", {6}},
          {"8/8/8/8/1k1Pp2R/8/8/4K3 b - d3 0 1", {8}},
          {"8/8/8/8/1R1Pp2k/8/8/4K3 b - d3 0 1", {6}},
          {"k7/8/4r3/3pP3/8/8/8/4K3 w - d6 0 1", {5}},
          {"k3K3/8/8/3pP3/8/8/8/4r3 w - d6 0 1", {6}},
          {"8/8/8/8/1k1PpN1R/8/8/4K3 b - d3 0 1", {9, 193, 1322}},
          {"8/8/8/8/1k1Ppn1R/8/8/4K3 b - d3 0 1", {17, 220, 3001}},
          {"4k3/8/8/2PpP3/8/8/8/4K3 w - d6 0 1", {9, 47, 376}},
          {"4k3/8/8/8/2pPp3/8/8/4K3 b - d3 0 1", {9, 47, 376}},
          {"4k3/8/8/4pP2/3K4/8/8/8 w - e6 0 2", {9}},
          {"8/8/8/4k3/5Pp1/8/8/3K4 b - f3 0 1", {9}},
          {"4k3/8/K6r/3pP3/8/8/8/8 w - d6 0 1", {6}},
        }};

        for (const auto& [fen, nodes] : positions)
        {
            Position position;
            fen_to_position(fen, &position);
            for (std::size_t i = 0; i < nodes.size(); ++i)
            {
                const uint64_t result = perft_multithreaded(position, i + 1, pool, false);
                REQUIRE(result == nodes[i]);
            }
        }
    }

    TEST_CASE("Perft - Double Check") {
        const std::array<pair_type, 2> positions = {{
          {"4k3/8/4r3/8/8/8/3p4/4K3 w - - 0 1", {1, 4, 80, 320}},
          {"4k3/8/4q3/8/8/8/3b4/4K3 w - - 0 1", {1, 4, 143, 496}},
        }};

        for (const auto& [fen, nodes] : positions)
        {
            Position position;
            fen_to_position(fen, &position);
            for (std::size_t i = 0; i < nodes.size(); ++i)
            {
                const uint64_t result = perft_multithreaded(position, i, pool, false);
                REQUIRE(result == nodes[i]);
            }
        }
    }

    TEST_CASE("Perft - Pins") {
        const std::array<pair_type, 6> positions = {{
          {"4k3/8/8/8/1b5b/8/3Q4/4K3 w - - 0 1", {1, 3, 54, 1256}},
          {"4k3/8/8/8/1b5b/8/3R4/4K3 w - - 0 1", {1, 3, 54, 836}},
          {"4k3/8/8/8/1b5b/2Q5/5P2/4K3 w - - 0 1", {1, 6, 98, 2274}},
          {"4k3/8/8/8/1b5b/2R5/5P2/4K3 w - - 0 1", {1, 4, 72, 1300}},
          {"4k3/8/8/8/1b2r3/8/3Q4/4K3 w - - 0 1", {1, 3, 66, 1390}},
          {"4k3/8/8/8/1b2r3/8/3QP3/4K3 w - - 0 1", {1, 6, 119, 2074}},
        }};

        for (const auto& [fen, nodes] : positions)
        {
            Position position;
            fen_to_position(fen, &position);
            for (std::size_t i = 0; i < nodes.size(); ++i)
            {
                const uint64_t result = perft_multithreaded(position, i, pool, false);
                REQUIRE(result == nodes[i]);
            }
        }
    }
}
