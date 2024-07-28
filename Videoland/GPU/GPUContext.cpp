#include <Videoland/Common/Verify.h>
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
static VkInstance CreateInstance() {
    VkInstance instance{};

    VkApplicationInfo applicationInfo = {
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

    VkInstanceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &applicationInfo,
        .enabledLayerCount = uint32_t(layers.size()),
        .ppEnabledLayerNames = layers.data(),
        .enabledExtensionCount = uint32_t(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
    };

    VK_CHECK(vkCreateInstance(&createInfo, nullptr, &instance));
    VERIFY(instance != VK_NULL_HANDLE);

    volkLoadInstance(instance);

    return instance;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugUtilsMessageCallback(
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

static VkDebugUtilsMessengerEXT CreateDebugUtilsMessenger(VkInstance instance) {
    VkDebugUtilsMessageSeverityFlagsEXT severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    VkDebugUtilsMessageTypeFlagsEXT type = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT;

    VkDebugUtilsMessengerCreateInfoEXT createInfo = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = severity,
        .messageType = type,
        .pfnUserCallback = DebugUtilsMessageCallback,
        .pUserData = nullptr,
    };

    VkDebugUtilsMessengerEXT messenger{};
    VK_CHECK(vkCreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &messenger));
    VERIFY(messenger != VK_NULL_HANDLE);

    return messenger;
}

static VkSurfaceKHR CreateSurfaceWin32(VkInstance instance, GLFWwindow* window) {
    HWND hwnd = glfwGetWin32Window(window);
    HINSTANCE hinstance = GetModuleHandle(nullptr);

    VkWin32SurfaceCreateInfoKHR createInfo = {
        .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .hinstance = hinstance,
        .hwnd = hwnd,
    };

    VkSurfaceKHR surface{};
    VK_CHECK(vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface));
    VERIFY(surface != VK_NULL_HANDLE);

    return surface;
}

VkSurfaceKHR CreateSurface(VkInstance instance, GLFWwindow* window) {
#if defined(_WIN32)
    return CreateSurfaceWin32(instance, window);
#else
#error "unsupported platform";
#endif
}

Adapter SelectAdapter(VkInstance instance, VkSurfaceKHR surface) {
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
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyProperties.data());

        for (uint32_t i = 0; i < queueFamilyProperties.size(); i++) {
            bool supportsGraphics = queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT;
            bool supportsCompute = queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT;

            if (supportsGraphics && supportsCompute) {
                adapter.graphicsComputeQueueIndex = i;
                break;
            }
        }

        VERIFY(adapter.graphicsComputeQueueIndex != VK_QUEUE_FAMILY_IGNORED);

        std::cout << "graphicsComputeQueueIndex: " << adapter.graphicsComputeQueueIndex << std::endl;

        // query surface support
        VkBool32 supportsSurface = VK_FALSE;
        VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(device, adapter.graphicsComputeQueueIndex, surface, &supportsSurface));

        VERIFY(supportsSurface == VK_TRUE);

        std::cout << properties2.properties.deviceName << std::endl;

        return adapter;
    }

    PANIC("no adapter found");
}

static VkDevice CreateDevice(const Adapter& adapter) {
    float priority = 1.0f;

    VkDeviceQueueCreateInfo graphicsComputeQueueCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = adapter.graphicsComputeQueueIndex,
        .queueCount = 1,
        .pQueuePriorities = &priority,
    };

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos = {
        graphicsComputeQueueCreateInfo,
    };

    std::vector<const char*> extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    VkDeviceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = uint32_t(queueCreateInfos.size()),
        .pQueueCreateInfos = queueCreateInfos.data(),
        // .enabledLayerCount = ,
        // .ppEnabledLayerNames = ,
        .enabledExtensionCount = uint32_t(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
        // .pEnabledFeatures = ,
    };

    VkDevice device = VK_NULL_HANDLE;
    VK_CHECK(vkCreateDevice(adapter.device, &createInfo, nullptr, &device));
    VERIFY(device != VK_NULL_HANDLE);

    volkLoadDevice(device);

    return device;
}

