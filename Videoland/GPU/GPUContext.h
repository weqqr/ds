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

    void AcquireImage();

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

private:
    VkInstance m_instance{VK_NULL_HANDLE};
    VkDebugUtilsMessengerEXT m_debugMessenger{VK_NULL_HANDLE};
    VkSurfaceKHR m_surface{VK_NULL_HANDLE};
    Adapter m_adapter{};
    VkDevice m_device{VK_NULL_HANDLE};
    VkQueue m_graphicsComputeQueue{VK_NULL_HANDLE};
    Swapchain m_swapchain{};
};
}
