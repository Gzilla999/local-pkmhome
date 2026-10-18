#include <switch.h>
#include <SDL2/SDL.h>
#include <cstdio>
#include <string>
#include <vector>
#include <sys/stat.h>
#include <dirent.h>

#include "storage/library.h"
#include "saves/save_detector.h"

#include "platform/paths.h"
namespace paths = pkmhome::paths;

static void ensure_dir(const std::string& d) { mkdir(d.c_str(), 0777); }

static std::vector<std::string> list_imports() {
    std::vector<std::string> out;
    DIR* dir = opendir(kImportDir);
    if (!dir) return out;
    while (dirent* e = readdir(dir)) {
        std::string n = e->d_name;
        if (n == "." || n == "..") continue;
        // Accept extensionless "main" and common save extensions.
        if (n == "main" || n.size() > 4) out.push_back(std::string(kImportDir) + "/" + n);
    }
    closedir(dir);
    return out;
}

static void draw_text(SDL_Renderer* r, int x, int y, const std::string& s) {
    // SDL2_ttf is not included in Phase 2 to reduce dependency risk.
    // Draw a simple 5x7 block font for diagnostics until TTF is verified.
    for (size_t i = 0; i < s.size(); ++i) {
        SDL_Rect g{x + static_cast<int>(i) * 6, y, 4, 6};
        SDL_SetRenderDrawColor(r, 230, 230, 230, 255);
        SDL_RenderFillRect(r, &g);
    }
}

int main(int argc, char** argv) {
    fsInitialize();
    fsdevMountSdmc();

    ensure_dir(kRoot);
    ensure_dir(kImportDir);
    ensure_dir(kBackupDir);

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
    SDL_Window* win = SDL_CreateWindow("Local Pokemon Home", 0, 0, 1280, 720, 0);
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    Library lib;
    lib.load(kLibPath);

    std::string status = "Press A to import from " + std::string(kImportDir);
    std::vector<std::string> files;
    int selected = 0;
    bool running = true;

    while (appletMainLoop() && running) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) running = false;
            if (ev.type == SDL_KEYDOWN) {
                switch (ev.key.keysym.sym) {
                    case SDLK_UP: if (selected > 0) --selected; break;
                    case SDLK_DOWN: if (selected + 1 < (int)files.size()) ++selected; break;
                    case SDLK_RETURN:
                    case SDLK_a: {
                        files = list_imports();
                        if (files.empty()) {
                            status = "No files in imports/. Export a save from Eden and place it there.";
                        } else {
                            SaveDetectionResult d = detect_save(files[selected]);
                            status = d.game_name.empty()
                                ? "Unsupported: " + d.diagnostic
                                : d.game_name + " — " + d.diagnostic;
                            if (d.structurally_valid) {
                                ImportedSave s;
                                s.save_id = "save-" + std::to_string(lib.saves().size() + 1);
                                s.path = files[selected];
                                s.game_name = d.game_name;
                                s.file_size = d.file_size;
                                s.imported_at_unix = static_cast<uint64_t>(time(nullptr));
                                s.read_only = true;
                                lib.saves().push_back(s);
                                lib.save();
                            }
                        }
                        break;
                    }
                    case SDLK_b:
                    case SDLK_ESCAPE:
                        running = false;
                        break;
                    case SDLK_y:
                        status = "Backup: " + lib.export_backup(kBackupDir);
                        break;
                }
            }
        }

        SDL_SetRenderDrawColor(ren, 18, 20, 26, 255);
        SDL_RenderClear(ren);

        draw_text(ren, 40, 30, "POKEMON VAULT  v0.1.0");
        draw_text(ren, 40, 60, "Local library: " + std::to_string(lib.saves().size()) + " saves");
        draw_text(ren, 40, 90, status);

        int y = 140;
        for (size_t i = 0; i < files.size() && i < 12; ++i) {
            std::string label = (static_cast<int>(i) == selected ? "> " : "  ") + files[i];
            draw_text(ren, 40, y, label);
            y += 20;
        }

        draw_text(ren, 40, 660, "A: import selected   Y: backup library   B: exit");
        SDL_RenderPresent(ren);
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    fsdevUnmountAll();
    fsExit();
    return 0;
}
