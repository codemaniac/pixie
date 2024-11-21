#include "../src/include/perft.h"
#include "../src/include/position.h"
#include "../src/include/threadpool.h"
#include "lib/doctest.h"


TEST_SUITE("Perft") {
    TEST_CASE("testing move generation using perft") {
        Position   position = Position();
        ThreadPool pool     = ThreadPool(2);

        position_init();

        position.set_start_pos();

        const uint64_t nodes = perft_multithreaded(position, 4, pool, false);
        CHECK(nodes == 197281);
    }
}
