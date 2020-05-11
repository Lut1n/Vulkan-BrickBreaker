#include "scene.h"


//--------------------------------------------------------------
bool BoundingBox::insideHorizontal(glm::vec3 p)
{
    return p.x >= center.x+tl.x && p.x < center.x+br.x;
}

//--------------------------------------------------------------
bool BoundingBox::insideVertical(glm::vec3 p)
{
    return p.y >= center.y+br.y && p.y < center.y+tl.y;
}

//--------------------------------------------------------------
bool BoundingBox::inside(glm::vec3 p)
{
    return insideHorizontal(p) && insideVertical(p);
}

//--------------------------------------------------------------
void SceneView::animate(State /*state*/, float /*time*/)
{
    updateMatrix(position, target);
}

//--------------------------------------------------------------
void SceneView::updateMatrix(glm::vec3 position,glm::vec3 target)
{
    projViewMatrix.view = glm::lookAt(position, target, glm::vec3(0.0f, 1.0f, 0.0f));
    projViewMatrix.proj = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);

    // GLM was originally designed for OpenGL, where the Y coordinate of the clip coordinates is inverted
    projViewMatrix.proj[1][1] *= -1;
}

//--------------------------------------------------------------
void SceneObject::animate(State /*state*/, float /*time*/)
{
    drawable->updateMatrix(position,rotation,scale,color);
}

//--------------------------------------------------------------
VkScene::VkScene()
     : m_camera(nullptr)
     , m_frameCount(0)
     , m_lastFpsSample(0.0)
{
}

//--------------------------------------------------------------
VkScene::~VkScene()
{
    cleanup();
}


//--------------------------------------------------------------
void VkScene::initialize()
{
}

//--------------------------------------------------------------
void VkScene::cleanup()
{
    uint32_t count = m_sceneObjects.size();
    for(uint32_t i=0; i<count; ++i) delete m_sceneObjects[i];
    m_sceneObjects.clear();
}

//--------------------------------------------------------------
void VkScene::animateScene()
{
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();

    // animation over time
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    m_frameCount++;
    if(time - m_lastFpsSample > 1.0)
    {
        std::cout << "FPS : " << m_frameCount << std::endl;
        m_frameCount = 0;
        m_lastFpsSample = time;
    }

    m_camera->animate(m_state, time);
    for(auto so : m_sceneObjects) so->animate(m_state, time);
}

//--------------------------------------------------------------
void VkScene::updateState(GLFWwindow* window)
{
    m_state.keyPressed[State::Left] = 1==glfwGetKey(window, GLFW_KEY_LEFT);
    m_state.keyPressed[State::Right] = 1==glfwGetKey(window, GLFW_KEY_RIGHT);
    m_state.keyPressed[State::Space] = 1==glfwGetKey(window, GLFW_KEY_SPACE);
}

//--------------------------------------------------------------
void VkScene::updateUniform(uint32_t currentFrameIndex)
{
    // upload uniform values
    m_camera->uniforms->update(currentFrameIndex, m_camera->projViewMatrix);
    for(auto so : m_sceneObjects)
    {
        if(!so->drawable->hidden)
            so->drawable->uniforms->update(currentFrameIndex, so->drawable->worldMatrix);
    }
}

//--------------------------------------------------------------
void VkScene::setView(SceneView* view)
{
    m_camera = view;
}

//--------------------------------------------------------------
void VkScene::add(SceneObject* obj)
{
    m_sceneObjects.push_back(obj);
}
