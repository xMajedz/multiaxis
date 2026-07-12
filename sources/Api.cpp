#include "Api.h"

#include <iostream>

#include <fstream>
#include <sstream>

#include <utility>

class Bytecode {
public:  
    int load(lua_State* L, const std::string& chunkname)
    {
	return luau_load(L, chunkname.data(), data(), size(), 0);
    };

    char* data()
    {
        return data_;
    };

    size_t size()
    {
	return size_;
    };

    Bytecode(const std::string& string) : data_(luau_compile(string.data(), string.size(), NULL, &size_)) {};

    ~Bytecode()
    {
        std::free(data_);
    };
private:
    char* data_;
    size_t size_;
};

static void log(const std::string& message)
{
    Api::GetInstance().Console(message);
    std::cout << message << std::endl;
}

static int loadstring(lua_State* L, const std::string& string, const std::string& chunkname)
{
    Bytecode bytecode(string);
    return bytecode.load(L, chunkname);
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
	
	Bytecode bytecode(text.str());

	return bytecode.load(L, chunkname);
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

	return bytecode.load(L, chunkname);
}

int luaopenApiRaylib(lua_State* L);
int luaopenApiRaygui(lua_State* L);
int luaopenApiRaymath(lua_State* L);

std::tuple<int, int(*)(lua_State*), std::string> builtinlibs[] = {
    {0, luaopenApiRaylib, "@Raylib"},
    {0, luaopenApiRaygui, "@Raygui"},
    {0, luaopenApiRaymath, "@Raymath"},
};

static int require_builtin(lua_State* L, const std::string& filename)
{
    for (auto& [ref, luaopen_lib, name] : builtinlibs) {
        if (filename == name) {
            if (ref == 0) {
	        lua_pushcfunction(L, luaopen_lib, NULL);
	        lua_call(L, 0, 1);
	        ref = lua_ref(L, -1);
	    } else {
	        lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
	    }
	    return 0;
	}
    }
    
    return 1;
}

static int require(lua_State* L, const std::string& filename)
{
    if (require_builtin(L, filename) == LUA_OK) return 0;
      
    std::string requirestring = "./scripts/?.luau;./scripts/?/?.luau";
    std::string chunkname = "=require:" + filename;
	 
    int result = requirefile(L, filename, chunkname, requirestring);

    if (result != 0 ) return 1;

    int status = lua_pcall(L, 0, 1, 0);

    if (status == LUA_OK) return 0;
	 
    log(lua_tostring(L, 1));

    return status;
}

