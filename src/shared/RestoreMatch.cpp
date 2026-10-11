#include "RestoreMatch.hpp"

#include <algorithm>
#include <cctype>

namespace RestoreMatch {
    static constexpr double MIN_SCORE = 0.6;  // best must reach this
    static constexpr double MIN_MARGIN = 0.15; // and lead the runner-up by this

    static std::string lower(std::string s) {
        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
        return s;
    }

    static bool endsWithSep(const std::string& s, const std::string& sep, size_t& pos) {
        pos = s.rfind(sep);
        return pos != std::string::npos && pos > 0;
    }

    std::string normalizeTitle(const std::string& title) {
        std::string s = title;

        // leading unread counters: "(3) ", "[12] "
        while (!s.empty() && (s[0] == '(' || s[0] == '[')) {
            const char close = s[0] == '(' ? ')' : ']';
            size_t     e     = s.find(close);
            if (e == std::string::npos || e < 2)
                break;
            bool digits = true;
            for (size_t i = 1; i < e; ++i)
                digits = digits && std::isdigit((unsigned char)s[i]);
            if (!digits)
                break;
            s = s.substr(e + 1);
            while (!s.empty() && std::isspace((unsigned char)s[0]))
                s.erase(0, 1);
        }

        // trailing " — App" / " - App" suffix (last segment)
        size_t pos;
        for (const char* sep : {" \xE2\x80\x94 ", " - ", " \xE2\x80\x93 "}) {
            if (endsWithSep(s, sep, pos)) {
                s = s.substr(0, pos);
                break;
            }
        }

        // trailing digits / whitespace
        while (!s.empty() && (std::isdigit((unsigned char)s.back()) || std::isspace((unsigned char)s.back())))
            s.pop_back();

        return lower(s);
    }

    double titleScore(const std::string& stored, const std::string& cand) {
        if (stored == cand)
            return 1.0;

        const auto a = normalizeTitle(stored), b = normalizeTitle(cand);
        if (a == b && !a.empty())
            return 0.9;
        if (a.empty() || b.empty())
            return 0.0;

        size_t p = 0;
        while (p < a.size() && p < b.size() && a[p] == b[p])
            ++p;
        size_t s = 0;
        while (s < a.size() - p && s < b.size() - p && a[a.size() - 1 - s] == b[b.size() - 1 - s])
            ++s;

        const double ratio = double(p + s) / double(std::max(a.size(), b.size()));
        return std::min(0.8, ratio * 0.8);
    }

    std::optional<size_t> findWindow(const SStored& stored, const std::vector<SCandidate>& cands) {
        // title-only mode: app_id was empty at selection time
        if (stored.windowClass.empty()) {
            if (stored.windowTitle.empty())
                return std::nullopt;
            std::optional<size_t> found;
            for (size_t i = 0; i < cands.size(); ++i) {
                if (cands[i].windowTitle != stored.windowTitle)
                    continue;
                if (found)
                    return std::nullopt; // ambiguous
                found = i;
            }
            return found;
        }

        const auto cls = lower(stored.windowClass);
        std::vector<size_t> idx;
        for (size_t i = 0; i < cands.size(); ++i)
            if (lower(cands[i].windowClass) == cls)
                idx.push_back(i);

        if (idx.empty())
            return std::nullopt;
        if (idx.size() == 1)
            return idx[0];

        std::vector<double> scores;
        for (auto i : idx)
            scores.push_back(stored.windowTitle.empty() ? 0.0 : titleScore(stored.windowTitle, cands[i].windowTitle));

        const double best = *std::max_element(scores.begin(), scores.end());

        // title unknown / all equally good: only the handle (same class) can break the tie
        std::vector<size_t> top;
        for (size_t k = 0; k < idx.size(); ++k)
            if (scores[k] == best)
                top.push_back(k);

        if (!stored.windowTitle.empty() && best < MIN_SCORE)
            return std::nullopt;

        if (top.size() > 1) {
            if (stored.handleId == 0)
                return std::nullopt;
            std::optional<size_t> hit;
            for (auto k : top) {
                if (cands[idx[k]].handleId == stored.handleId) {
                    if (hit)
                        return std::nullopt;
                    hit = idx[k];
                }
            }
            // an unknown-title (old) token only gets here with the handle hit; fine
            return hit;
        }

        double second = 0.0;
        for (size_t k = 0; k < idx.size(); ++k)
            if (k != top[0])
                second = std::max(second, scores[k]);

        if (best - second < MIN_MARGIN)
            return std::nullopt;

        return idx[top[0]];
    }
}
