#ifndef VKGAME_BALL_H
#define VKGAME_BALL_H

#include "app/scene.h"


class Ball : public SceneObject
{
public:
    Ball();
    virtual ~Ball();

    void animate(State state, float time) override;

    void ricochetX();
    void ricochetY();
    void setMoving(float angle, float speed);

    bool started;

protected:
    glm::vec3 direction;
    float lastUpdateTime;
    bool firstFrame;
};

#endif // VKGAME_BALL_H
