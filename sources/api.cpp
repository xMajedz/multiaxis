#include "api.h"

using namespace raylib;

enum Context
{
	NoContext = 0,

	ObjectContext,
	ObjectJointContext,

	PlayerContext,
	BodyContext,
	JointContext,

} DataContext = NoContext;

static void log_luau (const char* msg)
{
	Console::log(msg);
}

static void log_ode(int errnum, const char* msg, va_list ap)
{
	Console::log(TextFormat("%d: %s", errnum, msg));
}

void API::Init()
{
	L = luaL_newstate();
	luaopen_api_main(L);
	luaopen_api_game(L);
	luaopen_api_net(L);
	luaopen_api_replay(L);
	luaopen_api_raylib(L);
	luaopen_api_raygui(L);
	luaopen_api_raymath(L);
	luaopen_api_expermental(L);
	luaL_openlibs(L);
	luaL_sandbox(L);

	data = new Arena(1024*1024);

	Luau::setlogcallback(log_luau);

	dSetErrorHandler(log_ode);
	dSetDebugHandler(log_ode);
	dSetMessageHandler(log_ode);
}

void API::Reset()
{
	DataContext = NoContext;

	o_vector.clear();

	oj_vector.clear();

	p_vector.clear();
	
	o = nullptr;
	oj = nullptr;

	p = nullptr;

	o_count = 0;
	oj_count = 0;

	p_count = 0;

	b_vector.clear();
	j_vector.clear();

	b = nullptr;
	j = nullptr;

	b_count = 0;
	j_count = 0;
}

void API::Close()
{
	lua_close(L);
	data->clear();
	delete data;
}

lua_State* API::GetL()
{
	return L;
}

Gamerules API::GetRules()
{
	return rules;
}


std::vector<Body> API::GetObjects()
{
	return o_vector;
}

std::vector<Joint> API::GetJointObjects()
{
	return oj_vector;
}

std::vector<Player> API::GetPlayers()
{
	return p_vector;
}

size_t API::GetObjectsCount()
{
	return o_count;
}

size_t API::GetJointObjectsCount()
{
	return oj_count;
}

size_t API::GetPlayersCount()
{
	return p_count;
}

int API::DrawCallback()
{
	return Luau::dostring(L, TextFormat("for _, fn in _G[\"API\"][\"%s\"] do fn() end", "Draw"));
}

int API::NewGameCallback()
{
	return Luau::dostring(L, TextFormat("for _, fn in _G[\"API\"][\"%s\"] do fn() end", "NewGame"));
}

int API::FreezeCallback()
{
	return Luau::dostring(L, TextFormat("for _, fn in _G[\"API\"][\"%s\"] do fn() end", "Freeze"));
}

int API::StepCallback()
{
	return Luau::dostring(L, TextFormat("for _, fn in _G[\"API\"][\"%s\"] do fn() end", "Step"));
}

int API::UpdateCallback(dReal dt)
{
	return Luau::dostring(L, TextFormat("for _, fn in _G[\"API\"][\"%s\"] do fn(%lf) end", "Update", dt));
}

int API::ConsoleCallback(const char* message)
{
	return Luau::dostring(L, TextFormat("for _, fn in _G[\"API\"][\"%s\"] do fn(\"%s\") end", "Console", message));
}

int API::loadmod(std::string_view modpath)
{
	return Luau::dofile(
		L,
		TextFormat("./mods/%s", modpath.data()),
		TextFormat("%s:%s", "loadmod", modpath.data())
	);
}

int API::loadscript(std::string_view scriptpath)
{
	return Luau::dofile(
		L,
		TextFormat("./scripts/%s", scriptpath.data()),
		TextFormat("%s:%s", "loadscript", scriptpath.data())
	);
}

