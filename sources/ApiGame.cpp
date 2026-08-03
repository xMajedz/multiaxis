#include "Game.h"

#define GameInstance (static_cast<Game*>(lua_tolightuserdata(L, lua_upvalueindex(1))))

static int Game_Quit(lua_State* L)
{
    GameInstance->Quit();
    return 0;
}

static int Game_LoadModText(lua_State* L)
{
    if (lua_isstring(L, 1))
        GameInstance->LoadModText(lua_tostring(L, 1));

    return 0;
}

static int Game_LoadModFile(lua_State* L)
{
    if (lua_isstring(L, 1))
        GameInstance->LoadModFile(lua_tostring(L, 1));

    return 0;
}

static int Game_NewGame(lua_State* L)
{
    GameInstance->NewGame();
    return 0;
}

static const luaL_Reg ApiGame[] = {
    {"Quit", Game_Quit},
    
    {"LoadModText", Game_LoadModText},
    {"LoadModFile", Game_LoadModFile},

    {"NewGame", Game_NewGame},
	
    {NULL, NULL},
};

int luaopenGame(lua_State* L)
{
    luaL_registerwithclosure(L, "Game", ApiGame, 1);
    return 1;
}
