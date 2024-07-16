#pragma once

#include <GLFW/glfw3.h>
#include <volk.h>

namespace Videoland {
class GPUContext {
public:
    GPUContext() = default;
    GPUContext(GLFWwindow* window);

    ~GPUContext();

    GPUContext(const GPUContext&) = delete;
    GPUContext& operator=(const GPUContext&) = delete;

private:
    VkInstance m_instance{VK_NULL_HANDLE};
    VkDebugUtilsMessengerEXT m_debug_messenger{VK_NULL_HANDLE};
    VkSurfaceKHR m_surface{VK_NULL_HANDLE};
};
}
