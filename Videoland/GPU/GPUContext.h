#pragma once

#include <Videoland/Common/NoCopy.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <volk.h>

namespace Videoland {
struct Adapter {
    VkPhysicalDevice device{VK_NULL_HANDLE};
    std::string name{};
    uint32_t graphicsComputeQueueIndex{VK_QUEUE_FAMILY_IGNORED};
};

struct SurfaceInfo {
    VkSurfaceCapabilitiesKHR capabilities{};
    std::vector<VkSurfaceFormatKHR> formats{};
    VkSurfaceFormatKHR preferredFormat{};
};

struct SwapchainImage {
    VkImage image;
    VkImageView imageView;
    uint32_t index;
};

class Swapchain {
    NO_COPY(Swapchain)

    // Swapchain is managed by GPUContext.
    friend class GPUContext;

public:
    Swapchain() = default;

private:
    void Create(VkDevice device, VkSurfaceKHR surface, VkExtent2D extent, const Adapter &adapter, const SurfaceInfo& surface_info);
    void Destroy();
    void Recreate();

    size_t FrameCount() const;

    SwapchainImage AcquireImage(VkSemaphore semaphore);

    VkDevice m_device{VK_NULL_HANDLE};
    VkSwapchainKHR m_swapchain{VK_NULL_HANDLE};
    std::vector<VkImage> m_images{};
    std::vector<VkImageView> m_imageViews{};
};

class GPUContext {
    NO_COPY(GPUContext)
public:
    GPUContext() = default;
    GPUContext(GLFWwindow* window);
    ~GPUContext();

    SwapchainImage AcquireNextFrame();
    void SubmitFrame(SwapchainImage image);

private:
    VkInstance m_instance{VK_NULL_HANDLE};
    VkDebugUtilsMessengerEXT m_debugMessenger{VK_NULL_HANDLE};
    VkSurfaceKHR m_surface{VK_NULL_HANDLE};
    Adapter m_adapter{};
    VkDevice m_device{VK_NULL_HANDLE};
    VkQueue m_graphicsComputeQueue{VK_NULL_HANDLE};
    Swapchain m_swapchain{};
    VkCommandPool m_commandPool{VK_NULL_HANDLE};
    std::vector<VkCommandBuffer> m_frameCommandBuffers{};

    std::vector<VkSemaphore> m_frameSemaphores{};
    VkSemaphore m_currentFrameSemaphore{VK_NULL_HANDLE};
};
}
