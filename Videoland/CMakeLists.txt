add_library(Videoland STATIC
    GPU/GPUContext.cpp
    Renderer/Renderer.cpp
    Runtime/Runner.cpp
)

target_include_directories(Videoland PUBLIC ..)
target_link_libraries(Videoland glfw volk)
target_compile_features(Videoland PUBLIC cxx_std_20)
