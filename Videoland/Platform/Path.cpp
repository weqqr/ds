#if defined(_WIN32)
#include <Shlwapi.h>
#endif

#include <Videoland/Common/Verify.h>
#include <Videoland/Platform/Path.h>

namespace Videoland {
#if defined(_WIN32)
static std::string wideToNarrow(WCHAR* wideString, int wideLength) {
    int narrowSize = WideCharToMultiByte(CP_UTF8, 0, wideString, wideLength, NULL, 0, NULL, NULL);

    VERIFY(narrowSize >= 0);

    std::string narrow(size_t(narrowSize), ' ');
    WideCharToMultiByte(CP_UTF8, 0, wideString, wideLength, narrow.data(), narrowSize, NULL, NULL);

    return narrow;
}

std::string GetCurrentExecutablePath() {
    constexpr DWORD maxPathSize = 2048;

    WCHAR path[maxPathSize];
    int length = GetModuleFileNameW(NULL, path, maxPathSize);

    VERIFY(length > 0);

    return wideToNarrow(path, length);
}
#else
#error unsupported platform
#endif
}

