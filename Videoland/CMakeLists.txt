add_library(Videoland STATIC
    GPU/GPUContext.cpp
    Renderer/Renderer.cpp
    Runtime/Runner.cpp
)

target_include_directories(Videoland PUBLIC ..)
target_link_libraries(Videoland glfw volk)
