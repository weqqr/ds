#include <Videoland/Common/Verify.h>
#include <Videoland/FileSystem/VirtualFS.h>
#include <fstream>
#include <sstream>
#include <string>

namespace Videoland {
VirtualFS::VirtualFS(const std::string& rootPath)
    : m_rootPath(rootPath) {
}

std::string VirtualFS::ReadToString(const std::string& path) const {
    std::ifstream input(path);
    VERIFY(input.is_open());

    std::stringstream buffer;
    buffer << input.rdbuf();

    return buffer.str();
}
}
