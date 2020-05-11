#ifndef VKGAME_BRICK_H
#define VKGAME_BRICK_H

#include "app/scene.h"

class Brick : public SceneObject
{
public:
    Brick();
    virtual ~Brick();

    void animate(State state, float time) override;

    float m_timeOffset;
    BoundingBox bounds;
    bool toBreak;
protected:
};

#endif // VKGAME_BRICK_H
