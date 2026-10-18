#pragma once
#include "pokemon/pokemon_model.h"
#include <string>
#include <vector>
#include <cstdint>

struct ImportedSave {
    std::string save_id;
    std::string path;
    std::string game_name;
    uint64_t imported_at_unix = 0;
    uint64_t file_size = 0;
    bool read_only = true;
};

class Library {
public:
    bool load(const std::string& path);
    bool save() const;

    uint64_t next_local_id();
    std::vector<PokemonRecord>& pokemon();
    const std::vector<PokemonRecord>& pokemon() const;

    std::vector<ImportedSave>& saves();
    const std::vector<ImportedSave>& saves() const;

    std::string export_backup(const std::string& backup_dir) const;

private:
    std::string path_;
    uint64_t next_id_ = 1;
    std::vector<PokemonRecord> pokemon_;
    std::vector<ImportedSave> saves_;
};
