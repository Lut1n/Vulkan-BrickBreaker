#include "uniformbuffer.h"

//--------------------------------------------------------------
UniformBuffer::UniformBuffer(LogicalDevice* device, uint32_t bufferSize, uint32_t swapChainSize)
    : VkDevObject(device)
    , m_bufferSize(bufferSize)
    , m_swapChainSize(swapChainSize)
{
}

//--------------------------------------------------------------
UniformBuffer::~UniformBuffer()
{
    cleanup();
}

//--------------------------------------------------------------
void UniformBuffer::initialize()
{
    VkDeviceSize size = m_bufferSize;
    VkBufferUsageFlags usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    m_uniforms.resize(m_swapChainSize);
    for(uint32_t i=0; i<m_swapChainSize; ++i)
    {
        m_uniforms[i] = new Buffer(m_parent, size, usage, properties);
        m_uniforms[i]->initialize();
    }
}

//--------------------------------------------------------------
void UniformBuffer::cleanup()
{
    for(uint32_t i=0; i<m_swapChainSize; ++i) delete m_uniforms[i];
    m_uniforms.clear();
}

//--------------------------------------------------------------
void UniformBuffer::update(uint32_t currentFrameIndex, const ViewBufferObject& ubo)
{
    m_uniforms[currentFrameIndex]->copy(&ubo, sizeof(ubo));
}

//--------------------------------------------------------------
void UniformBuffer::update(uint32_t currentFrameIndex, const ColorBufferObject& ubo)
{
    m_uniforms[currentFrameIndex]->copy(&ubo, sizeof(ubo));
}
