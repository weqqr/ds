#include <Luau/Compiler.h>
#include <Videoland/Common/Verify.h>
#include <Videoland/Script/ScriptContext.h>
#include <fstream>
#include <iostream>
#include <lualib.h>
#include <sstream>

#define LUA_CHECK(error)                               \
    if ((error)) {                                     \
        std::cout << lua_tostring(L, -1) << std::endl; \
        PANIC("Lua error");                            \
    }

namespace Videoland {
ScriptContext::ScriptContext(std::shared_ptr<VirtualFS> vfs)
    : m_vfs(vfs) {
    L = luaL_newstate();
    luaL_openlibs(L);
}

ScriptContext::~ScriptContext() {
    lua_close(L);
}

void ScriptContext::ExecuteFile(const std::string& path) {
    std::string source = m_vfs->ReadToString(path);

    std::string chunk_name = "@" + path;
    std::string bytecode = Luau::compile(source);

    LUA_CHECK(luau_load(L, chunk_name.c_str(), bytecode.data(), bytecode.size(), 0));
    LUA_CHECK(lua_pcall(L, 0, 0, 0));
}
}
