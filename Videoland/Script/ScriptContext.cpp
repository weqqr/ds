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
ScriptContext::ScriptContext() {
    L = luaL_newstate();
    luaL_openlibs(L);
}

ScriptContext::~ScriptContext() {
    lua_close(L);
}

void ScriptContext::ExecuteFile(const std::string& path) {
    std::ifstream input(path);
    VERIFY(input.is_open());

    std::stringstream buffer;
    buffer << input.rdbuf();

    std::string source = buffer.str();

    std::string chunk_name = "@" + path;
    std::string bytecode = Luau::compile(source);

    LUA_CHECK(luau_load(L, chunk_name.c_str(), bytecode.data(), bytecode.size(), 0));
    LUA_CHECK(lua_pcall(L, 0, 0, 0));
}
}
