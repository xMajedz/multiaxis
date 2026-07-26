#include "Game.h"

static int Game_Quit(lua_State* L)
{
    static_cast<Game*>(lua_tolightuserdata(L, lua_upvalueindex(1)))->Quit();
    return 0;
}

static const luaL_Reg ApiGame[] = {
    {"Quit", Game_Quit},
	
    {NULL, NULL},
};

int luaopenGame(lua_State* L)
{
    luaL_registerwithclosure(L, "Game", ApiGame, 1);
    return 1;
}
