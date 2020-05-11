#include "framebuffer.h"
#include "logicaldevice.h"

//--------------------------------------------------------------
FrameBuffer::FrameBuffer(LogicalDevice* device, std::vector<ImageView*>& attachments,
                         RenderPass* renderpass, uint32_t width, uint32_t height)
    : VkDevObject(device)
    , m_attachments(attachments)
    , m_renderpass(renderpass)
    , m_width(width)
    , m_height(height)
{
}

//--------------------------------------------------------------
FrameBuffer::~FrameBuffer()
{
    cleanup();
}

//--------------------------------------------------------------
void FrameBuffer::initialize()
{
    VkFramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = m_renderpass->get();

    std::vector<VkImageView> attachments;
    for(auto iv : m_attachments) attachments.push_back(iv->get());

    framebufferInfo.attachmentCount = attachments.size();
    framebufferInfo.pAttachments = attachments.data();

    framebufferInfo.width = m_width;
    framebufferInfo.height = m_height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(m_parent->get(), &framebufferInfo, nullptr, &m_framebuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create framebuffer!");
    }
}

//--------------------------------------------------------------
void FrameBuffer::cleanup()
{
    vkDestroyFramebuffer(m_parent->get(), m_framebuffer, nullptr);
}
