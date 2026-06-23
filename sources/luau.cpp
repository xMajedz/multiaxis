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

static void query_replace(std::string& source, const std::string& from, const std::string& to)
{
    size_t start = 0;
    while ((start = source.find(from, start)) != std::string::npos) {
		source.replace(start, from.length(), to);
	    start += to.length();
	}
}

void Luau::log(const char* msg)
{
    logCallback(msg);
}

void Luau::setLogCallback(LuauLogCallback callback)
{
	logCallback = callback;
}

int Luau::loadstring(lua_State* L, std::string string, std::string chunkname)
{
    Bytecode bytecode(string);
    
	return bytecode.load(L, chunkname);
}

int Luau::loadfile(lua_State* L, std::string filepath, std::string chunkname)
{
    std::string path = filepath + ".luau";

	if (!FileExists(path.data())) {
	    log(TextFormat("%s no such file", path.data()));
	    return 1;
	}

	char* text = LoadFileText(path.data());
		
	Bytecode bytecode(text);

	UnloadFileText(text);

	int status = bytecode.load(L, chunkname);
		
	return status;
}

int Luau::requirefile(lua_State* L, std::string filename, std::string chunkname, std::string requirestring)
{  
     query_replace(requirestring, ";", " ");

	 std::stringstream requirestream(requirestring);
	
	 char* text = nullptr;

	 std:: string path;

	 while (requirestream >> path) {	
		query_replace(path, "?", filename);
		
		if (FileExists(path.data())) {
		    text = LoadFileText(path.data());
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

int Luau::require(lua_State* L, std::string filename)
{
     std::string requirestring = "./scripts/?.luau;./scripts/?/?.luau";
	 std::string chunkname = "require:" + filename;
	 
	 int result = requirefile(L, filename, chunkname, requirestring);

	 if (result != 0 ) return 1;

	 int status = lua_pcall(L, 0, 1, NULL);

	 if (status != LUA_OK) {
		 log(lua_tostring(L, 1));
		 return 1;
	 }
	
	 return 0;
}