static int API_reactiontime(lua_State* L)
{
	lua_rawgeti(L, -1, 1);
	lua_Number reactiontime = lua_tonumber(L, -1);
	switch(DataContext) {
		case NoContext: {
			API::rules.reaction_time = reactiontime;
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

static int API_turnframes(lua_State* L)
{
	lua_rawgeti(L, -1, 1);
	lua_Number turnframes = lua_tonumber(L, -1);

	switch(DataContext) {
		case NoContext: {
			API::rules.turnframes = turnframes;
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

static int API_numplayers(lua_State* L)
{
	lua_rawgeti(L, -1, 1);
	lua_Number numplayers = lua_tonumber(L, -1); 

	switch(DataContext) {
		case NoContext: {
			API::rules.numplayers = numplayers;
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

static int API_friction(lua_State* L)
{
	lua_rawgeti(L, -1, 1);
	lua_Number friction = lua_tonumber(L, -1);

	switch(DataContext) {
		case NoContext: {
			API::rules.friction = friction;
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

static int API_engagedistance(lua_State* L)
{
	lua_rawgeti(L, -1, 1);
	lua_Number distance = lua_tonumber(L, -1);

	switch(DataContext) {
		case NoContext: {
			API::rules.engagedistance = distance;
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

static int API_engageheight(lua_State* L)
{
	lua_rawgeti(L, -1, 1);
	lua_Number height = lua_tonumber(L, -1);

	switch(DataContext) {
		case NoContext: {
			API::rules.engageheight = height;
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

static int API_engagepos(lua_State* L)
{
	vec3 pos;
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
			API::p->use_engagepos = true;
			API::p->engagepos = pos;
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

static int API_engagerot(lua_State* L)
{
	vec3 rot;
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
			API::p->use_engagerot = true;
			API::p->engagerot = rot;
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

static int API_color(lua_State* L)
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
			API::p->m_color = color;
		} break;
		case ObjectContext: {
			API::o->m_color = color;
		} break;
		case BodyContext: {
			API::b->m_color = color;
		} break;
		case JointContext: {
			API::j->m_color = color;
		} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int API_gravity(lua_State* L)
{
	vec3 gravity;
	lua_rawgeti(L, -1, 1);
	gravity.x = lua_tonumber(L, -1); 
	lua_rawgeti(L, -2, 2);
	gravity.y = lua_tonumber(L, -1); 
	lua_rawgeti(L, -3, 3);
	gravity.z = lua_tonumber(L, -1); 

	switch(DataContext) {
		case NoContext: {
			API::rules.gravity = gravity;
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

static int API_mod(lua_State* L)
{
	API::rules.mod = lua_tostring(L, -1);
	lua_pushinteger(L, 1);
	return 1;
}

static int API_object(lua_State* L)
{
	DataContext = ObjectContext;
	std::string_view name = lua_tostring(L, -1);
	Body o(API::o_count, name.data());
	API::o_map[name] = API::o_count;
	API::o_vector.push_back(o);
	API::o = &API::o_vector[API::o_count];
	API::o_count += 1;

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int API_objectjoint(lua_State* L)
{
	DataContext = ObjectJointContext;
	Joint oj(API::oj_count, lua_tostring(L, -1));
	API::oj_vector.push_back(oj);
	API::oj = &API::oj_vector[API::oj_count];
	API::oj_count += 1;

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int API_player(lua_State* L)
{
	DataContext = PlayerContext;
	const char* name = lua_tostring(L, -1);
	lua_Number result = 0;
	if (API::p_count < API::rules.numplayers) {
		API::b_count = 0;
		API::j_count = 0;

		Player p(API::p_count, name);
		API::p_vector.push_back(p);
		API::p = &API::p_vector[API::p_count];
		API::p_count += 1;
		result = 1;
	}
	lua_pushnumber(L, result);
	return 1;
}

static int API_body(lua_State* L)
{
	DataContext = BodyContext;
	std::string_view name = lua_tostring(L, -1);
	Body b(API::b_count, name.data());
	API::b_map[name] = API::b_count;
	API::p->body.push_back(b);
	API::b = &API::p->body[API::b_count];
	API::b_count += 1;

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int API_joint(lua_State* L)
{
	DataContext = JointContext;
	Joint j(API::j_count, lua_tostring(L, -1));
	API::p->joint.push_back(j);
	API::j = &API::p->joint[API::j_count];
	API::j_count += 1;

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int API_shape(lua_State* L)
{
	lua_Integer shape = lua_tointeger(L, -1);

	switch(DataContext) {
	case ObjectContext: {
		API::o->shape = (BodyShape)shape;
	} break;
	case ObjectJointContext: {
		API::oj->shape = (BodyShape)shape;
	} break;
	case BodyContext: {
		API::b->shape = (BodyShape)shape;
	} break;
	case JointContext: {
		API::j->shape = (BodyShape)shape;
	} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int API_position(lua_State* L)
{
	vec3 position;
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
		API::o->m_position = position;
	} break;
	case ObjectJointContext: {
		API::oj->m_position = position;
	} break;
	case BodyContext: {
		API::b->m_position = position;
	} break;
	case JointContext: {
		API::j->m_position = position;
	} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int API_orientation(lua_State* L)
{
	vec4 orientation;
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
			API::o->m_orientation = orientation;

		} break;
		case BodyContext: {
			API::b->m_orientation = orientation;
		} break;
		case JointContext: {
			API::j->m_orientation = orientation;
		} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int API_sides(lua_State* L)
{
	vec3 sides;
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
			API::o->m_sides = sides;
		} break;
		case BodyContext: {
			API::b->m_sides = sides;
		} break;
		case JointContext: {
			API::j->m_sides = sides;
		} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int API_density(lua_State* L)
{
	lua_rawgeti(L, -1, 1);
	lua_Number density = lua_tonumber(L, -1); 

	switch(DataContext) {
	case NoContext: {
		// Error Handling
	} break;
	case ObjectContext: {
		API::o->density = density;
	} break;
	case ObjectJointContext: {
		API::oj->density = density;
	} break;
	case BodyContext: {
		API::b->density = density;
	} break;
	case JointContext: {
		API::j->density = density;
	} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int API_static(lua_State* L)
{
	switch(DataContext) {
		case NoContext: {
			// Error Handling
		} break;
		case ObjectContext: {
			API::o->m_static = true;
		} break;
		case BodyContext: {
			API::b->m_static = true;
		} break;
		case JointContext: {
			API::j->m_static = true;
		} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int API_flag(lua_State* L)
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
		API::o->m_static = flag_static;
		API::o->m_composite = flag_composite;
		API::o->m_interactive = flag_interactive;
	} break;
	case ObjectJointContext: {
		API::oj->m_static = flag_static;
		API::oj->m_composite = flag_composite;
		API::oj->m_interactive = flag_interactive;
	} break;
	case BodyContext: {
		API::b->m_static = flag_static;
		API::b->m_composite = flag_composite;
		API::b->m_interactive = flag_interactive;
	} break;
	case JointContext: {
		API::j->m_static = flag_static;
		API::j->m_composite = flag_composite;
		API::j->m_interactive = flag_interactive;
	} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int API_radius(lua_State* L)
{
	lua_rawgeti(L, -1, 1);
	lua_Number radius = lua_tonumber(L, -1); 

	switch(DataContext) {
	case NoContext: {
		// Error Handling
	} break;
	case ObjectContext: {
		API::o->radius = radius;
	} break;
	case ObjectJointContext: {
		API::oj->radius = radius;
	} break;
	case BodyContext: {
		API::b->radius = radius;
	} break;
	case JointContext: {
		API::j->radius = radius;
	} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int API_length(lua_State* L)
{
	lua_rawgeti(L, -1, 1);
	lua_Number length = lua_tonumber(L, -1); 

	switch(DataContext) {
		case NoContext: {
			// Error Handling
		} break;
		case ObjectContext: {
			API::o->length = length;
		} break;
		case BodyContext: {
			API::b->length = length;
		} break;
		case JointContext: {
			API::j->length = length;
		} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int API_strength(lua_State* L)
{
	lua_Number strength;
	lua_rawgeti(L, -1, 1);
	strength = lua_tonumber(L, -1); 

	switch(DataContext) {
	case ObjectJointContext: {
		API::oj->strength = strength;
	} break;
	case JointContext: {
		API::j->strength = strength;
	} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int API_strength_alt(lua_State* L)
{
	lua_Number strength_alt;
	lua_rawgeti(L, -1, 1);
	strength_alt = lua_tonumber(L, -1); 

	switch(DataContext) {
	case ObjectJointContext: {
		API::oj->strength_alt = strength_alt;
	} break;
	case JointContext: {
		API::j->strength_alt = strength_alt;
	} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}


static int API_velocity(lua_State* L)
{
	lua_Number velocity;
	lua_rawgeti(L, -1, 1);
	velocity = lua_tonumber(L, -1); 

	switch(DataContext) {
	case ObjectJointContext: {
		API::oj->velocity = velocity;
	} break;
	case JointContext: {
		API::j->velocity = velocity;
	} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int API_velocity_alt(lua_State* L)
{
	lua_Number velocity_alt;
	lua_rawgeti(L, -1, 1);
	velocity_alt = lua_tonumber(L, -1); 

	switch(DataContext) {
	case ObjectJointContext: {
		API::oj->velocity_alt = velocity_alt;
	} break;
	case JointContext: {
		API::j->velocity_alt = velocity_alt;
	} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int API_axis(lua_State* L)
{
	vec3 axis;
	lua_rawgeti(L, -1, 1);
	axis.x = lua_tonumber(L, -1); 
	lua_rawgeti(L, -2, 2);
	axis.y = lua_tonumber(L, -1); 
	lua_rawgeti(L, -3, 3);
	axis.z = lua_tonumber(L, -1); 

	switch(DataContext) {
	case ObjectJointContext: {
		API::oj->axis = axis;
	} break;
	case JointContext: {
		API::j->axis = axis;
	} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int API_axis_alt(lua_State* L)
{
	vec3 axis_alt;
	lua_rawgeti(L, -1, 1);
	axis_alt.x = lua_tonumber(L, -1); 
	lua_rawgeti(L, -2, 2);
	axis_alt.y = lua_tonumber(L, -1); 
	lua_rawgeti(L, -3, 3);
	axis_alt.z = lua_tonumber(L, -1); 

	switch(DataContext) {
	case ObjectJointContext: {
		API::oj->axis_alt = axis_alt;
	} break;
	case JointContext: {
		API::j->axis_alt = axis_alt;
	} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int API_range(lua_State* L)
{
	lua_Number range[2];
		
	lua_rawgeti(L, -1, 1);
	range[0] = lua_tonumber(L, -1); 
	lua_rawgeti(L, -2, 2);
	range[1] = lua_tonumber(L, -1); 

	switch(DataContext) {
	case ObjectJointContext: {
		API::oj->range[0] = range[0];
		API::oj->range[1] = range[1];
	} break;
	case JointContext: {
		API::j->range[0] = range[0];
		API::j->range[1] = range[1];
	} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int API_range_alt(lua_State* L)
{
	lua_Number range_alt[2];
	lua_rawgeti(L, -1, 1);
	range_alt[0] = lua_tonumber(L, -1); 
	lua_rawgeti(L, -2, 2);
	range_alt[1] = lua_tonumber(L, -1); 

	switch(DataContext) {
	case ObjectJointContext: {
		API::oj->range_alt[0] = range_alt[0];
		API::oj->range_alt[1] = range_alt[1];
	} break;
	case JointContext: {
		API::j->range_alt[0] = range_alt[0];
		API::j->range_alt[1] = range_alt[1];
	} break;
	}
	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int API_connections(lua_State* L)
{
	std::string_view connections[2];
		
	lua_rawgeti(L, -1, 1);
	connections[0] = lua_tostring(L, -1); 
	lua_rawgeti(L, -2, 2);
	connections[1] = lua_tostring(L, -1); 

	switch(DataContext) {
	case ObjectJointContext: {
		API::oj->connections[0] = API::o_map[connections[0]];
		API::oj->connections[1] = API::o_map[connections[1]];
	} break;
	case JointContext: {
		API::j->connections[0] = API::b_map[connections[0]];
		API::j->connections[1] = API::b_map[connections[1]];
	} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int API_connection_type(lua_State* L)
{
	lua_Integer type = lua_tointeger(L, -1);

	switch(DataContext) {
	case ObjectJointContext: {
		API::oj->type = (JointType)type;
	} break;
	case JointContext: {
		API::j->type = (JointType)type;
	} break;
	}

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int API_dofile(lua_State* L)
{
	const char* filepath = lua_tostring(L, -1);
	lua_Number result = Luau::dofile(L, filepath);
	lua_gettop(L);
	return result;
}

static int API_require(lua_State* L)
{
	const char* filename = lua_tostring(L, -1);
	lua_Number result = Luau::require(L, filename);
	lua_gettop(L);
	return 1;
}
static int API_loadmod(lua_State* L)
{
	const char* modpath = lua_tostring(L, -1);
	lua_Number result = API::loadmod(modpath);
	lua_pushnumber(L, result);
	return 1;
}

static int API_loadscript(lua_State* L)
{
	std::string_view  scriptpath = lua_tostring(L, -1);
	lua_Number result = API::loadscript(scriptpath);
	lua_pushnumber(L, result);
	return 1;
}

void Console::SetCallback(void(*callback)(const char*))
{
	if (callback != nullptr) {
		m_callback = callback;
	}
}

void Console::log(const char* message)
{
	SetMessage(message);
}

void Console::Update()
{
	for (int i = 0; message_count != 0; i += 1) {
		API::ConsoleCallback(messages[i]);
		message_count -= 1;
	}
}

void Console::SetMessage(const char* message)
{
	for (int i = 0; i < message_length; i += 1) {
		if (message[i] != '\0') {
			last_message[i] = message[i];
		} else {
			last_message[i] = '\0';
		}
	}

	if (message_count < 1024) {
		messages[message_count] = last_message;
		message_count += 1;
	} else {
		messages[message_count - 1] = last_message;
	}
}

static int API_log(lua_State* L)
{
	Console::log(lua_tostring(L, -1));
	lua_pushinteger(L, 1);
	return 1;
}

static int API_Reset(lua_State* L)
{
	API::Reset();
	lua_pushinteger(L, 1);
	return 1;
}

static int API_loadmod_t(lua_State* L)
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

	return 1;
}

static int API_CreateData(lua_State* L)
{
	lua_pushlightuserdata(L, (void*)API::data->allocate(lua_tointeger(L, -1)));
	return 1;
}

static int API_StringToData(lua_State* L)
{
	char* data = (char*)lua_touserdata(L, -1);
	const char* string = lua_tostring(L, -2);
	TextCopy(data, string);
	return 1;
}

static int API_NumberToData(lua_State* L)
{
	auto data = lua_touserdata(L, -1);
	lua_Number number = lua_tonumber(L, -2);
	*((float*)data) = (float)number;
	return 1;
}

static int API_ByteToData(lua_State* L)
{
	auto data = lua_touserdata(L, -1);
	bool byte = lua_toboolean(L, -2);
	*((bool*)data) = (bool)byte;
	return 1;
}

static int API_StringFromData(lua_State* L)
{
	lua_pushstring(L, (char*)lua_touserdata(L, -1));
	return 1;
}

static int API_NumberFromData(lua_State* L)
{
	lua_pushnumber(L, *((float*)lua_touserdata(L, -1)));
	return 1;
}

static int API_ByteFromData(lua_State* L)
{
	lua_pushboolean(L, *((bool*)lua_touserdata(L, -1)));
	return 1;
}

static const luaL_Reg api_main[] {
	{"CreateData", API_CreateData},
	{"StringToData", API_StringToData},
	{"NumberToData", API_NumberToData},
	{"ByteToData", API_ByteToData},
	{"StringFromData", API_StringFromData},
	{"NumberFromData", API_NumberFromData},
	{"ByteFromData", API_ByteFromData},

	{"log", API_log},
	{"Reset", API_Reset},

	{"dofile", API_dofile},
	{"require", API_require},

	{"loadscript", API_loadscript},
	{"loadmod", API_loadmod},
	{"loadmod_t", API_loadmod_t},
	
	{"reactiontime", API_reactiontime},
	{"turnframes", API_turnframes},
	{"numplayers", API_numplayers},
	{"friction", API_friction},
	{"engagedistance", API_engagedistance},
	{"engageheight", API_engageheight},

	{"gravity", API_gravity},

	{"engagepos", API_engagepos},
	{"engagerot", API_engagerot},
	{"color", API_color},

	{"mod", API_mod},
	{"object", API_object},
	{"objectjoint", API_objectjoint},

	{"player", API_player},
	{"body", API_body},
	{"joint", API_joint},
	{"shape", API_shape},
	{"position", API_position},
	{"orientation", API_orientation},
	{"sides", API_sides},
	{"density", API_density},
	{"static", API_static},
	{"flag", API_flag},

	{"radius", API_radius},
	{"length", API_length},
	{"strength", API_strength},
	{"strength_alt", API_strength_alt},
	{"velocity", API_velocity},
	{"velocity_alt", API_velocity_alt},
	{"axis", API_axis},
	{"axis_alt", API_axis_alt},
	{"range", API_range},
	{"range_alt", API_range_alt},
	{"connections", API_connections},
	{"connection_type", API_connection_type},

	{NULL, NULL},
};

static const char* events[] = {
	"NewGame",
	"Freeze",
	"Step",
	"Update",
	"Draw",
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

int luaopen_api_main(lua_State* L) {
	luaL_register(L, "API", api_main);

	lua_getglobal(L, "API");
	for (auto event : events) {
		lua_newtable(L);
		lua_setfield(L, -2, event);
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

	lua_pop(L, 1);
	return 1;
}
