#include <GLFW/glfw3.h>
#include <Videoland/Renderer/Renderer.h>
#include <Videoland/Runtime/Runner.h>
#include <Videoland/Script/ScriptContext.h>

namespace Videoland {
void Runner::Run() {
    if (!glfwInit()) {
        return;
    }

    ScriptContext scriptCx{};

    scriptCx.ExecuteFile("../Data/Script/init.lua");

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
