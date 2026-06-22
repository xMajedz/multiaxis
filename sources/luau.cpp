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
  
    int load(lua_State* L, std::string_view chunkname)
    {
	    return luau_load(L, TextFormat("=%s", chunkname.data()), data(), size(), 0);
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
}

void Luau::setLogCallback(LuauLogCallback callback)
{
	logCallback = callback;
}

int Luau::dostring(lua_State* L, std::string_view string, std::string_view chunkname)
{
    Bytecode bytecode(string);
    
	//int status = bytecode.executeThread(L, TextFormat("dostring:%s", chunkname.data()), 0);
	
	return 1;
}

int Luau::dofile(lua_State* L, std::string_view filepath, std::string_view chunkname)
{
	const char* path = TextFormat("%s.luau", filepath.data());

	if (!FileExists(path)) {
	  return 1;
	}

	char* text = LoadFileText(path);
		
	Bytecode bytecode(text);

	UnloadFileText(text);

	//int status = bytecode.executeThread(L, chunkname, 0);
	
	return 1;
}

int Luau::loadfile(lua_State* L, std::string_view filepath, std::string_view chunkname)
{
  	const char* path = TextFormat("%s.luau", filepath.data());

	if (!FileExists(path)) {
	    log(TextFormat("%s no such file", path));
	    return 1;
	}

	char* text = LoadFileText(path);
		
	Bytecode bytecode(text);

	UnloadFileText(text);

	int status = bytecode.load(L, chunkname);
		
	return status;
}

int Luau::requirefile(lua_State* L, std::string_view filename, std::string_view chunkname)
{
  	const char* requirepaths_fmt[] = { "./scripts/%s.luau", "./scripts/%s/%s.luau" };

	char* text = nullptr;
	
	for (const auto& fmt : requirepaths_fmt) {
	    const char* path = TextFormat(fmt, filename.data(), filename.data());
        
	    if (FileExists(path)) {
		  text = LoadFileText(path);
		  break;
	    }
	}
	
	if (text == nullptr) {
	  log(TextFormat("couldn't require %s", filename.data()));
	    return 1;
	}

	Bytecode bytecode(text);

	UnloadFileText(text);

	int status = bytecode.load(L, chunkname);
		
	return status;
}

int Luau::require(lua_State* L, std::string_view filename)
{
     std::string_view requirepaths = "./scripts/?.luau;./scripts/?/?.luau";

	 int result = requirefile(L, filename, TextFormat("require:%s", filename.data()));

	 if (result != 0 ) return 1;

	 int status = lua_pcall(L, 0, 1, NULL);

	 if (status != LUA_OK) {
		 log(lua_tostring(L, 1));
		 return 1;
	 }
	
	 return 0;
}
