#ifndef QUEUE_H
#define QUEUE_H

#include "vkiobject.h"
#include "swapchain.h"

//--------------------------------------------------------------
class Queue : public VkDevObject
{
public:
    Queue(LogicalDevice* device, uint32_t familyIndex);
    virtual ~Queue();

    void initialize() override;
    void cleanup() override;

    void wait();
    void present(VkSemaphore semaphore, SwapChain* swapchain, uint32_t imageIndex);

    VkQueue& get() {return m_queue;}
    uint32_t getFamilyIndex() {return m_familyIndex;}

protected:
    VkQueue m_queue;

    uint32_t m_familyIndex;
};

#endif // QUEUE_H
