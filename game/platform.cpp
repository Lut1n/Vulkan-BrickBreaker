#include "game/platform.h"
#include "app/assetsloader.h"

//--------------------------------------------------------------
Platform::Platform()
{
    mesh = AssetsLoader::instance()->getOrLoadMesh(MODEL_PLATFORM_PATH);
    material = AssetsLoader::instance()->getOrLoadTexture("textures/lava.jpg");
    drawable = DrawableAllocator::instance()->alloc();

    color = glm::vec3(1,0.5,0.3);
    position = glm::vec3(0,0,0);
    rotation = glm::vec3(0,0,0);
    scale = glm::vec3(1,1,1);

    bounds.center = position;
    bounds.tl = glm::vec3(-20.0,0.0,0.0);
    bounds.br = glm::vec3(20.0,-10.0,0.0);
}

//--------------------------------------------------------------
Platform::~Platform()
{
    DrawableAllocator::instance()->free(drawable);
    drawable = nullptr;
}

//--------------------------------------------------------------
void Platform::animate(State state, float time)
{
    position.y += std::sin(time*4.0) * 0.002;
    bounds.center = position;
    drawable->updateMatrix(position, rotation, scale,color);
}
