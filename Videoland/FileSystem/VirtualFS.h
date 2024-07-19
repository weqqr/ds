#pragma once

#include <string>

namespace Videoland {
class VirtualFS {
public:
    VirtualFS(const std::string& rootPath);

private:
    std::string m_rootPath;
};
}
