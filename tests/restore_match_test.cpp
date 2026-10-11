#include "../src/shared/RestoreMatch.hpp"
#include <cstdio>
using namespace RestoreMatch;

static int fails = 0;
#define CHECK(name, cond)                                                                                                                                                          \
    do {                                                                                                                                                                           \
        bool ok = (cond);                                                                                                                                                          \
        std::printf("%s %s\n", ok ? "PASS" : "FAIL", name);                                                                                                                        \
        fails += !ok;                                                                                                                                                              \
    } while (0)

int main() {
    {
        std::vector<SCandidate> c{{"kitty", "term", 1}, {"firefox", "Foo", 2}};
        auto                    r = findWindow({"Firefox", "whatever", 99}, c);
        CHECK("single match (case-insens, any title)", r && *r == 1);
        CHECK("no match -> prompt", !findWindow({"discord", "x", 0}, c));
    }
    {
        std::vector<SCandidate> c{
            {"firefox", "Inbox \xE2\x80\x94 Mozilla Firefox", 1}, {"firefox", "YouTube - Mozilla Firefox", 2}, {"firefox", "Gitea \xE2\x80\x94 Mozilla Firefox", 3}};
        auto r = findWindow({"firefox", "YouTube - Mozilla Firefox", 777}, c);
        CHECK("several firefox, exact title", r && *r == 1);
        r = findWindow({"firefox", "Inbox \xE2\x80\x94 Mozilla Firefox", 777}, c);
        CHECK("several firefox, exact title (em dash)", r && *r == 0);
        r = findWindow({"firefox", "Totally Unrelated Page - Mozilla Firefox", 777}, c);
        CHECK("several firefox, unrelated title -> prompt", !r);
    }
    {
        std::vector<SCandidate> c{{"discord", "(2) Discord", 1}, {"discord", "Settings", 2}};
        auto                    r = findWindow({"discord", "Discord", 0}, c);
        CHECK("counter changed: Discord vs (2) Discord", r && *r == 0);
        std::vector<SCandidate> c2{{"app", "Document 12 - App", 1}, {"app", "Other thing", 2}};
        r = findWindow({"app", "Document 3", 0}, c2);
        CHECK("trailing digits + suffix normalised", r && *r == 0);
    }
    {
        std::vector<SCandidate> c{{"foot", "shell", 10}, {"foot", "shell", 11}};
        CHECK("two identical titles, no handle -> prompt", !findWindow({"foot", "shell", 0}, c));
        CHECK("two identical titles, stale handle -> prompt", !findWindow({"foot", "shell", 99}, c));
        auto r = findWindow({"foot", "shell", 11}, c);
        CHECK("identical titles, reused pointer -> prompt", !r);
    }
    {
        std::vector<SCandidate> c{{"", "Weird Window", 1}, {"", "Other", 2}, {"x", "Weird Window", 3}};
        CHECK("empty class, title ambiguous -> prompt", !findWindow({"", "Weird Window", 0}, c));
        std::vector<SCandidate> d{{"", "Weird Window", 1}, {"", "Other", 2}};
        auto                    r = findWindow({"", "Weird Window", 0}, d);
        CHECK("empty class, exact title unique", r && *r == 0);
        CHECK("empty class, title differs -> prompt", !findWindow({"", "Weird Windo", 0}, d));
        CHECK("empty class + empty title -> prompt", !findWindow({"", "", 0}, d));
    }
    {
        std::vector<SCandidate> one{{"kitty", "a", 1}};
        CHECK("old token (no title), single window", findWindow({"kitty", "", 5}, one).has_value());
        std::vector<SCandidate> two{{"kitty", "a", 1}, {"kitty", "b", 2}};
        CHECK("old token, two windows, no handle hit -> prompt", !findWindow({"kitty", "", 5}, two));
        auto r = findWindow({"kitty", "", 2}, two);
        CHECK("old token, two windows, reused pointer -> prompt", !r);
    }
    std::printf("%s\n", fails ? "FAILED" : "ALL OK");
    return fails;
}
