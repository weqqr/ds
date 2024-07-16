#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <Videoland/GPU/GPUContext.h>
#include <iostream>
#include <vector>

#define VK_ASSERT(result)                                                                            \
    if ((result) != VK_SUCCESS) {                                                                    \
        std::cout << "" << __FILE__ << ":" << __LINE__ << ": Vulkan error: " << result << std::endl; \
        exit(1);                                                                                     \
    }

static VkInstance create_instance() {
    VkInstance instance{};

    VkApplicationInfo application_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .apiVersion = VK_API_VERSION_1_3,
    };

    std::vector<const char*> layers = {
        "VK_LAYER_KHRONOS_validation",
    };

    std::vector<const char*> extensions = {
#if defined(_WIN32)
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
        VK_KHR_SURFACE_EXTENSION_NAME,
#endif
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
    };

    VkInstanceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &application_info,
        .enabledLayerCount = uint32_t(layers.size()),
        .ppEnabledLayerNames = layers.data(),
        .enabledExtensionCount = uint32_t(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
    };

    VK_ASSERT(vkCreateInstance(&create_info, nullptr, &instance));

    volkLoadInstance(instance);

    return instance;
}

VKAPI_ATTR VkBool32 VKAPI_CALL debug_utils_message_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {
    if (!pCallbackData) {
        return VK_FALSE;
    }

    std::cout << "Vulkan Debug: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

static VkDebugUtilsMessengerEXT create_debug_utils_messenger(VkInstance instance) {
    VkDebugUtilsMessageSeverityFlagsEXT severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    VkDebugUtilsMessageTypeFlagsEXT type = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT;

    VkDebugUtilsMessengerCreateInfoEXT create_info = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = severity,
        .messageType = type,
        .pfnUserCallback = debug_utils_message_callback,
        .pUserData = nullptr,
    };

    VkDebugUtilsMessengerEXT messenger{};
    VK_ASSERT(vkCreateDebugUtilsMessengerEXT(instance, &create_info, nullptr, &messenger));

    return messenger;
}

VkSurfaceKHR create_surface_win32(VkInstance instance, GLFWwindow* window) {
    HWND hwnd = glfwGetWin32Window(window);
    HINSTANCE hinstance = GetModuleHandle(nullptr);

    VkWin32SurfaceCreateInfoKHR create_info = {
        .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .hinstance = hinstance,
        .hwnd = hwnd,
    };

    VkSurfaceKHR surface{};
    VK_ASSERT(vkCreateWin32SurfaceKHR(instance, &create_info, nullptr, &surface));

    return surface;
}

VkSurfaceKHR create_surface(VkInstance instance, GLFWwindow* window) {
#if defined(_WIN32)
    return create_surface_win32(instance, window);
#else
#error "unsupported platform";
#endif
}

namespace Videoland {
GPUContext::GPUContext(GLFWwindow* window) {
    VK_ASSERT(volkInitialize());

    m_instance = create_instance();
    m_debug_messenger = create_debug_utils_messenger(m_instance);
    m_surface = create_surface(m_instance, window);
}

GPUContext::~GPUContext() {
    if (!m_instance) {
        return;
    }

    if (m_surface)
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

    if (m_debug_messenger)
        vkDestroyDebugUtilsMessengerEXT(m_instance, m_debug_messenger, nullptr);

    vkDestroyInstance(m_instance, nullptr);
}
}
