#include <iostream>
#include <stdlib.h>

#include <Videoland/Common/Assert.h>

namespace Videoland::Impl {
void fatal_panic(const char* file, int line, const char* function, const char* message) {
    std::cout << std::endl
              << "---" << std::endl
              << "panic: " << message << std::endl
              << "in " << function << " at " << file << ":" << line << std::endl;
    exit(1);
}
}
