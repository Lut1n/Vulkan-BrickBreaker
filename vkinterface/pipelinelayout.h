#ifndef PIPELINE_LAYOUT_H
#define PIPELINE_LAYOUT_H

#include "vkiobject.h"

class PipelineLayout : VkDevObject
{
public:
    PipelineLayout(LogicalDevice* device);

    virtual ~PipelineLayout();

    void initialize() override;
    void cleanup() override;

    VkPipelineLayout& get() {return m_layout;}
    VkDescriptorSetLayout& getDescriptorSetLayout(uint32_t index) {return m_descriptorSetLayouts[index];}

protected:
    VkPipelineLayout m_layout;
    std::vector<VkDescriptorSetLayout> m_descriptorSetLayouts;
};

#endif // PIPELINE_LAYOUT_H
