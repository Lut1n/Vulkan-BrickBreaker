#ifndef VALIDATION_LAYERS_H
#define VALIDATION_LAYERS_H

#include "common.h"

//--------------------------------------------------------------
class ValidationLayers
{
public:
    ValidationLayers();

    void enable(const std::string& layername);
    bool isEnabled(const std::string& layername) const;
    void disable(const std::string& layername);

    bool checkSupport();

    uint32_t count() const;
    const char* const* data() const;

    static ValidationLayers s_default();

protected:
    void updateBuf();

    std::vector<const char*> m_layersBuf;
    std::vector<std::string> m_layers;
};

#endif // VALIDATION_LAYERS_H
