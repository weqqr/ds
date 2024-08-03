#include <Videoland/Common/Verify.h>
#include <Videoland/Content/ShaderCompiler.h>
#include <iostream>
#include <slang-com-ptr.h>
#include <slang.h>

namespace Videoland {
class SlangFSWrapper : public ISlangFileSystem {
public:
    virtual SLANG_NO_THROW SlangResult SLANG_MCALL loadFile(char const* path, ISlangBlob** outBlob) override {
        return SLANG_E_NOT_FOUND;
    };
};

ShaderCompiler::ShaderCompiler(std::shared_ptr<VirtualFS> vfs)
    : m_vfs(std::move(vfs)) {
    Slang::ComPtr<slang::IGlobalSession> globalSession;
    slang::createGlobalSession(globalSession.writeRef());
}

std::vector<uint8_t> ShaderCompiler::CompileShader(const std::string& path) {
    slang::TargetDesc target{};
    target.format = SlangCompileTarget::SLANG_SPIRV;
    target.profile = m_globalSession->findProfile("glsl_450");

    slang::SessionDesc sessionDesc{};
    sessionDesc.targetCount = 1;
    sessionDesc.targets = &target;
    // sessionDesc.fileSystem = fs;

    Slang::ComPtr<slang::ISession> session;
    m_globalSession->createSession(sessionDesc, session.writeRef());

    Slang::ComPtr<slang::IBlob> diagnostics;
    Slang::ComPtr<slang::IModule> shaderModule = Slang::ComPtr(session->loadModule("World", diagnostics.writeRef()));

    if (diagnostics) {
        auto text = static_cast<const char*>(diagnostics->getBufferPointer());
        std::cout << "slang: " << text << std::endl;
    }

    // Slang::ComPtr<IEntryPoint> computeEntryPoint;
    // module->findEntryPointByName("myComputeMain", computeEntryPoint.writeRef());

    VERIFY(shaderModule);

    return {};
}
}
