#include "pipeline.h"
#include "logicaldevice.h"

//--------------------------------------------------------------
Pipeline::Pipeline(LogicalDevice* device, PipelineLayout* layout, RenderPass* renderpass,
                   VkExtent2D extent, Shader* vertShader, Shader* fragShader)
    : VkDevObject(device)
    , m_layout(layout)
    , m_renderpass(renderpass)
    , m_extent(extent)
    , m_vertShader(vertShader)
    , m_fragShader(fragShader)
{
}

//--------------------------------------------------------------
Pipeline::~Pipeline()
{
    cleanup();
}

//--------------------------------------------------------------
std::vector<VkPipelineShaderStageCreateInfo> buildShaderInfo( Shader* vertShader, Shader* fragShader )
{

    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShader->get();
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShader->get();
    fragShaderStageInfo.pName = "main";

    std::vector<VkPipelineShaderStageCreateInfo> shaderInfo = {vertShaderStageInfo, fragShaderStageInfo};
    return shaderInfo;
}

//--------------------------------------------------------------
VkPipelineDepthStencilStateCreateInfo buildDepthStencilInfo()
{
    // if depth
    VkPipelineDepthStencilStateCreateInfo depthStencil = {};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f; // Optional
    depthStencil.maxDepthBounds = 1.0f; // Optional
    depthStencil.stencilTestEnable = VK_FALSE;
    depthStencil.front = {}; // Optional
    depthStencil.back = {}; // Optional

    return depthStencil;
}

//--------------------------------------------------------------
VkPipelineVertexInputStateCreateInfo buildVertexInputInfo()
{
    static VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    static auto bindingDescription = Vertex::getBindingDescription();
    static auto attributeDescriptions = Vertex::getAttributeDescriptions();

    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    return vertexInputInfo;
}

//--------------------------------------------------------------
VkPipelineInputAssemblyStateCreateInfo buildInputAssemblyInfo()
{
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    return inputAssembly;
}

//--------------------------------------------------------------
VkPipelineViewportStateCreateInfo buildViewportInfo(const VkExtent2D& extent)
{
    static VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) extent.width;
    viewport.height = (float) extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    static VkRect2D scissor = {};
    scissor.offset = {0, 0};
    scissor.extent = extent;

    static VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    return viewportState;
}

//--------------------------------------------------------------
VkPipelineRasterizationStateCreateInfo buildRasterizationInfo()
{
    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    // rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f; // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

    return rasterizer;
}

//--------------------------------------------------------------
VkPipelineMultisampleStateCreateInfo buildMultiSamplingInfo(bool enableMSAA = false, VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT)
{
    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.pSampleMask = nullptr; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional

    if(enableMSAA)
    {
        multisampling.rasterizationSamples = samples;
        // adv. msaa
        multisampling.sampleShadingEnable = VK_TRUE; // enable sample shading in the pipeline
        multisampling.minSampleShading = .2f; // min fraction for sample shading; closer to one is smoother
    }
    else
    {
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f; // Optional
    }

    return multisampling;
}

VkPipelineColorBlendStateCreateInfo buildBlendingInfo()
{
    // per attached framebuffer
    static VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

    // global
    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // Optional

    return colorBlending;
}

//--------------------------------------------------------------
void Pipeline::initialize()
{
    // global pipeline :
    // 1 - vertex/index buffer
    // 2 - input assembler
    // 3 - vertex shader
    // 4 - tessellation
    // 5 - geometry shader
    // 6 - rasterization
    // 7 - fragment shader
    // 8 - color blending

    /*
    VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_LINE_WIDTH
    };

    VkPipelineDynamicStateCreateInfo dynamicState = {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;
    */

    auto shaderStates = buildShaderInfo( m_vertShader, m_fragShader );
    VkPipelineVertexInputStateCreateInfo vertexInputState = buildVertexInputInfo();
    VkPipelineInputAssemblyStateCreateInfo assemblyState = buildInputAssemblyInfo();
    VkPipelineViewportStateCreateInfo viewportState = buildViewportInfo( m_extent );
    VkPipelineRasterizationStateCreateInfo rasterizerState = buildRasterizationInfo();
    VkPipelineMultisampleStateCreateInfo multisamplingState = buildMultiSamplingInfo(m_parent->msaaEnabled(), m_parent->getPhysical()->m_msaaSamples);
    VkPipelineDepthStencilStateCreateInfo depthStencilState = buildDepthStencilInfo();
    VkPipelineColorBlendStateCreateInfo blendingState = buildBlendingInfo();


    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = shaderStates.size();
    pipelineInfo.pStages = shaderStates.data();
    pipelineInfo.pVertexInputState = &vertexInputState;
    pipelineInfo.pInputAssemblyState = &assemblyState;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizerState;
    pipelineInfo.pMultisampleState = &multisamplingState;
    pipelineInfo.pDepthStencilState = &depthStencilState; // Optional
    pipelineInfo.pColorBlendState = &blendingState;
    pipelineInfo.pDynamicState = nullptr; // Optional

    pipelineInfo.layout = m_layout->get();

    pipelineInfo.renderPass = m_renderpass->get();
    pipelineInfo.subpass = 0;

    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1; // Optional

    if (vkCreateGraphicsPipelines(m_parent->get(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create graphics pipeline!");
    }
}

//--------------------------------------------------------------
void Pipeline::cleanup()
{
    vkDestroyPipeline(m_parent->get(), m_pipeline, nullptr);
}
