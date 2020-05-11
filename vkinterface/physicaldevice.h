#ifndef PHYSICALDEVICE_H
#define PHYSICALDEVICE_H

#include "common.h"
#include "extensions.h"


//--------------------------------------------------------------
struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    std::optional<uint32_t> transferFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value() && transferFamily.has_value();
    }

    std::set<uint32_t> getQueueFamilySet()
    {
        std::set<uint32_t> res = {graphicsFamily.value(),
                                  presentFamily.value(),
                                  transferFamily.value()};
        return res;
    }
};

//--------------------------------------------------------------
struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};


//--------------------------------------------------------------
class PhysicalDevice
{
public:
    PhysicalDevice();
    PhysicalDevice( VkPhysicalDevice& physical );

    void findSupportDetails(VkSurfaceKHR& surface);

    VkSampleCountFlagBits getMaxUsableSampleCount();

    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates,
                                 VkImageTiling tiling, VkFormatFeatureFlags features);

    VkFormat findDepthFormat();

    bool hasStencilComponent(VkFormat format);

    QueueFamilyIndices findQueueFamilies(VkSurfaceKHR& surface);

    SwapChainSupportDetails querySwapChainSupport(VkSurfaceKHR& surface);

    bool isDeviceSuitable(VkSurfaceKHR& surface);

    bool checkDeviceExtensionSupport();

    int rateDeviceSuitability();

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    static Extensions s_defaultExtensions();

    VkPhysicalDevice get();

    VkPhysicalDevice m_physical; // implicitly destroyed with instance

    VkSampleCountFlagBits m_msaaSamples;
    QueueFamilyIndices m_indices;
    SwapChainSupportDetails m_swapChainSupport;
};

#endif // PHYSICALDEVICE_H
