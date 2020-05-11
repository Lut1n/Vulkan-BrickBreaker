#ifndef VKGAME_COLLISIONS_H
#define VKGAME_COLLISIONS_H

#include "game/ball.h"
#include "game/brick.h"
#include "game/paddle.h"
#include "game/platform.h"


class CollisionManager
{
public:
    CollisionManager();
    virtual ~CollisionManager();

    void setBall(Ball* ball);
    void setPaddle(Paddle* paddle);
    void setPlatform(Platform* platform);
    void addBrick(Brick* brick);
    void remBrick(Brick* brick);

    void checkCollisions();

protected:
    glm::vec3 m_lastValidPosition;
    BoundingBox m_cage;
    std::vector<Brick*> m_bricks;
    Ball* m_ball;
    Paddle* m_paddle;
    Platform* m_platform;
};

#endif // VKGAME_COLLISIONS_H
