#include "api.h"

using namespace raylib;
#include "raymath.h"

static void log(std::string message)
{
    std::cout << message << std::endl;
}

static void log_luau(const char* msg)
{
	Console::log(msg);
}

static void log_ode(int errnum, const char* msg, va_list ap)
{
	Console::log(TextFormat("%d: %s", errnum, msg));
}

static void log_raylib(int logLevel, const char* text, va_list args)
{
    Console::log(TextFormat("%d: %s", logLevel, text));
}

void Api::Init()
{
	ML = luaL_newstate();

	luaL_openlibs(ML);
	
	luaopenApi(ML);
	
	luaL_sandbox(ML);

	Console::SetCallback(log);
	
	Luau::setLogCallback(log_luau);
    
	dSetErrorHandler(log_ode);
	dSetDebugHandler(log_ode);
	dSetMessageHandler(log_ode);
	
	SetTraceLogCallback(log_raylib);
}

void Api::Boot(std::string filename)
{
    lua_State* T = lua_newthread(ML);
	luaL_sandboxthread(T);

    loadscript(T, filename);
	
	int nargs = lua_gettop(T) - 1;
	
	int status = lua_resume(T, ML, nargs);

	switch(status)
	{
	case LUA_OK:
		 lua_pop(ML, 1);
		 break;
	case LUA_ERRRUN:     
		 Luau::log(luaL_tolstring(T, -1, NULL));
		 break;
	}
}

int HotKeys[300] = { 0 };

void Api::UpdateHotKeys()
{
    for (int key = 0; key < 300; key += 1) {
	    if (HotKeys[key] != NULL && IsKeyPressed(key)) {
		    lua_rawgeti(ML, LUA_REGISTRYINDEX, HotKeys[key]);
		    lua_call(ML, 0, 0);
	    }
    }
}

void Api::Close()
{
	lua_close(ML);
}

Gamerules Api::GetRules()
{
	return rules;
}

std::vector<EnvPlane> Api::GetEnvPlanes()
{
	return planes_vector;
}

std::vector<Body> Api::GetObjects()
{
	return objects_vector;
}

std::vector<Joint> Api::GetJointObjects()
{
	return object_joints_vector;
}

std::vector<Player> Api::GetPlayers()
{
	return players_vector;
}

size_t Api::GetEnvPlanesCount()
{
    return planes_vector.size();
}

size_t Api::GetObjectsCount()
{
    return objects_vector.size();
}

size_t Api::GetJointObjectsCount()
{
    return object_joints_vector.size();
}

size_t Api::GetPlayersCount()
{
    return players_vector.size();
}

enum CallbackEvent {
  PROTOTYPE = 0,
  
  NEW_GAME,
  FREEZE,
  STEP,
  UPDATE,
  DRAW,
  DRAW3D,
  NEAR_CALLBACK,
  FILE_DROPPED,
  CONSOLE,

  EVENT_COUNT,
};

CallbackEvent EventList[EVENT_COUNT];

int Api::DrawCallback()
{
    lua_rawgeti(ML, LUA_REGISTRYINDEX, EventList[DRAW]);
    lua_pcall(ML, 0, 0, 0);
    return 0;
}

int Api::Draw3DCallback()
{
    lua_rawgeti(ML, LUA_REGISTRYINDEX, EventList[DRAW3D]);
    lua_pcall(ML, 0, 0, 0);
    return 0;
}

int Api::NewGameCallback()
{
    lua_rawgeti(ML, LUA_REGISTRYINDEX, EventList[NEW_GAME]);
    lua_pcall(ML, 0, 0, 0);
    return 0;
}

int Api::FreezeCallback()
{
    return 0;
}

int Api::StepCallback()
{
    return 0;
}

int Api::UpdateCallback(dReal dt)
{
    lua_rawgeti(ML, LUA_REGISTRYINDEX, EventList[UPDATE]);
	lua_pushnumber(ML, dt);
    lua_pcall(ML, 1, 0, 0);
    return 0;
}

