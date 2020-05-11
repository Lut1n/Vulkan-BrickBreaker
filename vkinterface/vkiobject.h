#ifndef VK_OBJECT_H
#define VK_OBJECT_H

#include "common.h"

template<typename T>
class VkObject
{
public:
    VkObject(T* parent)
        : m_parent(parent)
    {}

    virtual ~VkObject() {}

    virtual void initialize() = 0;
    virtual void cleanup() = 0;

protected:
    T* m_parent;
};

using VkInsObject = VkObject<class Instance>;
using VkDevObject = VkObject<class LogicalDevice>;

#endif // VK_OBJECT_H
