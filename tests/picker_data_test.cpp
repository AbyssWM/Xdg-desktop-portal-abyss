#include "../src/shared/PickerData.hpp"

#include <cstdio>

static int fails = 0;
#define CHECK(name, cond)                                                                                                                                                          \
    do {                                                                                                                                                                           \
        const bool OK = (cond);                                                                                                                                                    \
        std::printf("%s %s\n", OK ? "PASS" : "FAIL", name);                                                                                                                        \
        fails += !OK;                                                                                                                                                              \
    } while (false)

int main() {
    auto result = PickerData::parseSelection("[SELECTION]r/screen:Output with spaces \r\n");
    CHECK("screen spaces and CRLF retained", result && result->output == "Output with spaces " && result->allowToken);
    result = PickerData::parseSelection("diagnostic\n[SELECTION]/screen:DP-1");
    CHECK("screen without final newline", result && result->output == "DP-1" && !result->allowToken);
    result = PickerData::parseSelection("[SELECTION]r/window:4294967295\n");
    CHECK("unsigned window ID and restore consent", result && result->type == PickerData::eType::WINDOW && result->id == 4294967295U && result->allowToken);
    result = PickerData::parseSelection("[SELECTION]r/region:Output @ name@10,20,100,200\n");
    CHECK("region output names and coordinates", result && result->output == "Output @ name" && result->x == 10 && result->y == 20 && result->w == 100 && result->h == 200);
    for (const auto OUTPUT :
         {"", "[SELECTION]", "[SELECTION]rscreen:DP-1", "[SELECTION]x/screen:DP-1", "[SELECTION]/screen:", "[SELECTION]/window:", "[SELECTION]/window:0", "[SELECTION]/window:-1",
          "[SELECTION]/window:4294967296", "[SELECTION]/window:12junk", "[SELECTION]/region:DP-1@1,2,3", "[SELECTION]/region:DP-1@1,2,3,4,5", "[SELECTION]/region:DP-1@1,2,0,4",
          "[SELECTION]/region:DP-1@-1,2,3,4", "[SELECTION]/region:DP-1@1,2,4294967295,4", "[SELECTION]/unknown:DP-1", "[SELECTION]/screen:DP-1\n[SELECTION]/screen:DP-2",
          "log [SELECTION]/screen:DP-1"})
        CHECK("malformed selection ignored", !PickerData::parseSelection(OUTPUT));

    PickerData::SWindow window{"quoted '\" $` title", "Firefox", 123, 456};
    auto                windows = PickerData::parseWindows(PickerData::encodeWindow(window));
    CHECK("list producer/consumer roundtrip preserves literal text",
          windows.size() == 1 && windows[0].name == window.name && windows[0].clazz == "Firefox" && windows[0].id == 123 && windows[0].address == 456);
    window.name = "malicious[HC>]1[HT>]app[HE>]0[HA>]";
    windows     = PickerData::parseWindows(PickerData::encodeWindow(window));
    CHECK("list delimiters cannot inject entries", windows.size() == 1 && windows[0].id == 123);
    for (const auto LIST : {"", "not a list", "1[HC>]app", "x[HC>]app[HT>]title[HE>]0[HA>]", "4294967296[HC>]app[HT>]title[HE>]0[HA>]", "1[HC>]app[HT>]title[HE>]bad[HA>]"})
        CHECK("malformed window list ignored", PickerData::parseWindows(LIST).empty());
    return fails;
}
