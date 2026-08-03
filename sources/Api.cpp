#include "Api.h"

#include <iostream>

#include <fstream>
#include <sstream>

struct Bytecode {
    Bytecode(const std::string& string) : data(luau_compile(string.data(), string.size(), NULL, &size)) {};

    ~Bytecode()
    {
        std::free(data);
    };
  
    char* data;
    size_t size;
};

static void log(const std::string& message)
{
    std::cout << message << std::endl;
}

static int loadstring(lua_State* L, const std::string& string, const std::string& chunkname)
{
    Bytecode bytecode(string);
    return luau_load(L, chunkname.data(), bytecode.data, bytecode.size, 0);
}

static int loadfile(lua_State* L, const std::string& filepath, const std::string& chunkname)
{
    std::ifstream file(filepath);

    if (!file) {
        log(filepath + " no such file");
        return 1;
    }
	
    std::stringstream text;

    text << file.rdbuf();

    return loadstring(L, text.str(), chunkname);
}

static void query_replace(std::string& source, const std::string& from, const std::string& to)
{
    size_t start = 0;
    while ((start = source.find(from, start)) != std::string::npos) {
	source.replace(start, from.length(), to);
	start += to.length();
    }
}

static int requirefile(lua_State* L, std::string requirename, std::string chunkname, std::string requirestring)
{  
     query_replace(requirestring, ";", " ");

     std::stringstream requirestream(requirestring);
	
     std::string path;
     std::stringstream text;

     bool found = false;	 

     while (!found && (requirestream >> path)) {	
	 query_replace(path, "?", requirename);

	 std::ifstream file(path);

	 if (file) {
	     text << file.rdbuf();
	     found = true;
	 }
    }
	
    if (!found) {
	log("failed to require " + requirename);
	return 1;
    }

    Bytecode bytecode(text.str());
    return luau_load(L, chunkname.data(), bytecode.data, bytecode.size, 0);
}

static bool compare_case_insenstive(const std::string s1, const std::string s2)
{
    if (s1.size() != s2.size())
        return false;
      
    bool match = true;
    
    for (int i = 0; match && i < s1.size(); i += 1) {
        match = std::tolower(s1[i]) == std::tolower(s2[i]);
    }

    return match;
}

int Api::require_builtin(lua_State* L, const std::string& filename)
{
    for (auto& [name, luaopen_lib] : builtinlibs) {
        if (compare_case_insenstive(filename, name)) {
	    lua_pushstring(L, name.data());
	    lua_gettable(L, LUA_REGISTRYINDEX);

	    if (lua_isnil(L, -1)) {
	        lua_pop(L, 1);
		lua_pushlightuserdata(L, this);
	        lua_pushcclosure(L, luaopen_lib, NULL, 1);
	        lua_call(L, 0, 1);
		lua_pushstring(L, name.data());
		lua_pushvalue(L, -2);
		lua_settable(L, LUA_REGISTRYINDEX);
	    }
	    
	    return 0;
	}
    }
    
    return 1;
}

static int require(lua_State* L, std::string filename)
{
    Api* ApiInstance = static_cast<Api*>(lua_tolightuserdata(L, lua_upvalueindex(1)));

    if (filename.at(0) == '@' && ApiInstance->require_builtin(L, filename) == LUA_OK) return 0;

    query_replace(filename, ".", "/");

    std::string requirestring = "./scripts/?.luau;./scripts/?/?.luau";
    std::string chunkname = "=require:" + filename;
	 
    int result = requirefile(L, filename, chunkname, requirestring);

    if (result != 0 ) return 1;

    int status = lua_pcall(L, 0, 1, 0);

    if (status == LUA_OK) return 0;
	 
    ApiInstance->Log(lua_tostring(L, 1));

    return status;
}

static int loadscript(lua_State* L, const std::string& scriptpath)
{
    std::string chunkname = "=loadscript:" + scriptpath;
    std::string filepath = "./scripts/" + scriptpath;
    return loadfile(L, filepath, chunkname);
}

static int runscript(lua_State* L, const std::string& scriptpath)
{
    int L_args = lua_gettop(L);
	
    lua_State* T = lua_newthread(L);
    luaL_sandboxthread(T);
    
    loadscript(T, scriptpath);
    
    for (int i = 2; i <= L_args; i += 1) {
        lua_xpush(L, T, i);
    }
    	
    int T_args = lua_gettop(T) - 1;
     
    int status = lua_resume(T, L, T_args);
    
    if (status != LUA_OK) {
        log(lua_tostring(T, -1));  
    }
    
    lua_pop(L, 1);

    return 0;
}

Api::Api() : ML(luaL_newstate())
{
    luaL_openlibs(ML);

    lua_pushlightuserdata(ML, this); 
    lua_pushcclosure(ML, luaopenApi, NULL, 1);
    lua_call(ML, 0, 0);
}

