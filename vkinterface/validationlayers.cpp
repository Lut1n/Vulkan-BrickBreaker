#include "vkinterface/validationlayers.h"


//--------------------------------------------------------------
ValidationLayers::ValidationLayers()
{
}

//--------------------------------------------------------------
void ValidationLayers::enable(const std::string& layername)
{
    auto it = std::find(m_layers.begin(),m_layers.end(),layername);
    if(it == m_layers.end())
    {
        m_layers.push_back(layername);
        updateBuf();
    }
}

//--------------------------------------------------------------
bool ValidationLayers::isEnabled(const std::string& layername) const
{
    if( std::find(m_layers.begin(),m_layers.end(),layername)==m_layers.end() )
        return false;
    return true;
}

//--------------------------------------------------------------
void ValidationLayers::disable(const std::string& layername)
{
    auto it = std::find(m_layers.begin(),m_layers.end(),layername);
    if(it != m_layers.end())
    {
        m_layers.erase(it);
        updateBuf();
    }
}

//--------------------------------------------------------------
bool ValidationLayers::checkSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : m_layersBuf)
    {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) return false;
    }

    return true;
}

//--------------------------------------------------------------
uint32_t ValidationLayers::count() const
{
    return m_layersBuf.size();
}

//--------------------------------------------------------------
void ValidationLayers::updateBuf()
{
    m_layersBuf.resize(m_layers.size());
    for(uint32_t i=0; i<m_layers.size(); ++i)
    {
        m_layersBuf[i] = m_layers[i].data();
    }
}

//--------------------------------------------------------------
const char* const* ValidationLayers::data() const
{
    return m_layersBuf.data();
}

//--------------------------------------------------------------
ValidationLayers ValidationLayers::s_default()
{
    ValidationLayers layers;
    layers.enable("VK_LAYER_KHRONOS_validation");
    return layers;
}
