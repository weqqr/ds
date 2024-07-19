#pragma once

#include <Videoland/Common/NoCopy.h>
#include <Videoland/FileSystem/VirtualFS.h>
#include <lua.h>
#include <memory>
#include <string>

namespace Videoland {
class ScriptContext {
    NO_COPY(ScriptContext)
public:
    ScriptContext(std::shared_ptr<VirtualFS> vfs);
    ~ScriptContext();

    void ExecuteFile(const std::string& path);

private:
    std::shared_ptr<VirtualFS> m_vfs;
    lua_State* L;
};
}
