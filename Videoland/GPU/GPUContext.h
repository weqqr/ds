#pragma once

#include <volk.h>
#include <GLFW/glfw3.h>

namespace Videoland {
class GPUContext {
public:
    GPUContext() = default;
    GPUContext(GLFWwindow *window);

    ~GPUContext();

    GPUContext(const GPUContext&) = delete;
    GPUContext& operator=(const GPUContext&) = delete;

private:
    VkInstance m_instance{VK_NULL_HANDLE};
};
}
