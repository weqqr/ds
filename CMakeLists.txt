cmake_minimum_required(VERSION 3.14)
project(plus)

if (WIN32)
    set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_WIN32_KHR)
endif()

add_subdirectory(Vendor/volk)
add_subdirectory(Vendor/glfw)

add_subdirectory(Videoland)

add_subdirectory(DS)
target_link_libraries(DS Videoland)
