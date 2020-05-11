#include "pipelinelayout.h"
#include "logicaldevice.h"

//--------------------------------------------------------------
PipelineLayout::PipelineLayout(LogicalDevice* device)
    : VkDevObject(device)
{
}

//--------------------------------------------------------------
PipelineLayout::~PipelineLayout()
{
    cleanup();
}

VkDescriptorSetLayoutBinding getUboBindingInfo(uint32_t binding = 0)
{
    // layout for the uniform buffer
    VkDescriptorSetLayoutBinding info = {};
    info.binding = binding;
    info.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    info.descriptorCount = 1;
    info.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    info.pImmutableSamplers = nullptr; // Optional

    return info;
}

VkDescriptorSetLayoutBinding getTexBindingInfo(uint32_t binding = 0)
{
    // layout for the image sampler
    VkDescriptorSetLayoutBinding info = {};
    info.binding = binding;
    info.descriptorCount = 1;
    info.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    info.pImmutableSamplers = nullptr;
    info.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    return info;
}


//--------------------------------------------------------------
void PipelineLayout::initialize()
{
    // descriptorSet #0 - global (proj/view matrix in a uniform buffer object)
    // descriptorSet #1 - per material (texture)
    // descriptorSet #2 - per object (model matrix and color)
    m_descriptorSetLayouts.resize(3);

    auto uboInfo = getUboBindingInfo(0);
    auto texInfo = getTexBindingInfo(0);

    // #0
    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboInfo;
    if (vkCreateDescriptorSetLayout(m_parent->get(), &layoutInfo, nullptr, &m_descriptorSetLayouts[0]) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor set layout!");
    }

    // #1
    layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &texInfo;
    if (vkCreateDescriptorSetLayout(m_parent->get(), &layoutInfo, nullptr, &m_descriptorSetLayouts[1]) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor set layout!");
    }

    // #2
    layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboInfo;
    if (vkCreateDescriptorSetLayout(m_parent->get(), &layoutInfo, nullptr, &m_descriptorSetLayouts[2]) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor set layout!");
    }


    // --- PIPELINE LAYOUT ---
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

    pipelineLayoutInfo.setLayoutCount = m_descriptorSetLayouts.size();
    pipelineLayoutInfo.pSetLayouts = m_descriptorSetLayouts.data();

    if (vkCreatePipelineLayout(m_parent->get(), &pipelineLayoutInfo, nullptr, &m_layout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}

//--------------------------------------------------------------
void PipelineLayout::cleanup()
{
    vkDestroyPipelineLayout(m_parent->get(), m_layout, nullptr);
    for(auto dslayout : m_descriptorSetLayouts)
        vkDestroyDescriptorSetLayout(m_parent->get(), dslayout, nullptr);
    m_descriptorSetLayouts.clear();
}
