#include "descriptorset.h"
#include "logicaldevice.h"

//--------------------------------------------------------------
Descriptors::Descriptors(LogicalDevice* device, VkDescriptorPool pool, VkDescriptorSetLayout layout, uint32_t swapChainSize)
    : VkDevObject(device)
    , m_layout(layout)
    , m_pool(pool)
    , m_swapChainSize(swapChainSize)
{
    initialize();
}

//--------------------------------------------------------------
Descriptors::~Descriptors()
{
    cleanup();
}

//--------------------------------------------------------------
void Descriptors::initialize()
{
    std::vector<VkDescriptorSetLayout> layouts(m_swapChainSize, m_layout);
    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_pool;
    allocInfo.descriptorSetCount = m_swapChainSize;
    allocInfo.pSetLayouts = layouts.data();

    m_descriptorset.resize(m_swapChainSize);
    m_renderingOnframes.resize(m_swapChainSize, false);
    if (vkAllocateDescriptorSets(m_parent->get(), &allocInfo, m_descriptorset.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }
}

//--------------------------------------------------------------
void Descriptors::cleanup()
{
    vkFreeDescriptorSets(m_parent->get(), m_pool, m_descriptorset.size(), m_descriptorset.data());
}

//--------------------------------------------------------------
void Descriptors::update(ShaderUniforms& uniforms)
{
    uint32_t dstSize  = uniforms._bindings.size();
    std::vector<VkWriteDescriptorSet> descriptorWrites(dstSize);

    for (size_t i = 0; i < m_descriptorset.size(); i++)
    {
        int index = 0;
        for(auto& kv : uniforms._bindings)
        {
            ShaderUniforms::UniformTy ty = kv.second.type();
            uint32_t dstBinding = kv.first;

            descriptorWrites[index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[index].dstSet = m_descriptorset[i];
            descriptorWrites[index].dstBinding = dstBinding;
            descriptorWrites[index].dstArrayElement = 0;

            if(ty == ShaderUniforms::ViewUniform)
            {
                UniformBuffer* ubo = kv.second.transforms();

                VkDescriptorBufferInfo bufferInfo = {};
                bufferInfo.buffer = ubo->get(i)->get();
                bufferInfo.offset = 0;
                bufferInfo.range = sizeof(ViewBufferObject);

                descriptorWrites[index].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptorWrites[index].descriptorCount = 1;
                descriptorWrites[index].pBufferInfo = &bufferInfo;
            }
            else if(ty == ShaderUniforms::EntityUniform)
            {
                UniformBuffer* ubo = kv.second.transforms();

                VkDescriptorBufferInfo bufferInfo = {};
                bufferInfo.buffer = ubo->get(i)->get();
                bufferInfo.offset = 0;
                bufferInfo.range = sizeof(ColorBufferObject);

                descriptorWrites[index].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptorWrites[index].descriptorCount = 1;
                descriptorWrites[index].pBufferInfo = &bufferInfo;
            }
            else if(ty == ShaderUniforms::MaterialUniform)
            {
                Texture* tex = kv.second.texture();

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

//--------------------------------------------------------------
void Descriptors::onDrawBegin(uint32_t frameIndex)
{
    m_renderingOnframes[frameIndex] = true;
}

//--------------------------------------------------------------
void Descriptors::onDrawEnd(uint32_t frameIndex)
{
    m_renderingOnframes[frameIndex] = false;
}

//--------------------------------------------------------------
bool Descriptors::isReadyToBeFree()
{
    for(auto st : m_renderingOnframes) if(st) return false;
    return true;
}

//--------------------------------------------------------------
DescriptorPool::DescriptorPool(LogicalDevice* device, uint32_t swapChainSize, uint32_t pageSize)
    : VkDevObject(device)
    , m_swapChainSize(swapChainSize)
    , m_pageSize(pageSize)
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
}

//--------------------------------------------------------------
void DescriptorPool::cleanup()
{
    for(auto& p : freePools) freePool(p);
    for(auto& p : pagesInUse) freePool(p.pool);
    freePools.clear();
    pagesInUse.clear();
}

//--------------------------------------------------------------
Descriptors* DescriptorPool::alloc(VkDescriptorSetLayout layout)
{
    if(currentPoolIsFull()) requestPool();

    Page& currentPage = pagesInUse.back();
    Descriptors* set = new Descriptors(m_parent, currentPage.pool,layout, m_swapChainSize);

    currentPage.descriptors.push_back(set);
    return set;
}

//--------------------------------------------------------------
DescriptorPool::PageIt DescriptorPool::findPoolPage(VkDescriptorPool pool)
{
    for(auto pageit = pagesInUse.begin(); pageit!=pagesInUse.end(); pageit++)
        if(pageit->pool == pool) return pageit;

    return pagesInUse.end();
}

//--------------------------------------------------------------
void DescriptorPool::free(Descriptors* set)
{
    auto pageit = findPoolPage(set->getPool());
    if(pageit == pagesInUse.end()) throw "error : orphan descriptorsSet";

    auto it = std::find(pageit->descriptors.begin(), pageit->descriptors.end(), set);
    if(it == pageit->descriptors.end()) throw "error : orphan descriptors set";

    pageit->descriptors.erase(it);
    delete set;

    freePoolIfEmpty(pageit);
}

//--------------------------------------------------------------
void DescriptorPool::freePoolIfEmpty(PageIt pageit)
{
    if(pageit->descriptors.empty())
    {
        freePools.push_back(pageit->pool);
        pagesInUse.erase(pageit);
    }
}

//--------------------------------------------------------------
bool DescriptorPool::currentPoolIsFull()
{
    return (pagesInUse.empty() || pagesInUse.back().descriptors.size() >= m_pageSize);
}

//--------------------------------------------------------------
void DescriptorPool::requestPool()
{
    Page page;

    if( !freePools.empty() )
    {
        page.pool = freePools.back();
        freePools.pop_back();
    }
    else
    {
        page.pool = allocPool();
    }

    pagesInUse.push_back(page);
}

//--------------------------------------------------------------
VkDescriptorPool DescriptorPool::allocPool()
{
    const uint32_t TEXTURES_PER_PAGE = 1 * m_pageSize;
    const uint32_t BUFFERS_PER_PAGE = 3 * m_pageSize;

    std::array<VkDescriptorPoolSize, 2> poolSizes = {};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = BUFFERS_PER_PAGE*m_swapChainSize;
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = TEXTURES_PER_PAGE*m_swapChainSize;

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = m_pageSize * m_swapChainSize;
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

    VkDescriptorPool pool;
    if (vkCreateDescriptorPool(m_parent->get(), &poolInfo, nullptr, &pool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor pool!");
    }
    return pool;
}

//--------------------------------------------------------------
void DescriptorPool::freePool(VkDescriptorPool pool)
{
    vkDestroyDescriptorPool(m_parent->get(), pool, nullptr);
}
