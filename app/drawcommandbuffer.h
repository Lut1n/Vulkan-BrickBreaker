#ifndef DRAWCOMMANDBUFFER_H
#define DRAWCOMMANDBUFFER_H

#include "common.h"

#include "swapchainresources.h"
#include "scene.h"
#include "vkinterface/commandbuffer.h"
#include "vkinterface/descriptorset.h"
#include "vkinterface/pipeline.h"


class DrawCommandBuffer
{
public:
    DrawCommandBuffer(SwapChainResources* resources, Pipeline* pipeline);
    virtual ~DrawCommandBuffer();

    void initialize();
    void cleanup();

    void drawScene(VkScene* scene);

    void beginDraw(int frameIndex);
    void bindCamera(SceneView* cam);
    void bindMaterial(Material* material);
    void bindMesh(Mesh* mesh);
    void drawEntity(Drawable* drawable);
    void endDraw();

    CommandBuffer* get(uint32_t index) {return m_cmdBuffer[index];}

protected:

    int currentFrameIndex;
    SceneView* m_pendingCamera;
    Material* m_pendingMaterial;
    Mesh* m_pendingMesh;

    enum {CAMERA,MATERIAL, MESH,
          COUNT // special value
         };
    bool m_changed[COUNT];

    std::vector<CommandBuffer*> m_cmdBuffer;
    SwapChainResources* m_resources;
    Pipeline* m_pipeline;
};

#endif // DRAWCOMMANDBUFFER_H
