#ifndef PIPELINE_H
#define PIPELINE_H

#include "vkiobject.h"
#include "pipelinelayout.h"
#include "renderpass.h"
#include "swapchain.h"
#include "shader.h"
#include "vertexbuffer.h"

class Pipeline : VkDevObject
{
public:
    Pipeline(LogicalDevice* device, PipelineLayout* layout, RenderPass* renderpass,
             VkExtent2D extent, Shader* vertShader, Shader* fragShader);

    virtual ~Pipeline();

    void initialize() override;
    void cleanup() override;

    PipelineLayout* getLayout() {return m_layout;}

    VkPipeline& get() {return m_pipeline;}

protected:
    VkPipeline m_pipeline;

    PipelineLayout* m_layout;
    RenderPass* m_renderpass;
    VkExtent2D m_extent;
    Shader* m_vertShader;
    Shader* m_fragShader;
};

#endif // PIPELINE_H
