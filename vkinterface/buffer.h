#ifndef BUFFER_H
#define BUFFER_H

#include "vkiobject.h"

//--------------------------------------------------------------
class Buffer : public VkDevObject
{
public:
    Buffer(LogicalDevice* device, uint32_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
    virtual ~Buffer();

    void initialize() override;
    void cleanup() override;

    void copy(const void* data, size_t size);
    void copy(Buffer* src);

    VkBuffer& get() {return m_buffer;}
    VkDeviceMemory& getMemory() {return m_memory;}
    uint32_t getSize() {return m_size;}

protected:
    VkBuffer m_buffer;
    VkDeviceMemory m_memory;
    VkBufferUsageFlags m_usage;
    VkMemoryPropertyFlags m_properties;

    uint32_t m_size;
};

#endif // BUFFER_H
