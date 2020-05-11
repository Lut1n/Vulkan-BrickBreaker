#include "game/brick.h"
#include "app/assetsloader.h"

//--------------------------------------------------------------
Brick::Brick()
    : m_timeOffset(0.0)
    , toBreak(false)
{
    mesh = AssetsLoader::instance()->getOrLoadMesh(MODEL_BLOCK_PATH);
    material = AssetsLoader::instance()->getOrLoadTexture(TEXTURE_PATH);
    drawable = DrawableAllocator::instance()->alloc();

    color = glm::vec3(1,1,1);
    position = glm::vec3(0,0,0);
    rotation = glm::vec3(0,0,0);
    scale = glm::vec3(1,1,1);

    bounds.center = position;
    bounds.tl = glm::vec3(-0.5 * 1.4,0.5 * 1.4,0.0);
    bounds.br = glm::vec3(0.5 * 1.4,-0.5 * 1.4,0.0);
}

//--------------------------------------------------------------
Brick::~Brick()
{
    DrawableAllocator::instance()->free(drawable);
    drawable = nullptr;
}

//--------------------------------------------------------------
void Brick::animate(State state, float time)
{
    if(!toBreak)
    {
        position.z += std::sin((time-m_timeOffset)*4.0) * 0.001;
        bounds.center = position;
        drawable->updateMatrix(position, rotation, scale,color);
    }
}
