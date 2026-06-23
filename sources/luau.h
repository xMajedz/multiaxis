#pragma once
#include "lua.h"
#include "lualib.h"
#include "luacode.h"

#include <string_view>

typedef void (*LuauLogCallback)(const char*);

namespace Luau
{
    int loadstring  (lua_State* L, std::string string,   std::string chunkname);

	int loadfile    (lua_State* L, std::string filename, std::string chunkname);

    int requirefile (lua_State* L, std::string filename, std::string chunkname, std::string requirestring);
	
    int require     (lua_State* L, std::string filename);

    void log(const char* msg);
	  
    static LuauLogCallback logCallback;

	void setLogCallback(LuauLogCallback callback);
}
