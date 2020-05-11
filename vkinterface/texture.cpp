#include "texture.h"
#include "logicaldevice.h"
#include "commandbuffer.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


//--------------------------------------------------------------
Texture::Texture(LogicalDevice* device, const std::string& filepath, bool hasmipmap)
    : VkDevObject(device)
    , m_mipmapEnabled(hasmipmap)
    , m_mipLevels(1)
    , m_filepath(filepath)
{
}

//--------------------------------------------------------------
Texture::~Texture()
{
    cleanup();
}

//--------------------------------------------------------------
uint32_t computeMipLevels(uint32_t w, uint32_t h)
{
    return static_cast<uint32_t>(std::floor(std::log2(std::max(w,h)))) + 1;
}

//--------------------------------------------------------------
Buffer* loadFromFile(LogicalDevice* device, const std::string& path, uint32_t& width, uint32_t& height, uint32_t& channels)
{
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels)
    {
        throw std::runtime_error("failed to load texture image!");
    }

    Buffer* buffer = new Buffer(device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    buffer->initialize();
    buffer->copy(pixels, imageSize);

    stbi_image_free(pixels);

    width = texWidth;
    height = texHeight;
    channels = texChannels;
    return buffer;
}

//--------------------------------------------------------------
void Texture::initialize()
{
    Buffer* stagingBuffer = loadFromFile(m_parent, m_filepath, m_width, m_height, m_channels);

    if(m_mipmapEnabled) m_mipLevels = computeMipLevels(m_width, m_height);


    // VkFormat format = VK_FORMAT_B8G8R8A8_UNORM;
    VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;

    m_image = new Image(m_parent, m_width, m_height, m_mipLevels, false, format,
                        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
    m_image->initialize();

    // layout transition (optimal for transfer)
    m_image->transitionLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    // update pixel data
    m_image->copy(stagingBuffer);

    // transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating mipmaps
    if(m_mipmapEnabled) m_image->generateMipmaps();
    // layout transition (optimal for shader)
    else m_image->transitionLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    delete stagingBuffer;


    // image view
    m_view = new ImageView(m_parent, m_image, VK_IMAGE_ASPECT_COLOR_BIT);
    m_view->initialize();

    // sampler creation
    VkSamplerCreateInfo samplerInfo = {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;

    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = 16;

    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;

    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = m_mipmapEnabled ? static_cast<float>(m_mipLevels) : 0.0f;

    if (vkCreateSampler(m_parent->get(), &samplerInfo, nullptr, &m_sampler) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create texture sampler!");
    }
}

//--------------------------------------------------------------
void Texture::cleanup()
{
    vkDestroySampler(m_parent->get(), m_sampler, nullptr);
    delete m_view;
    delete m_image;
}
