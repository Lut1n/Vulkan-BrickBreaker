#ifndef DEBUG_H
#define DEBUG_H

#include "vkiobject.h"

//--------------------------------------------------------------
class DebugMessenger : public VkInsObject
{
public:
    DebugMessenger( Instance* vkinstance );
    virtual ~DebugMessenger();

    void initialize() override;
    void cleanup() override;

    static VKAPI_ATTR VkBool32 VKAPI_CALL s_debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

    static void s_populateCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& info);

protected:
    VkDebugUtilsMessengerEXT m_messenger;
};

#endif // DEBUG_H
