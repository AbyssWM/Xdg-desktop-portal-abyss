#include "../src/helpers/MiscFunctions.hpp"

#include <cstdio>

int main() {
    int fails = 0;
    for (const auto DESKTOPS : {"Abyssland", "Abyssland:wlroots", "wlroots:Abyssland", ":Abyssland::"}) {
        if (!desktopListContains(DESKTOPS, "Abyssland")) {
            std::printf("FAIL expected Abyssland in %s\n", DESKTOPS);
            ++fails;
        }
    }
    for (const auto DESKTOPS : {"", "wlroots", "OtherAbyssland", "AbysslandOther", "abyssland", "::"}) {
        if (desktopListContains(DESKTOPS, "Abyssland")) {
            std::printf("FAIL unexpected Abyssland in %s\n", DESKTOPS);
            ++fails;
        }
    }
    if (desktopListContains("Abyssland::", ""))
        ++fails;
    return fails;
}
