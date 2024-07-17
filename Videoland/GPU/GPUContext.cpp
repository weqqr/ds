#include <Videoland/Common/Assert.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <Videoland/GPU/GPUContext.h>
#include <iostream>
#include <vector>

#define VK_CHECK(result)                                                                             \
    if ((result) != VK_SUCCESS) {                                                                    \
        std::cout << "" << __FILE__ << ":" << __LINE__ << ": Vulkan error: " << result << std::endl; \
        PANIC("Encountered unexpected Vulkan error");                                                \
    }

namespace Videoland {
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

    VK_CHECK(vkCreateInstance(&create_info, nullptr, &instance));

    volkLoadInstance(instance);

    return instance;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_utils_message_callback(
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
    VK_CHECK(vkCreateDebugUtilsMessengerEXT(instance, &create_info, nullptr, &messenger));

    return messenger;
}

static VkSurfaceKHR create_surface_win32(VkInstance instance, GLFWwindow* window) {
    HWND hwnd = glfwGetWin32Window(window);
    HINSTANCE hinstance = GetModuleHandle(nullptr);

    VkWin32SurfaceCreateInfoKHR create_info = {
        .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .hinstance = hinstance,
        .hwnd = hwnd,
    };

    VkSurfaceKHR surface{};
    VK_CHECK(vkCreateWin32SurfaceKHR(instance, &create_info, nullptr, &surface));

    return surface;
}

VkSurfaceKHR create_surface(VkInstance instance, GLFWwindow* window) {
#if defined(_WIN32)
    return create_surface_win32(instance, window);
#else
#error "unsupported platform";
#endif
}

Adapter select_adapter(VkInstance instance, VkSurfaceKHR surface) {
    uint32_t count = 0;
    VK_CHECK(vkEnumeratePhysicalDevices(instance, &count, nullptr));
    std::vector<VkPhysicalDevice> devices(count);
    VK_CHECK(vkEnumeratePhysicalDevices(instance, &count, devices.data()));

    Adapter adapter{};

    for (VkPhysicalDevice device : devices) {
        adapter.device = device;

        VkPhysicalDeviceProperties2 properties2 = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR,
        };
        vkGetPhysicalDeviceProperties2(device, &properties2);

        adapter.name = properties2.properties.deviceName;

        // get graphics and compute queue
        uint32_t queue_family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
        std::vector<VkQueueFamilyProperties> queue_family_properties(queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_family_properties.data());

        for (uint32_t i = 0; i < queue_family_properties.size(); i++) {
            bool supports_graphics = queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT;
            bool supports_compute = queue_family_properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT;

            if (supports_graphics && supports_compute) {
                adapter.graphics_compute_queue_index = i;
                break;
            }
        }

        VERIFY(adapter.graphics_compute_queue_index != VK_QUEUE_FAMILY_IGNORED);

        std::cout << "graphics_compute_queue_index: " << adapter.graphics_compute_queue_index << std::endl;

        // query surface support
        VkBool32 supports_surface = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, adapter.graphics_compute_queue_index, surface, &supports_surface);

        VERIFY(supports_surface == VK_TRUE);

        std::cout << properties2.properties.deviceName << std::endl;

        return adapter;
    }

    PANIC("no adapter found");
}

VkDevice create_device(const Adapter& adapter) {
    float priority = 1.0f;

    VkDeviceQueueCreateInfo graphics_compute_queue_create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = adapter.graphics_compute_queue_index,
        .queueCount = 1,
        .pQueuePriorities = &priority,
    };

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos = {
        graphics_compute_queue_create_info,
    };

    std::vector<const char*> extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    VkDeviceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = uint32_t(queue_create_infos.size()),
        .pQueueCreateInfos = queue_create_infos.data(),
        // .enabledLayerCount = ,
        // .ppEnabledLayerNames = ,
        .enabledExtensionCount = uint32_t(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
        // .pEnabledFeatures = ,
    };

    VkDevice device = VK_NULL_HANDLE;
    VK_CHECK(vkCreateDevice(adapter.device, &create_info, nullptr, &device));

    volkLoadDevice(device);

    return device;
}

SurfaceInfo query_surface_info(const Adapter& adapter, VkSurfaceKHR surface) {
    SurfaceInfo info;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(adapter.device, surface, &info.capabilities);

    uint32_t format_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(adapter.device, surface, &format_count, nullptr);

    VERIFY(format_count > 0);

    std::vector<VkSurfaceFormatKHR> formats(format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(adapter.device, surface, &format_count, formats.data());

    info.preferred_format = formats[0];

    return info;
}

void Swapchain::create(VkDevice device, VkSurfaceKHR surface, VkExtent2D extent, const Adapter &adapter, const SurfaceInfo& surface_info) {
    m_device = device;

    VkSwapchainCreateInfoKHR create_info = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface,
        .minImageCount = surface_info.capabilities.minImageCount,
        .imageFormat = surface_info.preferred_format.format,
        .imageColorSpace = surface_info.preferred_format.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 1,
        .pQueueFamilyIndices = &adapter.graphics_compute_queue_index,
        .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = VK_PRESENT_MODE_FIFO_KHR,
        .clipped = VK_FALSE,
        .oldSwapchain = VK_NULL_HANDLE,
    };

    VK_CHECK(vkCreateSwapchainKHR(device, &create_info, nullptr, &m_swapchain));
}

void Swapchain::destroy() {
    if (m_device && m_swapchain)
        vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
}

void Swapchain::recreate() {
    // can't recreate swapchain while there are frames in flight
    vkDeviceWaitIdle(m_device);

    // TODO
}

GPUContext::GPUContext(GLFWwindow* window) {
    VK_CHECK(volkInitialize());

    int width = 0;
    int height = 0;
    glfwGetWindowSize(window, &width, &height);

    VkExtent2D window_extent = {
        .width = uint32_t(width),
        .height = uint32_t(height),
    };

    m_instance = create_instance();
    m_debug_messenger = create_debug_utils_messenger(m_instance);
    m_surface = create_surface(m_instance, window);
    m_adapter = select_adapter(m_instance, m_surface);
    m_device = create_device(m_adapter);
    vkGetDeviceQueue(m_device, m_adapter.graphics_compute_queue_index, 0, &m_graphics_compute_queue);
    SurfaceInfo surface_info = query_surface_info(m_adapter, m_surface);
    m_swapchain.create(m_device, m_surface, window_extent, m_adapter, surface_info);
}

GPUContext::~GPUContext() {
    if (!m_instance) {
        return;
    }

    m_swapchain.destroy();

    if (m_device)
        vkDestroyDevice(m_device, nullptr);

    if (m_surface)
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

    if (m_debug_messenger)
        vkDestroyDebugUtilsMessengerEXT(m_instance, m_debug_messenger, nullptr);

    vkDestroyInstance(m_instance, nullptr);
}
}
