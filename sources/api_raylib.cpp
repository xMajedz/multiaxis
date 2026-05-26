#include "common.h"
#include "api.h"

using namespace raylib;

static int RAYLIB_DrawText(lua_State* L)
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

	int fontSize = lua_tointeger(L, -6);
	int posY = lua_tointeger(L, -7);
	int posX = lua_tointeger(L, -8);
	const char* text = lua_tostring(L, -9); 	
	DrawText(text, posX, posY, fontSize, color);

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int RAYLIB_DrawRectangle(lua_State* L)
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

	int height = lua_tointeger(L, -6);
	int width = lua_tointeger(L, -7);
	int posY = lua_tointeger(L, -8);
	int posX = lua_tointeger(L, -9);
	DrawRectangle(posX, posY, width, height, color);

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}

static int RAYLIB_DrawRectangleLines(lua_State* L)
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

	int height = lua_tointeger(L, -6);
	int width = lua_tointeger(L, -7);
	int posY = lua_tointeger(L, -8);
	int posX = lua_tointeger(L, -9);
	DrawRectangleLines(posX, posY, width, height, color);

	lua_Number result = 1;
	lua_pushnumber(L, result);
	return 1;
}
static int RAYLIB_LoadFileText(lua_State* L)
{
	const char* filename = lua_tostring(L, -1);
	char* text = LoadFileText(filename);
	lua_pushstring(L, text);
	UnloadFileText(text);
	return 1;
}

static int RAYLIB_LoadDirectoryFilesEx(lua_State* L)
{
	const char* path = lua_tostring(L, -1);
	const char* filter = lua_tostring(L, -2);
	bool scanSubdirs = lua_toboolean(L, -3);
	FilePathList list = LoadDirectoryFilesEx(path, filter, scanSubdirs == true);
	lua_newtable(L);
	for (int i = 0; i < list.count; ++i) {
		lua_pushstring(L, list.paths[i]);
		lua_rawseti(L, -2, i + 1);
	}
	UnloadDirectoryFiles(list);
	return 1;
}


static int RAYLIB_LoadDirectoryFiles(lua_State* L)
{
	const char* path = lua_tostring(L, -1);
	FilePathList list = LoadDirectoryFiles(path);
	lua_newtable(L);
	for (int i = 0; i < list.count; ++i) {
		lua_pushstring(L, list.paths[i]);
		lua_rawseti(L, -2, i + 1);
	}
	UnloadDirectoryFiles(list);
	return 1;
}

static int RAYLIB_GetMousePosition(lua_State* L)
{
	Vector2 MousePosition = GetMousePosition();
	lua_newtable(L);
	lua_pushnumber(L, MousePosition.x);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L, MousePosition.y);
	lua_setfield(L, -2, "y");
	return 1;
}

static int RAYLIB_GetMouseWheelMove(lua_State* L)
{
	lua_pushnumber(L, GetMouseWheelMove());
    return 1;
}

static int RAYLIB_GetTime(lua_State* L)
{
	lua_pushnumber(L, GetTime());
	return 1;
}

static int RAYLIB_GetFrameTime(lua_State* L)
{
	lua_pushnumber(L, GetFrameTime());
	return 1;
}

static int RAYLIB_GetFPS(lua_State* L)
{
	lua_pushnumber(L, GetFPS());
	return 1;
}

static int RAYLIB_IsMouseButtonPressed(lua_State* L)
{
	lua_pushboolean(L, IsMouseButtonPressed(lua_tointeger(L, -1)));
	return 1;
}

static int RAYLIB_IsKeyPressed(lua_State* L)
{
	lua_pushboolean(L, IsKeyPressed(lua_tointeger(L, -1)));
	return 1;
}

static int RAYLIB_IsKeyDown(lua_State* L)
{
	lua_pushboolean(L, IsKeyDown(lua_tointeger(L, -1)));
	return 1;
}

static int RAYLIB_IsKeyUp(lua_State* L)
{
	lua_pushboolean(L, IsKeyUp(lua_tointeger(L, -1)));
	return 1;
}

static int RAYLIB_GetScreenWidth(lua_State* L)
{
	lua_pushinteger(L, GetScreenWidth());
	return 1;
}

static int RAYLIB_GetScreenHeight(lua_State* L)
{
	lua_pushinteger(L, GetScreenHeight());
	return 1;
}

static int RAYLIB_ToggleFullscreen(lua_State* L)
{
	ToggleFullscreen();
	lua_pushinteger(L, 1);
	return 1;
}

static int RAYLIB_SetWindowSize(lua_State* L)
{
	auto h = lua_tointeger(L, -1);
	auto w = lua_tointeger(L, -2);
	SetWindowSize(w, h);
	lua_pushinteger(L, 1);
	return 1;
}

