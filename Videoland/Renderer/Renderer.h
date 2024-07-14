#pragma once

#include <Videoland/GPU/GPUContext.h>

namespace Videoland {
class Renderer {
public:
    Renderer(GLFWwindow* window);
    void render_frame();

private:
    GPUContext m_context{};
};
}
