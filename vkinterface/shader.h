#ifndef SHADER_H
#define SHADER_H

#include "vkiobject.h"

//--------------------------------------------------------------
class Shader : public VkDevObject
{
public:
    Shader(LogicalDevice* device, const std::string& filename);
    virtual ~Shader();

    void initialize() override;
    void cleanup() override;

    VkShaderModule& get() {return m_module;}

protected:

    VkShaderModule m_module;

    std::string m_filename;
};

#endif // SHADER_H
