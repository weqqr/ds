cmake_minimum_required(VERSION 3.14)
project(plus)

if (WIN32)
    set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_WIN32_KHR)
endif()

set(LUAU_BUILD_CLI OFF CACHE BOOL "" FORCE)
set(LUAU_BUILD_TESTS OFF CACHE BOOL "" FORCE)

add_subdirectory(Vendor/volk)
add_subdirectory(Vendor/glfw)
add_subdirectory(Vendor/luau)

add_subdirectory(Videoland)

add_subdirectory(DS)
target_link_libraries(DS Videoland)
