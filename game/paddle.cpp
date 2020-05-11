#include "game/paddle.h"
#include "app/assetsloader.h"

//--------------------------------------------------------------
Paddle::Paddle()
    : firstFrame(true)
{
    mesh = AssetsLoader::instance()->getOrLoadMesh(MODEL_BAR_PATH);
    material = AssetsLoader::instance()->getOrLoadTexture("textures/randomtex.jpg");
    drawable = DrawableAllocator::instance()->alloc();

    color = glm::vec3(0.5,1,0.5);
    position = glm::vec3(0,0,0);
    rotation = glm::vec3(0,0,0);
    scale = glm::vec3(1,1,1);

    bounds.center = position;
    bounds.tl = glm::vec3(-2.5,1.0,0.0);
    bounds.br = glm::vec3(2.5,-1.0,0.0);
}

//--------------------------------------------------------------
Paddle::~Paddle()
{
    DrawableAllocator::instance()->free(drawable);
    drawable = nullptr;
}

//--------------------------------------------------------------
void Paddle::animate(State state, float time)
{
    if(firstFrame) lastUpdateTime = time;

    if(time != lastUpdateTime)
    {
        float elapsed = time-lastUpdateTime;
        if(state.keyPressed[State::Left]) position.x -= 20.0 * elapsed;
        if(state.keyPressed[State::Right]) position.x += 20.0 * elapsed;
    }

    bounds.center = position;
    drawable->updateMatrix(position, rotation, scale,color);

    firstFrame = false;
    lastUpdateTime = time;
}
