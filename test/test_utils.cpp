#include "doctest/doctest.h"
#include "types.h"
#include "utils.h"

using namespace tejas;

TEST_SUITE("Utils") {

    TEST_CASE("utils::BitScanForward") {
        u64 x = (1 << 4) | (1 << 8) | (1 << 16);

        u8 pos = utils::BitScanForward(&x);
        CHECK(pos == 4);

        pos = utils::BitScanForward(&x);
        CHECK(pos == 8);

        pos = utils::BitScanForward(&x);
        CHECK(pos == 16);

        CHECK(x == 0ULL);
    }

    TEST_CASE("utils::BitCount1s") {
        CHECK(utils::BitCount1s(0) == 0);
        CHECK(utils::BitCount1s(1) == 1);
        CHECK(utils::BitCount1s(2) == 1);
        CHECK(utils::BitCount1s(3) == 2);
        CHECK(utils::BitCount1s(4) == 1);
        CHECK(utils::BitCount1s(5) == 2);
        CHECK(utils::BitCount1s(6) == 2);
        CHECK(utils::BitCount1s(7) == 3);
        CHECK(utils::BitCount1s(8) == 1);
    }
}
