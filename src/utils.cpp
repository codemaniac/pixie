#include "utils.h"

namespace tejas {

    namespace utils {

        [[nodiscard]] u8 bitScanForward(u64* x) {
            u8 position = __builtin_ctzll(*x);
            *x &= *x - 1;
            return position;
        }

        u8 bitCount1s(const u64 x) { return __builtin_popcountll(x); }

        u64 prng() {
            // http://vigna.di.unimi.it/ftp/papers/xorshift.pdf

            static u64 seed = 1070372ULL;

            seed ^= seed >> 12;
            seed ^= seed << 25;
            seed ^= seed >> 27;

            return seed * 2685821657736338717ULL;
        }

    }

}
