#include "utils.h"

namespace tejas {

    namespace utils {

        [[nodiscard]] u8 BitScanForward(u64* x) {
            u8 position = __builtin_ctzll(*x);
            *x &= *x - 1;
            return position;
        }

        u8 BitCount1s(const u64 x) { return __builtin_popcountll(x); }

    }

}
