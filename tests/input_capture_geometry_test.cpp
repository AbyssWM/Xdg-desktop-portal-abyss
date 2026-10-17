#include "../src/shared/InputCaptureGeometry.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <limits>
#include <random>
#include <span>
#include <vector>

using InputCaptureGeometry::SOutputGeometry;

struct SCase {
    std::vector<SOutputGeometry> outputs;
    std::array<int32_t, 4>       barrier;
    bool                         expected = false;
    const char*                  name     = "";
};

// Pointwise oracle for small coordinates, independent of the segment algorithm.
static bool pointwiseValid(std::span<const SOutputGeometry> outputs, std::array<int32_t, 4> barrier) {
    auto [x1, y1, x2, y2] = barrier;
    if ((x1 != x2 && y1 != y2) || (x1 == x2 && y1 == y2))
        return false;
    if (x1 > x2)
        std::swap(x1, x2);
    if (y1 > y2)
        std::swap(y1, y2);
    for (const auto& output : outputs)
        if (output.width <= 0 || output.height <= 0)
            return false;

    const bool VERTICAL = x1 == x2;
    for (int64_t position = VERTICAL ? y1 : x1; position <= (VERTICAL ? y2 : x2); ++position) {
        bool before = false;
        bool after  = false;
        for (const auto& output : outputs) {
            const int64_t START  = VERTICAL ? output.y : output.x;
            const int64_t LENGTH = VERTICAL ? output.height : output.width;
            if (position < START || position >= START + LENGTH)
                continue;
            const int64_t EDGE   = VERTICAL ? output.x : output.y;
            const int64_t SIZE   = VERTICAL ? output.width : output.height;
            const int64_t TARGET = VERTICAL ? x1 : y1;
            before |= TARGET == EDGE + SIZE;
            after |= TARGET == EDGE;
        }
        if (before == after)
            return false;
    }
    return true;
}

int main() {
    constexpr int32_t LOW     = std::numeric_limits<int32_t>::min();
    constexpr int32_t HIGH    = std::numeric_limits<int32_t>::max();
    const SCase       CASES[] = {
        {{{0, 0, 100, 100}}, {0, 0, 0, 99}, true, "left"},
        {{{0, 0, 100, 100}}, {100, 0, 100, 99}, true, "right"},
        {{{0, 0, 100, 100}}, {0, 0, 99, 0}, true, "top"},
        {{{0, 0, 100, 100}}, {0, 100, 99, 100}, true, "bottom"},
        {{{0, 0, 100, 100}}, {0, 99, 0, 0}, true, "reversed vertical"},
        {{{0, 0, 100, 100}}, {99, 0, 0, 0}, true, "reversed horizontal"},
        {{{0, 0, 100, 100}}, {1, 0, 1, 99}, false, "interior"},
        {{{0, 0, 100, 100}}, {0, 0, 0, 100}, false, "outside output"},
        {{{0, 0, 100, 100}}, {0, 0, 99, 99}, false, "diagonal"},
        {{{0, 0, 100, 100}}, {0, 0, 0, 0}, false, "zero length"},
        {{}, {0, 0, 0, 99}, false, "no outputs"},
        {{{0, 0, 0, 100}}, {0, 0, 0, 99}, false, "invalid size"},
        {{{0, 0, 100, 100}, {100, 0, 100, 100}}, {100, 0, 100, 99}, false, "shared vertical edge"},
        {{{0, 0, 100, 100}, {0, 100, 100, 100}}, {0, 100, 99, 100}, false, "shared horizontal edge"},
        {{{0, 0, 100, 100}, {0, 100, 100, 100}}, {0, 0, 0, 199}, true, "continuous exterior"},
        {{{0, 0, 100, 100}, {0, 101, 100, 100}}, {0, 0, 0, 200}, false, "gap"},
        {{{-100, -100, 100, 100}}, {-100, -100, -100, -1}, true, "negative coordinates"},
        {{{0, HIGH - 9, 10, 10}}, {0, HIGH - 9, 0, HIGH}, true, "maximum vertical endpoint"},
        {{{HIGH - 9, 0, 10, 10}}, {HIGH - 9, 0, HIGH, 0}, true, "maximum horizontal endpoint"},
        {{{0, HIGH - 2, 10, 10}}, {0, HIGH - 2, 0, HIGH}, true, "bottom beyond int32"},
        {{{HIGH - 2, 0, 10, 10}}, {HIGH - 2, 0, HIGH, 0}, true, "right beyond int32"},
        {{{HIGH - 2, 0, 10, 10}}, {LOW + 7, 0, LOW + 7, 9}, false, "no wrapped right edge"},
        {{{0, HIGH - 2, 10, 10}}, {0, LOW + 7, 9, LOW + 7}, false, "no wrapped bottom edge"},
        {{{HIGH - 10, 0, 10, 10}}, {HIGH, 0, HIGH, 9}, true, "maximum boundary"},
        {{{LOW, LOW, 10, 10}}, {LOW, LOW, LOW, LOW + 9}, true, "minimum coordinates"},
        {{{0, LOW, 5, HIGH}, {0, -1, 5, HIGH}, {0, HIGH - 1, 5, 2}}, {0, LOW, 0, HIGH}, true, "full int32 span"},
    };
    int failures = 0;
    for (const auto& test : CASES) {
        const auto& [x1, y1, x2, y2] = test.barrier;
        if (InputCaptureGeometry::isBarrierValid(test.outputs, x1, y1, x2, y2) != test.expected) {
            std::cerr << "FAIL: " << test.name << '\n';
            ++failures;
        }
    }

    std::mt19937 random{17102026};
    for (int sample = 0; sample < 2000; ++sample) {
        std::vector<SOutputGeometry> outputs;
        for (int i = 0; i < 3; ++i)
            outputs.push_back(
                {static_cast<int32_t>(random() % 11) - 5, static_cast<int32_t>(random() % 11) - 5, static_cast<int32_t>(random() % 6), static_cast<int32_t>(random() % 6)});
        std::array<int32_t, 4> barrier{};
        for (auto& coordinate : barrier)
            coordinate = static_cast<int32_t>(random() % 21) - 10;
        if (sample % 2 == 0)
            barrier[2] = barrier[0];
        else
            barrier[3] = barrier[1];
        if (InputCaptureGeometry::isBarrierValid(outputs, barrier[0], barrier[1], barrier[2], barrier[3]) != pointwiseValid(outputs, barrier)) {
            std::cerr << "FAIL: pointwise comparison " << sample << '\n';
            ++failures;
        }
    }
    std::cout << "26 boundary cases and 2000 pointwise comparisons; failures: " << failures << '\n';
    return failures ? 1 : 0;
}
