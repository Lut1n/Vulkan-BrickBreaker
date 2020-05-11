#include "physicaldevice.h"


//--------------------------------------------------------------
PhysicalDevice::PhysicalDevice()
    : m_physical(VK_NULL_HANDLE)
    , m_msaaSamples(VK_SAMPLE_COUNT_1_BIT)
{
}

//--------------------------------------------------------------
PhysicalDevice::PhysicalDevice( VkPhysicalDevice& physical )
    : m_physical(physical)
    , m_msaaSamples(VK_SAMPLE_COUNT_1_BIT)
{
}

//--------------------------------------------------------------
void PhysicalDevice::findSupportDetails(VkSurfaceKHR& surface)
{
    m_indices = findQueueFamilies(surface);
    m_swapChainSupport = querySwapChainSupport(surface);
    m_msaaSamples = getMaxUsableSampleCount();
}


//--------------------------------------------------------------
VkSampleCountFlagBits PhysicalDevice::getMaxUsableSampleCount()
{
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(m_physical, &physicalDeviceProperties);

    VkSampleCountFlags counts =
            physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
    if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
    if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
    if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
    if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
    if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
    if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

    return VK_SAMPLE_COUNT_1_BIT;
}


//--------------------------------------------------------------
VkFormat PhysicalDevice::findSupportedFormat(const std::vector<VkFormat>& candidates,
                                             VkImageTiling tiling, VkFormatFeatureFlags features)
{
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(m_physical, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}


//--------------------------------------------------------------
VkFormat PhysicalDevice::findDepthFormat() {
    return findSupportedFormat(
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}


//--------------------------------------------------------------
bool PhysicalDevice::hasStencilComponent(VkFormat format) {
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}


//--------------------------------------------------------------
QueueFamilyIndices PhysicalDevice::findQueueFamilies(VkSurfaceKHR& surface) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(m_physical, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(m_physical, &queueFamilyCount, queueFamilies.data());
    int i = 0;

    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(m_physical, i, surface, &presentSupport);
        if (queueFamily.queueCount > 0 && presentSupport) {
            indices.presentFamily = i;
        }

        if (queueFamily.queueCount > 0 &&
                !(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
                (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT)) {
            indices.transferFamily = i;
        }

        if (indices.isComplete()) {
            break;
        }

        i++;
    }
    return indices;
}

//--------------------------------------------------------------
SwapChainSupportDetails PhysicalDevice::querySwapChainSupport(VkSurfaceKHR& surface) {
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physical, surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_physical, surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_physical, surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(m_physical, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_physical, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}


//--------------------------------------------------------------
bool PhysicalDevice::isDeviceSuitable(VkSurfaceKHR& surface) {
    QueueFamilyIndices indices = findQueueFamilies(surface);

    bool extensionsSupported = checkDeviceExtensionSupport();

    bool swapChainAdequate = false;
    if (extensionsSupported) {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(surface);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(m_physical, &supportedFeatures);

    return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}


//--------------------------------------------------------------
bool PhysicalDevice::checkDeviceExtensionSupport()
{
    Extensions deviceExtensions = s_defaultExtensions();
    return deviceExtensions.checkDeviceSupport(m_physical);
}


//--------------------------------------------------------------
int PhysicalDevice::rateDeviceSuitability()
{
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(m_physical, &deviceProperties);

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(m_physical, &deviceFeatures);

    int score = 0;

    // Discrete GPUs have a significant performance advantage
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 1000;
    }

    // Maximum possible size of textures affects graphics quality
    score += deviceProperties.limits.maxImageDimension2D;

    // Application can't function without geometry shaders
    if (!deviceFeatures.geometryShader) {
        return 0;
    }

    return score;
}

//--------------------------------------------------------------
uint32_t PhysicalDevice::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(m_physical, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i))
                && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

//--------------------------------------------------------------
Extensions PhysicalDevice::s_defaultExtensions()
{
    Extensions deviceExtensions;
    deviceExtensions.enable(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    return deviceExtensions;
}

//--------------------------------------------------------------
VkPhysicalDevice PhysicalDevice::get()
{
    return m_physical;
}
