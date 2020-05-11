#ifndef APPLICATION_H
#define APPLICATION_H

#include "common.h"
#include "scene.h"
#include "swapchainresources.h"
#include "drawcommandbuffer.h"

#include "vkinterface/descriptorset.h"
#include "vkinterface/synchronization.h"
#include "shaderpipeline.h"
#include "gcore.h"

#include "assetsloader.h"
#include "game/collisions.h"

class MiniVkApplication
{
public:
    MiniVkApplication();
    virtual ~MiniVkApplication();

    void run();

protected:
    void initWindow();

    static void framebufferResizeCallback(GLFWwindow* window, int, int);

    void initVulkan();
    void initScene();

    void mainLoop();
    void drawFrame();

    void cleanup();

    void updateUniformBuffer(uint32_t currentImage);

    AssetsLoader* m_assets;
    CollisionManager m_collisions;
    DrawableAllocator* m_drawables;
    GraphicsCore* m_gcore;
    SwapChainResources* m_swapchain;
    VkScene m_sceneObjects;
    DrawCommandBuffer* m_drawcmd;
    Synchronization* m_sync;
    ShaderPipeline* m_pipeline;

    DescriptorPool* m_descpool;

    size_t currentFrame;
    bool framebufferResized;

    GLFWwindow* m_window;
};

#endif // APPLICATION_H
