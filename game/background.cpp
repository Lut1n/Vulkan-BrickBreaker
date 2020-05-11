#include "game/background.h"
#include "app/assetsloader.h"

//--------------------------------------------------------------
Background::Background()
{
    mesh = AssetsLoader::instance()->getOrLoadMesh(MODEL_BACKGROUND_PATH);
    material = AssetsLoader::instance()->getOrLoadTexture("textures/background.jpg");
    drawable = DrawableAllocator::instance()->alloc();

    color = glm::vec3(0.7,0.7,1);
    color = glm::vec3(1,1,1);
    position = glm::vec3(0,0,0);
    rotation = glm::vec3(0,0,0);
    scale = glm::vec3(1,1,1);
}

//--------------------------------------------------------------
Background::~Background()
{
    DrawableAllocator::instance()->free(drawable);
    drawable = nullptr;
}

//--------------------------------------------------------------
void Background::animate(State state, float time)
{
    drawable->updateMatrix(position, rotation, scale,color);
}
