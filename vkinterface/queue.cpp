#include "queue.h"
#include "logicaldevice.h"

//--------------------------------------------------------------
Queue::Queue(LogicalDevice* device, uint32_t familyIndex)
    : VkDevObject(device)
    , m_familyIndex(familyIndex)
{
}

//--------------------------------------------------------------
Queue::~Queue()
{
    cleanup();
}

//--------------------------------------------------------------
void Queue::initialize()
{
    vkGetDeviceQueue(m_parent->get(), m_familyIndex, 0, &m_queue);
}

//--------------------------------------------------------------
void Queue::cleanup()
{
    m_queue = VK_NULL_HANDLE;
}

//--------------------------------------------------------------
void Queue::wait()
{
    vkQueueWaitIdle(m_queue);
}

//--------------------------------------------------------------
void Queue::present(VkSemaphore semaphore, SwapChain* swapchain, uint32_t imageIndex)
{
    VkSemaphore signalSemaphores[] = {semaphore};

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapchain->get()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    presentInfo.pResults = nullptr; // Optional

    VkResult result = vkQueuePresentKHR(m_queue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR/* || framebufferResized*/)
    {
        // framebufferResized = false;
        //
        // int width = 0, height = 0;
        // while (width == 0 || height == 0) {
        //     glfwGetFramebufferSize(window, &width, &height);
        //     glfwWaitEvents();
        // }

        // instance.device.recreateSwapChain(width, height, vertexBuffer, texture);

        throw std::runtime_error("framebuffer need to be resized");

    } else if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to present swap chain image!");
    }
}
