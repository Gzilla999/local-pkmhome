#include "saves/save_detector.h"
#include "saves/swsh_parser.h"
#include <sys/stat.h>
#include <cstdio>
#include <cstring>

static uint64_t file_size_of(const std::string& path) {
    struct stat st{};
    if (stat(path.c_str(), &st) != 0) return 0;
    return static_cast<uint64_t>(st.st_size);
}

SaveDetectionResult detect_save(const std::string& path) {
    SaveDetectionResult r;
    r.file_size = file_size_of(path);
    if (r.file_size == 0) {
        r.diagnostic = "File missing or empty.";
        return r;
    }

    // Pokémon Sword/Shield saves exported by Eden are SCBlock containers.
    // Detection is structural: look for SCBlock magic and plausible size.
    // This does NOT yet parse Pokémon entities.
    SwshProbeResult probe = swsh_probe(path);
    if (probe.detected) {
        r.game = SaveGame::SwordShield;
        r.game_name = "Pokémon Sword / Shield";
        r.format_name = "SCBlock (main)";
        r.generation = 8;
        r.structurally_valid = probe.structurally_valid;
        r.read_supported = true; // read-only structural support only
        r.diagnostic = probe.diagnostic;
        r.trainer_name = probe.trainer_name;
        r.trainer_id = probe.trainer_id;
        r.secret_id = probe.secret_id;
        r.box_count = probe.box_count;
        r.slots_per_box = probe.slots_per_box;
        r.occupied_slots = probe.occupied_slots;
        return r;
    }

    r.diagnostic = "No supported save signature detected. "
                   "Expected a Pokémon Sword/Shield SCBlock save exported from Eden.";
    return r;
}
