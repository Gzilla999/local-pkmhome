#pragma once
// Single source of truth for on-SD paths.
// If the project is ever renamed again, only this file and the Makefile change.
namespace pkmhome::paths {
    inline constexpr const char* kRoot      = "sdmc:/switch/local-pkmhome";
    inline constexpr const char* kImportDir = "sdmc:/switch/local-pkmhome/imports";
    inline constexpr const char* kLibPath   = "sdmc:/switch/local-pkmhome/library.txt";
    inline constexpr const char* kBackupDir = "sdmc:/switch/local-pkmhome/backups";
    inline constexpr const char* kLogDir    = "sdmc:/switch/local-pkmhome/logs";
}
