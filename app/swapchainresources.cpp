#include "swapchainresources.h"

//--------------------------------------------------------------
SwapChainResources::SwapChainResources(LogicalDevice* device, Surface* surface)
    : m_device(device)
    , m_surface(surface)
    , m_swapchain(nullptr)
    , m_renderpass(nullptr)
    , m_msaaImage(nullptr)
    , m_depthImage(nullptr)
    , m_msaaView(nullptr)
    , m_depthView(nullptr)
{
}

//--------------------------------------------------------------
SwapChainResources::~SwapChainResources()
{
    cleanup();
}

//--------------------------------------------------------------
void SwapChainResources::initialize()
{
    m_swapchain = new SwapChain(m_device, m_surface);
    m_swapchain->initialize();

    uint32_t size = m_swapchain->count();
    m_views.resize(size);
    m_framebuffers.resize(size);

    for(uint32_t i=0;i<size;++i)
    {
        m_views[i] = new ImageView(m_device, m_swapchain, i, VK_IMAGE_ASPECT_COLOR_BIT);
        m_views[i]->initialize();
    }

    m_renderpass = new RenderPass(m_device, m_swapchain->getFormat());
    m_renderpass->initialize();

    PhysicalDevice* physical = m_device->getPhysical();
    VkExtent2D extent = m_swapchain->getExtent();
    VkFormat colorformat = m_swapchain->getFormat();
    VkFormat depthformat = physical->findDepthFormat();
    // VkSampleCountFlagBits samples = m_device->msaaEnabled() ? physical->m_msaaSamples : VK_SAMPLE_COUNT_1_BIT;

    // create color ressource for msaa
    if(m_device->msaaEnabled())
    {
        VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        m_msaaImage = new Image(m_device, extent.width, extent.height, 1, m_device->msaaEnabled(), colorformat, usage);
        m_msaaImage->initialize();
        m_msaaView = new ImageView(m_device, m_msaaImage, VK_IMAGE_ASPECT_COLOR_BIT);
        m_msaaView->initialize();
    }

    // create depth ressource
    VkImageUsageFlags usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    m_depthImage = new Image(m_device, extent.width, extent.height, 1, m_device->msaaEnabled(), depthformat, usage);
    m_depthImage->initialize();
    m_depthImage->transitionLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    m_depthView = new ImageView(m_device, m_depthImage, VK_IMAGE_ASPECT_DEPTH_BIT);
    m_depthView->initialize();

    for(uint32_t i=0;i<size;++i)
    {
        std::vector<ImageView*> attachments;
        if(m_device->msaaEnabled()) attachments = {m_msaaView, m_depthView, m_views[i]};
        else attachments = {m_views[i], m_depthView};

        m_framebuffers[i] = new FrameBuffer(m_device, attachments, m_renderpass, extent.width, extent.height);
        m_framebuffers[i]->initialize();
    }

    // draw cmd pools (one per frame)
    m_drawCmdPool.resize(size, nullptr);
    for(uint32_t i=0;i<size;++i)
    {
        m_drawCmdPool[i] = new DrawCmdPool(m_device);
        m_drawCmdPool[i]->initialize();
    }
}

//--------------------------------------------------------------
void SwapChainResources::cleanup()
{
    uint32_t size = m_swapchain->count();
    for(uint32_t i=0;i<size;++i) delete m_drawCmdPool[i];

    if(m_msaaView) delete m_msaaView;
    if(m_msaaImage) delete m_msaaImage;

    if(m_depthView) delete m_depthView;
    if(m_depthImage) delete m_depthImage;

    for(auto fb : m_framebuffers) delete fb;
    m_framebuffers.clear();

    if(m_renderpass) delete m_renderpass;

    for(auto iv : m_views) delete iv;
    m_views.clear();

    if(m_swapchain) delete m_swapchain;
}