static SurfaceInfo QuerySurfaceInfo(const Adapter& adapter, VkSurfaceKHR surface) {
    SurfaceInfo info;

    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(adapter.device, surface, &info.capabilities));

    uint32_t formatCount = 0;
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(adapter.device, surface, &formatCount, nullptr));

    VERIFY(formatCount > 0);

    std::vector<VkSurfaceFormatKHR> formats(formatCount);
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(adapter.device, surface, &formatCount, formats.data()));

    info.preferredFormat = formats[0];

    return info;
}

VkSemaphore CreateSemaphore(VkDevice device) {
    VkSemaphoreCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };

    VkSemaphore semaphore = VK_NULL_HANDLE;
    VK_CHECK(vkCreateSemaphore(device, &createInfo, nullptr, &semaphore));
    VERIFY(semaphore != VK_NULL_HANDLE);

    return semaphore;
}

VkCommandPool CreateCommandPool(VkDevice device, uint32_t graphicsComputeQueueIndex) {
    VkCommandPoolCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = graphicsComputeQueueIndex,
    };

    VkCommandPool commandPool = VK_NULL_HANDLE;
    VK_CHECK(vkCreateCommandPool(device, &createInfo, nullptr, &commandPool));
    VERIFY(commandPool != VK_NULL_HANDLE);

    return commandPool;
}

VkCommandBuffer AllocateCommandBuffer(VkDevice device, VkCommandPool commandPool) {
    VkCommandBufferAllocateInfo allocateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };

    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
    vkAllocateCommandBuffers(device, &allocateInfo, &commandBuffer);
    VERIFY(commandBuffer != VK_NULL_HANDLE);

    return commandBuffer;
}

void Swapchain::Create(VkDevice device, VkSurfaceKHR surface, VkExtent2D extent, const Adapter& adapter, const SurfaceInfo& surfaceInfo) {
    m_device = device;

    VkSwapchainCreateInfoKHR createInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface,
        .minImageCount = surfaceInfo.capabilities.minImageCount,
        .imageFormat = surfaceInfo.preferredFormat.format,
        .imageColorSpace = surfaceInfo.preferredFormat.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 1,
        .pQueueFamilyIndices = &adapter.graphicsComputeQueueIndex,
        .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = VK_PRESENT_MODE_FIFO_KHR,
        .clipped = VK_FALSE,
        .oldSwapchain = VK_NULL_HANDLE,
    };

    VK_CHECK(vkCreateSwapchainKHR(device, &createInfo, nullptr, &m_swapchain));

    uint32_t imageCount = 0;
    vkGetSwapchainImagesKHR(m_device, m_swapchain, &imageCount, nullptr);
    m_images.resize(imageCount);
    vkGetSwapchainImagesKHR(m_device, m_swapchain, &imageCount, m_images.data());

    m_imageViews.resize(imageCount);
    for (size_t i = 0; i < m_images.size(); i++) {
        VkImageViewCreateInfo imageViewCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = m_images[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = surfaceInfo.preferredFormat.format,
            .components = VkComponentMapping{
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY,
            },
            .subresourceRange = VkImageSubresourceRange{
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };

        vkCreateImageView(m_device, &imageViewCreateInfo, nullptr, &m_imageViews[i]);
    }
}

void Swapchain::Destroy() {
    for (auto imageView : m_imageViews) {
        vkDestroyImageView(m_device, imageView, nullptr);
    }

    if (m_device && m_swapchain) {
        vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
    }
}

void Swapchain::Recreate() {
    // can't recreate swapchain while there are frames in flight
    vkDeviceWaitIdle(m_device);

    // create();
}

size_t Swapchain::FrameCount() const {
    return m_images.size();
}

