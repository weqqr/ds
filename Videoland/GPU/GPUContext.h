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
    uint32_t graphics_compute_queue_index{VK_QUEUE_FAMILY_IGNORED};
};

struct SurfaceInfo {
    VkSurfaceCapabilitiesKHR capabilities{};
    std::vector<VkSurfaceFormatKHR> formats{};
    VkSurfaceFormatKHR preferred_format{};
};

class Swapchain {
    NO_COPY(Swapchain)

    // Swapchain is managed by GPUContext.
    friend class GPUContext;

public:
    Swapchain() = default;

private:
    void create(VkDevice device, VkSurfaceKHR surface, VkExtent2D extent, const Adapter &adapter, const SurfaceInfo& surface_info);
    void destroy();
    void recreate();

    VkDevice m_device{VK_NULL_HANDLE};
    VkSwapchainKHR m_swapchain{VK_NULL_HANDLE};
};

class GPUContext {
    NO_COPY(GPUContext)
public:
    GPUContext() = default;
    GPUContext(GLFWwindow* window);

    ~GPUContext();

private:
    VkInstance m_instance{VK_NULL_HANDLE};
    VkDebugUtilsMessengerEXT m_debug_messenger{VK_NULL_HANDLE};
    VkSurfaceKHR m_surface{VK_NULL_HANDLE};
    Adapter m_adapter{};
    VkDevice m_device{VK_NULL_HANDLE};
    VkQueue m_graphics_compute_queue{VK_NULL_HANDLE};
    Swapchain m_swapchain{};
};
}
