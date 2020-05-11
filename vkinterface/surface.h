#ifndef SURFACE_H
#define SURFACE_H

#include "vkiobject.h"
#include "physicaldevice.h"

//--------------------------------------------------------------
class Surface : public VkInsObject
{
public:
    Surface( Instance* vkinstance, GLFWwindow* window );
    virtual ~Surface();

    void initialize() override;
    void cleanup() override;

    VkSurfaceKHR& get();

    void getSize(uint32_t& width, uint32_t& height);

protected:

    GLFWwindow* m_window;
    VkSurfaceKHR m_surface;
};

#endif // SURFACE_H