int Api::ConsoleCallback(std::string message)
{
    lua_rawgeti(ML, LUA_REGISTRYINDEX, EventList[CONSOLE]);
	lua_pushstring(ML, message.data());
    lua_pcall(ML, 1, 0, 0);
    return 0;
}

int Api::FileDroppedCallback(FilePathList files)
{
    lua_rawgeti(ML, LUA_REGISTRYINDEX, EventList[FILE_DROPPED]);
    lua_newtable(ML);
	for (int i = 0; i < files.count; i += 1) {
	    lua_pushstring(ML, files.paths[i]);
		lua_rawseti(ML, -2, 1 + i);
	}
    lua_pcall(ML, 1, 0, 0);
    return 0;
}


int Api::loadscript(lua_State* L, std::string scriptpath)
{
    Luau::loadfile(
        L,
	    TextFormat("./scripts/%s", scriptpath.data()),
	    TextFormat("%s:%s", "loadscript", scriptpath.data())
    );

	return 0;
}

enum Context
{
	NoContext = 0,

	ObjectContext,
	ObjectJointContext,

	PlayerContext,
	BodyContext,
	JointContext,

} DataContext = NoContext;

template<typename T>
static void parsemod(T& data)
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
			    std::string name = "object_" + std::to_string(env_obj_plane_id);	
				Body object;
				Api::o_map[name] = object_count;
	            Api::objects_vector.push_back(object);
				current_object = &Api::objects_vector[object_count];

				object_count += 1;
			}
				
			continue;
		} else if (dataname == "env_obj_plane") {
			context = 2;

			if (datastream >> env_obj_plane_id) {
				std::string name = "plane_" + std::to_string(env_obj_plane_id);
				EnvPlane plane;
	            Api::planes_vector.push_back(plane);
				current_plane = &Api::planes_vector[plane_count];

				plane_count += 1;
            }
				
			continue;
		 } else if (dataname == "env_obj_joint") {
			context = 2;

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
	}
}

static void parsemodstring(std::string content)
{
    std::stringstream s(content);
	Api::rules.mod = "parsemodstring";

	parsemod(s);
}

static void parsemodfile(std::string filename)
{
    std::ifstream file(filename);
	Api::rules.mod = "parsemodfile";

	if (!file.is_open()) return;

	parsemod(file);

	file.close();
}

void Api::Reset()
{
	DataContext = NoContext;

	planes_vector.clear();
	objects_vector.clear();
	object_joints_vector.clear();
	players_vector.clear();
  
	b_vector.clear();
	j_vector.clear();
}

void Api::SetHotKey(int key, int ref)
{
    HotKeys[key] = ref;
}

int Api::loadmodstring(lua_State* L, std::string content)
{
    Reset();
    parsemodstring(content);
	Game& Game_ = Game::GetInstance();
	Game_.Reset();
	Game_.ImportMod();
	Game_.NewGame();
	return 0;
}

int Api::loadmodfile(lua_State* L, std::string modpath)
{
    Reset();
    parsemodfile("./mods/" + modpath);
	Game& Game_ = Game::GetInstance();
	Game_.Reset();
	Game_.ImportMod();
	Game_.NewGame();
	return 0;
}


static int Api_loadmodstring(lua_State* L)
{
	Api::loadmodstring(L, lua_tostring(L, 1));
	return 0;
}

static int Api_loadmodfile(lua_State* L)
{
	Api::loadmodfile(L, lua_tostring(L, 1));
	return 0;
}


