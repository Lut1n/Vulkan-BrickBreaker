#include "imageview.h"
#include "logicaldevice.h"
#include "swapchain.h"

//--------------------------------------------------------------
ImageView::ImageView(LogicalDevice* device, Image* image, VkImageAspectFlags aspect)
    : VkDevObject(device)
    , m_aspect(aspect)
    , m_image(image)
    , m_owner(nullptr)
    , m_ownerIndex(0)
{

}

//--------------------------------------------------------------
ImageView::ImageView(LogicalDevice* device, SwapChain* swapchain, uint32_t index,
                     VkImageAspectFlags aspect)
    : VkDevObject(device)
    , m_aspect(aspect)
    , m_image(nullptr)
    , m_owner(swapchain)
    , m_ownerIndex(index)
{

}

//--------------------------------------------------------------
ImageView::~ImageView()
{
    cleanup();
}

//--------------------------------------------------------------
void ImageView::initialize()
{
    VkImage image = VK_NULL_HANDLE;
    VkFormat format;
    if(m_owner)
    {
        image = m_owner->getImage(m_ownerIndex);
        format = m_owner->getFormat();
    }
    else
    {
        image = m_image->get();
        format = m_image->getFormat();
    }

    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = m_aspect; // VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1; //mipLevels
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(m_parent->get(), &viewInfo, nullptr, &m_view) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create texture image view!");
    }
}

//--------------------------------------------------------------
void ImageView::cleanup()
{
    vkDestroyImageView(m_parent->get(), m_view, nullptr);
}
