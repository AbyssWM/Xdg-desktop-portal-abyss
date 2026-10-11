#include "RestoreData.hpp"

#include <ctime>
#include <limits>
#include <unordered_map>

std::optional<RestoreData::SData> RestoreData::decode(const sdbus::Variant& token) {
    try {
        const auto TOKEN = token.get<Token>();
        if (TOKEN.get<0>() != "abyssland")
            return std::nullopt;

        SData result;
        if (TOKEN.get<1>() == 2) {
            const auto LEGACY = TOKEN.get<2>().get<sdbus::Struct<std::string, uint32_t, std::string, bool, uint64_t>>();
            // Old window tokens contain only a transient address; only outputs can restore.
            result.output     = LEGACY.get<2>();
            result.cursorMode = LEGACY.get<3>() ? 2 : 1;
        } else if (TOKEN.get<1>() == 3) {
            const auto VALUES = TOKEN.get<2>().get<std::unordered_map<std::string, sdbus::Variant>>();
            for (const auto& [key, value] : VALUES) {
                if (key == "output")
                    result.output = value.get<std::string>();
                else if (key == "windowClass")
                    result.windowClass = value.get<std::string>();
                else if (key == "windowTitle")
                    result.windowTitle = value.get<std::string>();
                else if (key == "withCursor")
                    result.cursorMode = value.get<uint32_t>();
                else if (key == "geometry") {
                    const auto GEOMETRY = value.get<sdbus::Struct<uint32_t, uint32_t, uint32_t, uint32_t>>();
                    result.x            = GEOMETRY.get<0>();
                    result.y            = GEOMETRY.get<1>();
                    result.w            = GEOMETRY.get<2>();
                    result.h            = GEOMETRY.get<3>();
                    if (!result.w || !result.h || result.x > uint32_t{std::numeric_limits<int32_t>::max()} || result.y > uint32_t{std::numeric_limits<int32_t>::max()} ||
                        result.w > uint32_t{std::numeric_limits<int32_t>::max()} || result.h > uint32_t{std::numeric_limits<int32_t>::max()})
                        return std::nullopt;
                } else if (key == "windowHandle" || key == "timeIssued")
                    static_cast<void>(value.get<uint64_t>());
                else if (key == "token")
                    static_cast<void>(value.get<std::string>());
            }
        } else
            return std::nullopt;

        const bool WINDOW = !result.windowClass.empty() || !result.windowTitle.empty();
        if ((WINDOW ? !result.output.empty() : result.output.empty()) || (WINDOW && (result.w || result.h)) || (result.cursorMode != 1 && result.cursorMode != 2))
            return std::nullopt;
        return result;
    } catch (const std::exception&) { return std::nullopt; }
}

RestoreData::Token RestoreData::encode(const SData& data) {
    std::unordered_map<std::string, sdbus::Variant> values;
    if (!data.output.empty()) {
        values["output"] = sdbus::Variant{data.output};
        if (data.w && data.h)
            values["geometry"] = sdbus::Variant{sdbus::Struct<uint32_t, uint32_t, uint32_t, uint32_t>{data.x, data.y, data.w, data.h}};
    } else {
        values["windowClass"] = sdbus::Variant{data.windowClass};
        values["windowTitle"] = sdbus::Variant{data.windowTitle};
    }
    values["timeIssued"] = sdbus::Variant{static_cast<uint64_t>(std::time(nullptr))};
    values["token"]      = sdbus::Variant{std::string{"todo"}};
    values["withCursor"] = sdbus::Variant{data.cursorMode};
    return {"abyssland", 3, sdbus::Variant{values}};
}
