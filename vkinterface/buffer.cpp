#include "buffer.h"
#include "logicaldevice.h"
#include "commandbuffer.h"

//--------------------------------------------------------------
Buffer::Buffer(LogicalDevice* device, uint32_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
    : VkDevObject(device)
    , m_usage(usage)
    , m_properties(properties)
    , m_size(size)
{

}

//--------------------------------------------------------------
Buffer::~Buffer()
{
    cleanup();
}

//--------------------------------------------------------------
void Buffer::initialize()
{
    PhysicalDevice* physical = m_parent->getPhysical();
    QueueFamilyIndices indices = physical->m_indices;

    // declaration
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = m_size;
    bufferInfo.usage = m_usage;
    // bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.transferFamily.value()};
    bufferInfo.queueFamilyIndexCount = 2; // Optional
    bufferInfo.pQueueFamilyIndices = queueFamilyIndices; // Optional

    if (vkCreateBuffer(m_parent->get(), &bufferInfo, nullptr, &m_buffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create buffer!");
    }

    // requirements
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_parent->get(), m_buffer, &memRequirements);

    // allocation
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = physical->findMemoryType(memRequirements.memoryTypeBits, m_properties);

    if (vkAllocateMemory(m_parent->get(), &allocInfo, nullptr, &m_memory) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    // binding
    vkBindBufferMemory(m_parent->get(), m_buffer, m_memory, 0);
}

//--------------------------------------------------------------
void Buffer::cleanup()
{
    vkDestroyBuffer(m_parent->get(), m_buffer, nullptr);
    vkFreeMemory(m_parent->get(), m_memory, nullptr);
}

//--------------------------------------------------------------
void Buffer::copy(const void* data, size_t size)
{
    void* mapped;
    vkMapMemory(m_parent->get(), m_memory, 0, size, 0, &mapped);
    {
        memcpy(mapped, data, size);
    }
    vkUnmapMemory(m_parent->get(), m_memory);
}

//--------------------------------------------------------------
void Buffer::copy(Buffer* src)
{
    CommandBuffer* command = TransferCmdPool::instance()->alloc();
    command->beginOneTime();
    command->copyBuffer(m_buffer, src->get(), m_size);
    command->end();
    command->submitAndWait();
    delete command;
}

