#include "swapchain.h"
#include "logicaldevice.h"


//--------------------------------------------------------------
VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    VkFormat expectedFormat = VK_FORMAT_R8G8B8A8_UNORM;
    // VkFormat expectedFormat = VK_FORMAT_B8G8R8A8_UNORM;
    VkColorSpaceKHR expectedColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

    for (const auto& candidate : availableFormats)
    {
        if(candidate.format == expectedFormat &&
                candidate.colorSpace == expectedColorSpace)
            return candidate;
    }

    // default
    return availableFormats[0];
}


//--------------------------------------------------------------
VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    VkPresentModeKHR expected = VK_PRESENT_MODE_MAILBOX_KHR;

    for (const auto& candidate : availablePresentModes)
    {
        if(candidate == expected) return candidate;
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

//--------------------------------------------------------------
uint32_t clamp(uint32_t v, uint32_t mn, uint32_t mx)
{
    return std::max(mn,std::min(mx,v));
}

//--------------------------------------------------------------
VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height)
{
    VkExtent2D actualExtent = {width,height};

    if (capabilities.currentExtent.width != UINT32_MAX)
    {
        actualExtent = capabilities.currentExtent;
    }
    else
    {
        const VkExtent2D& minExtent = capabilities.minImageExtent;
        const VkExtent2D& maxExtent = capabilities.maxImageExtent;
        actualExtent.width = clamp(actualExtent.width, minExtent.width, maxExtent.width);
        actualExtent.height = clamp(actualExtent.height, minExtent.height, maxExtent.height);
    }

    return actualExtent;
}


//--------------------------------------------------------------
SwapChain::SwapChain(LogicalDevice* device, Surface* surface)
    : VkDevObject(device)
    , m_surface(surface)
{
}

//--------------------------------------------------------------
SwapChain::~SwapChain()
{
    cleanup();
}


//--------------------------------------------------------------
void SwapChain::initialize()
{
    PhysicalDevice* physical = m_parent->getPhysical();
    SwapChainSupportDetails swapChainSupport = physical->m_swapChainSupport;

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);

    uint32_t width, height;
    m_surface->getSize(width,height);
    m_extent = chooseSwapExtent(swapChainSupport.capabilities, width, height);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

    if (swapChainSupport.capabilities.maxImageCount > 0
        && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_surface->get();

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = m_extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = physical->m_indices;
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(m_parent->get(), &createInfo, nullptr, &m_swapChain) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create swap chain!");
    }

    // retrieve handles to the swap chain vkImage(s)
    vkGetSwapchainImagesKHR(m_parent->get(), m_swapChain, &imageCount, nullptr);
    m_images.resize(imageCount);
    vkGetSwapchainImagesKHR(m_parent->get(), m_swapChain, &imageCount, m_images.data());
    m_imageFormat = surfaceFormat.format;

    std::cout << "swapchain image count = " << imageCount << std::endl;

}

//--------------------------------------------------------------
void SwapChain::cleanup()
{
    vkDestroySwapchainKHR(m_parent->get(), m_swapChain, nullptr);
}

//--------------------------------------------------------------
uint32_t SwapChain::acquireNextImage(VkSemaphore semaphore)
{
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(m_parent->get(), m_swapChain, UINT64_MAX, semaphore, VK_NULL_HANDLE, &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR/* || framebufferResized*/)
    {
        // framebufferResized = false;

        // int width = 0, height = 0;
        // while (width == 0 || height == 0) {
        //     glfwGetFramebufferSize(window, &width, &height);
        //     glfwWaitEvents();
        // }
        //
        // instance.device.recreateSwapChain(width, height, vertexBuffer, texture);
        // return;

        throw std::runtime_error("need to recreate swapchain");
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    return imageIndex;
}