void lua_close_uielement(lua_State* L);
void lua_close_uielement3d(lua_State* L);

Api::~Api()
{
    /*
       this code block is so i can use Api.Log inside uielements just before they get destroyed
       destroy all uielements 2d/3d before calling lua_close and destroying Api
    */
    /* BEGIN */
    lua_close_uielement(ML);
    lua_close_uielement3d(ML);
    lua_gc(ML, LUA_GCCOLLECT, 0);
    /* END */
    lua_close(ML);
}

void Api::Boot(const std::string& bootfile)
{
    luaL_sandbox(ML);
    runscript(ML, bootfile);
}

void Api::Log(const std::string& message)
{
    std::cout << message << std::endl;
    Console(message);
}

void Api::SetGame(Game* GameInstance)
{
    lua_pushlightuserdata(ML, GameInstance);
    lua_pushcclosure(ML, luaopenGame, NULL, 1);
    lua_call(ML, 0, 0);
}

void Api::SetRenderer(Renderer* RendererInstance)
{
    lua_pushlightuserdata(ML, RendererInstance);
    lua_pushcclosure(ML, luaopenRenderer, NULL, 1);
    lua_call(ML, 0, 0);
}

void Api::Console(const std::string& message)
{
    lua_getref(ML, Hooks[CONSOLE].key);
    lua_pushlstring(ML, message.data(), message.size());
    lua_pcall(ML, 1, 0, 0);
}

void Api::Update()
{
    lua_getref(ML, Hooks[UPDATE].key);
    lua_pcall(ML, 0, 0, 0);
}

void Api::RenderGame(RenderPass* renderPass)
{
    lua_getref(ML, Hooks[RENDER_GAME].key);
    lua_pushlightuserdata(ML, renderPass);
    lua_pcall(ML, 1, 0, 0);
}

void Api::RenderBackground()
{
    lua_getref(ML, Hooks[RENDER_BG].key);
    lua_pcall(ML, 0, 0, 0);
}

void Api::RenderForeground()
{
    lua_getref(ML, Hooks[RENDER_FG].key);
    lua_pcall(ML, 0, 0, 0);
}

void Api::MouseMoved(float x, float y)
{
    lua_getref(ML, Hooks[MOUSE_MOVED].key);
    lua_pushnumber(ML, x);
    lua_pushnumber(ML, y);
    lua_pcall(ML, 2, 0, 0);
}

void Api::MouseButtonPressed(int btn, float x, float y)
{
    lua_getref(ML, Hooks[MOUSE_PRESSED].key);
    lua_pushinteger(ML, btn);
    lua_pushnumber(ML, x);
    lua_pushnumber(ML, y);
    lua_pcall(ML, 3, 0, 0);
}

void Api::MouseButtonReleased(int btn, float x, float y)
{
    lua_getref(ML, Hooks[MOUSE_RELEASED].key);
    lua_pushinteger(ML, btn);
    lua_pushnumber(ML, x);
    lua_pushnumber(ML, y);
    lua_pcall(ML, 3, 0, 0);
}

void Api::KeyPressed(int key)
{
    lua_getref(ML, Hooks[KEY_PRESSED].key);
    lua_pushinteger(ML, key);
    lua_pcall(ML, 1, 0, 0);
}

void Api::KeyReleased(int key)
{
    lua_getref(ML, Hooks[KEY_RELEASED].key);
    lua_pushinteger(ML, key);
    lua_pcall(ML, 1, 0, 0);
}

static int Api_log(lua_State* L)
{
    int nargs = lua_gettop(L);

    if (nargs == 0)
        return 0;

    std::stringstream ss;
	
    for (int n = 1; n <= nargs; n += 1) {
	const char* s = luaL_tolstring(L, n, NULL);

	if (n > 1) ss << "    ";
        
	ss << s;
    }
    
    static_cast<Api*>(lua_tolightuserdata(L, lua_upvalueindex(1)))->Log(ss.str());

    return 0;
}

static int Api_require(lua_State* L)
{
    int nargs = lua_gettop(L);
	
    if (nargs == 0 || lua_isnil(L, 1))
        return 0;
    
    require(L, lua_tostring(L, -1));

    return 1;
}

static int Api_runscript(lua_State* L)
{
    runscript(L, lua_tostring(L, 1));
    return 0;
}

static void parsemod(std::istream& data)
{
}

static void parsemodstring(std::string content)
{
}

static void parsemodfile(std::string filename)
{
}

static int loadmodstring(lua_State* L, std::string content)
{
    return 0;
}

static int loadmodfile(lua_State* L, std::string modpath)
{
     return 0;
}

static int Api_loadmodstring(lua_State* L)
{
    loadmodstring(L, lua_tostring(L, 1));
    return 0;
}

