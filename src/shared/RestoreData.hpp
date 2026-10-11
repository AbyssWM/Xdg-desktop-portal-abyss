#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <sdbus-c++/sdbus-c++.h>

namespace RestoreData {
    struct SData {
        std::string output;
        std::string windowClass;
        std::string windowTitle;
        uint32_t    cursorMode = 1; // hidden
        uint32_t    x = 0, y = 0, w = 0, h = 0;
    };

    using Token = sdbus::Struct<std::string, uint32_t, sdbus::Variant>;

    // Invalid, foreign, and unsupported tokens are ignored so the caller prompts.
    std::optional<SData> decode(const sdbus::Variant& token);
    Token                encode(const SData& data);
}
