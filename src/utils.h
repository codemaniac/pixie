#pragma once

#include "types.h"

namespace tejas {

    namespace utils {

        [[nodiscard]] u8 BitScanForward(u64* x);

        u8 BitCount1s(const u64 x);

    }

}
