#ifndef DRAWABLES_H
#define DRAWABLES_H

#include "common.h"
#include "vkinterface/uniformbuffer.h"
#include "vkinterface/descriptorset.h"
#include "vkinterface/pipelinelayout.h"
#include "swapchainresources.h"


//--------------------------------------------------------------
struct Drawable
{
    ColorBufferObject worldMatrix;
    UniformBuffer* uniforms;
    UniformBuffer* indColor;
    Descriptors* descriptors;
    bool hidden;

    Drawable()
        : uniforms(nullptr)
        , indColor(nullptr)
        , descriptors(nullptr)
        , hidden(false)
    {}

    void updateMatrix(glm::vec3 p,glm::vec3 r,glm::vec3 s,glm::vec3 c);
};

//--------------------------------------------------------------
class DrawableAllocator
{
public:
    DrawableAllocator(LogicalDevice* device, SwapChainResources* swapChain, PipelineLayout* layout, DescriptorPool* pool);
    virtual ~DrawableAllocator();

    Drawable* alloc();
    void free(Drawable* drawable);

    static DrawableAllocator* instance(){return s_instance;}

protected:
    std::vector<Drawable*> m_drawables;

    LogicalDevice* m_device;
    SwapChainResources* m_swapChain;
    PipelineLayout* m_layout;
    DescriptorPool* m_pool;

    static DrawableAllocator* s_instance;
};

#endif // DRAWABLES_H
