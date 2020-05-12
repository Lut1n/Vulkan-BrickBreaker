#include "commandbuffer.h"
#include "logicaldevice.h"

//--------------------------------------------------------------
CommandBuffer::CommandBuffer(LogicalDevice* device, CommandPool* pool)
    : VkDevObject(device)
    , m_commandbuffer(VK_NULL_HANDLE)
    , m_pool(pool)
    , m_valid(true)
{
}

//--------------------------------------------------------------
CommandBuffer::CommandBuffer(LogicalDevice* device, CommandPool* pool, VkCommandBuffer buffer)
    : VkDevObject(device)
    , m_commandbuffer(buffer)
    , m_pool(pool)
    , m_valid(true)
{
}

//--------------------------------------------------------------
CommandBuffer::~CommandBuffer()
{
    cleanup();
}

//--------------------------------------------------------------
void CommandBuffer::initialize()
{
    if(m_commandbuffer != VK_NULL_HANDLE) return;

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_pool->get();
    allocInfo.commandBufferCount = 1;

    vkAllocateCommandBuffers(m_parent->get(), &allocInfo, &m_commandbuffer);
}

//--------------------------------------------------------------
void CommandBuffer::cleanup()
{
    vkFreeCommandBuffers(m_parent->get(), m_pool->get(), 1, &m_commandbuffer);
}

// CommandBuffer::reset
// the pool must have the flag VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT

//--------------------------------------------------------------
void CommandBuffer::beginOneTime()
{
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(m_commandbuffer, &beginInfo);
}

//--------------------------------------------------------------
void CommandBuffer::begin()
{
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer(m_commandbuffer, &beginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to begin recording command buffer!");
    }
}

//--------------------------------------------------------------
void CommandBuffer::end()
{
    if (vkEndCommandBuffer(m_commandbuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to record command buffer!");
    }
}

//--------------------------------------------------------------
void CommandBuffer::submitAndWait()
{
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_commandbuffer;

    Queue* queue = m_pool->getQueue();
    vkQueueSubmit(queue->get(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue->get());
}

//--------------------------------------------------------------
void CommandBuffer::copyBuffer(VkBuffer dst, VkBuffer src, uint32_t size)
{
    VkBufferCopy copyRegion = {};
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = 0; // Optional
    copyRegion.size = size;
    vkCmdCopyBuffer(m_commandbuffer, src, dst, 1, &copyRegion);
}

//--------------------------------------------------------------
CommandPool::CommandPool(LogicalDevice* device, Queue* queue)
    : VkDevObject(device)
    , m_queue(queue)
{
}

//--------------------------------------------------------------
CommandPool::~CommandPool()
{
    cleanup();
}

//--------------------------------------------------------------
void CommandPool::initialize()
{
    PhysicalDevice* physical = m_parent->getPhysical();
    auto indices = physical->m_indices;

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = m_queue->getFamilyIndex();
    poolInfo.flags = 0; // Optional
    if (vkCreateCommandPool(m_parent->get(), &poolInfo, nullptr, &m_pool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create command pool!");
    }
}

//--------------------------------------------------------------
void CommandPool::cleanup()
{
    vkDestroyCommandPool(m_parent->get(), m_pool, nullptr);
}

//--------------------------------------------------------------
void CommandPool::reset()
{
    VkCommandPoolResetFlags flags = 0; // see VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT
    vkResetCommandPool(m_parent->get(), m_pool, flags);
    for(auto it=m_allocated.begin();it!=m_allocated.end();++it)
    {
        (*it)->invalidate();
        delete *it;
    }
    m_allocated.clear();
}

//--------------------------------------------------------------
CommandBuffer* CommandPool::alloc()
{
    CommandBuffer* cmd = new CommandBuffer(m_parent, this);
    cmd->initialize();
    m_allocated.push_back(cmd);
    return cmd;
}

//--------------------------------------------------------------
std::vector<CommandBuffer*> CommandPool::alloc(uint32_t count)
{
    std::vector<VkCommandBuffer> buffers(count);

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_pool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = buffers.size();

    if (vkAllocateCommandBuffers(m_parent->get(), &allocInfo, buffers.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate command buffers!");
    }

    std::vector<CommandBuffer*> cmds(count);
    for(uint32_t i=0;i<count;++i)
    {
        cmds[i] = new CommandBuffer(m_parent, this, buffers[i]);
        m_allocated.push_back(cmds[i]);
    }
    return cmds;
}


//--------------------------------------------------------------
GraphicCmdPool* GraphicCmdPool::s_instance = nullptr;
TransferCmdPool* TransferCmdPool::s_instance = nullptr;
std::vector<DrawCmdPool*> DrawCmdPool::s_instances;

//--------------------------------------------------------------
GraphicCmdPool::GraphicCmdPool(LogicalDevice* device)
    : CommandPool(device, device->getGraphicQueue())
{
    s_instance = this;
}
//--------------------------------------------------------------
DrawCmdPool::DrawCmdPool(LogicalDevice* device)
    : CommandPool(device, device->getGraphicQueue())
{
    s_instances.push_back(this);
}
//--------------------------------------------------------------
TransferCmdPool::TransferCmdPool(LogicalDevice* device)
    : CommandPool(device, device->getTransferQueue())
{
    s_instance = this;
}
