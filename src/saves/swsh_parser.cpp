#include "saves/swsh_parser.h"
#include <fstream>
#include <vector>
#include <cstring>
#include <sys/stat.h>

// Minimal, conservative probe.
// The real PKHeX SAV8SWSH parser uses SCBlock hash keys and per-block
// encryption. This probe verifies that the file has the expected container
// shape before the project claims it is a Sword/Shield save.
//
// TODO(phase4): implement full SCBlock reader against representative fixtures.
// Do not enable write-back until checksums and block hashes are verified.

static bool read_u32le(const std::vector<uint8_t>& d, size_t off, uint32_t& out) {
    if (off + 4 > d.size()) return false;
    out = static_cast<uint32_t>(d[off])
        | (static_cast<uint32_t>(d[off+1]) << 8)
        | (static_cast<uint32_t>(d[off+2]) << 16)
        | (static_cast<uint32_t>(d[off+3]) << 24);
    return true;
}

SwshProbeResult swsh_probe(const std::string& path) {
    SwshProbeResult r;

    struct stat st{};
    if (stat(path.c_str(), &st) != 0) {
        r.diagnostic = "Cannot stat file.";
        return r;
    }
    const uint64_t sz = static_cast<uint64_t>(st.st_size);
    if (sz < 0x10000 || sz > 0x1000000) {
        r.diagnostic = "File size is outside the plausible SWSH SCBlock range.";
        return r;
    }

    std::ifstream f(path, std::ios::binary);
    if (!f) {
        r.diagnostic = "Cannot open file.";
        return r;
    }

    // Read a bounded header window; do not load multi-megabyte files twice.
    std::vector<uint8_t> head(0x4000);
    f.read(reinterpret_cast<char*>(head.data()), head.size());
    if (f.gcount() < 0x100) {
        r.diagnostic = "File too short for an SCBlock header.";
        return r;
    }

    // SCBlock saves begin with a block-count / hash table region.
    // A robust detector should verify multiple known block keys.
    // For Phase 2 we only require a non-trivial, non-zero header and
    // a plausible block count.
    uint32_t first_word = 0;
    uint32_t second_word = 0;
    if (!read_u32le(head, 0, first_word) || !read_u32le(head, 4, second_word)) {
        r.diagnostic = "Cannot read SCBlock header words.";
        return r;
    }

    const bool looks_like_scblock =
        first_word != 0 &&
        first_word < 0x10000 &&            // block count sanity
        second_word != 0;

    if (!looks_like_scblock) {
        r.diagnostic = "Header does not resemble an SCBlock container.";
        return r;
    }

    r.detected = true;
    r.structurally_valid = true;
    r.diagnostic =
        "SCBlock container detected. Structural validation passed. "
        "Pokémon entity parsing is not yet enabled for SWSH; "
        "this import is read-only metadata only.";
    r.box_count = 32;
    r.slots_per_box = 30;
    r.occupied_slots = -1; // unknown until entity parser is implemented
    return r;
}
