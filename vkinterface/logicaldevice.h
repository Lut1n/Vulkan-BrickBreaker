#ifndef LOGICAL_DEVICE_H
#define LOGICAL_DEVICE_H


#include "common.h"
#include "physicaldevice.h"
#include "swapchain.h"
#include "queue.h"

#include "validationlayers.h"
#include "extensions.h"

class LogicalDevice
{
public:
    LogicalDevice(PhysicalDevice* phyDevice);
    virtual ~LogicalDevice();

    void initialize();
    void cleanup();

    VkDevice& get();
    PhysicalDevice* getPhysical();
    Queue* getGraphicQueue() {return m_graphicsQueue;}
    Queue* getPresentQueue() {return m_presentQueue;}
    Queue* getTransferQueue() {return m_transferQueue;}

    bool msaaEnabled() {return m_msaaEnabled;}

protected:
    VkDevice m_device;

    // associated queue to this device.
    Queue* m_graphicsQueue;
    Queue* m_presentQueue;
    Queue* m_transferQueue;

    // physical device from which this object has been create.
    PhysicalDevice* m_physicalDevice;

    // options
    bool m_msaaEnabled;

    ValidationLayers m_validationLayers;
    Extensions m_extensions;
};

#endif // LOGICAL_DEVICE_H