static int Api_loadmod_t(lua_State* L)
{
	if (lua_istable(L, -1)) {
		lua_getfield(L, -1, "mod");
		lua_getfield(L, -2, "rules");
		if (lua_istable(L, -1)) {
			lua_getfield(L, -3, "gravity");
			if (lua_istable(L, -4)) {
				lua_rawgeti(L, -4, 1);
				lua_rawgeti(L, -5, 2);
				lua_rawgeti(L, -6, 3);
			}
		}
	}

	return 0;
}
/*
static int Api_reactiontime(lua_State* L)
{
	lua_rawgeti(L, -1, 1);
	lua_Number reactiontime = lua_tonumber(L, -1);
	switch(DataContext) {
		case NoContext: {
			Api::rules.reaction_time = reactiontime;
		} break;
		case ObjectContext: {
			// Error Handling
		} break;
		case BodyContext: {
			// Error Handling
		} break;
		case JointContext: {
			// Error Handling
		} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int Api_turnframes(lua_State* L)
{
	lua_rawgeti(L, -1, 1);
	lua_Number turnframes = lua_tonumber(L, -1);

	switch(DataContext) {
		case NoContext: {
			Api::rules.turnframes = turnframes;
		} break;
		case ObjectContext: {
			// Error Handling
		} break;
		case BodyContext: {
			// Error Handling
		} break;
		case JointContext: {
			// Error Handling
		} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int Api_numplayers(lua_State* L)
{
	lua_rawgeti(L, -1, 1);
	lua_Number numplayers = lua_tonumber(L, -1); 

	switch(DataContext) {
		case NoContext: {
			Api::rules.numplayers = numplayers;
		} break;
		case ObjectContext: {
			// Error Handling
		} break;
		case BodyContext: {
			// Error Handling
		} break;
		case JointContext: {
			// Error Handling
		} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int Api_friction(lua_State* L)
{
	lua_rawgeti(L, -1, 1);
	lua_Number friction = lua_tonumber(L, -1);

	switch(DataContext) {
		case NoContext: {
			Api::rules.friction = friction;
		} break;
		case ObjectContext: {
			// Error Handling
		} break;
		case BodyContext: {
			// Error Handling
		} break;
		case JointContext: {
			// Error Handling
		} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int Api_engagedistance(lua_State* L)
{
	lua_rawgeti(L, -1, 1);
	lua_Number distance = lua_tonumber(L, -1);

	switch(DataContext) {
		case NoContext: {
			Api::rules.engagedistance = distance;
		} break;
		case ObjectContext: {
			// Error Handling
		} break;
		case BodyContext: {
			// Error Handling
		} break;
		case JointContext: {
			// Error Handling
		} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int Api_engageheight(lua_State* L)
{
	lua_rawgeti(L, -1, 1);
	lua_Number height = lua_tonumber(L, -1);

	switch(DataContext) {
		case NoContext: {
			Api::rules.engageheight = height;
		} break;
		case ObjectContext: {
			// Error Handling
		} break;
		case BodyContext: {
			// Error Handling
		} break;
		case JointContext: {
			// Error Handling
		} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int Api_engagepos(lua_State* L)
{
    vec3<dReal> pos;
	lua_rawgeti(L, -1, 1);
	pos.x = lua_tonumber(L, -1);
	lua_rawgeti(L, -2, 2);
	pos.y = lua_tonumber(L, -1);
	lua_rawgeti(L, -3, 3);
	pos.z = lua_tonumber(L, -1);

	switch(DataContext) {
		case NoContext: {
			// Error Handling
		} break;
		case PlayerContext: {
			Api::p->use_engagepos = true;
			Api::p->engagepos = pos;
		} break;
		case ObjectContext: {
			// Error Handling
		} break;
		case BodyContext: {
			// Error Handling
		} break;
		case JointContext: {
			// Error Handling
		} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int Api_engagerot(lua_State* L)
{
    vec3<dReal> rot;
	lua_rawgeti(L, -1, 1);
	rot.x = lua_tonumber(L, -1);
	lua_rawgeti(L, -2, 2);
	rot.y = lua_tonumber(L, -1);
	lua_rawgeti(L, -3, 3);
	rot.z = lua_tonumber(L, -1);

	switch(DataContext) {
		case NoContext: {
			// Error Handling
		} break;
		case PlayerContext: {
			Api::p->use_engagerot = true;
			Api::p->engagerot = rot;
		} break;
		case ObjectContext: {
			// Error Handling
		} break;
		case BodyContext: {
			// Error Handling
		} break;
		case JointContext: {
			// Error Handling
		} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int Api_color(lua_State* L)
{
	Color color;

	lua_rawgeti(L, -1, 1);
	color.r = lua_tonumber(L, -1);
	lua_rawgeti(L, -2, 2);
	color.g = lua_tonumber(L, -1);
	lua_rawgeti(L, -3, 3);
	color.b = lua_tonumber(L, -1);
	lua_rawgeti(L, -4, 4);
	color.a = lua_tonumber(L, -1);

	switch(DataContext) {
		case NoContext: {
			// Error Handling
		} break;
		case PlayerContext: {
			Api::p->m_color = color;
		} break;
		case ObjectContext: {
			Api::o->m_color = color;
		} break;
		case BodyContext: {
			Api::b->m_color = color;
		} break;
		case JointContext: {
			Api::j->m_color = color;
		} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int Api_gravity(lua_State* L)
{
    vec3<dReal> gravity;
	lua_rawgeti(L, -1, 1);
	gravity.x = lua_tonumber(L, -1); 
	lua_rawgeti(L, -2, 2);
	gravity.y = lua_tonumber(L, -1); 
	lua_rawgeti(L, -3, 3);
	gravity.z = lua_tonumber(L, -1); 

	switch(DataContext) {
		case NoContext: {
			Api::rules.gravity = gravity;
		} break;
		case ObjectContext: {
			// Error Handling	
		} break;
		case BodyContext: {
			// Error Handling	
		} break;
		case JointContext: {
			// Error Handling	
		} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int Api_mod(lua_State* L)
{
	Api::rules.mod = lua_tostring(L, -1);
	lua_pushinteger(L, 1);
	return 1;
}

static int Api_object(lua_State* L)
{
	DataContext = ObjectContext;
	std::string_view name = lua_tostring(L, -1);
	Body o(Api::o_count, name.data());
	Api::o_map[name] = Api::o_count;
	Api::o_vector.push_back(o);
	Api::o = &Api::o_vector[Api::o_count];
	Api::o_count += 1;

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int Api_objectjoint(lua_State* L)
{
	DataContext = ObjectJointContext;
	Joint oj(Api::oj_count, lua_tostring(L, -1));
	Api::oj_vector.push_back(oj);
	Api::oj = &Api::oj_vector[Api::oj_count];
	Api::oj_count += 1;

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int Api_player(lua_State* L)
{
	DataContext = PlayerContext;
	const char* name = lua_tostring(L, -1);
	lua_Number result = 0;
	if (Api::p_count < Api::rules.numplayers) {
		Api::b_count = 0;
		Api::j_count = 0;

		Player p(Api::p_count, name);
		Api::p_vector.push_back(p);
		Api::p = &Api::p_vector[Api::p_count];
		Api::p_count += 1;
		result = 1;
	}
	lua_pushnumber(L, result);
	return 1;
}

static int Api_body(lua_State* L)
{
	DataContext = BodyContext;
	std::string_view name = lua_tostring(L, -1);
	Body b(Api::b_count, name.data());
	Api::b_map[name] = Api::b_count;
	Api::p->body.push_back(b);
	Api::b = &Api::p->body[Api::b_count];
	Api::b_count += 1;

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int Api_joint(lua_State* L)
{
	DataContext = JointContext;
	Joint j(Api::j_count, lua_tostring(L, -1));
	Api::p->joint.push_back(j);
	Api::j = &Api::p->joint[Api::j_count];
	Api::j_count += 1;

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int Api_shape(lua_State* L)
{
	lua_Integer shape = lua_tointeger(L, -1);

	switch(DataContext) {
	case ObjectContext: {
		Api::o->shape = (BodyShape)shape;
	} break;
	case ObjectJointContext: {
		Api::oj->shape = (BodyShape)shape;
	} break;
	case BodyContext: {
		Api::b->shape = (BodyShape)shape;
	} break;
	case JointContext: {
		Api::j->shape = (BodyShape)shape;
	} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int Api_position(lua_State* L)
{
    vec3<dReal> position;
	lua_rawgeti(L, -1, 1);
	position.x = lua_tonumber(L, -1); 
	lua_rawgeti(L, -2, 2);
	position.y = lua_tonumber(L, -1); 
	lua_rawgeti(L, -3, 3);
	position.z = lua_tonumber(L, -1); 
	
	switch(DataContext) {
	case NoContext: {
		// Error Handling
	} break;
	case ObjectContext: {
		Api::o->m_position = position;
	} break;
	case ObjectJointContext: {
		Api::oj->m_position = position;
	} break;
	case BodyContext: {
		Api::b->m_position = position;
	} break;
	case JointContext: {
		Api::j->m_position = position;
	} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int Api_orientation(lua_State* L)
{
    vec4<dReal> orientation;
	lua_rawgeti(L, -1, 1);
	orientation.w = lua_tonumber(L, -1); 
	lua_rawgeti(L, -2, 2);
	orientation.x = lua_tonumber(L, -1); 
	lua_rawgeti(L, -3, 3);
	orientation.y = lua_tonumber(L, -1); 
	lua_rawgeti(L, -4, 4);
	orientation.z = lua_tonumber(L, -1);

	switch(DataContext) {
		case NoContext: {
			// Error Handling
		} break;
		case ObjectContext: {
			Api::o->m_orientation = orientation;

		} break;
		case BodyContext: {
			Api::b->m_orientation = orientation;
		} break;
		case JointContext: {
			Api::j->m_orientation = orientation;
		} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int Api_sides(lua_State* L)
{
    vec3<dReal> sides;
	lua_rawgeti(L, -1, 1);
	sides.x = lua_tonumber(L, -1); 
	lua_rawgeti(L, -2, 2);
	sides.y = lua_tonumber(L, -1); 
	lua_rawgeti(L, -3, 3);
	sides.z = lua_tonumber(L, -1); 

	switch(DataContext) {
		case NoContext: {
			// Error Handling
		} break;
		case ObjectContext: {
			Api::o->m_sides = sides;
		} break;
		case BodyContext: {
			Api::b->m_sides = sides;
		} break;
		case JointContext: {
			Api::j->m_sides = sides;
		} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int Api_density(lua_State* L)
{
	lua_rawgeti(L, -1, 1);
	lua_Number density = lua_tonumber(L, -1); 

	switch(DataContext) {
	case NoContext: {
		// Error Handling
	} break;
	case ObjectContext: {
		Api::o->density = density;
	} break;
	case ObjectJointContext: {
		Api::oj->density = density;
	} break;
	case BodyContext: {
		Api::b->density = density;
	} break;
	case JointContext: {
		Api::j->density = density;
	} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int Api_static(lua_State* L)
{
	switch(DataContext) {
		case NoContext: {
			// Error Handling
		} break;
		case ObjectContext: {
			Api::o->static_ = true;
		} break;
		case BodyContext: {
			Api::b->static_ = true;
		} break;
		case JointContext: {
			Api::j->static_ = true;
		} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int Api_flag(lua_State* L)
{
	lua_getfield(L, -1, "static");
	bool flag_static = !lua_isnil(L, -1);
	lua_getfield(L, -2, "composite");
	bool flag_composite= !lua_isnil(L, -1);
	lua_getfield(L, -3, "interactive");
	bool flag_interactive = !lua_isnil(L, -1);

	switch(DataContext)
	{
	case ObjectContext: {
		Api::o->static_ = flag_static;
		Api::o->composite_ = flag_composite;
		Api::o->interactive_ = flag_interactive;
	} break;
	case ObjectJointContext: {
		Api::oj->static_ = flag_static;
		Api::oj->composite_ = flag_composite;
		Api::oj->interactive_ = flag_interactive;
	} break;
	case BodyContext: {
		Api::b->static_ = flag_static;
		Api::b->composite_ = flag_composite;
		Api::b->interactive_ = flag_interactive;
	} break;
	case JointContext: {
		Api::j->static_ = flag_static;
		Api::j->composite_ = flag_composite;
		Api::j->interactive_ = flag_interactive;
	} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int Api_radius(lua_State* L)
{
	lua_rawgeti(L, -1, 1);
	lua_Number radius = lua_tonumber(L, -1); 

	switch(DataContext) {
	case NoContext: {
		// Error Handling
	} break;
	case ObjectContext: {
		Api::o->radius = radius;
	} break;
	case ObjectJointContext: {
		Api::oj->radius = radius;
	} break;
	case BodyContext: {
		Api::b->radius = radius;
	} break;
	case JointContext: {
		Api::j->radius = radius;
	} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int Api_length(lua_State* L)
{
	lua_rawgeti(L, -1, 1);
	lua_Number length = lua_tonumber(L, -1); 

	switch(DataContext) {
		case NoContext: {
			// Error Handling
		} break;
		case ObjectContext: {
			Api::o->length = length;
		} break;
		case BodyContext: {
			Api::b->length = length;
		} break;
		case JointContext: {
			Api::j->length = length;
		} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int Api_strength(lua_State* L)
{
	lua_Number strength;
	lua_rawgeti(L, -1, 1);
	strength = lua_tonumber(L, -1); 

	switch(DataContext) {
	case ObjectJointContext: {
		Api::oj->strength = strength;
	} break;
	case JointContext: {
		Api::j->strength = strength;
	} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int Api_strength_alt(lua_State* L)
{
	lua_Number strength_alt;
	lua_rawgeti(L, -1, 1);
	strength_alt = lua_tonumber(L, -1); 

	switch(DataContext) {
	case ObjectJointContext: {
		Api::oj->strength_alt = strength_alt;
	} break;
	case JointContext: {
		Api::j->strength_alt = strength_alt;
	} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}


static int Api_velocity(lua_State* L)
{
	lua_Number velocity;
	lua_rawgeti(L, -1, 1);
	velocity = lua_tonumber(L, -1); 

	switch(DataContext) {
	case ObjectJointContext: {
		Api::oj->velocity = velocity;
	} break;
	case JointContext: {
		Api::j->velocity = velocity;
	} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int Api_velocity_alt(lua_State* L)
{
	lua_Number velocity_alt;
	lua_rawgeti(L, -1, 1);
	velocity_alt = lua_tonumber(L, -1); 

	switch(DataContext) {
	case ObjectJointContext: {
		Api::oj->velocity_alt = velocity_alt;
	} break;
	case JointContext: {
		Api::j->velocity_alt = velocity_alt;
	} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int Api_axis(lua_State* L)
{
    vec3<dReal> axis;
	lua_rawgeti(L, -1, 1);
	axis.x = lua_tonumber(L, -1); 
	lua_rawgeti(L, -2, 2);
	axis.y = lua_tonumber(L, -1); 
	lua_rawgeti(L, -3, 3);
	axis.z = lua_tonumber(L, -1); 

	switch(DataContext) {
	case ObjectJointContext: {
		Api::oj->axis = axis;
	} break;
	case JointContext: {
		Api::j->axis = axis;
	} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int Api_axis_alt(lua_State* L)
{
    vec3<dReal> axis_alt;
	lua_rawgeti(L, -1, 1);
	axis_alt.x = lua_tonumber(L, -1); 
	lua_rawgeti(L, -2, 2);
	axis_alt.y = lua_tonumber(L, -1); 
	lua_rawgeti(L, -3, 3);
	axis_alt.z = lua_tonumber(L, -1); 

	switch(DataContext) {
	case ObjectJointContext: {
		Api::oj->axis_alt = axis_alt;
	} break;
	case JointContext: {
		Api::j->axis_alt = axis_alt;
	} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int Api_range(lua_State* L)
{
	lua_Number range[2];
		
	lua_rawgeti(L, -1, 1);
	range[0] = lua_tonumber(L, -1); 
	lua_rawgeti(L, -2, 2);
	range[1] = lua_tonumber(L, -1); 

	switch(DataContext) {
	case ObjectJointContext: {
		Api::oj->range[0] = range[0];
		Api::oj->range[1] = range[1];
	} break;
	case JointContext: {
		Api::j->range[0] = range[0];
		Api::j->range[1] = range[1];
	} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int Api_range_alt(lua_State* L)
{
	lua_Number range_alt[2];
	lua_rawgeti(L, -1, 1);
	range_alt[0] = lua_tonumber(L, -1); 
	lua_rawgeti(L, -2, 2);
	range_alt[1] = lua_tonumber(L, -1); 

	switch(DataContext) {
	case ObjectJointContext: {
		Api::oj->range_alt[0] = range_alt[0];
		Api::oj->range_alt[1] = range_alt[1];
	} break;
	case JointContext: {
		Api::j->range_alt[0] = range_alt[0];
		Api::j->range_alt[1] = range_alt[1];
	} break;
	}
	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int Api_connections(lua_State* L)
{
	std::string_view connections[2];
		
	lua_rawgeti(L, -1, 1);
	connections[0] = lua_tostring(L, -1); 
	lua_rawgeti(L, -2, 2);
	connections[1] = lua_tostring(L, -1); 

	switch(DataContext) {
	case ObjectJointContext: {
		Api::oj->connections[0] = Api::o_map[connections[0]];
		Api::oj->connections[1] = Api::o_map[connections[1]];
	} break;
	case JointContext: {
		Api::j->connections[0] = Api::b_map[connections[0]];
		Api::j->connections[1] = Api::b_map[connections[1]];
	} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int Api_connection_type(lua_State* L)
{
	lua_Integer type = lua_tointeger(L, -1);

	switch(DataContext) {
	case ObjectJointContext: {
		Api::oj->type = (JointType)type;
	} break;
	case JointContext: {
		Api::j->type = (JointType)type;
	} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}
*/
static int Api_require(lua_State* L)
{
    int nargs = lua_gettop(L);
	
	if (nargs > 0 && !lua_isnil(L, 1)) {
	    const char* filename = lua_tostring(L, -1);
		lua_pop(L, 1);
		Luau::require(L, filename);
	    return 1;
	}

	return 0;
}

