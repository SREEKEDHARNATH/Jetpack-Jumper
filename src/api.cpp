#include "api.hpp"
#include "window.hpp"
#include <SDL_image.h>
#include <SDL_render.h>
#include <iostream>

namespace app::api {
    image::image(const char* name, bool load, bool autoset){
        if (load){
            SDL_Texture* tex=IMG_LoadTexture(getRenderer(), name);
            if (!tex){
                std::cerr << IMG_GetError() << "\n";
                return;
            }
            ptr.reset(tex, SDL_DestroyTexture);
            if (autoset){
                SDL_QueryTexture(tex, NULL, NULL, &dstrect.w, &dstrect.h);
            }
        }
    }
    void image::setTex(SDL_Texture* tex, bool autoset){
        if (tex){
            ptr.reset(tex, SDL_DestroyTexture);
            if (autoset){
                SDL_QueryTexture(tex, NULL, NULL, &dstrect.w, &dstrect.h);
            }
        }
    }
    void image::draw(){
        if (ptr.get()){
            if (dstrect.w || dstrect.h){
                SDL_RenderCopy(getRenderer(), ptr.get(), NULL,&dstrect);
            }
            else {
                SDL_RenderCopy(getRenderer(), ptr.get(), NULL,NULL);
            }
        }
    }
    std::shared_ptr<SDL_Texture> image::getTex(){
        return ptr;
    }
}