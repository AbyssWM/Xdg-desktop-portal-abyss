#include "InputCaptureGeometry.hpp"

#include <algorithm>
#include <cstddef>
#include <set>
#include <vector>

using namespace InputCaptureGeometry;

static bool rangeContains(int64_t start, int64_t end, int64_t value) {
    return start <= value && value <= end;
}

static bool intervalOverlapsInclusive(int64_t a1, int64_t a2, int64_t b1, int64_t b2) {
    return std::max(a1, b1) <= std::min(a2, b2);
}

static bool isVerticalBarrierOnExteriorBoundary(std::span<const SOutputGeometry> outputs, int64_t x, int64_t y1, int64_t y2) {
    std::set<int64_t> breakpoints = {y1, y2 + 1};

    for (const auto& logical : outputs) {
        if (logical.width <= 0 || logical.height <= 0)
            return false;

        const int64_t top    = logical.y;
        const int64_t bottom = static_cast<int64_t>(logical.y) + logical.height - 1;
        if (!intervalOverlapsInclusive(y1, y2, top, bottom))
            continue;

        if (x == logical.x || x == static_cast<int64_t>(logical.x) + logical.width) {
            breakpoints.insert(std::max(y1, top));
            breakpoints.insert(std::min(y2, bottom) + 1);
        }
    }

    const std::vector<int64_t> points{breakpoints.begin(), breakpoints.end()};
    for (size_t i = 0; i + 1 < points.size(); ++i) {
        const int64_t segmentStart = points[i];
        const int64_t segmentEnd   = points[i + 1] - 1;
        if (segmentStart > segmentEnd)
            continue;

        bool hasLeftMonitor  = false;
        bool hasRightMonitor = false;
        for (const auto& logical : outputs) {
            if (logical.width <= 0 || logical.height <= 0)
                return false;

            const int64_t top    = logical.y;
            const int64_t bottom = static_cast<int64_t>(logical.y) + logical.height - 1;
            if (!intervalOverlapsInclusive(segmentStart, segmentEnd, top, bottom))
                continue;

            if (x == static_cast<int64_t>(logical.x) + logical.width && rangeContains(top, bottom, segmentStart))
                hasLeftMonitor = true;

            if (x == logical.x && rangeContains(top, bottom, segmentStart))
                hasRightMonitor = true;
        }

        if (hasLeftMonitor == hasRightMonitor)
            return false;
    }

    return true;
}

static bool isHorizontalBarrierOnExteriorBoundary(std::span<const SOutputGeometry> outputs, int64_t y, int64_t x1, int64_t x2) {
    std::set<int64_t> breakpoints = {x1, x2 + 1};

    for (const auto& logical : outputs) {
        if (logical.width <= 0 || logical.height <= 0)
            return false;

        const int64_t left  = logical.x;
        const int64_t right = static_cast<int64_t>(logical.x) + logical.width - 1;
        if (!intervalOverlapsInclusive(x1, x2, left, right))
            continue;

        if (y == logical.y || y == static_cast<int64_t>(logical.y) + logical.height) {
            breakpoints.insert(std::max(x1, left));
            breakpoints.insert(std::min(x2, right) + 1);
        }
    }

    const std::vector<int64_t> points{breakpoints.begin(), breakpoints.end()};
    for (size_t i = 0; i + 1 < points.size(); ++i) {
        const int64_t segmentStart = points[i];
        const int64_t segmentEnd   = points[i + 1] - 1;
        if (segmentStart > segmentEnd)
            continue;

        bool hasTopMonitor    = false;
        bool hasBottomMonitor = false;
        for (const auto& logical : outputs) {
            if (logical.width <= 0 || logical.height <= 0)
                return false;

            const int64_t left  = logical.x;
            const int64_t right = static_cast<int64_t>(logical.x) + logical.width - 1;
            if (!intervalOverlapsInclusive(segmentStart, segmentEnd, left, right))
                continue;

            if (y == static_cast<int64_t>(logical.y) + logical.height && rangeContains(left, right, segmentStart))
                hasTopMonitor = true;

            if (y == logical.y && rangeContains(left, right, segmentStart))
                hasBottomMonitor = true;
        }

        if (hasTopMonitor == hasBottomMonitor)
            return false;
    }

    return true;
}

bool InputCaptureGeometry::isBarrierValid(std::span<const SOutputGeometry> outputs, int32_t x1, int32_t y1, int32_t x2, int32_t y2) {
    if (x1 != x2 && y1 != y2) //At least one axis should be aligned
        return false;

    if (x1 == x2 && y1 == y2) //The barrier should have non-null area
        return false;

    if (x1 > x2)
        std::swap(x1, x2);

    if (y1 > y2)
        std::swap(y1, y2);

    if (x1 == x2)
        return isVerticalBarrierOnExteriorBoundary(outputs, x1, y1, y2);

    return isHorizontalBarrierOnExteriorBoundary(outputs, y1, x1, x2);
}