static int Api_loadscript(lua_State* L)
{
    const char* scriptpath = lua_tostring(L, 1);

    int L_args = lua_gettop(L);
	
	lua_State* T = lua_newthread(L);
	luaL_sandboxthread(T);
	
	Api::loadscript(T, scriptpath);

	for (int i = 2; i <= L_args; i += 1) {
	    lua_xpush(L, T, i);
	}
	
	int T_args = lua_gettop(T) - 1;
	  
	int status = lua_resume(T, L, T_args);

    
	switch(status)
	{
	case LUA_OK:
		 lua_pop(L, 1);
		 break;
	case LUA_ERRRUN:  
		 Luau::log(luaL_tolstring(T, -1, NULL));
		 break;
	}
    
	return 0;
}

void Console::SetCallback(ConsoleCallback_t callback)
{
	console_callback = callback;
}

void Console::log(std::string message)
{
    Api::ConsoleCallback(message);
	if (console_callback != nullptr) console_callback(message);
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
	
	Console::log(ss.str());

	return 0;
}

static int Api_SetHotKey(lua_State* L)
{
    Api::SetHotKey(lua_tointeger(L, 1), lua_ref(L, 2));
	return 0;
}

static int Api_Reset(lua_State* L)
{
	Api::Reset();
	return 0;
}