SwapchainImage Swapchain::AcquireImage(VkSemaphore semaphore) {
    constexpr uint64_t SWAPCHAIN_TIMEOUT = 5'000'000'000; // nanoseconds

    SwapchainImage image;

    VK_CHECK(vkAcquireNextImageKHR(m_device, m_swapchain, SWAPCHAIN_TIMEOUT, semaphore, VK_NULL_HANDLE, &image.index));

    VERIFY(image.index < m_images.size());

    image.image = m_images[image.index];
    image.imageView = m_imageViews[image.index];

    return image;
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

    m_instance = CreateInstance();
    m_debugMessenger = CreateDebugUtilsMessenger(m_instance);
    m_surface = CreateSurface(m_instance, window);
    m_adapter = SelectAdapter(m_instance, m_surface);
    m_device = CreateDevice(m_adapter);
    vkGetDeviceQueue(m_device, m_adapter.graphicsComputeQueueIndex, 0, &m_graphicsComputeQueue);
    SurfaceInfo surfaceInfo = QuerySurfaceInfo(m_adapter, m_surface);
    m_swapchain.Create(m_device, m_surface, window_extent, m_adapter, surfaceInfo);

    m_frameSemaphores.resize(m_swapchain.FrameCount());
    for (size_t i = 0; i < m_swapchain.FrameCount(); i++) {
        m_frameSemaphores[i] = CreateSemaphore(m_device);
    }

    m_currentFrameSemaphore = CreateSemaphore(m_device);

    m_commandPool = CreateCommandPool(m_device, m_adapter.graphicsComputeQueueIndex);

    m_frameCommandBuffers.resize(m_swapchain.FrameCount());
    for (size_t i = 0; i < m_swapchain.FrameCount(); i++) {
        m_frameCommandBuffers[i] = AllocateCommandBuffer(m_device, m_commandPool);
    }
}

GPUContext::~GPUContext() {
    if (!m_instance) {
        return;
    }

    for (auto commandBuffer : m_frameCommandBuffers) {
        vkFreeCommandBuffers(m_device, m_commandPool, 1, &commandBuffer);
    }

    if (m_commandPool) {
        vkDestroyCommandPool(m_device, m_commandPool, nullptr);
    }

    if (m_currentFrameSemaphore) {
        vkDestroySemaphore(m_device, m_currentFrameSemaphore, nullptr);
    }

    for (auto semaphore : m_frameSemaphores) {
        vkDestroySemaphore(m_device, semaphore, nullptr);
    }

    m_swapchain.Destroy();

    if (m_device) {
        vkDestroyDevice(m_device, nullptr);
    }

    if (m_surface) {
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    }

    if (m_debugMessenger) {
        vkDestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
    }

    vkDestroyInstance(m_instance, nullptr);
}

SwapchainImage GPUContext::AcquireNextFrame() {
    SwapchainImage frame = m_swapchain.AcquireImage(m_currentFrameSemaphore);

    std::swap(m_currentFrameSemaphore, m_frameSemaphores[frame.index]);

    return frame;
}

void GPUContext::SubmitFrame(SwapchainImage image) {
    // VkResult presentResult = VK_SUCCESS;

    // VkPipelineStageFlags waitDstStageMask[1] = {
    //     VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
    // };

    // VkSubmitInfo submitInfo = {
    //     .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
    //     // .pNext = ,
    //     .waitSemaphoreCount = 1,
    //     .pWaitSemaphores = &m_currentFrameSemaphore,
    //     .pWaitDstStageMask = waitDstStageMask,
    //     .commandBufferCount = 1,
    //     .pCommandBuffers = ,
    //     .signalSemaphoreCount = 0,
    //     .pSignalSemaphores = nullptr,
    // };

    // vkQueueSubmit(m_graphicsComputeQueue, 1, &submitInfo, fence);

    // VkPresentInfoKHR presentInfo = {
    //     .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
    //     .waitSemaphoreCount = 1,
    //     .pWaitSemaphores = waitSemaphores,
    //     .swapchainCount = 1,
    //     .pSwapchains = &m_swapchain.m_swapchain,
    //     .pImageIndices = &image.index,
    //     .pResults = &presentResult,
    // };

    // vkQueuePresentKHR(m_graphicsComputeQueue, &presentInfo);

    // VK_CHECK(presentResult);
}
}
