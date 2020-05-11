#include "application.h"

#include "game/brick.h"
#include "game/paddle.h"
#include "game/platform.h"
#include "game/background.h"
#include "game/camera.h"
#include "game/ball.h"

//--------------------------------------------------------------
MiniVkApplication::MiniVkApplication()
    : currentFrame(0)
    , framebufferResized(false)
    , m_window(nullptr)
{
}


//--------------------------------------------------------------
MiniVkApplication::~MiniVkApplication()
{
}


//--------------------------------------------------------------
void MiniVkApplication::run() {
    initWindow();
    initVulkan();
    initScene();
    mainLoop();
    cleanup();
}

//--------------------------------------------------------------
void MiniVkApplication::initWindow()
{
    glfwInit();

    // dont create OGL context
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // special case of resizing (disable for this time)
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_window = glfwCreateWindow(APP_WIDTH, APP_HEIGHT, "Brick Breaker with Vulkan", nullptr, nullptr);
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, framebufferResizeCallback);
}

//--------------------------------------------------------------
void MiniVkApplication::framebufferResizeCallback(GLFWwindow* window, int /*width*/, int /*height*/)
{
    auto app = reinterpret_cast<MiniVkApplication*>(glfwGetWindowUserPointer(window));
    app->framebufferResized = true;
}

//--------------------------------------------------------------
void MiniVkApplication::initVulkan()
{
    m_gcore = new GraphicsCore(m_window);
    m_gcore->initialize();

    m_swapchain = new SwapChainResources(m_gcore->device(), m_gcore->surface());
    m_swapchain->initialize();

    m_descpool = new DescriptorPool(m_gcore->device(), m_swapchain->count(), 10);
    m_pipeline = new ShaderPipeline(m_gcore->device(), m_swapchain->getRenderPass(), m_swapchain->getSwapChain()->getExtent());

    m_drawcmd = new DrawCommandBuffer(m_swapchain, m_pipeline->pipeline());
    m_drawcmd->initialize();

    m_sync = new Synchronization(m_gcore->device());
    m_sync->initialize();
}


//--------------------------------------------------------------
void MiniVkApplication::initScene()
{
    m_assets = new AssetsLoader(m_gcore->device(), m_descpool, m_pipeline->layout());
    m_drawables = new DrawableAllocator(m_gcore->device(), m_swapchain, m_pipeline->layout(), m_descpool);

    Camera* cam = new Camera(30.0, m_gcore->device(), m_descpool, m_pipeline->layout(), m_swapchain);
    m_sceneObjects.setView(cam);

    Paddle* paddle = new Paddle();
    paddle->position.y = -6.0;
    m_sceneObjects.add(paddle);
    m_collisions.setPaddle(paddle);

    std::srand(0);
    float depf = 1.4;

    for(int x=0;x<20;++x) for(int y=0;y<5;++y)
    {
        float xf = float(x) * depf - 13.0;
        float yf = float(y) * depf + 3.5;

        Brick* brick = new Brick();
        brick->position.x = xf;
        brick->position.y = yf;
        brick->color.x = float(std::rand())/RAND_MAX;
        brick->color.y = float(std::rand())/RAND_MAX;
        brick->color.z = float(std::rand())/RAND_MAX;
        brick->m_timeOffset = float(std::rand());

        m_sceneObjects.add(brick);
        m_collisions.addBrick(brick);
    }

    Platform* platform = new Platform();
    platform->position.y = -10.0;
    m_sceneObjects.add(platform);
    m_collisions.setPlatform(platform);


    Background* background = new Background();
    background->position.z = -2.0;
    m_sceneObjects.add(background);


    Ball* ball = new Ball();
    m_sceneObjects.add(ball);
    m_collisions.setBall(ball);
}



//--------------------------------------------------------------
void MiniVkApplication::mainLoop()
{
    while (!glfwWindowShouldClose(m_window))
    {
        glfwPollEvents();
        drawFrame();
    }
    vkDeviceWaitIdle(m_gcore->device()->get());
}


//--------------------------------------------------------------
void MiniVkApplication::drawFrame()
{
    m_sync->waitForFence();
    uint32_t imageIndex = m_swapchain->getSwapChain()->acquireNextImage(m_sync->getImgAvailableSemaphore());

    m_sceneObjects.updateState(m_window);
    m_sceneObjects.animateScene();
    m_collisions.checkCollisions();
    m_sceneObjects.updateUniform(imageIndex);


    m_drawcmd->beginDraw(imageIndex);
    m_drawcmd->drawScene(&m_sceneObjects);
    m_drawcmd->endDraw();


    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {m_sync->getImgAvailableSemaphore()};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_drawcmd->get(imageIndex)->get();

    VkSemaphore signalSemaphores[] = {m_sync->getRenderFinishedSemaphore()};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    m_sync->resetFence();
    if (vkQueueSubmit(m_gcore->device()->getGraphicQueue()->get(), 1, &submitInfo, m_sync->getFence()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    m_gcore->device()->getPresentQueue()->present(m_sync->getRenderFinishedSemaphore(), m_swapchain->getSwapChain(), imageIndex);

    m_sync->nextFrame();
    m_gcore->device()->getPresentQueue()->wait();
}


//--------------------------------------------------------------
void MiniVkApplication::cleanup()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}
