#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include "vkiobject.h"
#include "surface.h"

class SwapChain : VkDevObject
{
public:
    SwapChain(LogicalDevice* device, Surface* surface);

    virtual ~SwapChain();

    void initialize() override;
    void cleanup() override;

    uint32_t acquireNextImage(VkSemaphore semaphore);

    VkSwapchainKHR& get() {return m_swapChain;}

    VkImage getImage(uint32_t index) {return m_images[index];}
    VkFormat getFormat() {return m_imageFormat;}
    uint32_t count() {return m_images.size();}

    VkExtent2D getExtent() {return m_extent;}

protected:
    VkSwapchainKHR m_swapChain;
    Surface* m_surface;

    std::vector<VkImage> m_images;

    VkFormat m_imageFormat;
    VkExtent2D m_extent;

    float aspectRatio;
};

#endif // SWAPCHAIN_H
