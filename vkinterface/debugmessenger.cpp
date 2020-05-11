#include "debugmessenger.h"
#include "instance.h"

//--------------------------------------------------------------
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

//--------------------------------------------------------------
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

//--------------------------------------------------------------
VKAPI_ATTR VkBool32 VKAPI_CALL DebugMessenger::s_debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT /*messageSeverity*/,
    VkDebugUtilsMessageTypeFlagsEXT /*messageType*/,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* /*pUserData*/) {

    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

//--------------------------------------------------------------
DebugMessenger::DebugMessenger(Instance* vkinstance)
    : VkInsObject(vkinstance)
{
}

//--------------------------------------------------------------
DebugMessenger::~DebugMessenger()
{
    cleanup();
}

//--------------------------------------------------------------
void DebugMessenger::initialize()
{
    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    s_populateCreateInfo(createInfo);

    if (CreateDebugUtilsMessengerEXT(m_parent->get(), &createInfo, nullptr, &m_messenger) != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

//--------------------------------------------------------------
void DebugMessenger::cleanup()
{
    DestroyDebugUtilsMessengerEXT(m_parent->get(), m_messenger, nullptr);
}

//--------------------------------------------------------------
void DebugMessenger::s_populateCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& info)
{
    info = {};
    info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    info.pfnUserCallback = s_debugCallback;
    info.pUserData = nullptr; // Optional
}
