#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "vkiobject.h"
#include "imageview.h"
#include "renderpass.h"

class FrameBuffer : VkDevObject
{
public:
    FrameBuffer(LogicalDevice* device, std::vector<ImageView*>& attachments,
                RenderPass* renderpass, uint32_t width, uint32_t height);

    virtual ~FrameBuffer();

    void initialize() override;
    void cleanup() override;

    VkFramebuffer& get() {return m_framebuffer;}

protected:
    VkFramebuffer m_framebuffer;
    std::vector<ImageView*>& m_attachments;

    RenderPass* m_renderpass;

    uint32_t m_width;
    uint32_t m_height;
};

#endif // FRAMEBUFFER_H
