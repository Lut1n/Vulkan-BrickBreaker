#include "drawcommandbuffer.h"

//--------------------------------------------------------------
DrawCommandBuffer::DrawCommandBuffer(SwapChainResources* resources, Pipeline* pipeline)
    : m_resources(resources)
    , m_pipeline(pipeline)
{
}

//--------------------------------------------------------------
DrawCommandBuffer::~DrawCommandBuffer()
{
    cleanup();
}

//--------------------------------------------------------------
void DrawCommandBuffer::initialize()
{
    uint32_t count = m_resources->count();
    m_cmdBuffer.resize(count, nullptr);
}

//--------------------------------------------------------------
void DrawCommandBuffer::cleanup()
{
    for(uint32_t i=0;i<m_cmdBuffer.size();++i) delete m_cmdBuffer[i];
    m_cmdBuffer.clear();
}

//--------------------------------------------------------------
void DrawCommandBuffer::drawScene(VkScene* scene)
{
    bindCamera(scene->getCamera());
    for(auto& e : scene->getSceneObjects())
    {
        if(e->drawable->hidden) continue;

        bindMesh(e->mesh);
        bindMaterial(e->material);
        drawEntity(e->drawable);
    }
}

//--------------------------------------------------------------
void DrawCommandBuffer::beginDraw(int frameIndex)
{
    currentFrameIndex = frameIndex;

    DrawCmdPool::instance(currentFrameIndex)->reset();
    m_cmdBuffer[currentFrameIndex] = DrawCmdPool::instance(currentFrameIndex)->alloc(1)[0];
    m_cmdBuffer[currentFrameIndex]->begin();

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_resources->getRenderPass()->get();
    renderPassInfo.framebuffer = m_resources->getFrameBuffer(frameIndex)->get();

    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = m_resources->getSwapChain()->getExtent();

    std::array<VkClearValue, 2> clearValues = {};
    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();


    VkCommandBuffer cmdbuf= m_cmdBuffer[currentFrameIndex]->get();

    vkCmdBeginRenderPass(cmdbuf, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->get());

    m_pendingCamera = nullptr;
    m_pendingMaterial = nullptr;
    m_pendingMesh = nullptr;
    for(auto& c : m_changed) c=false;
}

//--------------------------------------------------------------
void DrawCommandBuffer::bindCamera(SceneView* cam)
{
    m_changed[CAMERA] = cam != m_pendingCamera;
    m_pendingCamera = cam;
}

//--------------------------------------------------------------
void DrawCommandBuffer::bindMaterial(Material* material)
{
    m_changed[MATERIAL] = material != m_pendingMaterial;
    m_pendingMaterial = material;
}

//--------------------------------------------------------------
void DrawCommandBuffer::bindMesh(Mesh* mesh)
{
    m_changed[MESH] = mesh != m_pendingMesh;
    m_pendingMesh = mesh;
}

//--------------------------------------------------------------
void DrawCommandBuffer::drawEntity(Drawable* drawable)
{
    VkCommandBuffer cmdbuf= m_cmdBuffer[currentFrameIndex]->get();
    PipelineLayout* layout = m_pipeline->getLayout();


    std::vector<VkDescriptorSet> descriptorsToBind;

    Descriptors* descriptor = drawable->descriptors;
    VkDescriptorSet perobj_desc = descriptor->get(currentFrameIndex);


    if(m_changed[MESH])
    {
        VertexBuffer* mesh = m_pendingMesh->buffer;
        VkBuffer vertexBuffers[] = {mesh->getVertexBuffer()->get()};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(cmdbuf, 0, 1, vertexBuffers, offsets);

        // if has index buffer
        vkCmdBindIndexBuffer(cmdbuf, mesh->getIndexBuffer()->get(), 0, VK_INDEX_TYPE_UINT32);
    }

    // if(m_changed[CAMERA])
    {
        descriptorsToBind.push_back(m_pendingCamera->descriptors->get(currentFrameIndex));
    }
    // if(m_changed[MATERIAL])
    {
        descriptorsToBind.push_back(m_pendingMaterial->descriptors->get(currentFrameIndex));
    }

    descriptorsToBind.push_back(perobj_desc);

    vkCmdBindDescriptorSets(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, layout->get(),
                            0, descriptorsToBind.size(), descriptorsToBind.data(), 0, nullptr);

    // if has index buffer
    vkCmdDrawIndexed(cmdbuf, m_pendingMesh->buffer->count(), 1, 0, 0, 0);
    // vkCmdDraw(commandBuffers[i], static_cast<uint32_t>(vertexBuffer.vertices.size()), 1, 0, 0);


    for(auto& c : m_changed) c=false;
}

//--------------------------------------------------------------
void DrawCommandBuffer::endDraw()
{
    VkCommandBuffer cmdbuf= m_cmdBuffer[currentFrameIndex]->get();
    vkCmdEndRenderPass(cmdbuf);
    m_cmdBuffer[currentFrameIndex]->end();
}
