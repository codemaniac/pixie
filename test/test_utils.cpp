#include "../src/include/utils.h"
#include "lib/doctest.h"

TEST_SUITE("Utils") {
    TEST_CASE("Test utils_bit_count1s") {
        CHECK(utils_bit_count1s(0) == 0);
        CHECK(utils_bit_count1s(1) == 1);
        CHECK(utils_bit_count1s(2) == 1);
        CHECK(utils_bit_count1s(3) == 2);
        CHECK(utils_bit_count1s(4) == 1);
        CHECK(utils_bit_count1s(5) == 2);
        CHECK(utils_bit_count1s(6) == 2);
        CHECK(utils_bit_count1s(7) == 3);
        CHECK(utils_bit_count1s(8) == 1);
    }
}
