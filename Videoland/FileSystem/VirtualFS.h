#pragma once

#include <string>

namespace Videoland {
class VirtualFS {
public:
    VirtualFS(const std::string& rootPath);
    std::string ReadToString(const std::string& path) const;

private:
    std::string m_rootPath;
};
}
