add_library(Videoland STATIC
    Common/Verify.cpp
    FileSystem/VirtualFS.cpp
    GPU/GPUContext.cpp
    Platform/Path.cpp
    Renderer/Renderer.cpp
    Runtime/Runner.cpp
    Scene/Scene.cpp
    Script/ScriptContext.cpp
)

target_include_directories(Videoland PUBLIC ..)
target_link_libraries(Videoland glfw volk Luau.Compiler Luau.VM)
target_compile_features(Videoland PUBLIC cxx_std_20)
