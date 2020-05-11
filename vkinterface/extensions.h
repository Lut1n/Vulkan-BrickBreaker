#ifndef EXTENSIONS_H
#define EXTENSIONS_H

#include "common.h"

//--------------------------------------------------------------
class Extensions
{
public:
    Extensions();

    void enable(const std::string& extname);
    bool isEnabled(const std::string& extname) const;
    void disable(const std::string& extname);

    bool checkSupport();
    bool checkDeviceSupport(VkPhysicalDevice& device);

    void operator+=(const Extensions& that);

    uint32_t count() const;
    const char* const* data() const;

    static Extensions s_default();

protected:
    void updateBuf();

    std::vector<const char*> m_extBuf;
    std::vector<std::string> m_exts;
};

#endif // EXTENSIONS_H
