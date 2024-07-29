#pragma once

#include <slang-com-ptr.h>
#include <slang.h>
#include <string>
#include <vector>
#include <memory>
#include <Videoland/FileSystem/VirtualFS.h>

namespace Videoland {
class ShaderCompiler {
public:
    ShaderCompiler(std::shared_ptr<VirtualFS> vfs);

    std::vector<uint8_t> CompileShader(const std::string& path);

private:
    std::shared_ptr<VirtualFS> m_vfs;
    Slang::ComPtr<slang::IGlobalSession> m_globalSession;
};
}
