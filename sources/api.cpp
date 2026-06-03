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

void Api::Init()
{
	L = luaL_newstate();
	luaopenApiMain(L);
	luaopenApiGame(L);
	luaopenApiNet(L);
	luaopenApiReplay(L);
	luaopenApiRaylib(L);
	luaopenApiRaygui(L);
	luaopenApiRaymath(L);
	luaopenApiExpermental(L);
	luaL_openlibs(L);
	luaL_sandbox(L);

	Luau::setlogcallback(log_luau);

	dSetErrorHandler(log_ode);
	dSetDebugHandler(log_ode);
	dSetMessageHandler(log_ode);
}

void Api::Boot()
{
    loadscript("boot");
}

void Api::Reset()
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

void Api::Close()
{
	lua_close(L);
}

lua_State* Api::GetL()
{
	return L;
}

Gamerules Api::GetRules()
{
	return rules;
}


std::vector<Body> Api::GetObjects()
{
	return o_vector;
}

std::vector<Joint> Api::GetJointObjects()
{
	return oj_vector;
}

std::vector<Player> Api::GetPlayers()
{
	return p_vector;
}

size_t Api::GetObjectsCount()
{
	return o_count;
}

size_t Api::GetJointObjectsCount()
{
	return oj_count;
}

size_t Api::GetPlayersCount()
{
	return p_count;
}

int Api::DrawCallback()
{
	return Luau::dostring(L, TextFormat("for _, fn in _G[\"Api\"][\"%s\"] do fn() end", "Draw"));
}

int Api::Draw3DCallback()
{
	return Luau::dostring(L, TextFormat("for _, fn in _G[\"Api\"][\"%s\"] do fn() end", "Draw3D"));
}

int Api::NewGameCallback()
{
	return Luau::dostring(L, TextFormat("for _, fn in _G[\"Api\"][\"%s\"] do fn() end", "NewGame"));
}

int Api::FreezeCallback()
{
	return Luau::dostring(L, TextFormat("for _, fn in _G[\"Api\"][\"%s\"] do fn() end", "Freeze"));
}

int Api::StepCallback()
{
	return Luau::dostring(L, TextFormat("for _, fn in _G[\"Api\"][\"%s\"] do fn() end", "Step"));
}

int Api::UpdateCallback(dReal dt)
{
	return Luau::dostring(L, TextFormat("for _, fn in _G[\"Api\"][\"%s\"] do fn(%lf) end", "Update", dt));
}

int Api::ConsoleCallback(const char* message)
{
	return Luau::dostring(L, TextFormat("for _, fn in _G[\"Api\"][\"%s\"] do fn(\"%s\") end", "Console", message));
}

int Api::loadmod(std::string_view modpath)
{
	return Luau::dofile(
		L,
		TextFormat("./mods/%s", modpath.data()),
		TextFormat("%s:%s", "loadmod", modpath.data())
	);
}

int Api::loadscript(std::string_view scriptpath)
{
	return Luau::dofile(
		L,
		TextFormat("./scripts/%s", scriptpath.data()),
		TextFormat("%s:%s", "loadscript", scriptpath.data())
	);
}

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
	vec3 gravity;
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
			Api::o->m_static = true;
		} break;
		case BodyContext: {
			Api::b->m_static = true;
		} break;
		case JointContext: {
			Api::j->m_static = true;
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
		Api::o->m_static = flag_static;
		Api::o->m_composite = flag_composite;
		Api::o->m_interactive = flag_interactive;
	} break;
	case ObjectJointContext: {
		Api::oj->m_static = flag_static;
		Api::oj->m_composite = flag_composite;
		Api::oj->m_interactive = flag_interactive;
	} break;
	case BodyContext: {
		Api::b->m_static = flag_static;
		Api::b->m_composite = flag_composite;
		Api::b->m_interactive = flag_interactive;
	} break;
	case JointContext: {
		Api::j->m_static = flag_static;
		Api::j->m_composite = flag_composite;
		Api::j->m_interactive = flag_interactive;
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
	vec3 axis;
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
	vec3 axis_alt;
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

static int Api_dofile(lua_State* L)
{
	const char* filepath = lua_tostring(L, -1);
	lua_Number result = Luau::dofile(L, filepath);
	lua_gettop(L);
	return result;
}

static int Api_require(lua_State* L)
{
	const char* filename = lua_tostring(L, -1);
	lua_Number result = Luau::require(L, filename);
	lua_gettop(L);
	return 1;
}
static int Api_loadmod(lua_State* L)
{
	const char* modpath = lua_tostring(L, -1);
	lua_Number result = Api::loadmod(modpath);
	lua_pushnumber(L, result);
	return 1;
}

static int Api_loadscript(lua_State* L)
{
	std::string_view  scriptpath = lua_tostring(L, -1);
	lua_Number result = Api::loadscript(scriptpath);
	lua_pushnumber(L, result);
	return 1;
}

void Console::SetCallback(ConsoleCallback_t callback)
{
	if (callback != nullptr) m_callback = callback;
}

void Console::Update()
{
    uint32_t message_buffer_start = 0;
	
	for (int i = 0; message_count != 0; i += 1) {
	    char message[256];
		
		for (int i = 0; i < message_length; i += 1) {
		    message[i] = message_buffer[message_buffer_start * message_length + i];
		}
		
		Api::ConsoleCallback(message);
		
		message_buffer_start += 1;
		message_count -= 1;
	}
}

void Console::log(const char* message)
{	
	messages[message_count] = &message_buffer[message_count * message_length];

	for (int i = 0; i < message_length; i += 1) { 
        if (message[i] == '\0') {
		   message_buffer[message_count * message_length + i] = '\0';
		   break;
		} else if (i > message_length - 1) {
		   message_buffer[message_count * message_length + i] = '\0';
		   break;
		} else {
		   message_buffer[message_count * message_length + i] = message[i];
		}
    }
	
	message_count += 1;
}
	
static int Api_log(lua_State* L)
{
	Console::log(lua_tostring(L, -1));
	lua_pushinteger(L, 1);
	return 1;
}

static int Api_Reset(lua_State* L)
{
	Api::Reset();
	lua_pushinteger(L, 1);
	return 1;
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

	return 1;
}

static const luaL_Reg ApiMain[] {
	{"log", Api_log},
	{"Reset", Api_Reset},

	{"dofile", Api_dofile},
	{"require", Api_require},

	{"loadscript", Api_loadscript},
	{"loadmod", Api_loadmod},
	{"loadmod_t", Api_loadmod_t},
	
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

	{NULL, NULL},
};

static const char* events[] = {
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

int luaopenApiMain(lua_State* L)
{
	luaL_register(L, "Api", ApiMain);

	lua_getglobal(L, "Api");
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
