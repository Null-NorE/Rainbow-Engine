#pragma once

#include "RE_includes.h"

int initSDLVideo() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    return 0;
}

void quitSDL() {
    SDL_Quit();
}

int GetRefreshRate() {
    SDL_DisplayMode dm;
    if (SDL_GetCurrentDisplayMode(0, &dm) != 0) {
        std::cerr << "SDL_GetCurrentDisplayMode Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
    }

    return dm.refresh_rate;
}