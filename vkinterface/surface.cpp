#include "surface.h"
#include "instance.h"

//--------------------------------------------------------------
Surface::Surface(Instance* vkinstance, GLFWwindow* window)
    : VkInsObject(vkinstance)
    , m_window(window)
{
}

//--------------------------------------------------------------
Surface::~Surface()
{
    cleanup();
}

//--------------------------------------------------------------
void Surface::initialize()
{
    if (glfwCreateWindowSurface(m_parent->get(), m_window, nullptr, &m_surface) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create window surface!");
    }
}

//--------------------------------------------------------------
void Surface::cleanup()
{
    vkDestroySurfaceKHR(m_parent->get(), m_surface, nullptr);
}

//--------------------------------------------------------------
VkSurfaceKHR& Surface::get()
{
    return m_surface;
}

//--------------------------------------------------------------
void Surface::getSize(uint32_t& width, uint32_t& height)
{
    int w, h;
    glfwGetFramebufferSize(m_window, &w, &h);
    width=w; height=h;
}
