#ifndef VKGAME_CAMERA_H
#define VKGAME_CAMERA_H

#include "app/scene.h"

class Camera : public SceneView
{
public:
    Camera(float dist, LogicalDevice* device, DescriptorPool* pool, PipelineLayout* layout, SwapChainResources* swapChain);
    virtual ~Camera();

    void animate(State state, float time) override;
protected:
    DescriptorPool* m_pool;
};

#endif // VKGAME_CAMERA_H
