#ifndef UNIFORMBUFFER_H
#define UNIFORMBUFFER_H

#include "vkiobject.h"
#include "buffer.h"

struct ViewBufferObject
{
    alignas(16) glm::mat4 proj;
    alignas(16) glm::mat4 view;
};

struct ColorBufferObject
{
    alignas(16) glm::mat4 model;
    alignas(16) glm::vec3 color;
};


class UniformBuffer : public VkDevObject
{
public:
    UniformBuffer(LogicalDevice* device, uint32_t bufferSize, uint32_t swapChainSize);
    virtual ~UniformBuffer();

    void initialize() override;
    void cleanup() override;

    void update(uint32_t currentFrameIndex, const ViewBufferObject& ubo);
    void update(uint32_t currentFrameIndex, const ColorBufferObject& ubo);
    Buffer* get(uint32_t currentFrameIndex) {return m_uniforms[currentFrameIndex];}

protected:
    std::vector<Buffer*> m_uniforms;
    uint32_t m_bufferSize;
    uint32_t m_swapChainSize;
};

#endif // UNIFORMBUFFER_H
