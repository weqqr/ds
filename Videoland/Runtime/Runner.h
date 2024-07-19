#pragma once

#include <string>

namespace Videoland {
struct ApplicationInfo {
    std::string internalName;
    std::string productName;
};

class Runner {
public:
    void Run(const ApplicationInfo& applicationInfo);

private:
};
}
