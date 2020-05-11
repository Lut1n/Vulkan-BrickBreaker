#ifndef INSTANCE_H
#define INSTANCE_H

#include "common.h"

#include "surface.h"
#include "validationlayers.h"
#include "extensions.h"
#include "debugmessenger.h"

class Instance
{
public:
    Instance();

    void initialize();
    void cleanup();

    Surface* createSurface(GLFWwindow* window);

    DebugMessenger* createDebugMessenger();

    void getPhysicalDevices(std::vector<PhysicalDevice>& physicals);

    PhysicalDevice pickPhysicalDevice(Surface* surface);

    VkInstance& get();

protected:
    VkInstance m_instance;

    ValidationLayers m_validationLayers;
    Extensions m_extensions;
};

#endif // INSTANCE_H
