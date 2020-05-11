#include "synchronization.h"
#include "logicaldevice.h"


//--------------------------------------------------------------
Synchronization::Synchronization(LogicalDevice* device, uint32_t framesInFlight)
    : VkDevObject(device)
    , m_framesInFlight(framesInFlight)
    , m_currentFrame(0)
{
}

//--------------------------------------------------------------
Synchronization::~Synchronization()
{
    cleanup();
}

//--------------------------------------------------------------
void Synchronization::initialize()
{
    m_imgAvailableSemaphores.resize(m_framesInFlight);
    m_renderFinishedSemaphores.resize(m_framesInFlight);
    m_inFlightFences.resize(m_framesInFlight);

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < m_framesInFlight; i++)
    {
        if (vkCreateSemaphore(m_parent->get(), &semaphoreInfo, nullptr, &m_imgAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(m_parent->get(), &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(m_parent->get(), &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
}

//--------------------------------------------------------------
void Synchronization::cleanup()
{
    for (size_t i = 0; i < m_framesInFlight; i++)
    {
        vkDestroySemaphore(m_parent->get(), m_imgAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(m_parent->get(), m_renderFinishedSemaphores[i], nullptr);
        vkDestroyFence(m_parent->get(), m_inFlightFences[i], nullptr);
    }
    m_imgAvailableSemaphores.clear();
    m_renderFinishedSemaphores.clear();
    m_inFlightFences.clear();
}

//--------------------------------------------------------------
void Synchronization::waitForFence()
{
    vkWaitForFences(m_parent->get(), 1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);
}

//--------------------------------------------------------------
void Synchronization::resetFence()
{
    vkResetFences(m_parent->get(), 1, &m_inFlightFences[m_currentFrame]);
}

//--------------------------------------------------------------
void Synchronization::nextFrame()
{
    m_currentFrame = (m_currentFrame + 1) % m_framesInFlight;
}

//--------------------------------------------------------------
VkSemaphore& Synchronization::getImgAvailableSemaphore()
{
    return m_imgAvailableSemaphores[m_currentFrame];
}

//--------------------------------------------------------------
VkSemaphore& Synchronization::getRenderFinishedSemaphore()
{
    return m_renderFinishedSemaphores[m_currentFrame];
}

//--------------------------------------------------------------
VkFence& Synchronization::getFence()
{
    return m_inFlightFences[m_currentFrame];
}