static int RAYLIB_IsWindowFullscreen(lua_State* L)
{
	lua_pushboolean(L, IsWindowFullscreen());
	return 1;
}

static const luaL_Reg api_raylib[] {
	{"DrawText", RAYLIB_DrawText},
	{"DrawRectangle", RAYLIB_DrawRectangle},
	{"DrawRectangleLines", RAYLIB_DrawRectangleLines},
	{"LoadFileText", RAYLIB_LoadFileText},
	{"LoadDirectoryFilesEx", RAYLIB_LoadDirectoryFilesEx},
	{"LoadDirectoryFiles", RAYLIB_LoadDirectoryFiles},
	{"GetMousePosition", RAYLIB_GetMousePosition},
    {"GetMouseWheelMove", RAYLIB_GetMouseWheelMove},
	{"GetTime", RAYLIB_GetTime},
	{"GetFrameTime", RAYLIB_GetFrameTime},
	{"GetFPS", RAYLIB_GetFPS},

	{"IsMouseButtonPressed", RAYLIB_IsMouseButtonPressed},

	{"IsKeyPressed", RAYLIB_IsKeyPressed},
	{"IsKeyDown", RAYLIB_IsKeyDown},
	{"IsKeyUp", RAYLIB_IsKeyUp},

	{"SetWindowSize", RAYLIB_SetWindowSize},
	
	{"GetScreenWidth", RAYLIB_GetScreenWidth},
	{"GetScreenHeight", RAYLIB_GetScreenHeight},

	{"ToggleFullscreen", RAYLIB_ToggleFullscreen},
	{"IsWindowFullscreen", RAYLIB_IsWindowFullscreen},

	{NULL, NULL},
};

struct RaylibColor
{
	const char* name;
	Color color;
};

static RaylibColor raylib_colors[] = {
	{"LIGHTGRAY",LIGHTGRAY},
	{"GRAY",GRAY},
	{"DARKGRAY",DARKGRAY},
	{"YELLOW",YELLOW},
	{"GOLD",GOLD},
	{"ORANGE",ORANGE},
	{"PINK",PINK},
	{"RED",RED},
	{"MAROON",MAROON},
	{"GREEN",GREEN},
	{"LIME",LIME},
	{"DARKGREEN",DARKGREEN},
	{"SKYBLUE",SKYBLUE},
	{"BLUE",BLUE},
	{"DARKBLUE",DARKBLUE},
	{"PURPLE",PURPLE},
	{"VIOLET",VIOLET},
	{"DARKPURPLE",DARKPURPLE},
	{"BEIGE",BEIGE},
	{"BROWN",BROWN},
	{"DARKBROWN",DARKBROWN},
	{"WHITE",WHITE},
	{"BLACK",BLACK},
	{"BLANK",BLANK},
	{"MAGENTA",MAGENTA},
	{"RAYWHITE",RAYWHITE},
};

struct RaylibMouseBtn
{
	const char* btn;
	int constant;
};

static RaylibMouseBtn raylib_mousebtns[] = {
	{"MOUSE_BUTTON_LEFT",0},
	{"MOUSE_BUTTON_RIGHT",1},
	{"MOUSE_BUTTON_MIDDLE",2},
	{"MOUSE_BUTTON_SIDE",3},
	{"MOUSE_BUTTON_EXTRA",4},
	{"MOUSE_BUTTON_FORWARD",5},
	{"MOUSE_BUTTON_BACK",6},
};

struct RaylibKey
{
	const char* key;
	int constant;
};

