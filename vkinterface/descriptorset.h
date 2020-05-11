#ifndef DESCRIPTORSET_H
#define DESCRIPTORSET_H

#include "vkiobject.h"
#include "pipelinelayout.h"
#include "uniformbuffer.h"
#include "texture.h"

struct ShaderUniforms
{
    enum UniformTy
    {
        Undefined,
        ViewUniform,
        MaterialUniform,
        EntityUniform,
    };

    struct Uniform
    {
        UniformTy _type = Undefined;
        Texture* _tex;
        UniformBuffer* _buffer;

        UniformTy type() {return _type;}
        Texture* texture() {return _tex;}
        UniformBuffer* transforms() {return _buffer;}
    };

    std::map<uint32_t, Uniform> _bindings;

    ShaderUniforms(){}

    virtual ~ShaderUniforms(){}

    void setUniform(uint32_t index, Uniform u) {_bindings[index] = u;}
    void setMaterialUniform(uint32_t index, Texture* tex) {_bindings[index] = Uniform{MaterialUniform, tex, nullptr};}
    void setViewUniform(uint32_t index, UniformBuffer* b) {_bindings[index] = Uniform{ViewUniform, nullptr, b};}
    void setEntityUniform(uint32_t index, UniformBuffer* b) {_bindings[index] = Uniform{EntityUniform, nullptr, b};}
    void remUniform(uint32_t index) {_bindings.erase(index);}
};

class DescriptorPool;
class Descriptors : VkDevObject
{
public:
    Descriptors(LogicalDevice* device, VkDescriptorPool pool, VkDescriptorSetLayout layout, uint32_t swapChainSize);

    virtual ~Descriptors();

    void initialize() override;
    void cleanup() override;

    void update(ShaderUniforms& uniforms);

    VkDescriptorSet get(uint32_t frameIndex) {return m_descriptorset[frameIndex];}
    VkDescriptorSetLayout getLayout() {return m_layout;}
    VkDescriptorPool getPool() {return m_pool;}

    // track state over swap chain
    void onDrawBegin(uint32_t frameIndex);
    void onDrawEnd(uint32_t frameIndex);
    bool isReadyToBeFree();

protected:
    std::vector<VkDescriptorSet> m_descriptorset;
    VkDescriptorSetLayout m_layout;
    VkDescriptorPool m_pool;
    uint32_t m_swapChainSize;

    std::vector<bool> m_renderingOnframes;
};


class DescriptorPool : VkDevObject
{
public:

    struct Page
    {
        VkDescriptorPool pool;
        std::vector<Descriptors*> descriptors;
    };
    using PageIt = std::vector<Page>::iterator;


    DescriptorPool(LogicalDevice* device, uint32_t swapChainSize, uint32_t pageSize = 3);

    virtual ~DescriptorPool();

    void initialize() override;
    void cleanup() override;

    Descriptors* alloc(VkDescriptorSetLayout layout);
    void free(Descriptors* set);

protected:
   std::vector<VkDescriptorPool> freePools;
   std::vector<Page> pagesInUse;
   uint32_t m_swapChainSize;
   uint32_t m_pageSize;

   void requestPool();
   bool currentPoolIsFull();
   void freePoolIfEmpty(PageIt pageit);
   PageIt findPoolPage(VkDescriptorPool pool);

   VkDescriptorPool allocPool();
   void freePool(VkDescriptorPool pool);
};

#endif // DESCRIPTORSET_H
