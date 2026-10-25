#include "window.hpp"
#include <SDL.h>
#include <SDL_error.h>
#include <SDL_events.h>
#include <SDL_keycode.h>
#include <SDL_mouse.h>
#include <SDL_render.h>
#include <SDL_timer.h>
#include <SDL_video.h>
#include <SDL_image.h>
#include <iostream>
#include <utility>
#include "listeners.hpp"

std::pair<bool, const char*> init(){
    int img_flags=IMG_INIT_JPG;
    if (SDL_Init(SDL_INIT_EVERYTHING)){
        return {false, SDL_GetError()};
    }
    if ((IMG_Init(img_flags) & img_flags) != img_flags){
        return {false, IMG_GetError()};
    }
    app::getWindow()=SDL_CreateWindow(app::getTitle(), SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED, app::getW(), app::getH(), 0);
    if (!app::getWindow()){
        return {false, SDL_GetError()};
    }
    app::getRenderer()=SDL_CreateRenderer(app::getWindow(), -1, 0);
    if (!app::getRenderer()){
        return {false, SDL_GetError()};
    }
    return {true, "Great"};;
}
namespace app {
    void run(){
        auto i=init();
        if (!i.first){
            std::cerr << i.second<<"\n";
            return;
        }

        SDL_Event e;
        bool running=true;
        double fps=60;

        while (running) {
            while (SDL_PollEvent(&e)) {
                switch (e.type) {
                    case SDL_QUIT:
                        running=false;
                        break;
                    case SDL_KEYUP:
                    case SDL_KEYDOWN:
                        keylistener::reg(e.key);
                        break;
                    case SDL_MOUSEMOTION:
                        mouselistener::reg(e.motion);
                        break;
                    case SDL_MOUSEBUTTONDOWN:
                    case SDL_MOUSEBUTTONUP:
                        mouselistener::reg(e.button);
                        break;
                    case SDL_MOUSEWHEEL:
                        mouselistener::reg(e.wheel);
                        break;
                    
                }
            }

            SDL_RenderClear(getRenderer());
            SDL_RenderPresent(getRenderer());
            
            if (keylistener::isPressed(SDLK_ESCAPE)){
                running=false;
            }
            std::cout << mouselistener::getScrlY()<<"\n";
            
            
            keylistener::update();
            mouselistener::update();
            SDL_Delay(1000/fps);
        }
    }
    void quit(){
        if (getRenderer()){
            SDL_DestroyRenderer(getRenderer());
        }
        if (getWindow()){
            SDL_DestroyWindow(getWindow());
        }
        IMG_Quit();
        SDL_Quit();
    }

    int getW(){
        return 1280;
    }
    int getH(){
        return 720;
    }
    const char* getTitle(){
        return "Jetpack Jumper";
    }
    
    SDL_Window*& getWindow(){
        static SDL_Window* window=nullptr;
        return window;
    }
    SDL_Renderer*& getRenderer(){
        static SDL_Renderer* renderer=nullptr;
        return renderer;
    }
}