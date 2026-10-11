#include "PickerData.hpp"

#include <array>
#include <charconv>
#include <limits>
#include <utility>

template <typename T>
static bool parseNumber(std::string_view input, T& value) {
    if (input.empty())
        return false;
    const auto RESULT = std::from_chars(input.data(), input.data() + input.size(), value);
    return RESULT.ec == std::errc{} && RESULT.ptr == input.data() + input.size();
}

std::optional<PickerData::SSelection> PickerData::parseSelection(std::string_view output) {
    const auto MARKER = output.find("[SELECTION]");
    if (MARKER == std::string_view::npos || (MARKER && output[MARKER - 1] != '\n'))
        return std::nullopt;
    output.remove_prefix(MARKER + 11);
    while (!output.empty() && (output.back() == '\n' || output.back() == '\r'))
        output.remove_suffix(1);
    if (output.find_first_of("\r\n") != std::string_view::npos || output.find('\0') != std::string_view::npos)
        return std::nullopt;

    const auto SLASH = output.find('/');
    if (SLASH == std::string_view::npos)
        return std::nullopt;
    const auto FLAGS = output.substr(0, SLASH);
    if (FLAGS != "" && FLAGS != "r")
        return std::nullopt;
    output.remove_prefix(SLASH + 1);

    SSelection result;
    result.allowToken = FLAGS == "r";
    if (output.substr(0, 7) == "screen:") {
        result.type   = eType::OUTPUT;
        result.output = output.substr(7);
        if (result.output.empty())
            return std::nullopt;
    } else if (output.substr(0, 7) == "window:") {
        result.type = eType::WINDOW;
        if (!parseNumber(output.substr(7), result.id) || !result.id)
            return std::nullopt;
    } else if (output.substr(0, 7) == "region:") {
        result.type = eType::REGION;
        output.remove_prefix(7);
        const auto AT = output.rfind('@');
        if (AT == std::string_view::npos || !AT)
            return std::nullopt;
        result.output = output.substr(0, AT);
        output.remove_prefix(AT + 1);
        const std::array<uint32_t*, 4> COORDINATES = {&result.x, &result.y, &result.w, &result.h};
        for (size_t i = 0; i < COORDINATES.size(); ++i) {
            const auto COMMA = output.find(',');
            if ((i < 3 && COMMA == std::string_view::npos) || (i == 3 && COMMA != std::string_view::npos))
                return std::nullopt;
            if (!parseNumber(output.substr(0, COMMA), *COORDINATES[i]) || *COORDINATES[i] > uint32_t{std::numeric_limits<int32_t>::max()})
                return std::nullopt;
            if (i < 3)
                output.remove_prefix(COMMA + 1);
        }
        if (!result.w || !result.h)
            return std::nullopt;
    } else
        return std::nullopt;
    return result;
}

std::vector<PickerData::SWindow> PickerData::parseWindows(std::string_view list) {
    std::vector<SWindow>                      result;
    constexpr std::array<std::string_view, 4> SEPARATORS = {"[HC>]", "[HT>]", "[HE>]", "[HA>]"};
    while (!list.empty()) {
        std::array<std::string_view, 4> fields;
        for (size_t i = 0; i < fields.size(); ++i) {
            const auto SEP = list.find(SEPARATORS[i]);
            if (SEP == std::string_view::npos)
                return result;
            fields[i] = list.substr(0, SEP);
            list.remove_prefix(SEP + SEPARATORS[i].size());
        }
        SWindow window;
        if (!parseNumber(fields[0], window.id) || !window.id || !parseNumber(fields[3], window.address))
            continue;
        window.clazz = fields[1];
        window.name  = fields[2];
        result.push_back(std::move(window));
    }
    return result;
}

static std::string escapeSeparators(std::string text) {
    constexpr std::array<std::string_view, 4> SEPARATORS = {"[HC>]", "[HT>]", "[HE>]", "[HA>]"};
    for (const auto SEP : SEPARATORS) {
        size_t pos = 0;
        while ((pos = text.find(SEP, pos)) != std::string::npos)
            text[pos + 3] = ' ';
    }
    return text;
}

std::string PickerData::encodeWindow(const SWindow& window) {
    return std::to_string(window.id) + "[HC>]" + escapeSeparators(window.clazz) + "[HT>]" + escapeSeparators(window.name) + "[HE>]" + std::to_string(window.address) + "[HA>]";
}
