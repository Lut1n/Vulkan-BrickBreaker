#ifndef SCENERESOURCES_H
#define SCENERESOURCES_H

#include "common.h"
#include "vkinterface/uniformbuffer.h"
#include "vkinterface/descriptorset.h"

#include "assetsloader.h"
#include "drawables.h"


//--------------------------------------------------------------
struct State
{
    enum Key
    {
        Left,
        Right,
        Space,

        Count // special value
    };
    bool keyPressed[Count];
};

//--------------------------------------------------------------
struct BoundingBox
{
    glm::vec3 center;
    glm::vec3 tl;
    glm::vec3 br;

    bool inside(glm::vec3 p);
    bool insideHorizontal(glm::vec3 p);
    bool insideVertical(glm::vec3 p);
};

//--------------------------------------------------------------
struct SceneView
{
    ViewBufferObject projViewMatrix;
    float aspectRatio;
    glm::vec3 position;
    glm::vec3 target;

    UniformBuffer* uniforms;
    Descriptors* descriptors;

    SceneView()
        : position(5.0,5.0,5.0)
        , target(0.0,0.0,0.0)
        , uniforms(nullptr)
        , descriptors(nullptr)
    {}

    virtual void animate(State state, float time);
    void updateMatrix(glm::vec3 p,glm::vec3 t);
};

//--------------------------------------------------------------
struct SceneObject
{
    Mesh* mesh;
    Material* material;
    Drawable* drawable;

    glm::vec3 color;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    SceneObject(){}
    virtual ~SceneObject() {}

    virtual void animate(State state, float time);
};

//--------------------------------------------------------------
class VkScene
{
public:
    VkScene();
    virtual ~VkScene();

    void initialize();
    void cleanup();

    void updateState(GLFWwindow* window);
    void animateScene();
    void updateUniform(uint32_t currentFrameIndex);

    void setView(SceneView* view);
    void add(SceneObject* obj);

    SceneView* getCamera(){return m_camera;}
    std::vector<SceneObject*>& getSceneObjects(){return m_sceneObjects;}

protected:
    std::vector<SceneObject*> m_sceneObjects;
    SceneView* m_camera;
    State m_state;

    // fps computation
    uint32_t m_frameCount;
    float m_lastFpsSample;
};

#endif // SCENERESOURCES_H
