#pragma once
#include <string>
#include <vector>
#include <cstdint>

enum class SaveGame {
    Unknown,
    SwordShield,
    BrilliantDiamondShiningPearl,
    LegendsArceus,
    ScarletViolet
};

struct SaveDetectionResult {
    SaveGame game = SaveGame::Unknown;
    std::string game_name;
    std::string format_name;
    uint64_t file_size = 0;
    bool structurally_valid = false;
    bool read_supported = false;
    std::string diagnostic;
    std::string trainer_name;
    uint16_t trainer_id = 0;
    uint16_t secret_id = 0;
    uint8_t generation = 0;
    int box_count = 0;
    int slots_per_box = 0;
    int occupied_slots = -1;
};

SaveDetectionResult detect_save(const std::string& path);
