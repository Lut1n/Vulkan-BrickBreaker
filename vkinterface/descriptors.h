#ifndef DESCRIPTORS_H
#define DESCRIPTORS_H

#include "vkiobject.h"
#include "pipelinelayout.h"
#include "uniformbuffer.h"
#include "texture.h"

struct ShaderUniforms
{
    enum UniformTy
    {
        Undefined,
        Transforms,
        TextureSampler,
    };

    struct Uniform
    {
        virtual UniformTy type() {return Undefined;}
        virtual Texture* texture() {return nullptr;}
        virtual UniformBufferObject* transforms() {return nullptr;}
    };

    std::map<uint32_t, Uniform*> _bindings;

    ShaderUniforms();

    virtual ~ShaderUniforms();

    void setUniform(uint32_t index, Uniform* u) {_bindings[index] = u;}
    void remUniform(uint32_t index) {_bindings.erase(index);}
};


//--------------------------------------------------------------
void DescriptorSet::update(ShaderUniforms* params)
{
    uint32_t dstSize  = params._binding.size();
    std::vector<VkWriteDescriptorSet> descriptorWrites(dstSize);

    for (size_t i = 0; i < m_count; i++)
    {
        int index = 0;
        for(auto& kv : params._bindings)
        {
            ParamTy ty = kv.second->type();
            uint32_t dstBinding = *kv.first;

            descriptorWrites[index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[index].dstSet = m_descriptorset[i];
            descriptorWrites[index].dstBinding = dstBinding;
            descriptorWrites[index].dstArrayElement = 0;

            if(ty == ShaderParameters::Transforms)
            {
                UniformBufferObject* ubo = kv.second->transforms();

                VkDescriptorBufferInfo bufferInfo = {};
                bufferInfo.buffer = ubo->get(i)->get();
                bufferInfo.offset = 0;
                bufferInfo.range = sizeof(UniformBufferObject);

                descriptorWrites[index].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptorWrites[index].descriptorCount = 1;
                descriptorWrites[index].pBufferInfo = &bufferInfo;
            }
            else if(ty == ShaderParameters::TextureSampler)
            {
                Texture* tex = kv.second->texture();

                VkDescriptorImageInfo imageInfo = {};
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfo.imageView = tex->getView()->get();
                imageInfo.sampler = tex->get();

                descriptorWrites[index].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                descriptorWrites[index].descriptorCount = 1;
                descriptorWrites[index].pImageInfo = &imageInfo;
            }
            index++;
        }

        vkUpdateDescriptorSets(m_parent->get(), descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
    }
}






class DescriptorPool : VkDevObject
{
public:
    DescriptorPool(LogicalDevice* device, uint32_t count);

    virtual ~DescriptorPool();

    void initialize() override;
    void cleanup() override;

    VkDescriptorPool& get() {return m_pool;}

protected:
    VkDescriptorPool m_pool;
    PipelineLayout* m_layout;

    uint32_t m_count;
};

class DescriptorSet : VkDevObject
{
public:
    DescriptorSet(LogicalDevice* device, DescriptorPool* pool, PipelineLayout* layout, uint32_t index, uint32_t count);

    virtual ~DescriptorSet();

    void initialize() override;
    void cleanup() override;

    void update(UniformBuffer* uniforms, Texture* texture = nullptr);

    std::vector<VkDescriptorSet>& get() {return m_descriptorset;}
    DescriptorPool* getPool() {return m_pool;}

protected:
    std::vector<VkDescriptorSet> m_descriptorset;
    PipelineLayout* m_layout;
    DescriptorPool* m_pool;

    uint32_t m_count;
    uint32_t m_index;
};

#endif // DESCRIPTORS_H
