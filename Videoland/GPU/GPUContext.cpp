#include <Videoland/GPU/GPUContext.h>

namespace Videoland {
GPUContext::GPUContext(GLFWwindow* window) {
    volkInitialize();
}
}
