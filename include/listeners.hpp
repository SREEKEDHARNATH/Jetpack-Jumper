#pragma once

#include <SDL_events.h>
#include <SDL_keycode.h>
namespace app::keylistener {
    void reg(SDL_KeyboardEvent key);

    bool isPressed(SDL_KeyCode k);
    bool isPressed(SDL_Scancode sc);
    
    bool isDown(SDL_KeyCode k);
    bool isDown(SDL_Scancode sc);
    
    void update();
}

namespace app::mouselistener {
    void reg(SDL_MouseMotionEvent motion);
    void reg(SDL_MouseButtonEvent button);
    void reg(SDL_MouseWheelEvent wheel);

    bool isPressed(int b);
    bool isDown(int b);
    
    int getX();
    int getY();
    
    float getScrlX();
    float getScrlY();
    
    void update();
}
