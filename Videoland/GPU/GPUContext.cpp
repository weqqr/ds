#include <Videoland/GPU/GPUContext.h>
#include <iostream>

#define VK_ASSERT(result)                                                                            \
    if ((result) != VK_SUCCESS) {                                                                    \
        std::cout << "" << __FILE__ << ":" << __LINE__ << ": Vulkan error: " << result << std::endl; \
        exit(1);                                                                                     \
    }

static VkInstance create_instance() {
    VkInstance instance{};

    VkApplicationInfo application_info = {
        .apiVersion = VK_API_VERSION_1_3,
    };

    VkInstanceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &application_info,
    };

    VK_ASSERT(vkCreateInstance(&create_info, nullptr, &instance));

    volkLoadInstance(instance);

    return instance;
}

namespace Videoland {
GPUContext::GPUContext(GLFWwindow* window) {
    VK_ASSERT(volkInitialize());

    m_instance = create_instance();
}

GPUContext::~GPUContext() {
    if (!m_instance) {
        return;
    }

    vkDestroyInstance(m_instance, nullptr);
}
}
