#pragma once
#include <SDL.h>

namespace app {
    void run();
    void quit();

    int getW();
    int getH();
    const char* getTitle();
    
    SDL_Window*& getWindow();
    SDL_Renderer*& getRenderer();

    SDL_Rect& getCamera();
}