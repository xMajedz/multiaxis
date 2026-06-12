#include "common.h"
#include "luau.h"

using namespace raylib;

struct Bytecode {
  	Bytecode(std::string_view string)
	{
		m_data = luau_compile(string.data(), string.size(), NULL, &m_size);
	};

	~Bytecode()
	{
		delete m_data;
	};
  
    int execute(lua_State* L, std::string_view chunkname, int nargs, int nresults)
    {
	    luau_load(L, TextFormat("=%s", chunkname.data()), data(), size(), 0);
		int status = lua_pcall(L, nargs, nresults, NULL);
		if (status != LUA_OK) {
		    Luau::log(lua_tostring(L, -1));
	    }
		return status;
    };

    int executeThread(lua_State* L, std::string_view chunkname, int nargs)
    {
	    lua_State* T = lua_newthread(L);
		luaL_sandboxthread(T);
	    luau_load(T, TextFormat("=%s", chunkname.data()), data(), size(), 0);
		int status = lua_resume(T, L, nargs);
		if (status != LUA_OK) {
		    Luau::log(lua_tostring(T, -2));
	    }
	    return status; 
    };

    void dump(std::string_view path)
    {
	    const char* data = m_data;
	    SaveFileText(path.data(), data);
    };
  
	char* data()
	{
		return m_data;
	};

	size_t size()
	{
		return m_size;
	};
  
private:
	char* m_data;
	size_t m_size;
};

void Luau::log(const char* msg)
{
    logCallback(msg);
    LOG(msg)
}

void Luau::setLogCallback(LuauLogCallback callback)
{
	logCallback = callback;
}

int Luau::dostring(lua_State* L, std::string_view string, std::string_view chunkname)
{
    Bytecode bytecode(string);
    
	int status = bytecode.executeThread(L, TextFormat("dostring:%s", chunkname.data()), 0);
	
	return status;
}

int Luau::dofile(lua_State* L, std::string_view filepath, std::string_view chunkname)
{
	const char* path = TextFormat("%s.luau", filepath.data());

	if (!FileExists(path)) {
	  //lua_pushstring(L, "");
	  return 1;
	}

	char* text = LoadFileText(path);
		
	Bytecode bytecode(text);

	UnloadFileText(text);
    
	int status = bytecode.executeThread(L, chunkname, 0);
	
	return status;
}

int Luau::require(lua_State* L, std::string_view filename)
{
	const char* requirepaths = "./scripts/?.luau;./scripts/?/?.luau";
	const char* requirepaths_fmt[] = { "./scripts/%s", "./scripts/%s/%s" };

	int status = 1;
	for (const auto& path : requirepaths_fmt) {
	    const char* path_fmt = TextFormat(path, filename.data(), filename.data());

		if (!FileExists(path_fmt)) {
		  //lua_error(L, TextFormat("file: %s doens't exist", path_fmt));
	       return 1;
	    }

	    char* text = LoadFileText(path_fmt);
	
	    Bytecode bytecode(text);
		
		UnloadFileText(text);

	    status = bytecode.execute(L, TextFormat("require:%s", filename.data()), 1, 1);

		if (status == LUA_OK) return status;
	}
	
	return status;
}
