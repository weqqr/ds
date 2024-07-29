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


set(SLANG_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Slang")

add_library(Slang INTERFACE)
target_include_directories(Slang INTERFACE ${SLANG_PATH}/include)
target_link_libraries(Slang INTERFACE
    # ${SLANG_PATH}/lib/gfx.lib
    ${SLANG_PATH}/lib/slang-rt.lib
    ${SLANG_PATH}/lib/slang.lib
)

add_subdirectory(Videoland)

add_subdirectory(DS)
target_link_libraries(DS Videoland)
