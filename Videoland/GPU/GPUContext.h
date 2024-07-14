#pragma once

#include <volk.h>
#include <GLFW/glfw3.h>

namespace Videoland {
class GPUContext {
public:
    GPUContext() = default;
    GPUContext(GLFWwindow *window);

private:
    VkInstance m_instance{VK_NULL_HANDLE};
};
}
