#ifndef COMMANDBUFFER_H
#define COMMANDBUFFER_H

#include "vkiobject.h"
#include "queue.h"

class CommandPool;

//--------------------------------------------------------------
class CommandBuffer : public VkDevObject
{
public:
    CommandBuffer(LogicalDevice* device, CommandPool* pool);
    CommandBuffer(LogicalDevice* device, CommandPool* pool, VkCommandBuffer buffer);
    virtual ~CommandBuffer();

    void initialize() override;
    void cleanup() override;

    void beginOneTime();
    void begin();
    void end();
    void submitAndWait();

    void copyBuffer(VkBuffer dst, VkBuffer src, uint32_t size);

    VkCommandBuffer& get() {return m_commandbuffer;}

    bool isValid() {return m_valid;}
    void invalidate(){m_valid = false;}

protected:
    VkCommandBuffer m_commandbuffer;
    CommandPool* m_pool;
    bool m_valid;
};

//--------------------------------------------------------------
class CommandPool : public VkDevObject
{
public:
    CommandPool(LogicalDevice* device, Queue* queue);
    virtual ~CommandPool();

    void initialize() override;
    void cleanup() override;

    void reset();

    CommandBuffer* alloc();
    std::vector<CommandBuffer*> alloc(uint32_t count);

    Queue* getQueue() {return m_queue;}

    VkCommandPool& get() {return m_pool;}

protected:
    VkCommandPool m_pool;
    Queue* m_queue;
    std::vector<CommandBuffer*> m_allocated;
};


//--------------------------------------------------------------
class GraphicCmdPool : public CommandPool
{
public:
    GraphicCmdPool(LogicalDevice* device);
    static GraphicCmdPool* instance() {return s_instance;}
protected:
    static GraphicCmdPool* s_instance;
};

//--------------------------------------------------------------
class DrawCmdPool : public CommandPool
{
public:
    DrawCmdPool(LogicalDevice* device);
    static DrawCmdPool* instance(int index) {return s_instances[index];}
protected:
    static std::vector<DrawCmdPool*> s_instances;
};

//--------------------------------------------------------------
class TransferCmdPool : public CommandPool
{
public:
    TransferCmdPool(LogicalDevice* device);
    static TransferCmdPool* instance() {return s_instance;}
protected:
    static TransferCmdPool* s_instance;
};

#endif // COMMANDBUFFER_H
