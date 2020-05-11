#include "logicaldevice.h"

//--------------------------------------------------------------
LogicalDevice::LogicalDevice(PhysicalDevice* physical)
    : m_device(VK_NULL_HANDLE)
    , m_physicalDevice(physical)
    , m_msaaEnabled(true)
{
}

//--------------------------------------------------------------
LogicalDevice::~LogicalDevice()
{
    cleanup();
}

//--------------------------------------------------------------
void LogicalDevice::initialize()
{
    QueueFamilyIndices indices = m_physicalDevice->m_indices;

    float queuePriority = 1.0f;
    std::set<uint32_t> queueFamilies = indices.getQueueFamilySet();
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    for (uint32_t queueFamily : queueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }


    // enable features
    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    deviceFeatures.sampleRateShading = m_msaaEnabled? VK_TRUE : VK_FALSE;


    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = queueCreateInfos.size();
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;


    m_extensions = PhysicalDevice::s_defaultExtensions();
    createInfo.enabledExtensionCount = m_extensions.count();
    createInfo.ppEnabledExtensionNames = m_extensions.data();

    if (enableValidationLayers)
    {
        m_validationLayers = ValidationLayers::s_default();
        createInfo.enabledLayerCount = m_validationLayers.count();
        createInfo.ppEnabledLayerNames = m_validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(m_physicalDevice->get(), &createInfo, nullptr, &m_device) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create logical device!");
    }

    // get device queues
    m_graphicsQueue = new Queue(this, indices.graphicsFamily.value());
    m_presentQueue = new Queue(this, indices.presentFamily.value());
    m_transferQueue = new Queue(this, indices.transferFamily.value());
    m_graphicsQueue->initialize();
    m_presentQueue->initialize();
    m_transferQueue->initialize();
}

//--------------------------------------------------------------
void LogicalDevice::cleanup()
{
    vkDestroyDevice(m_device, nullptr);
}

//--------------------------------------------------------------
VkDevice& LogicalDevice::get()
{
    return m_device;
}

//--------------------------------------------------------------
PhysicalDevice* LogicalDevice::getPhysical()
{
    return m_physicalDevice;
}
