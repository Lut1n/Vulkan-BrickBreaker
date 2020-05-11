#ifndef SYNCHRONIZATION_H
#define SYNCHRONIZATION_H

#include "vkiobject.h"

class Synchronization : public VkDevObject
{
public:
    Synchronization(LogicalDevice* device, uint32_t framesInFlight = MAX_FRAMES_IN_FLIGHT);
    virtual ~Synchronization();

    void initialize() override;
    void cleanup() override;

    void waitForFence();
    void resetFence();
    void nextFrame();

    VkSemaphore& getImgAvailableSemaphore();
    VkSemaphore& getRenderFinishedSemaphore();
    VkFence& getFence();

protected:
    std::vector<VkSemaphore> m_imgAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;

    uint32_t m_framesInFlight;
    uint32_t m_currentFrame;
};

#endif // SYNCHRONIZATION_H
