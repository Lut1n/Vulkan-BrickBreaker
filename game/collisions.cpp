#include "game/collisions.h"


//--------------------------------------------------------------
CollisionManager::CollisionManager()
{
    m_cage.center = glm::vec3(0,0,0);
    m_cage.tl = glm::vec3(-19.87,12.42,0.0);
    m_cage.br = glm::vec3(19.87,-12.42,0.0);
}

//--------------------------------------------------------------
CollisionManager::~CollisionManager()
{

}

//--------------------------------------------------------------
void CollisionManager::setBall(Ball* ball)
{
    m_ball = ball;
}

//--------------------------------------------------------------
void CollisionManager::setPaddle(Paddle* paddle)
{
    m_paddle = paddle;
}

//--------------------------------------------------------------
void CollisionManager::setPlatform(Platform* platform)
{
    m_platform = platform;
}

//--------------------------------------------------------------
void CollisionManager::addBrick(Brick* brick)
{
    m_bricks.push_back(brick);
}

//--------------------------------------------------------------
void CollisionManager::remBrick(Brick* brick)
{
    auto it = std::find(m_bricks.begin(),m_bricks.end(),brick);
    if(it != m_bricks.end()) m_bricks.erase(it);
}

//--------------------------------------------------------------
void CollisionManager::checkCollisions()
{
    if( !m_ball->started )
    {
        m_ball->position = m_paddle->position;
        m_ball->position.y += 1.0;
        return;
    }

    // check cage
    if(m_cage.inside(m_ball->position))
    {
        m_lastValidPosition = m_ball->position;
    }
    else
    {
        if( !m_cage.insideVertical(m_ball->position) ) m_ball->ricochetY();
        else if( !m_cage.insideHorizontal(m_ball->position) ) m_ball->ricochetX();

        m_ball->position = m_lastValidPosition;
    }

    // check paddle
    if(m_paddle->bounds.inside(m_ball->position) && (m_ball->position.y - m_paddle->position.y) > 0.0)
    {
        float oft = (m_ball->position.x - m_paddle->position.x)/2.5;
        float angle = 90.0 - 60.0*oft;
        m_ball->setMoving(angle, 15.0);
    }

    // check bricks
    for(auto b : m_bricks)
    {
        if(!b->toBreak && b->bounds.inside(m_ball->position))
        {
            glm::vec3 db = m_ball->position - b->position;
            db = db*db;
            if(db.x > db.y) m_ball->ricochetX();
            else m_ball->ricochetY();

            b->toBreak = true;
            b->drawable->hidden = true;
        }
    }

    // check game-over
    if(m_platform->bounds.insideVertical(m_ball->position))
    {
        for(auto b : m_bricks)
        {
            b->toBreak = false;
            b->drawable->hidden = false;
        }
        m_paddle->position.x = 0.0;
        m_ball->setMoving(90.0,15.0);
        m_ball->started = false;
    }
}
