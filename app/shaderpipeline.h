#ifndef SHADERPIPELINE_H
#define SHADERPIPELINE_H

#include "common.h"
#include "vkinterface/pipeline.h"
#include "vkinterface/pipelinelayout.h"

#include "swapchainresources.h"

class ShaderPipeline
{
public:
    ShaderPipeline(LogicalDevice* device, RenderPass* renderpass, VkExtent2D extent);
    virtual ~ShaderPipeline();

    void initialize();
    void cleanup();

    PipelineLayout* layout(){return m_layout;}
    Pipeline* pipeline() {return m_pipeline;}

protected:
    LogicalDevice* m_device;
    RenderPass* m_renderpass;
    PipelineLayout* m_layout;
    Shader* m_vertexShader;
    Shader* m_fragmentShader;
    VkExtent2D m_extent;
    Pipeline* m_pipeline;
};

#endif // SHADERPIPELINE_H
