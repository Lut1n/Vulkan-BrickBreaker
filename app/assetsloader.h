#ifndef VKGAME_ASSETS_LOADER_H
#define VKGAME_ASSETS_LOADER_H

#include "common.h"
#include "vkinterface/vertexbuffer.h"
#include "vkinterface/texture.h"
#include "vkinterface/descriptorset.h"

static const std::string MODEL_BLOCK_PATH = "models/vkgame_block.obj.lut1";
static const std::string MODEL_BAR_PATH = "models/vkgame_bar.obj.lut1";
static const std::string MODEL_BALL_PATH = "models/vkgame_ball.obj.lut1";
static const std::string MODEL_BACKGROUND_PATH = "models/vkgame_background.obj.lut1";
static const std::string MODEL_PLATFORM_PATH = "models/vkgame_platform.obj.lut1";
static const std::string TEXTURE_PATH = "textures/handdrawing.jpg";



//--------------------------------------------------------------
struct Material
{
    Descriptors* descriptors;
    Texture* texture;

    Material() : descriptors(nullptr), texture(nullptr) {}
};

//--------------------------------------------------------------
struct Mesh
{
    VertexBuffer* buffer;
    Mesh() : buffer(nullptr) {}
};

//--------------------------------------------------------------
class AssetsLoader
{
public:
    AssetsLoader(LogicalDevice* device, DescriptorPool* pool, PipelineLayout* layout);
    virtual ~AssetsLoader();

    Mesh* getOrLoadMesh(const std::string& path);
    Material* getOrLoadTexture(const std::string& path);

    static AssetsLoader* instance() {return m_instance;}

protected:

    std::map<std::string, Mesh*> meshCache;
    std::map<std::string, Material*> materialCache;

    LogicalDevice* m_device;
    DescriptorPool* m_pool;
    PipelineLayout* m_layout;

    static AssetsLoader* m_instance;
};

#endif // VKGAME_ASSETS_LOADER_H
