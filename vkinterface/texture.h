#ifndef TEXTURE_H
#define TEXTURE_H

#include "vkiobject.h"
#include "imageview.h"
#include "buffer.h"

class Texture : public VkDevObject
{
public:
    Texture(LogicalDevice* device, const std::string& filepath, bool hasmipmap = false);
    virtual ~Texture();

    void initialize() override;
    void cleanup() override;

    void generateMipmaps(LogicalDevice* device, VkImage image, VkFormat imageFormat,
                                  int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

    VkSampler& get() {return m_sampler;}
    ImageView* getView() {return m_view;}

    bool hasMipmap();
    uint32_t miplevels() {return m_mipLevels;}

protected:
    VkSampler m_sampler;

    Buffer* m_pixels;
    uint32_t m_width, m_height, m_channels;

    bool m_mipmapEnabled;
    uint32_t m_mipLevels;

    ImageView* m_view;
    Image* m_image;
    std::string m_filepath;
};

#endif // TEXTURE_H
