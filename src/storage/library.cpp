#include "storage/library.h"
#include <fstream>
#include <sstream>
#include <ctime>
#include <sys/stat.h>

static bool ensure_dir(const std::string& d) {
    return mkdir(d.c_str(), 0777) == 0 || errno == EEXIST;
}

bool Library::load(const std::string& path) {
    path_ = path;
    std::ifstream f(path_, std::ios::binary);
    if (!f) return false;
    std::string magic;
    int version = 0;
    f >> magic >> version;
    if (magic != "PVLT" || version != 1) return false;
    // Minimal line-oriented record store for Phase 2/3.
    // Replace with a checksummed transactional store before public release.
    std::string line;
    std::getline(f, line); // rest of header line
    while (std::getline(f, line)) {
        if (line.rfind("SAVE ", 0) == 0) {
            std::istringstream ss(line.substr(5));
            ImportedSave s;
            ss >> s.save_id >> s.file_size >> s.imported_at_unix
               >> std::quoted(s.path) >> std::quoted(s.game_name);
            s.read_only = true;
            saves_.push_back(s);
        } else if (line.rfind("MON ", 0) == 0) {
            // Pokémon records are written by the entity parser in Phase 4.
            // Phase 2 only stores saves and diagnostics.
        } else if (line.rfind("NEXTID ", 0) == 0) {
            next_id_ = std::stoull(line.substr(7));
        }
    }
    return true;
}

bool Library::save() const {
    std::ofstream f(path_, std::ios::binary | std::ios::trunc);
    if (!f) return false;
    f << "PVLT 1\n";
    f << "NEXTID " << next_id_ << "\n";
    for (const auto& s : saves_) {
        f << "SAVE " << s.save_id << ' ' << s.file_size << ' '
          << s.imported_at_unix << ' ' << std::quoted(s.path) << ' '
          << std::quoted(s.game_name) << '\n';
    }
    return f.good();
}

uint64_t Library::next_local_id() { return next_id_++; }
std::vector<PokemonRecord>& Library::pokemon() { return pokemon_; }
const std::vector<PokemonRecord>& Library::pokemon() const { return pokemon_; }
std::vector<ImportedSave>& Library::saves() { return saves_; }
const std::vector<ImportedSave>& Library::saves() const { return saves_; }

std::string Library::export_backup(const std::string& backup_dir) const {
    ensure_dir(backup_dir);
    const std::string out = backup_dir + "/library-backup.txt";
    std::ofstream f(out, std::ios::binary | std::ios::trunc);
    if (!f) return {};
    f << "PVLT 1\n";
    for (const auto& s : saves_) {
        f << "SAVE " << s.save_id << ' ' << s.file_size << ' '
          << s.imported_at_unix << ' ' << std::quoted(s.path) << ' '
          << std::quoted(s.game_name) << '\n';
    }
    return out;
}
