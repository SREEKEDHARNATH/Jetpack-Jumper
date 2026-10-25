#pragma once

#include "box2d/id.h"
#include "box2d/math_types.h"
#include "box2d/types.h"
#include <memory>
namespace app::physics {
    class world{
        std::shared_ptr<b2WorldId> ptr;
        public:
            world(b2Vec2 gravity={0,10});
            void simulate(float fps=60, int substepcount=4);
            b2WorldId get();
    };

    class body{
        std::shared_ptr<b2BodyId> ptr;
        public:
            body(world w,b2Vec2 pos, float wi, float h,b2BodyType type,bool capsule=false);
            b2BodyId get();
    };
}