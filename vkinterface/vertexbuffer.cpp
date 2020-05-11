#include "vertexbuffer.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

//--------------------------------------------------------------
VertexBuffer::VertexBuffer(LogicalDevice* device, const std::string& filepath)
    : VkDevObject(device)
    , m_filepath(filepath)
{
}

//--------------------------------------------------------------
VertexBuffer::VertexBuffer(LogicalDevice* device,
                           const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
    : VkDevObject(device)
    , m_vertices(vertices)
    , m_indices(indices)
    , m_filepath("")
{
}

//--------------------------------------------------------------
VertexBuffer::~VertexBuffer()
{
    cleanup();
}

//--------------------------------------------------------------
template<typename T>
void load(LogicalDevice* device, const std::vector<T>& src, Buffer** dst, bool isIndex)
{
    VkDeviceSize bufferSize = sizeof(T) * src.size();

    Buffer stagingBuffer(device, bufferSize,
                         VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    stagingBuffer.initialize();
    stagingBuffer.copy((void*)src.data(), bufferSize);

    VkBufferUsageFlags usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    if(isIndex) usage = usage | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    else usage = usage | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    *dst = new Buffer(device, bufferSize, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    Buffer* buffer = *dst;

    buffer->initialize();
    buffer->copy(&stagingBuffer);
}

//--------------------------------------------------------------
void loadFromFile(const std::string& filepath, std::vector<uint32_t>& indices, std::vector<Vertex>& vertices)
{
    const bool FLIP_Y = true;

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if ( !tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str()) )
    {
        throw std::runtime_error(warn + err);
    }


    std::unordered_map<Vertex, uint32_t> uniqueVertices = {};

    for (const auto& shape : shapes)
        for (const auto& index : shape.mesh.indices)
    {
        Vertex vertex = {};

        vertex.pos = {
            attrib.vertices[3 * index.vertex_index + 0],
            attrib.vertices[3 * index.vertex_index + 1],
            attrib.vertices[3 * index.vertex_index + 2]
        };

        if(FLIP_Y)
        {
            vertex.texCoord = {
                attrib.texcoords[2 * index.texcoord_index + 0],
                1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
            };
        }
        else
        {
            vertex.texCoord = {
                attrib.texcoords[2 * index.texcoord_index + 0],
                attrib.texcoords[2 * index.texcoord_index + 1]
            };
        }

        vertex.color = {1.0f, 1.0f, 1.0f};

        // vertex deduplication
        if (uniqueVertices.count(vertex) == 0) {
            uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
            vertices.push_back(vertex);
        }

        indices.push_back(uniqueVertices[vertex]);
    }
}

//--------------------------------------------------------------
void VertexBuffer::initialize()
{
    if( !m_filepath.empty() )
    {
        loadFromFile(m_filepath, m_indices, m_vertices);
    }

    load(m_parent, m_indices, &m_indexbuffer,true);
    load(m_parent, m_vertices, &m_vertexbuffer,false);
}

//--------------------------------------------------------------
void VertexBuffer::cleanup()
{
    delete m_indexbuffer;
    delete m_vertexbuffer;
}