static const luaL_Reg ApiBase[] = {
	{"log", Api_log},
	{"require", Api_require},
	{"loadscript", Api_loadscript},
	
	{"loadmodstring", Api_loadmodstring},
	{"loadmodfile", Api_loadmodfile},

	{"loadmod_t", Api_loadmod_t},
	
	{NULL, NULL},
};

static const luaL_Reg ApiMain[] = {
    {"SetHotKey", Api_SetHotKey},

	{"Reset", Api_Reset},
	/*	
	{"reactiontime", Api_reactiontime},
	{"turnframes", Api_turnframes},
	{"numplayers", Api_numplayers},
	{"friction", Api_friction},
	{"engagedistance", Api_engagedistance},
	{"engageheight", Api_engageheight},

	{"gravity", Api_gravity},

	{"engagepos", Api_engagepos},
	{"engagerot", Api_engagerot},
	{"color", Api_color},

	{"mod", Api_mod},
	{"object", Api_object},
	{"objectjoint", Api_objectjoint},

	{"player", Api_player},
	{"body", Api_body},
	{"joint", Api_joint},
	{"shape", Api_shape},
	{"position", Api_position},
	{"orientation", Api_orientation},
	{"sides", Api_sides},
	{"density", Api_density},
	{"static", Api_static},
	{"flag", Api_flag},

	{"radius", Api_radius},
	{"length", Api_length},
	{"strength", Api_strength},
	{"strength_alt", Api_strength_alt},
	{"velocity", Api_velocity},
	{"velocity_alt", Api_velocity_alt},
	{"axis", Api_axis},
	{"axis_alt", Api_axis_alt},
	{"range", Api_range},
	{"range_alt", Api_range_alt},
	{"connections", Api_connections},
	{"connection_type", Api_connection_type},
	*/
	{NULL, NULL},
};

