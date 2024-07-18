#include <iostream>
#include <stdlib.h>

#include <Videoland/Common/Verify.h>

namespace Videoland::Impl {
void FatalPanic(const char* file, int line, const char* function, const char* message) {
    std::cout << std::endl
              << "---" << std::endl
              << "panic: " << message << std::endl
              << "in " << function << " at " << file << ":" << line << std::endl;
    exit(1);
}
}