static int Api_loadmodfile(lua_State* L)
{
    loadmodfile(L, lua_tostring(L, 1));
    return 0;
}

int luaL_registerwithclosure(lua_State* L, const char* libname, const luaL_Reg* l, int upvalues)
{
   if (libname) {
       lua_getglobal(L, libname);
       if (lua_isnil(L, -1)) {
	   lua_pop(L, 1);
	   lua_newtable(L);
	   lua_pushvalue(L, -1);
	   lua_setglobal(L, libname);
       }
   }
   
   for (const luaL_Reg* f = l; f->name; f += 1) {    
       lua_pushvalue(L, lua_upvalueindex(upvalues));
       lua_pushcclosure(L, f->func, NULL, upvalues);
       lua_setfield(L, -2, f->name);
   }
   
   return 1;
}
    

static const luaL_Reg ApiBase[] = {
    {"log",       Api_log},
    {"require",   Api_require},
    {"runscript", Api_runscript},
	
    //{"loadmodstring", Api_loadmodstring},
    //{"loadmodfile", Api_loadmodfile},
    //{"loadmod_t", Api_loadmod_t},
	
    {NULL, NULL},
};

static int luaopenApiBase(lua_State* L)
{
    lua_pushvalue(L, LUA_GLOBALSINDEX);
    luaL_registerwithclosure(L, NULL, ApiBase, 1);
    return 1;
}

static int hook__call(lua_State* L)
{
    int nargs = lua_gettop(L);

    lua_pushvalue(L, lua_upvalueindex(2));
    lua_pushnil(L);
	
    while (lua_next(L, -2) != 0) {
	const char* key = lua_tostring(L, -2);
	if (lua_isfunction(L, -1)) {
	    for (int i = 2; i <= nargs; i += 1) {
	        lua_pushvalue(L, i);
	    }

	    int status = lua_pcall(L, nargs - 1, 0, 0);

	    if (status != LUA_OK) {
	        log(lua_tostring(L, -1));
	        lua_pop(L, nargs - 3);
	    }
	}
    }

    return 0;
}

static int hook__index(lua_State* L)
{
    lua_gettable(L, lua_upvalueindex(2));
    return 1;
}

static int hook__newindex(lua_State* L)
{
    if (!lua_isfunction(L, 3))
        return 0;

    lua_settable(L, lua_upvalueindex(2));

    return 0;
}

int Api_AddHook(lua_State* L)
{
    if (!lua_isfunction(L, 3))
        return 0;
	
    lua_getglobal(L, "Api");
    lua_getfield(L, -1, lua_tostring(L, 1));
    lua_pushvalue(L, -3);
    lua_setfield(L, -2, lua_tostring(L, 2));

    return 0;
}

int Api_RemoveHook(lua_State* L)
{
    lua_getglobal(L, "Api");
    lua_getfield(L, -1, lua_tostring(L, 1));
    lua_pushnil(L);
    lua_setfield(L, -2, lua_tostring(L, 2));

    return 0;
}

int Api_RemoveHooks(lua_State* L)
{
    return 0;
}

static const luaL_Reg ApiMain[] = {
    {"AddHook",     Api_AddHook},
    {"RemoveHook",  Api_RemoveHook},
    {"RemoveHooks", Api_RemoveHooks},

    {NULL, NULL},
};

int luaopenApiMain(lua_State* L)
{
    luaL_registerwithclosure(L, "Api", ApiMain, 1);

    for (auto& hook : Hooks) {		
        lua_newtable(L);
        lua_newtable(L);
	
        lua_pushstring(L, hook.name);
        lua_newtable(L);

        lua_pushvalue(L, -2);
	lua_pushvalue(L, -2);
        lua_pushcclosure(L, hook__call, NULL, 2);
        lua_setfield(L, 3, "__call");

	lua_pushvalue(L, -2);
        lua_pushvalue(L, -2);
        lua_pushcclosure(L, hook__index, NULL, 2);
	lua_setfield(L, 3, "__index");

	lua_pushvalue(L, -2);
	lua_pushvalue(L, -2);
	lua_pushcclosure(L, hook__newindex, NULL, 2);
        lua_setfield(L, 3, "__newindex");

        lua_pop(L, 2);

	hook.key = lua_ref(L, -2);

	lua_setmetatable(L, -2);
	lua_setfield(L, -2, hook.name);
    }
    
    return 1;
}


static const luaL_Reg libs[] {
    {"",    luaopenApiBase},
    {"Api", luaopenApiMain},
    
    {NULL, NULL},
};

int luaopenApi(lua_State* L)
{
    for (const luaL_Reg* lib = libs; lib->func; lib += 1) {
        lua_pushvalue(L, lua_upvalueindex(1));
        lua_pushcclosure(L, lib->func, NULL, 1);
	//lua_pushstring(L, lib->name);
	lua_call(L, 0, 0);
    }
    
    return 1;
}
