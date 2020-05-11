#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include "vkiobject.h"
#include "image.h"

class SwapChain;

class ImageView : VkDevObject
{
public:
    ImageView(LogicalDevice* device, Image* image, VkImageAspectFlags aspect);
    ImageView(LogicalDevice* device, SwapChain* swapchain, uint32_t index, VkImageAspectFlags aspect);

    virtual ~ImageView();

    void initialize() override;
    void cleanup() override;

    VkImageView& get() {return m_view;}

protected:
    VkImageView m_view;
    VkImageAspectFlags m_aspect;

    Image* m_image;

    SwapChain* m_owner;
    uint32_t m_ownerIndex;
};

#endif // IMAGEVIEW_H
