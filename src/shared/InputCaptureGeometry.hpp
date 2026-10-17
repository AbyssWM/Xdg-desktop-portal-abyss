#pragma once

#include <cstdint>
#include <span>

namespace InputCaptureGeometry {
    struct SOutputGeometry {
        int32_t x      = 0;
        int32_t y      = 0;
        int32_t width  = 0;
        int32_t height = 0;
    };

    bool isBarrierValid(std::span<const SOutputGeometry> outputs, int32_t x1, int32_t y1, int32_t x2, int32_t y2);
}
