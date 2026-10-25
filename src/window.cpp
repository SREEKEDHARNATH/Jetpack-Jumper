#include "window.hpp"
#include <SDL.h>
#include <SDL_error.h>
#include <SDL_events.h>
#include <SDL_keycode.h>
#include <SDL_mouse.h>
#include <SDL_pixels.h>
#include <SDL_rect.h>
#include <SDL_render.h>
#include <SDL_stdinc.h>
#include <SDL_surface.h>
#include <SDL_timer.h>
#include <SDL_video.h>
#include <SDL_image.h>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <utility>
#include <vector>
#include "api.hpp"
#include "box2d/box2d.h"
#include "box2d/types.h"
#include "listeners.hpp"
#include "physics.hpp"
#include <SDL2/SDL_ttf.h>

std::pair<bool, const char*> init(){
    int img_flags=IMG_INIT_JPG;
    if (SDL_Init(SDL_INIT_EVERYTHING)){
        return {false, SDL_GetError()};
    }
    if ((IMG_Init(img_flags) & img_flags) != img_flags){
        return {false, IMG_GetError()};
    }
    if (TTF_Init()){
        return {false, TTF_GetError()};
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
std::shared_ptr<SDL_Texture> getRGB(int w,int h,Uint8 r, Uint8 g, Uint8 b){
    std::shared_ptr<SDL_Texture> ptr;
    SDL_Surface* surf=SDL_CreateRGBSurface(0, w, h, 32, 0,0,0,0);
    if (!surf){
        std::cerr << SDL_GetError() << "\n";
        return ptr;
    }
    if (SDL_FillRect(surf, NULL, SDL_MapRGB(surf->format, r, g, b))){
        std::cerr << SDL_GetError() << "\n";
        return ptr;
    }
    SDL_Texture* tex=SDL_CreateTextureFromSurface(app::getRenderer(), surf);;
    if (!tex){
        std::cerr << SDL_GetError() << "\n";
        return ptr;
    }
    ptr.reset(tex, SDL_DestroyTexture);
    return ptr;

}
class platform{
    app::api::image img;
    std::shared_ptr<SDL_Texture> ptr;
    std::vector<SDL_Rect> pos;
    std::vector<app::physics::body> bodies;
    public:
        platform(app::physics::world w, Uint16 num=100, int gap=128) : img("", false){
            Uint16 width=128;
            Uint16 height=32;
            ptr = getRGB(width,height, 255,0,0);
            img.setTex(ptr.get());
            int curY=app::getH()-height;
            pos.resize(num);
            int x=0;
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<int> distrib(0,1);

            SDL_Rect a={0,curY,width,height};
            pos[0]=a;
            bodies.emplace_back(w, (b2Vec2){(a.x+a.w/2.0f)/32.0f,a.y/32.0f}, a.w/32.0f, a.h/32.0f, b2_staticBody);

            for (int i=1;i<num;i++){
                switch (x) {
                    case 0:
                        if (distrib(gen)){
                            x=560;
                        }
                        break;
                    case 560:
                        if (distrib(gen)){
                            x=1152;
                        }
                        else {
                            x=0;
                        }
                        break;
                    case 1152:
                        if (distrib(gen)){
                            x=560;
                        }
                        break;
                        
                }
                curY-=gap;
                a={x,curY,width,height};
                pos[i]=a;
                bodies.emplace_back(w, (b2Vec2){(a.x+a.w/2.0f)/32.0f,a.y/32.0f}, a.w/32.0f, a.h/32.0f, b2_staticBody);
                
            }

        }
        void draw(){
            for (auto& a : pos){
                if (SDL_HasIntersection(&a, &app::getCamera())){
                    img.dstrect=a;
                    img.dstrect.y-=(app::getCamera().y);
                    img.draw();
                }
            }
        }
};
class text{
    app::api::image img;
    std::string txt;
    std::shared_ptr<TTF_Font> ptr;
    void init(const char* t){
        SDL_Surface* surf = TTF_RenderUTF8_Blended(ptr.get(), t, {255,255,255,255});
        if (!surf){
            std::cerr << IMG_GetError() << "\n";
            return;
        }
        SDL_Texture* tex=SDL_CreateTextureFromSurface(app::getRenderer(), surf);
        if (!tex){
            std::cerr << SDL_GetError() << "\n";
            return;
        }
        SDL_FreeSurface(surf);
        img.setTex(tex);
    }
    public:
        text(const char* text, const char* fontfile, int ptsize) : img("", false){
            txt=text;
            TTF_Font* font=TTF_OpenFont(fontfile, ptsize);
            if (!font){
                std::cerr << IMG_GetError() << "\n";
                return;
            }
            ptr.reset(font, TTF_CloseFont);
            init(text);
        }
        void draw(){
            img.draw();
        }
        void setText(const char* t){
            if (ptr.get() && img.getTex().get()){
                init(t);
            }
        }
        std::string getText(){
            return txt;
        }
};
std::string getIndicator(float fuel, float maxfuel){
    int i=std::ceil(((fuel/maxfuel)*100));
    std::string st;
    for (int j=0;j<i;j++){
        st+="|";
    }
    return st;
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

        physics::world world;
        api::image im("assets/images/little-guy.jpg");
        physics::body body(world, (b2Vec2){0,660/32.0f},im.dstrect.w/32.0f,im.dstrect.h/32.0f, b2_dynamicBody, true);
        platform pl(world);
        b2Vec2 vel={0,0};
        
        constexpr int basespeed=5;
        constexpr float maxfuel=8.0f;
        float fuel=maxfuel;
        
        text t("Initialising", "assets/fonts/Montserrat-BoldItalic.ttf", 14);
        std::string te;
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

            if (keylistener::isPressed(SDLK_ESCAPE)){
                running=false;
            }
            if (keylistener::isDown(SDLK_RIGHT)){
                vel.x=basespeed;
            }
            else if (keylistener::isDown(SDLK_LEFT)){
                vel.x=-1*basespeed;
            }
            else {
                vel.x=0;
            }
            if (keylistener::isDown(SDLK_SPACE)){
                if (fuel>0){
                    vel.y-=0.05f;
                    fuel-=0.05f;
                }
                else {
                    vel.y=0;    
                }
            }
            else {
                if (fuel < maxfuel){
                    fuel+=0.05f;
                }
                vel.y=0;
            }
            auto a=getIndicator(fuel, maxfuel);
            if (te != a){
                te=a;
                t.setText(a.c_str());
            }

            if (vel.x!=0 || vel.y!=0){
                b2Body_ApplyForceToCenter(body.get(), vel, false);
            }
            world.simulate();
            auto pos=b2Body_GetPosition(body.get());
            im.dstrect.x=pos.x*32;
            im.dstrect.y=pos.y*32;
            getCamera().y=im.dstrect.y+im.dstrect.h/2 - getH()/2;

            SDL_RenderClear(getRenderer());
            pl.draw();
            im.draw();
            t.draw();
            SDL_RenderPresent(getRenderer());
            
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
        TTF_Quit();
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

    SDL_Rect& getCamera(){
        static SDL_Rect camera={0,0,getW(),getH()};
        return camera;
    }
}