#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#include "vkiobject.h"
#include "buffer.h"

//--------------------------------------------------------------
struct Vertex
{
    // glm::vec2 pos;
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

    //--------------------------------------------------------------
    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription = {};

        std::cout << "vertex size " << sizeof(Vertex) << std::endl;

        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    //--------------------------------------------------------------
    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        // attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

        return attributeDescriptions;
    }

    bool operator==(const Vertex& other) const
    {
        return pos == other.pos && color == other.color && texCoord == other.texCoord;
    }
};


// hash function for Vertex
namespace std {
    template<> struct hash<Vertex> {
        size_t operator()(Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.pos) ^
                   (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
                   (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}

//--------------------------------------------------------------
class VertexBuffer : public VkDevObject
{
public:
    VertexBuffer(LogicalDevice* device, const std::string& filepath);
    VertexBuffer(LogicalDevice* device,
                 const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
    virtual ~VertexBuffer();

    void initialize() override;
    void cleanup() override;

    Buffer* getVertexBuffer() {return m_vertexbuffer;}
    Buffer* getIndexBuffer() {return m_indexbuffer;}
    uint32_t count() {return m_indices.size();}

protected:
    Buffer* m_vertexbuffer;
    Buffer* m_indexbuffer;

    std::vector<Vertex> m_vertices;
    std::vector<uint32_t> m_indices;
    std::string m_filepath;
};

#endif // VERTEXBUFFER_H
