#include "extensions.h"

//--------------------------------------------------------------
Extensions::Extensions()
{
}

//--------------------------------------------------------------
void Extensions::enable(const std::string& extname)
{
    auto it = std::find(m_exts.begin(),m_exts.end(),extname);
    if(it == m_exts.end())
    {
        m_exts.push_back(extname);
        updateBuf();
    }
}

//--------------------------------------------------------------
bool Extensions::isEnabled(const std::string& extname) const
{
    if( std::find(m_exts.begin(),m_exts.end(),extname)==m_exts.end() )
        return false;
    return true;
}

//--------------------------------------------------------------
void Extensions::disable(const std::string& extname)
{
    auto it = std::find(m_exts.begin(),m_exts.end(),extname);
    if(it != m_exts.end())
    {
        m_exts.erase(it);
        updateBuf();
    }
}

//--------------------------------------------------------------
bool Extensions::checkSupport()
{
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExts(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExts.data());

    for (const auto& extName : m_extBuf)
    {
        bool layerFound = false;

        for (const auto& extProperties : availableExts) {
            if (strcmp(extName, extProperties.extensionName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) return false;
    }

    return true;
}

//--------------------------------------------------------------
bool Extensions::checkDeviceSupport(VkPhysicalDevice& device)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> required (m_exts.begin(), m_exts.end());

    for (const auto& ext : availableExtensions)
        required.erase(ext.extensionName);

    return required.empty();
}

//--------------------------------------------------------------
void Extensions::operator+=(const Extensions& that)
{
    for(auto it : that.m_exts) enable(it);
    updateBuf();
}

//--------------------------------------------------------------
void Extensions::updateBuf()
{
    m_extBuf.resize(m_exts.size());
    for(uint32_t i=0; i<m_exts.size(); ++i)
    {
        m_extBuf[i] = m_exts[i].data();
    }
}

//--------------------------------------------------------------
uint32_t Extensions::count() const
{
    return m_extBuf.size();
}

//--------------------------------------------------------------
const char* const* Extensions::data() const
{
    return m_extBuf.data();
}

//--------------------------------------------------------------
Extensions Extensions::s_default()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    Extensions extensions;
    for(uint32_t ext=0;ext<glfwExtensionCount;++ext) extensions.enable( glfwExtensions[ext] );

    if (enableValidationLayers) extensions.enable(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    return extensions;
}
