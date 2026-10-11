#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

// Pure logic for re-finding a window from a screencast restore token. No wayland deps.
namespace RestoreMatch {
    struct SCandidate {
        std::string windowClass;
        std::string windowTitle;
        uint64_t    handleId = 0; // legacy value, never trusted for persistent matching
    };

    struct SStored {
        std::string windowClass;
        std::string windowTitle;  // empty = unknown (old token)
        uint64_t    handleId = 0; // legacy value, ignored across reconnects and restarts
    };

    std::string normalizeTitle(const std::string& title);
    double      titleScore(const std::string& stored, const std::string& cand);

    // Returns index into `cands`, or nullopt if no safe unambiguous match (caller must prompt).
    std::optional<size_t> findWindow(const SStored& stored, const std::vector<SCandidate>& cands);
}
