#ifndef GRAPHICS_CORE_H
#define GRAPHICS_CORE_H

#include "common.h"
#include "vkinterface/instance.h"
#include "vkinterface/logicaldevice.h"
#include "vkinterface/commandbuffer.h"

class GraphicsCore
{
public:
    GraphicsCore(GLFWwindow* window);
    virtual ~GraphicsCore();

    void initialize();
    void cleanup();

    LogicalDevice* device() {return m_device;}
    Surface* surface(){return m_surface;}

protected:
    GLFWwindow* m_window;

    Instance* m_instance;
    Surface* m_surface;
    DebugMessenger* m_debugMessenger;
    PhysicalDevice m_physicalDevice;
    LogicalDevice* m_device;
    GraphicCmdPool* m_graphicCmdPool;
    TransferCmdPool* m_transferCmdPool;
};

#endif // GRAPHICS_CORE_H