static const char* Events[] = {
    "prototype",

	"NewGame",
	"Freeze",
	"Step",
	"Update",
	"Draw",
	"Draw3D",
	"NearCallback",
	"FileDropped",
	"Console",
};

static const char* shapes[] = {
	"SHAPE_BOX",
	"SHAPE_SPHERE",
	"SHAPE_CAPSULE",
	"SHAPE_CYLINDER",
	"SHAPE_COMPOSITE",
};

static const char* joint_types[] = {
	"JOINT_NULL",

	"JOINT_BALL",
	"JOINT_HINGE",
	"JOINT_SLIDER",

	"JOINT_UNIVERSAL",
	"JOINT_HINGE2",

	"JOINT_FIXED",
	"JOINT_CONTACT",
};

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

			lua_pcall(L, nargs - 1, 0, 0);
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


int luaopenApiBase(lua_State* L)
{
	luaL_register(L, "_G", ApiBase);
	return 1;
}

int callback_table = 0;

int luaopenApiMain(lua_State* L)
{
	luaL_register(L, "Api", ApiMain);

	for (int i = 0; i < EVENT_COUNT; i += 1) {
	    auto Event = Events[i];
		
	    lua_newtable(L);
		lua_newtable(L);

		// closure table
		lua_newtable(L);
		lua_newtable(L);
		int closure_table = lua_ref(L, -2);
		lua_rawseti(L, -2, 1);
		lua_remove(L, -1);

		lua_pushstring(L, Event);
		lua_rawgeti(L, LUA_REGISTRYINDEX, closure_table);
		lua_pushcclosure(L, metamethod_call, TextFormat("Api.%s.__call", Event), 2);
	    lua_setfield(L, -2, "__call");

		lua_pushstring(L, Event);
        lua_rawgeti(L, LUA_REGISTRYINDEX, closure_table);
		lua_pushcclosure(L, metamethod_index, TextFormat("Api.%s.__index", Event), 2);
		lua_setfield(L, -2, "__index");

		lua_pushstring(L, Event);
		lua_rawgeti(L, LUA_REGISTRYINDEX, closure_table);
		lua_pushcclosure(L, metamethod_newindex, TextFormat("Api.%s.__newindex", Event), 2);
        lua_setfield(L, -2, "__newindex");
      
		EventList[i] = (CallbackEvent)lua_ref(L, -2);

		lua_setmetatable(L, -2);
	    lua_setfield(L, -2, Event);
	}
	
	
	int count;
	count = 0;
	for (auto shape : shapes) {
		lua_pushinteger(L, count);
		lua_setfield(L, -2, shape);
		count += 1;
	}
	count = 0;
	for (auto joint : joint_types) {
		lua_pushinteger(L, count);
		lua_setfield(L, -2, joint);
		count += 1;
	}
	
	return 1;
}

static const luaL_Reg Api_libs[] = {
    {"",            luaopenApiBase},
	{"Api",         luaopenApiMain},
	
	{"Game",        luaopenApiGame},
	{"Replay",      luaopenApiReplay},
	{"Expermental", luaopenApiExpermental},
	{"Net",         luaopenApiNet},
	
	{"RAYLIB",  luaopenApiRaylib},
	{"RAYGUI",  luaopenApiRaygui},
	{"RAYMATH", luaopenApiRaymath},

	{NULL, NULL},
};

void luaopenApi(lua_State* L)
{
    for (const luaL_Reg* lib = Api_libs; lib->func; lib += 1) {
        lua_pushcfunction(L, lib->func, NULL);
        lua_pushstring(L, lib->name);
        lua_call(L, 1, 0);
    }
}
