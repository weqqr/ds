#pragma once

#include <string>
#include <Videoland/Common/NoCopy.h>
#include <lua.h>

namespace Videoland {
class ScriptContext {
    NO_COPY(ScriptContext)
public:
    ScriptContext();
    ~ScriptContext();

    void ExecuteFile(const std::string& path);

private:
    lua_State *L;
};
}
