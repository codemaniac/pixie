#pragma once

#include "pch.h"
#include "types.h"

namespace tejas {

    namespace utils {

        [[nodiscard]] u8 bitScanForward(u64* x);

        u8 bitCount1s(const u64 x);

        u64 prng();

    }

}
