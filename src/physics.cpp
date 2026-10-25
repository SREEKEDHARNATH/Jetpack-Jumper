#include "physics.hpp"
#include "box2d/box2d.h"
#include "box2d/collision.h"
#include "box2d/types.h"
#include <iostream>

void delW(b2WorldId* id){
    if (b2World_IsValid(*id)){
        b2DestroyWorld(*id);
    }
    delete id;
}
void delB(b2BodyId* id){
    if (b2Body_IsValid(*id)){
        b2DestroyBody(*id);
    }
    delete id;
}

namespace app::physics {
    world::world(b2Vec2 gravity){
        b2WorldDef def=b2DefaultWorldDef();
        def.gravity=gravity;
        auto id=b2CreateWorld(&def);
        b2WorldId* wid=new b2WorldId();
        *wid=id;
        if (!wid){
            std::cerr << "Failed to create world\n";
            return;
        }
        ptr.reset(wid, delW);
    }
    void world::simulate(float fps, int substepcount){
        if (b2World_IsValid(get())){
            b2World_Step(get(), 1/fps, substepcount);
        }
    }
    b2WorldId world::get(){
        return *(ptr.get());
    }

    body::body(world w,b2Vec2 pos, float wi, float h,b2BodyType type,bool cir){
        b2BodyDef def=b2DefaultBodyDef();
        def.position=pos;
        def.type=type;
        def.isAwake=true;
        b2BodyId id=b2CreateBody(w.get(), &def);
        b2ShapeId shid;
        b2ShapeDef shdef=b2DefaultShapeDef();
        shdef.density=1.0f;
        shdef.enableContactEvents=true;
        shdef.material.friction=0;
        shdef.material.restitution=0;

        if (cir){
            b2Circle circle;
            circle.radius=wi/2;
            circle.center={0,0};
            shid = b2CreateCircleShape(id, &shdef, &circle);
        }
        else{
            b2Polygon poly=b2MakeBox(wi/2.0f, h/2.0f);
            shid=b2CreatePolygonShape(id, &shdef, &poly);
        }
        if (!b2Shape_IsValid(shid)){
            std::cerr << "Shape Creation failed\n";
            return;
        }
        if (!b2Body_IsValid(id)){
            std::cerr << "Body Creation failed\n";
            return;
        }
        b2BodyId* pt=new b2BodyId();
        *pt=id;
        ptr.reset(pt, delB);
    }
    b2BodyId body::get(){
        return *(ptr.get());
    }
}