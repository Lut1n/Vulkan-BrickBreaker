#include "shader.h"
#include "logicaldevice.h"

//--------------------------------------------------------------
Shader::Shader(LogicalDevice* device, const std::string& filename)
    : VkDevObject(device)
    , m_filename(filename)
{

}

//--------------------------------------------------------------
Shader::~Shader()
{
    cleanup();
}

//--------------------------------------------------------------
static std::vector<char> readFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        std::cout << "shader " << filename << std::endl;
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();
    return buffer;
}

//--------------------------------------------------------------
void Shader::initialize()
{
    auto code = readFile(m_filename);

    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    if (vkCreateShaderModule(m_parent->get(), &createInfo, nullptr, &m_module) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create shader module!");
    }
}

//--------------------------------------------------------------
void Shader::cleanup()
{
    vkDestroyShaderModule(m_parent->get(), m_module, nullptr);
}
