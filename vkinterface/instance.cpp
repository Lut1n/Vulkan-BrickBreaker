#include "instance.h"

//--------------------------------------------------------------
Instance::Instance()
{
}

//--------------------------------------------------------------
void Instance::initialize()
{
    m_validationLayers = ValidationLayers::s_default();

    if (enableValidationLayers && !m_validationLayers.checkSupport()) {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "VkBrickBreaker";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "[Lut1n]MiniVulkan";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;
    // default value is nullptr for .pNext

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = Extensions::s_default();
    extensions.checkSupport();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.count());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    if (enableValidationLayers)
    {
        std::cout << "validationLayers is enabled" << std::endl;
        createInfo.enabledLayerCount = m_validationLayers.count();
        createInfo.ppEnabledLayerNames = m_validationLayers.data();

        DebugMessenger::s_populateCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
}

//--------------------------------------------------------------
void Instance::cleanup()
{
    // device.cleanup();
    vkDestroyInstance(m_instance, nullptr);
}

//--------------------------------------------------------------
Surface* Instance::createSurface(GLFWwindow* window)
{
    Surface* s = new Surface(this, window);
    s->initialize();
    return s;
}

//--------------------------------------------------------------
void Instance::getPhysicalDevices(std::vector<PhysicalDevice>& physicals)
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);

    physicals.resize(deviceCount);

    if (deviceCount > 0)
    {
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

        for(uint32_t i=0;i<deviceCount;++i)
            physicals[i] = PhysicalDevice(devices[i]);
    }
    else
    {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }
}

//--------------------------------------------------------------
PhysicalDevice Instance::pickPhysicalDevice(Surface* surface)
{
    std::vector<PhysicalDevice> devices;
    getPhysicalDevices(devices);

    // Use an ordered map to automatically sort candidates by increasing score
    std::multimap<int, PhysicalDevice> candidates;
    for (auto& device : devices)
    {
        device.findSupportDetails(surface->get());
        int score = device.rateDeviceSuitability();
        candidates.insert(std::make_pair(score, device));
    }

    // Check if the best candidate is suitable at all
    if (candidates.rbegin()->first > 0)
    {
        return candidates.rbegin()->second;
    }
    else
    {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

//--------------------------------------------------------------
VkInstance& Instance::get()
{
    return m_instance;
}

//--------------------------------------------------------------
DebugMessenger* Instance::createDebugMessenger()
{
    if (!enableValidationLayers) return nullptr;

    DebugMessenger* debug = new DebugMessenger(this);
    debug->initialize();
    return debug;
}
