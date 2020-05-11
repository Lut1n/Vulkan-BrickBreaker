#include "shaderpipeline.h"

ShaderPipeline::ShaderPipeline(LogicalDevice* device, RenderPass* renderpass, VkExtent2D extent)
    : m_device(device)
    , m_renderpass(renderpass)
    , m_extent(extent)
{
    initialize();
}

ShaderPipeline::~ShaderPipeline()
{
    cleanup();
}

void ShaderPipeline::initialize()
{
    m_vertexShader = new Shader(m_device, SHADER_VERT_PATH);
    m_fragmentShader = new Shader(m_device, SHADER_FRAG_PATH);
    m_vertexShader->initialize();
    m_fragmentShader->initialize();

    m_layout = new PipelineLayout(m_device);
    m_layout->initialize();

    m_pipeline = new Pipeline(m_device, m_layout, m_renderpass, m_extent, m_vertexShader, m_fragmentShader);
    m_pipeline->initialize();
}

void ShaderPipeline::cleanup()
{
    delete m_pipeline;
    delete m_layout;
    delete m_fragmentShader;
    delete m_vertexShader;
}
