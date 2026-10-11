#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace PickerData {
    enum class eType {
        OUTPUT,
        WINDOW,
        REGION
    };

    struct SSelection {
        eType       type = eType::OUTPUT;
        std::string output;
        uint32_t    id = 0;
        uint32_t    x = 0, y = 0, w = 0, h = 0;
        bool        allowToken = false;
    };

    struct SWindow {
        std::string name;
        std::string clazz;
        uint32_t    id      = 0;
        uint64_t    address = 0;
    };

    std::optional<SSelection> parseSelection(std::string_view output);
    std::vector<SWindow>      parseWindows(std::string_view list);
    std::string               encodeWindow(const SWindow& window);
}
