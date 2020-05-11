#ifndef VKGAME_PADDLE_H
#define VKGAME_PADDLE_H

#include "app/scene.h"

class Paddle : public SceneObject
{
public:
    Paddle();
    virtual ~Paddle();

    void animate(State state, float time) override;

    BoundingBox bounds;
protected:

    float lastUpdateTime;
    bool firstFrame;
};

#endif // VKGAME_PADDLE_H
