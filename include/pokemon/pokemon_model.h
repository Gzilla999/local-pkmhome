#pragma once
#include <string>
#include <vector>
#include <cstdint>

struct PokemonRecord {
    uint64_t local_id = 0;          // stable, never reused
    uint32_t species = 0;
    uint16_t form = 0;
    std::string nickname;
    std::string ot_name;
    uint32_t pid = 0;
    uint16_t tid16 = 0;
    uint16_t sid16 = 0;
    uint8_t level = 0;
    uint8_t gender = 0;
    uint8_t nature = 0;
    uint8_t ability_slot = 0;
    uint16_t held_item = 0;
    uint8_t ball = 0;
    bool shiny = false;
    bool egg = false;
    bool favourite = false;
    bool locked = false;

    // provenance
    std::string origin_game;
    uint8_t origin_generation = 0;
    std::string source_save_id;
    int source_box = -1;
    int source_slot = -1;
    uint64_t imported_at_unix = 0;

    // capability flags
    bool has_ivs = false;
    bool has_evs = false;
    bool has_tera_type = false;
    bool has_dynamax_level = false;
    bool has_effort_levels = false;

    // optional stored fields
    int ivs[6] = {-1,-1,-1,-1,-1,-1};
    int evs[6] = {-1,-1,-1,-1,-1,-1};
    uint8_t tera_type = 0;
    uint8_t dynamax_level = 0;
    uint8_t effort_levels[6] = {0,0,0,0,0,0};

    // serialized raw source bytes if available
    std::vector<uint8_t> raw_entity;
};
