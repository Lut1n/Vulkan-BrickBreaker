#include "descriptorset.h"
#include "logicaldevice.h"


//--------------------------------------------------------------
DescriptorSet::DescriptorSet(LogicalDevice* device, DescriptorPool* pool, PipelineLayout* layout, uint32_t index, uint32_t count)
    : VkDevObject(device)
    , m_layout(layout)
    , m_pool(pool)
    , m_count(count)
    , m_index(index)
{
}

//--------------------------------------------------------------
DescriptorSet::~DescriptorSet()
{
    cleanup();
}

//--------------------------------------------------------------
void DescriptorSet::initialize()
{
    // --- DESCRIPTORSET ---
    std::vector<VkDescriptorSetLayout> layouts(m_count, m_layout->getDescriptorSetLayout(m_index));
    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_pool->get();
    allocInfo.descriptorSetCount = m_count;
    allocInfo.pSetLayouts = layouts.data();

    m_descriptorset.resize(m_count);
    if (vkAllocateDescriptorSets(m_parent->get(), &allocInfo, m_descriptorset.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

}

//--------------------------------------------------------------
void DescriptorSet::cleanup()
{
    vkFreeDescriptorSets(m_parent->get(), m_pool->get(), m_descriptorset.size(), m_descriptorset.data());
}

//--------------------------------------------------------------
void DescriptorSet::update(UniformBuffer* uniforms, Texture* texture)
{
    for (size_t i = 0; i < m_count; i++)
    {
        // uniform buffer part
        VkDescriptorBufferInfo bufferInfo = {};
        bufferInfo.buffer = uniforms->get(i)->get();
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        // image sampler part
        VkDescriptorImageInfo imageInfo = {};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        if(texture)
        {
            imageInfo.imageView = texture->getView()->get();
            imageInfo.sampler = texture->get();
        }

        std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};

        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = m_descriptorset[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;
        // descriptorWrites[0].pImageInfo = nullptr; // Optional
        // descriptorWrites[0].pTexelBufferView = nullptr; // Optional


        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = m_descriptorset[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;

        uint32_t dscsize = 1; if(texture) dscsize = 2;
        vkUpdateDescriptorSets(m_parent->get(), dscsize, descriptorWrites.data(), 0, nullptr);
    }
}


//--------------------------------------------------------------
DescriptorPool::DescriptorPool(LogicalDevice* device, uint32_t count)
    : VkDevObject(device)
    , m_count(count)
{

}

//--------------------------------------------------------------
DescriptorPool::~DescriptorPool()
{
    cleanup();
}

//--------------------------------------------------------------
void DescriptorPool::initialize()
{
    const uint32_t MAX_OBJ = 2;

    // take in account uniform buffer and image sampler
    std::array<VkDescriptorPoolSize, 2> poolSizes = {};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = m_count + m_count*MAX_OBJ;
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = m_count;

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = m_count + m_count*MAX_OBJ;

    if (vkCreateDescriptorPool(m_parent->get(), &poolInfo, nullptr, &m_pool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

//--------------------------------------------------------------
void DescriptorPool::cleanup()
{
    // descriptorSets are implicitly destroyed with the pool
    vkDestroyDescriptorPool(m_parent->get(), m_pool, nullptr);
}
