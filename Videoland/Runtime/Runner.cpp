#include <GLFW/glfw3.h>
#include <Videoland/Renderer/Renderer.h>
#include <Videoland/Runtime/Runner.h>

namespace Videoland {
void Runner::run() {
    if (!glfwInit()) {
        return;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(1600, 900, "ds", nullptr, nullptr);
    if (!window) {
        return;
    }

    Renderer renderer{window};

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        renderer.render_frame();
    }

    glfwTerminate();
}
}