static RaylibKey raylib_keys[] = {
	{"KEY_NULL",0},

	{"KEY_APOSTROPHE",39},
	{"KEY_COMMA",44},
	{"KEY_MINUS",45},
	{"KEY_PERIOD",46},
	{"KEY_SLASH",47},
	{"KEY_ZERO",48},
	{"KEY_ONE",49},
	{"KEY_TWO",50},
	{"KEY_THREE",51},
	{"KEY_FOUR",52},
	{"KEY_FIVE",53},
	{"KEY_SIX",54},
	{"KEY_SEVEN",55},
	{"KEY_EIGHT",56},
	{"KEY_NINE",57},
	{"KEY_SEMICOLON",59},
	{"KEY_EQUAL",61},
	{"KEY_A",65},
	{"KEY_B",66},
	{"KEY_C",67},
	{"KEY_D",68},
	{"KEY_E",69},
	{"KEY_F",70},
	{"KEY_G",71},
	{"KEY_H",72},
	{"KEY_I",73},
	{"KEY_J",74},
	{"KEY_K",75},
	{"KEY_L",76},
	{"KEY_M",77},
	{"KEY_N",78},
	{"KEY_O",79},
	{"KEY_P",80},
	{"KEY_Q",81},
	{"KEY_R",82},
	{"KEY_S",83},
	{"KEY_T",84},
	{"KEY_U",85},
	{"KEY_V",86},
	{"KEY_W",87},
	{"KEY_X",88},
	{"KEY_Y",89},
	{"KEY_Z",90},
	{"KEY_LEFT_BRACKET",91},
	{"KEY_BACKSLASH",92},
	{"KEY_RIGHT_BRACKET",93},
	{"KEY_GRAVE",96},

	{"KEY_SPACE",32},
	{"KEY_ESCAPE",256},
	{"KEY_ENTER",257},
	{"KEY_TAB",258},
	{"KEY_BACKSPACE",259},
	{"KEY_INSERT",260},
	{"KEY_DELETE",261},
	{"KEY_RIGHT",262},
	{"KEY_LEFT",263},
	{"KEY_DOWN",264},
	{"KEY_UP",265},
	{"KEY_PAGE_UP",266},
	{"KEY_PAGE_DOWN",267},
	{"KEY_HOME",268},
	{"KEY_END",269},
	{"KEY_CAPS_LOCK",280},
	{"KEY_SCROLL_LOCK",281},
	{"KEY_NUM_LOCK",282},
	{"KEY_PRINT_SCREEN",283},
	{"KEY_PAUSE",284},
	{"KEY_F1",290},
	{"KEY_F2",291},
	{"KEY_F3",292},
	{"KEY_F4",293},
	{"KEY_F5",294},
	{"KEY_F6",295},
	{"KEY_F7",296},
	{"KEY_F8",297},
	{"KEY_F9",298},
	{"KEY_F10",299},
	{"KEY_F11",300},
	{"KEY_F12",301},
	{"KEY_LEFT_SHIFT",340},
	{"KEY_LEFT_CONTROL",341},
	{"KEY_LEFT_ALT",342},
	{"KEY_LEFT_SUPER",343},
	{"KEY_RIGHT_SHIFT",344},
	{"KEY_RIGHT_CONTROL",345},
	{"KEY_RIGHT_ALT",346},
	{"KEY_RIGHT_SUPER",347},
	{"KEY_KB_MENU",348},

	{"KEY_KP_0",320},
	{"KEY_KP_1",321},
	{"KEY_KP_2",322},
	{"KEY_KP_3",323},
	{"KEY_KP_4",324},
	{"KEY_KP_5",325},
	{"KEY_KP_6",326},
	{"KEY_KP_7",327},
	{"KEY_KP_8",328},
	{"KEY_KP_9",329},
	{"KEY_KP_DECIMAL",330},
	{"KEY_KP_DIVIDE",331},
	{"KEY_KP_MULTIPLY",332},
	{"KEY_KP_SUBTRACT",333},
	{"KEY_KP_ADD",334},
	{"KEY_KP_ENTER",335},
	{"KEY_KP_EQUAL",336},

	{"KEY_BACK",4},
	{"KEY_MENU",5},
	{"KEY_VOLUME_UP",24},
	{"KEY_VOLUME_DOWN",25},
};

int luaopen_api_raylib(lua_State* L)
{
	luaL_register(L, "RAYLIB", api_raylib);

	lua_getglobal(L, "RAYLIB");

	lua_pushnumber(L, RAYLIB_VERSION_MAJOR);
	lua_setfield(L, -2, "VERSION_MAJOR");
	lua_pushnumber(L, RAYLIB_VERSION_MINOR);
	lua_setfield(L, -2, "VERSION_MINOR");
	lua_pushnumber(L, RAYLIB_VERSION_PATCH);
	lua_setfield(L, -2, "VERSION_PATCH");
	lua_pushstring(L, RAYLIB_VERSION);
	lua_setfield(L, -2, "VERSION");
	lua_pushnumber(L, PI);
	lua_setfield(L, -2, "PI");
	lua_pushnumber(L, DEG2RAD);
	lua_setfield(L, -2, "DEG2RAD");
	lua_pushnumber(L, RAD2DEG);
	lua_setfield(L, -2, "RAD2DEG");

	for (auto& [name, col] : raylib_colors) {
		lua_newtable(L);
		lua_pushinteger(L, col.r);
		lua_rawseti(L, -2, 1);
		lua_pushinteger(L, col.g);
		lua_rawseti(L, -2, 2);
		lua_pushinteger(L, col.b);
		lua_rawseti(L, -2, 3);
		lua_pushinteger(L, col.a);
		lua_rawseti(L, -2, 4);
		lua_setfield(L, -2, name);
	}

	for (auto& [key, constant] : raylib_keys) {
		lua_pushinteger(L, constant);
		lua_setfield(L, -2, key);
	}

	for (auto& [btn, constant] : raylib_mousebtns) {
		lua_pushinteger(L, constant);
		lua_setfield(L, -2, btn);
	}

	lua_pop(L, 1);
	return 1;
}
