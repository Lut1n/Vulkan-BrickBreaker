#include "assetsloader.h"


AssetsLoader* AssetsLoader::m_instance = nullptr;

//--------------------------------------------------------------
AssetsLoader::AssetsLoader(LogicalDevice* device, DescriptorPool* pool, PipelineLayout* layout)
    : m_device(device)
    , m_pool(pool)
    , m_layout(layout)
{
    m_instance = this;
}

//--------------------------------------------------------------
AssetsLoader::~AssetsLoader()
{
    for(auto& it : meshCache)
    {
        delete it.second->buffer;
        delete it.second;
    }
    meshCache.clear();


    for(auto& it : materialCache)
    {
        delete it.second->texture;
        delete it.second;
    }
    materialCache.clear();
}

//--------------------------------------------------------------
Mesh* AssetsLoader::getOrLoadMesh(const std::string& path)
{
    auto found = meshCache.find(path);
    if(found == meshCache.end())
    {
        Mesh* mesh = new Mesh();
        mesh->buffer = new VertexBuffer(m_device, path);
        mesh->buffer->initialize();
        mdebug("mesh loading ok");

        meshCache[path] = mesh;
        return mesh;
    }

    return found->second;
}

//--------------------------------------------------------------
Material* AssetsLoader::getOrLoadTexture(const std::string& path)
{
    auto found = materialCache.find(path);
    if(found == materialCache.end())
    {
        Material* mat = new Material();
        mat->texture = new Texture(m_device, path, true);
        mat->texture->initialize();
        mdebug("texture loading ok");

        mat->descriptors = m_pool->alloc(m_layout->getDescriptorSetLayout(1));
        ShaderUniforms uniform;
        uniform.setMaterialUniform(0, mat->texture);
        mat->descriptors->update(uniform);

        materialCache[path] = mat;
        return mat;
    }

    return found->second;
}
