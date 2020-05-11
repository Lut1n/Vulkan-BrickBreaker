#include "gcore.h"

//--------------------------------------------------------------
GraphicsCore::GraphicsCore(GLFWwindow* window)
    : m_window(window)
{
}

//--------------------------------------------------------------
GraphicsCore::~GraphicsCore()
{
    cleanup();
}

//--------------------------------------------------------------
void GraphicsCore::initialize()
{
    mdebug("start init vulkan");
    m_instance = new Instance();
    m_instance->initialize();
    m_debugMessenger = m_instance->createDebugMessenger();
    m_debugMessenger->initialize();
    m_surface = m_instance->createSurface(m_window);
    m_surface->initialize();

    m_physicalDevice = m_instance->pickPhysicalDevice(m_surface);
    m_device = new LogicalDevice(&m_physicalDevice);
    m_device->initialize();

    // int width, height;
    // glfwGetFramebufferSize(m_window, &width, &height);

    m_graphicCmdPool = new GraphicCmdPool(m_device);
    m_graphicCmdPool->initialize();
    m_transferCmdPool = new TransferCmdPool(m_device);
    m_transferCmdPool->initialize();
}

void GraphicsCore::cleanup()
{
    delete m_transferCmdPool;
    delete m_graphicCmdPool;
    delete m_device;
    delete m_surface;
    delete m_debugMessenger;
    delete m_instance;
}
