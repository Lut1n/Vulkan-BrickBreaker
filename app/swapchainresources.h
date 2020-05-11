#ifndef SWAPCHAIN_RESOURCES_H
#define SWAPCHAIN_RESOURCES_H

#include "common.h"

#include "vkinterface/logicaldevice.h"
#include "vkinterface/surface.h"
#include "vkinterface/swapchain.h"
#include "vkinterface/imageview.h"
#include "vkinterface/framebuffer.h"
#include "vkinterface/renderpass.h"
#include "vkinterface/commandbuffer.h"

class SwapChainResources
{
public:

    SwapChainResources(LogicalDevice* device, Surface* surface);
    virtual ~SwapChainResources();

    void initialize();
    void cleanup();

    LogicalDevice* getDevice() {return m_device;}
    Surface* getSurface() {return m_surface;}
    SwapChain* getSwapChain() {return m_swapchain;}
    RenderPass* getRenderPass() {return m_renderpass;}
    FrameBuffer* getFrameBuffer(uint32_t index) {return m_framebuffers[index];}
    uint32_t count() {return m_swapchain->count();}

protected:
    LogicalDevice* m_device;
    Surface* m_surface;

    SwapChain* m_swapchain;
    std::vector<ImageView*> m_views;

    RenderPass* m_renderpass;
    Image* m_msaaImage;
    Image* m_depthImage;
    ImageView* m_msaaView;
    ImageView* m_depthView;

    std::vector<FrameBuffer*> m_framebuffers;
    std::vector<DrawCmdPool*> m_drawCmdPool;
};

#endif // SWAPCHAIN_RESOURCES_H
