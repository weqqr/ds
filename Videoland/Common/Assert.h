#pragma once

#define PANIC(message) ::Videoland::Impl::fatal_panic(__FILE__, __LINE__, __FUNCTION__, message)

#define VERIFY(expr)                               \
    if (!(expr)) {                                 \
        PANIC("VERIFY expression failed: " #expr); \
    }

namespace Videoland::Impl {
[[noreturn]] void fatal_panic(const char* file, int line, const char* function, const char* message);
}
