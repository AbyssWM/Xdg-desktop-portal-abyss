#include "../src/shared/RestoreData.hpp"

#include <cstdio>
#include <unordered_map>

static int fails = 0;
#define CHECK(name, cond)                                                                                                                                                          \
    do {                                                                                                                                                                           \
        const bool OK = (cond);                                                                                                                                                    \
        std::printf("%s %s\n", OK ? "PASS" : "FAIL", name);                                                                                                                        \
        fails += !OK;                                                                                                                                                              \
    } while (false)

static sdbus::Variant token(std::unordered_map<std::string, sdbus::Variant> values, std::string issuer = "abyssland", uint32_t version = 3) {
    return sdbus::Variant{RestoreData::Token{issuer, version, sdbus::Variant{values}}};
}

int main() {
    RestoreData::SData stored;
    stored.windowClass = "Firefox";
    stored.windowTitle = "Document — Browser";
    stored.cursorMode  = 2;
    const auto encoded = RestoreData::encode(stored);
    const auto window  = RestoreData::decode(sdbus::Variant{encoded});
    CHECK("window v3 roundtrip", window && window->windowClass == stored.windowClass && window->windowTitle == stored.windowTitle && window->cursorMode == 2);
    const auto values = encoded.get<2>().get<std::unordered_map<std::string, sdbus::Variant>>();
    CHECK("new token never stores a process address", !values.contains("windowHandle"));

    stored             = {};
    stored.output      = "DP-1 with spaces";
    const auto monitor = RestoreData::decode(sdbus::Variant{RestoreData::encode(stored)});
    CHECK("monitor v3 roundtrip", monitor && monitor->output == stored.output && monitor->cursorMode == 1);
    stored.x          = 10;
    stored.y          = 20;
    stored.w          = 100;
    stored.h          = 200;
    const auto region = RestoreData::decode(sdbus::Variant{RestoreData::encode(stored)});
    CHECK("geometry v3 roundtrip", region && region->x == 10 && region->y == 20 && region->w == 100 && region->h == 200);

    CHECK("foreign issuer ignored", !RestoreData::decode(token({{"output", sdbus::Variant{std::string{"DP-1"}}}}, "other")));
    CHECK("unsupported version ignored", !RestoreData::decode(token({}, "abyssland", 99)));
    CHECK("malformed outer variant ignored", !RestoreData::decode(sdbus::Variant{std::string{"not a token"}}));
    CHECK("empty v3 token ignored", !RestoreData::decode(token({})));
    CHECK("wrong field type ignored", !RestoreData::decode(token({{"output", sdbus::Variant{uint32_t{1}}}})));
    CHECK("wrong v3 container ignored", !RestoreData::decode(sdbus::Variant{RestoreData::Token{"abyssland", 3, sdbus::Variant{std::string{"bad"}}}}));
    CHECK("missing optional cursor defaults hidden", RestoreData::decode(token({{"output", sdbus::Variant{std::string{"DP-1"}}}}))->cursorMode == 1);
    CHECK("pointer-only token ignored", !RestoreData::decode(token({{"windowHandle", sdbus::Variant{uint64_t{123}}}})));
    CHECK("mixed window/output token ignored", !RestoreData::decode(token({{"output", sdbus::Variant{std::string{"DP-1"}}}, {"windowClass", sdbus::Variant{std::string{"app"}}}})));
    CHECK("unsupported cursor mode ignored", !RestoreData::decode(token({{"output", sdbus::Variant{std::string{"DP-1"}}}, {"withCursor", sdbus::Variant{uint32_t{4}}}})));
    CHECK("malformed geometry ignored", !RestoreData::decode(token({{"output", sdbus::Variant{std::string{"DP-1"}}}, {"geometry", sdbus::Variant{std::string{"bad"}}}})));
    CHECK("zero geometry ignored",
          !RestoreData::decode(
              token({{"output", sdbus::Variant{std::string{"DP-1"}}}, {"geometry", sdbus::Variant{sdbus::Struct<uint32_t, uint32_t, uint32_t, uint32_t>{0, 0, 0, 12}}}})));
    CHECK("unknown v3 field retained compatibility",
          RestoreData::decode(token({{"output", sdbus::Variant{std::string{"DP-1"}}}, {"futureField", sdbus::Variant{true}}})).has_value());
    CHECK("old v3 address ignored", RestoreData::decode(token({{"windowClass", sdbus::Variant{std::string{"app"}}}, {"windowHandle", sdbus::Variant{uint64_t{123}}}})).has_value());

    using Legacy      = sdbus::Struct<std::string, uint32_t, std::string, bool, uint64_t>;
    const auto legacy = RestoreData::decode(sdbus::Variant{RestoreData::Token{"abyssland", 2, sdbus::Variant{Legacy{"old", 0, "DP-1", true, 1}}}});
    CHECK("v2 monitor compatibility", legacy && legacy->output == "DP-1" && legacy->cursorMode == 2);
    CHECK("v2 transient window address prompts", !RestoreData::decode(sdbus::Variant{RestoreData::Token{"abyssland", 2, sdbus::Variant{Legacy{"old", 123, "", false, 1}}}}));
    CHECK("malformed v2 ignored", !RestoreData::decode(sdbus::Variant{RestoreData::Token{"abyssland", 2, sdbus::Variant{std::string{"bad"}}}}));
    return fails;
}
