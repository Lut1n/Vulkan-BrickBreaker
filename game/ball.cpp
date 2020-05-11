#include "game/ball.h"
#include "app/assetsloader.h"

//--------------------------------------------------------------
Ball::Ball()
    : started(false)
    , firstFrame(true)
{
    mesh = AssetsLoader::instance()->getOrLoadMesh(MODEL_BALL_PATH);
    material = AssetsLoader::instance()->getOrLoadTexture("textures/randomtex.jpg");
    drawable = DrawableAllocator::instance()->alloc();

    color = glm::vec3(0.3,0.7,0.3);
    position = glm::vec3(0,0,0);
    rotation = glm::vec3(0,0,0);
    scale = glm::vec3(1,1,1);

    setMoving(90.0, 15.0);
}

//--------------------------------------------------------------
Ball::~Ball()
{
    DrawableAllocator::instance()->free(drawable);
    drawable = nullptr;
}

//--------------------------------------------------------------
void Ball::animate(State state, float time)
{
    if(firstFrame) lastUpdateTime = time;


    if(state.keyPressed[State::Space]) started = true;

    if(time != lastUpdateTime && started)
    {
        float elapsed = time-lastUpdateTime;
        position += direction * elapsed;
        rotation.x += elapsed * 5.0;
        rotation.y += elapsed * 5.0;
        rotation.z += elapsed * 5.0;
    }

    drawable->updateMatrix(position, rotation, scale,color);

    firstFrame = false;
    lastUpdateTime = time;
}


//--------------------------------------------------------------
void Ball::ricochetX()
{
    direction.x *= -1.0;
}

//--------------------------------------------------------------
void Ball::ricochetY()
{
    direction.y *= -1.0;
}

//--------------------------------------------------------------
void Ball::setMoving(float angle, float speed)
{
    float rad = glm::radians(angle);
    direction = glm::vec3( std::cos(rad)*speed, std::sin(rad)*speed, 0.0);
}
