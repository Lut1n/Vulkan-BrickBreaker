#include "drawables.h"

//--------------------------------------------------------------
void Drawable::updateMatrix(glm::vec3 position,glm::vec3 rotation,glm::vec3 scale,glm::vec3 color)
{
    if(!hidden)
    {
        worldMatrix.model = glm::mat4(1.0f);
        worldMatrix.color = color;

        worldMatrix.model = glm::translate(worldMatrix.model, position);
        worldMatrix.model = glm::rotate(worldMatrix.model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        worldMatrix.model = glm::rotate(worldMatrix.model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        worldMatrix.model = glm::rotate(worldMatrix.model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        worldMatrix.model = glm::scale(worldMatrix.model, scale);
    }
}

//--------------------------------------------------------------
DrawableAllocator* DrawableAllocator::s_instance = nullptr;

//--------------------------------------------------------------
DrawableAllocator::DrawableAllocator(LogicalDevice* device, SwapChainResources* swapChain, PipelineLayout* layout, DescriptorPool* pool)
    : m_device(device)
    , m_swapChain(swapChain)
    , m_layout(layout)
    , m_pool(pool)
{
    s_instance = this;
}

//--------------------------------------------------------------
DrawableAllocator::~DrawableAllocator()
{
    for(auto d : m_drawables) free(d);
}

//--------------------------------------------------------------
Drawable* DrawableAllocator::alloc()
{
    Drawable* drawable = new Drawable();

    drawable->uniforms = new UniformBuffer(m_device,sizeof(ColorBufferObject), m_swapChain->count());
    drawable->uniforms->initialize();
    drawable->descriptors = m_pool->alloc(m_layout->getDescriptorSetLayout(2));
    ShaderUniforms uniforms;
    uniforms.setEntityUniform(0, drawable->uniforms);
    drawable->descriptors->update(uniforms);

    m_drawables.push_back(drawable);
    return drawable;
}

//--------------------------------------------------------------
void DrawableAllocator::free(Drawable* drawable)
{
    auto found = std::find(m_drawables.begin(), m_drawables.end(), drawable);
    if(found == m_drawables.end()) throw "error drawable is not from this allocator.";

    m_drawables.erase(found);
    delete drawable->uniforms;
    delete drawable;
}
