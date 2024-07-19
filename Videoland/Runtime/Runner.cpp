#include <iostream>
#include <GLFW/glfw3.h>
#include <Videoland/Renderer/Renderer.h>
#include <Videoland/Runtime/Runner.h>
#include <Videoland/Script/ScriptContext.h>
#include <Videoland/Platform/Path.h>
#include <Videoland/FileSystem/VirtualFS.h>

namespace Videoland {
void Runner::Run() {
    if (!glfwInit()) {
        return;
    }

    // FIXME: detect Data directory
    std::shared_ptr<VirtualFS> vfs(new VirtualFS("../Data"));

    ScriptContext scriptCx{vfs};
    scriptCx.ExecuteFile("Script/init.lua");

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(1600, 900, "ds", nullptr, nullptr);
    if (!window) {
        return;
    }

    Renderer renderer{window};

    bool closeRequested = false;

    while (!glfwWindowShouldClose(window) && !closeRequested) {
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            closeRequested = true;
        }

        renderer.RenderFrame();
    }

    glfwTerminate();
}
}
