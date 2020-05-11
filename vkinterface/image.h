#ifndef IMAGE_H
#define IMAGE_H

#include "vkiobject.h"
#include "buffer.h"

class Image : VkDevObject
{
public:
    Image(LogicalDevice* device, uint32_t width, uint32_t height,
          uint32_t miplevels, bool enableMsaa, VkFormat format, VkImageUsageFlags usage);

    virtual ~Image();

    void initialize() override;
    void cleanup() override;

    void transitionLayout(VkImageLayout newLayout);
    void copy(Buffer* buffer);
    void generateMipmaps();

    VkImage& get() {return m_image;}
    VkImageLayout& getLayout() {return m_layout;}
    VkFormat getFormat() {return m_format;}

protected:
    VkImage m_image;
    VkImageLayout m_layout;
    VkDeviceMemory m_memory;

    uint32_t m_width;
    uint32_t m_height;
    uint32_t m_mipLevels;
    bool m_msaaEnabled;
    VkFormat m_format;
    VkImageUsageFlags m_usage;
};

#endif // IMAGE_H
