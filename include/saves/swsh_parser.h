#pragma once
#include <string>
#include <cstdint>

struct SwshProbeResult {
    bool detected = false;
    bool structurally_valid = false;
    std::string diagnostic;
    std::string trainer_name;
    uint16_t trainer_id = 0;
    uint16_t secret_id = 0;
    int box_count = 0;
    int slots_per_box = 0;
    int occupied_slots = -1;
};

// Read-only structural probe. Does not decrypt or decode Pokémon entities yet.
SwshProbeResult swsh_probe(const std::string& path);
