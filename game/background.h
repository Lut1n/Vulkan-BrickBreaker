#ifndef VKGAME_BACKGROUND_H
#define VKGAME_BACKGROUND_H

#include "app/scene.h"

class Background : public SceneObject
{
public:
    Background();
    virtual ~Background();

    void animate(State state, float time) override;

protected:

};

#endif // VKGAME_BACKGROUND_H
