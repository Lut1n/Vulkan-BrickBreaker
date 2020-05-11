#include "game/camera.h"

//--------------------------------------------------------------
Camera::Camera(float dist, LogicalDevice* device, DescriptorPool* pool, PipelineLayout* layout, SwapChainResources* swapChain)
    : m_pool(pool)
{
    uniforms = new UniformBuffer(device, sizeof(ViewBufferObject), swapChain->count());
    uniforms->initialize();
    descriptors = pool->alloc(layout->getDescriptorSetLayout(0));
    ShaderUniforms uniform;
    uniform.setViewUniform(0, uniforms);
    descriptors->update(uniform);

    VkExtent2D extent = swapChain->getSwapChain()->getExtent();
    aspectRatio = extent.width / (float) extent.height;

    position = glm::vec3(0.0,0.0,dist);
}

//--------------------------------------------------------------
Camera::~Camera()
{
    m_pool->free(descriptors);
    delete uniforms;
}

//--------------------------------------------------------------
void Camera::animate(State state, float time)
{
    updateMatrix(position, target);
}
