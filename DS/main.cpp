#include <Videoland/Runtime/Runner.h>

using namespace Videoland;

int main() {
    Runner runner{};

    runner.Run(ApplicationInfo{
        .internalName = "ds",
        .productName = "DS",
    });
}
