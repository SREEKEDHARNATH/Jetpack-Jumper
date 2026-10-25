#pragma once

#include <SDL_render.h>
#include <memory>
namespace app::api {
    class image{
        std::shared_ptr<SDL_Texture> ptr;
        public:
            SDL_Rect dstrect={0};
            image(const char* name, bool load=true, bool autoset=true);
            void setTex(SDL_Texture* tex, bool autoset=true);
            void draw();
            std::shared_ptr<SDL_Texture> getTex();
    };
}