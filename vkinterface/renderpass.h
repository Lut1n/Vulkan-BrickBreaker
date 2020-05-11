#ifndef RENDERPASS_H
#define RENDERPASS_H

#include "vkiobject.h"

class RenderPass : public VkDevObject
{
public:
    RenderPass(LogicalDevice* device, VkFormat format);
    virtual ~RenderPass();

    void initialize() override;
    void cleanup() override;

    VkRenderPass& get() {return m_renderPass;}

protected:
    VkRenderPass m_renderPass;

    VkFormat m_format;
};

#endif // RENDERPASS_H
