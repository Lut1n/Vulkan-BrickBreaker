#ifndef VKGAME_PLATFORM_H
#define VKGAME_PLATFORM_H

#include "app/scene.h"

class Platform : public SceneObject
{
public:
    Platform();
    virtual ~Platform();

    void animate(State state, float time) override;

    BoundingBox bounds;
protected:

};

#endif // VKGAME_PLATFORM_H