static int loadscript(lua_State* L, const std::string& scriptpath)
{
    std::string chunkname = "=loadscript:" + scriptpath;
    std::string filepath = "./scripts/" + scriptpath + ".luau";
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

void luaopenApi(lua_State* L);

Api::Api() : ML(luaL_newstate())
{
    luaL_openlibs(ML);

    luaopenApi(ML);

    luaL_sandbox(ML);
}

Api::~Api()
{
    lua_close(ML);
}

void Api::Boot(const std::string& bootfile)
{
    runscript(ML, bootfile);
}

void Api::Console(const std::string& message)
{
    lua_rawgeti(ML, LUA_REGISTRYINDEX, HookList[CONSOLE]);
    lua_pushlstring(ML, message.data(), message.size());
    lua_pcall(ML, 1, 0, 0);
}

void Api::Update()
{
    lua_rawgeti(ML, LUA_REGISTRYINDEX, HookList[UPDATE]);
    lua_pcall(ML, 0, 0, 0);
}

void Api::RenderBackground()
{
    lua_rawgeti(ML, LUA_REGISTRYINDEX, HookList[RENDER_BG]);
    lua_pcall(ML, 0, 0, 0);
}

void Api::RenderForeground()
{
    lua_rawgeti(ML, LUA_REGISTRYINDEX, HookList[RENDER_FG]);
    lua_pcall(ML, 0, 0, 0);
}

static int Api_log(lua_State* L)
{
    std::stringstream ss;

    int nargs = lua_gettop(L);
	
    if (nargs == 0) return 0;
	
    for (int n = 1; n <= nargs; n += 1) {
	const char* s = luaL_tolstring(L, n, NULL);

	if (n > 1) ss << "    ";
        
	ss << s;
    }
	
    log(ss.str());

    return 0;
}

static int Api_require(lua_State* L)
{
    int nargs = lua_gettop(L);
	
    if (nargs == 0 || lua_isnil(L, 1)) return 0;
    
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
	int version = 0;
	int context = 0;

	int env_obj_id = 0;
	int env_obj_plane_id = 0;
	int env_obj_joint_id = 0;

	int player_id = 0;
    int body_id = 0;
	int joint_id = 0;

	std::string body_name;
	std::string joint_name;

	std::string line;
	/*
    EnvPlane* current_plane = nullptr;
    size_t plane_count = 0;
	
    Body* current_object = nullptr;
    size_t object_count = 0;

	Joint* current_object_joint = nullptr;
    size_t object_joint_count = 0;

	Player* current_player = nullptr;
	size_t player_count = 0;

	Body* current_body = nullptr;
	size_t b_count = 0;

	Joint* current_joint = nullptr;
	size_t j_count = 0;
	
	while (std::getline(data, line)) {
		std::stringstream datastream(line);
		std::string dataname;
	    		
		datastream >> dataname;
		    
		if (dataname == "version") {
		    datastream >> version;
				
            continue;
		} else if (dataname == "gamerule") {
			continue;
		} else if (dataname == "env_obj") {
			context = 1;
				
			if (datastream >> env_obj_id) {
			    std::string name = "object_" + std::to_string(env_obj_id);
				//Body object;
	            Api::objects_vector.push_back(Body());
				Api::o_map[name] = object_count;
				current_object = &Api::objects_vector[object_count];

				object_count += 1;
			}
				
			continue;
		} else if (dataname == "env_obj_plane") {
			context = 2;

			if (datastream >> env_obj_plane_id) {
				std::string name = "plane_" + std::to_string(env_obj_plane_id);
				//EnvPlane plane;
	            Api::planes.push_back(EnvPlane());
				current_plane = &Api::planes[plane_count];

				plane_count += 1;
            }
			
			continue;
		 } else if (dataname == "env_obj_joint") {
			context = 3;

			if (datastream >> env_obj_joint_id) {
			  std::string name = "object_joint_" + std::to_string(env_obj_joint_id);
              Joint object_joint;
			  Api::object_joints_vector.push_back(object_joint);
			  current_object_joint = &Api::object_joints_vector[object_joint_count];

			  object_joint_count += 1;
            }
				
			continue;
		 } else if (dataname == "player") {
			context = 3;

			if (datastream >> player_id) {
			    std::string name = "player_" + player_id;
	            if (player_count < Api::rules.numplayers) {
		            b_count = 0;
		            j_count = 0;

		            Player player(player_count, name.data());
		            Api::players_vector.push_back(player);
		            current_player = &Api::players_vector[player_count];

					player_count += 1;
				}
			}
				
			continue;
	     } else if (dataname == "body") {
			context = 4;

			if (datastream >> body_name) {
			    Body body;
				body.id_ = b_count;
				body.name_ = body_name;
			    
	            Api::b_map[body_name] = b_count;
				current_player->body.push_back(body);
				current_body = &current_player->body[b_count];

				b_count += 1;
			}

			continue;
		 } else if (dataname == "joint") {
			context = 5;

			if (datastream >> joint_name) {
			    Joint joint;
				joint.id_ = j_count;
				joint.name_ = joint_name;
			    
	            current_player->joint.push_back(joint);
	            current_joint = &current_player->joint[j_count];

				j_count += 1;
			}
				
			continue;
		 }

		 switch(context)
		 {
		 case 0:
			if (dataname == "turnframes") {
			  datastream >> Api::rules.turnframes;
			} else if (dataname == "engagedistance") {
			  datastream >> Api::rules.engagedistance;
			} else if (dataname == "engageheight") {
			  datastream >> Api::rules.engageheight;
			} else if (dataname == "gravity") {
			  datastream >> Api::rules.gravity.x;
			  datastream >> Api::rules.gravity.y;
			  datastream >> Api::rules.gravity.z;
			} else if (dataname == "numplayers") {
			  datastream >> Api::rules.numplayers;
			}
			    
			break;
		  case 1:
			if (dataname == "shape") {
			     if (datastream >> dataname) {
				   	 if (dataname == "box") current_object->shape = BOX;
				     else if (dataname == "sphere") current_object->shape = SPHERE;
				     else if (dataname == "capsule") current_object->shape = CAPSULE;
					 else if (dataname == "cylinder") current_object->shape = CYLINDER;
					 else if (dataname == "composite") current_object->shape = COMPOSITE;
			     }
			} else if (dataname == "pos") {
			   	 datastream >> current_object->m_position.x;
				 datastream >> current_object->m_position.y;
				 datastream >> current_object->m_position.z;
			} else if (dataname == "mass") {
			     datastream >> current_object->mass;
			} else if (dataname == "density") {
			     datastream >> current_object->density;
			} else if (dataname == "color") {
			     float r, g, b, a;
			     datastream >> r;
				 datastream >> g;
				 datastream >> b;
				 datastream >> a;
				 current_object->m_color.r = 255 * r;
				 current_object->m_color.g = 255 * g;
				 current_object->m_color.b = 255 * b;
				 current_object->m_color.a = 255 * a;
			} else if (dataname == "rot") {
			     Vector3 rot;
				 datastream >> rot.x;
			     datastream >> rot.y;
				 datastream >> rot.z;
			   	 Quaternion q = QuaternionFromMatrix(MatrixRotateXYZ(rot));
			     current_object->m_orientation.x = q.x;
				 current_object->m_orientation.y = q.y;
				 current_object->m_orientation.z = q.z;
				 current_object->m_orientation.w = q.w;
			} else if (dataname == "sides") {
			     datastream >> current_object->sides.x;
				 datastream >> current_object->sides.y;
				 datastream >> current_object->sides.z;
			} else if (dataname == "radius") {
			     datastream >> current_object->radius;
			} else if (dataname == "length") {
			     datastream >> current_object->length;
			} else if (dataname == "force") {
			     //datastream >> Api::o->force.x;
				 //datastream >> Api::o->force.y;
				 //datastream >> Api::o->force.z;
			} else if (dataname == "flag") {
			     datastream >> current_object->flag_;

				 current_object->static_ = current_object->flag_ & 1;
				 current_object->composite_ = current_object->flag_ & 2;
				 current_object->interactive_ = current_object->flag_ & 4;
			} else if (dataname == "bounce") {
			     datastream >> current_object->bounce;
			} else if (dataname == "friction") {
			     datastream >> current_object->friction;
			}	
			break;
		case 2:
			if (dataname == "param") {
			   	 datastream >> current_plane->param.x;
				 datastream >> current_plane->param.y;
				 datastream >> current_plane->param.z;
				 datastream >> current_plane->param.w;
			} else if (dataname == "bounce") {
			     datastream >> current_plane->bounce;
			} else if (dataname == "friction") {
			     datastream >> current_plane->friction;
			}

			break;
		}
	}*/
}

static void parsemodstring(std::string content)
{
    std::stringstream s(content);

    parsemod(s);
}

static void parsemodfile(std::string filename)
{
    std::ifstream file(filename);

    if (!file) return;

    parsemod(file);
}


static int loadmodstring(lua_State* L, std::string content)
{
    //Reset();
    parsemodstring(content);
    //Game& Game_ = Game::GetInstance();
    //Game_.Reset();
    //Game_.ImportMod();
    //Game_.NewGame();
    return 0;
}

static int loadmodfile(lua_State* L, std::string modpath)
{
   //Reset();
    parsemodfile("./mods/" + modpath);
    //Game& Game_ = Game::GetInstance();
    //Game_.Reset();
    //Game_.ImportMod();
    //Game_.NewGame();
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

static const luaL_Reg ApiBase[] = {
    {"log", Api_log},
    {"require", Api_require},
    {"runscript", Api_runscript},
	
    {"loadmodstring", Api_loadmodstring},
    {"loadmodfile", Api_loadmodfile},
  //{"loadmod_t", Api_loadmod_t},
	
    {NULL, NULL},
};

static int luaopenApiBase(lua_State* L)
{
    luaL_register(L, "_G", ApiBase);
    return 1;
}

static int metamethod_call(lua_State* L)
{
    const char* callback = lua_tostring(L, lua_upvalueindex(1));

    size_t nargs = lua_gettop(L);

    lua_rawgeti(L, lua_upvalueindex(2), 1);
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
	
    lua_pop(L, 1);

    return 0;
}

static int metamethod_index(lua_State* L)
{
    const char* callback = lua_tostring(L, lua_upvalueindex(1));
    lua_rawgeti(L, lua_upvalueindex(2), 1);
    lua_getfield(L, -1, lua_tostring(L, 2));
    return 1;
}

static int metamethod_newindex(lua_State* L)
{
    const char* callback = lua_tostring(L, lua_upvalueindex(1));
    lua_rawgeti(L, lua_upvalueindex(2), 1);
    lua_pushvalue(L, 3);
    lua_setfield(L, -2, lua_tostring(L, 2));
    return 0;
}

static int Api_SetHook(lua_State* L)
{
    return 0;
}

static const luaL_Reg ApiMain[] = {
    {"SetHook", Api_SetHook},
  
    {NULL, NULL},
};

int luaopenApiMain(lua_State* L)
{
    luaL_register(L, "Api", ApiMain);

    for (int i = 0; i < HOOK_COUNT; i += 1) {
        auto Event = Hooks[i];
		
        lua_newtable(L);
        lua_newtable(L);

       // closure table
        lua_newtable(L);
        lua_newtable(L);
        int closure_table = lua_ref(L, -2);
        lua_rawseti(L, -2, 1);
        lua_remove(L, -1);

	std::string chunknames = "Api.?.__call Api.?.__index Api.?.__newindex";
	query_replace(chunknames, "?", Event);

	std::string chunkname;
	std::stringstream chunkstream(chunknames);

	chunkstream >> chunkname;
	
        lua_pushstring(L, Event);
        lua_rawgeti(L, LUA_REGISTRYINDEX, closure_table);
        lua_pushcclosure(L, metamethod_call, chunkname.data(), 2);
        lua_setfield(L, -2, "__call");

	chunkstream >> chunkname;

	lua_pushstring(L, Event);
        lua_rawgeti(L, LUA_REGISTRYINDEX, closure_table);
	lua_pushcclosure(L, metamethod_index, chunkname.data(), 2);
	lua_setfield(L, -2, "__index");

	chunkstream >> chunkname;

	lua_pushstring(L, Event);
	lua_rawgeti(L, LUA_REGISTRYINDEX, closure_table);
	lua_pushcclosure(L, metamethod_newindex, chunkname.data(), 2);
        lua_setfield(L, -2, "__newindex");
      
	HookList[i] = (Hook)lua_ref(L, -2);

	lua_setmetatable(L, -2);
	lua_setfield(L, -2, Event);
    }
	
    return 1;
}

static const luaL_Reg libs[] = {
    {"",            luaopenApiBase},
    {"Api",         luaopenApiMain},
	
  //{"Game",        luaopenApiGame},
  //{"Replay",      luaopenApiReplay},
  //{"Expermental", luaopenApiExpermental},
  //{"Net",         luaopenApiNet},
	
    {NULL, NULL},
};

void luaopenApi(lua_State* L)
{
    for (const luaL_Reg* lib = libs; lib->func; lib += 1) {
        lua_pushcfunction(L, lib->func, NULL);
        lua_pushstring(L, lib->name);
        lua_call(L, 1, 0);
    }
}
