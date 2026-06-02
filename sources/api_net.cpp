#include "api.h"
#include "luau.h"
#ifndef OFFLINE
#include "netcode_server.h"
#include "netcode_client.h"

static int Net_ClientReady(lua_State* L)
{
	Client::Ready();
	return 1;
}

static int Net_ClientWhisper(lua_State* L)
{
	Client::Whisper(lua_tointeger(L, -2) ,lua_tostring(L, -1));
	return 1;
}

static int Net_ClientEcho(lua_State* L)
{
	Client::Echo(lua_tostring(L, -1));
	return 1;
}
#endif
static const luaL_Reg api_net[] = {
#ifndef OFFLINE
	{"ClientReady", Net_ClientReady},
	{"ClientWhisper", Net_ClientWhisper},
	{"ClientEcho", Net_ClientEcho},
#endif
	{NULL, NULL},
};

int luaopenApiNet(lua_State* L)
{
#ifndef OFFLINE
	luaL_register(L, "Net", api_net);
#endif
	return 1;

}
