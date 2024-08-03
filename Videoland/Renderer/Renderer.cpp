#include <Videoland/Renderer/Renderer.h>

namespace Videoland {
Renderer::Renderer(GLFWwindow* window)
    : m_context(window) { }

void Renderer::RenderFrame() {
    SwapchainImage frame = m_context.AcquireNextFrame();

    m_context.SubmitFrame(frame);
}
}
