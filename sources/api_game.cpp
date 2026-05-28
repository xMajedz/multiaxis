#include "api.h"
#include "game.h"

static int Game_SetMode(lua_State* L)
{
    Game::SetMode((Gamemode)lua_tointeger(L, -1));
    return 1;
}

static int Game_SetBackgroundColor(lua_State* L)
{
    Game::SetBackgroundColor(lua_tointeger(L, -4), lua_tointeger(L, -3), lua_tointeger(L, -2), lua_tointeger(L, -1));
    return 1;
}

static int Game_ImportMod(lua_State* L)
{
    Game::ImportMod();
    return 1;
}

static int Game_NewGame(lua_State* L)
{
    Game::NewGame();
    return 1;
}

static int Game_Step(lua_State* L)
{
    Game::Step(lua_tointeger(L, -1));
    return 1;
}

static int Game_EnterMode(lua_State* L)
{
    Game::EnterMode((Gamemode)lua_tointeger(L, -1));
    return 1;
}

static int Game_GhostCacheEnabled(lua_State* L)
{
    lua_pushboolean(L, Game::GhostCacheEnabled());
    return 1;
}

static int Game_GhostCacheIsReady(lua_State* L)
{
    lua_pushboolean(L, Game::GhostCacheIsReady());
    return 1;
}

static int Game_ToggleGhostCache(lua_State* L)
{
    Game::ToggleGhostCache();
    return 1;
}

static int Game_TurnFrameGhostEnabled(lua_State* L)
{
    lua_pushboolean(L, Game::TurnFrameGhostEnabled());
    return 1;
}

static int Game_ToggleTurnFrameGhost(lua_State* L)
{
    Game::ToggleTurnFrameGhost();
    return 1;
}

static int Game_ReplayCacheEnabled(lua_State* L)
{
    lua_pushboolean(L, Game::ReplayCacheEnabled());
    return 1;
}

static int Game_ToggleReplayCache(lua_State* L)
{
    Game::ToggleReplayCache();
    return 1;
}

static int Game_ToggleGhosts(lua_State* L)
{
    Game::ToggleGhosts();
    return 1;
}

static int Game_UndoSelectedPlayerMove(lua_State* L)
{
    Game::UndoSelectedPlayerMove();
    return 1;
}

static int Game_ToggleBodyState(lua_State* L)
{
    Game::ToggleBodyState(lua_tointeger(L, -1));
    return 1;
}

static int Game_ToggleSelectedBodyState(lua_State* L)
{
    Game::ToggleSelectedBodyState();
    return 1;
}

static int Game_ToggleSelectedPlayerBodyStates(lua_State* L)
{
    Game::ToggleSelectedPlayerBodyStates();
    return 1;
}

static int Game_TogglePause(lua_State* L)
{
    Game::TogglePause();
    return 1;
}

static int Game_SetGravity(lua_State* L)
{
    Game::SetGravity(lua_tonumber(L, -3), lua_tonumber(L, -2), lua_tonumber(L, -1));
    return 1;
}

static int Game_SetMaxContacts(lua_State* L)
{
    Game::SetMaxContacts(lua_tointeger(L, -1));
    return 1;
}

static int Game_SetFriction(lua_State* L)
{
    Game::SetFriction(lua_tonumber(L, -1));
    return 1;
}

static int Game_SetBounce(lua_State* L)
{
    Game::SetBounce(lua_tonumber(L, -1));
    return 1;
}

static int Game_SetTurnFrames(lua_State* L)
{
    Game::SetTurnFrames(lua_tointeger(L, -1));
    return 1;
}

static int Game_SetReactionTime(lua_State* L)
{
    Game::SetReactionTime(lua_tointeger(L, -1));
    return 1;
}

static int Game_GetFreeze(lua_State* L)
{
    lua_pushboolean(L, Game::GetFreeze());
    return 1;
}

static int Game_GetPause(lua_State* L)
{
    lua_pushboolean(L, Game::GetPause());
    return 1;
}

static int Game_IsMode(lua_State* L)
{
    lua_pushboolean(L, Game::GetGamemode() == (Gamemode)lua_tointeger(L, -1));
    return 1;
}

static int Game_GetMod(lua_State* L)
{
    auto mod = Game::GetMod();
    lua_pushlstring(L, mod.data(), mod.size());
    return 1;
}

static int Game_GetMaxContacts(lua_State* L)
{
    lua_pushinteger(L, Game::GetMaxContacts());
    return 1;
}

static int Game_GetGameFrame(lua_State* L)
{
    lua_pushinteger(L, Game::GetGameFrame());
    return 1;
}

static int Game_GetReactionTime(lua_State* L)
{
    lua_pushnumber(L, Game::GetReactionTime());
    return 1;
}

static int Game_GetReactionCount(lua_State* L)
{
    lua_pushnumber(L, Game::GetReactionCount());
    return 1;
}

static int Game_GetContactCount(lua_State* L)
{
    lua_pushinteger(L, Game::GetContactCount());
    return 1;
}

static int Game_GetObjectCount(lua_State* L)
{
    lua_pushnumber(L, Game::GetObjectCount());
    return 1;
}

static int Game_GetPlayerCount(lua_State* L)
{
    lua_pushnumber(L, Game::GetPlayerCount());
    return 1;
}

static int Game_GetPlayerBodyCount(lua_State* L)
{
    lua_pushnumber(L, Game::GetPlayerBodyCount(lua_tointeger(L, -1)));
    return 1;
}

static int Game_GetPlayerJointCount(lua_State* L)
{
    lua_pushnumber(L, Game::GetPlayerJointCount(lua_tointeger(L, -1)));
    return 1;
}

static int Game_GetSelectedPlayerID(lua_State* L)
{
    lua_pushinteger(L, Game::GetSelectedPlayerID());
    return 1;
}

static int Game_IsSelectedPlayerValid(lua_State* L)
{
    lua_pushboolean(L, Game::GetSelectedPlayerID() != -1);
    return 1;
}

static int Game_Refreeze(lua_State* L)
{
    Game::Refreeze();
    return 1;
}

static int Game_TogglePlayerPassiveStatesAlt(lua_State* L)
{
    Game::TogglePlayerPassiveStatesAlt(lua_tointeger(L, -1));
    return 1;
}

static int Game_TogglePlayerPassiveStates(lua_State* L)
{
    Game::TogglePlayerPassiveStates(lua_tointeger(L, -1));
    return 1;
}

static int Game_GetSelectedJointID(lua_State* L)
{
    lua_pushinteger(L, Game::GetSelectedJointID());
    return 1;
}

static int Game_GetSelectedJointVelocity(lua_State* L)
{
    lua_pushnumber(L, Game::GetSelectedJointVelocity());
    return 1;
}

static int Game_GetSelectedJointVelocityAlt(lua_State* L)
{
    lua_pushnumber(L, Game::GetSelectedJointVelocityAlt());
    return 1;
}

static int Game_IsSelectedJointValid(lua_State* L)
{
    lua_pushboolean(L, Game::GetSelectedJointID() != -1);
    return 1;
}

static int Game_ToggleJointActiveStateAlt(lua_State* L)
{
    Game::ToggleJointActiveStateAlt(lua_tointeger(L, -1));
    return 1;
}
static int Game_ToggleJointActiveState(lua_State* L)
{
    Game::ToggleJointActiveState(lua_tointeger(L, -1));
    return 1;
}
static int Game_ToggleJointPassiveStateAlt(lua_State* L)
{
    Game::ToggleJointPassiveStateAlt(lua_tointeger(L, -1));
    return 1;
}
static int Game_ToggleJointPassiveState(lua_State* L)
{
    Game::ToggleJointPassiveState(lua_tointeger(L, -1));
    return 1;
}
static int Game_CycleJointStateAlt(lua_State* L)
{
    Game::CycleJointStateAlt(lua_tointeger(L, -1));
    return 1;
}
static int Game_CycleJointState(lua_State* L)
{
    Game::CycleJointState(lua_tointeger(L, -1));
    return 1;
}

static int Game_ReverseCycleJointStateAlt(lua_State* L)
{
    Game::ReverseCycleJointStateAlt(lua_tointeger(L, -1));
    return 1;
}
static int Game_ReverseCycleJointState(lua_State* L)
{
    Game::ReverseCycleJointState(lua_tointeger(L, -1));
    return 1;
}

static int Game_ToggleSelectedPlayerPassiveStatesAlt(lua_State* L)
{
    Game::ToggleSelectedPlayerPassiveStatesAlt();
    return 1;
}

static int Game_ToggleSelectedPlayerPassiveStates(lua_State* L)
{
    Game::ToggleSelectedPlayerPassiveStates();
    return 1;
}

static int Game_ToggleSelectedJointActiveStateAlt(lua_State* L)
{
    Game::ToggleSelectedJointActiveStateAlt();
    return 1;
}

static int Game_ToggleSelectedJointActiveState(lua_State* L)
{
    Game::ToggleSelectedJointActiveState();
    return 1;
}

static int Game_ToggleSelectedJointPassiveStateAlt(lua_State* L)
{
    Game::ToggleSelectedJointPassiveStateAlt();
    return 1;
}

static int Game_ToggleSelectedJointPassiveState(lua_State* L)
{
    Game::ToggleSelectedJointPassiveState();
    return 1;
}

static int Game_CycleSelectedJointStateAlt(lua_State* L)
{
    Game::CycleSelectedJointStateAlt();
    return 1;
}

static int Game_CycleSelectedJointState(lua_State* L)
{
    Game::CycleSelectedJointState();
    return 1;
}

static int Game_ReverseCycleSelectedJointStateAlt(lua_State* L)
{
    Game::ReverseCycleSelectedJointStateAlt();
    return 1;
}

static int Game_ReverseCycleSelectedJointState(lua_State* L)
{
    Game::CycleSelectedJointState();
    return 1;
}

static int Game_GetGamerules(lua_State* L)
{
    auto rules = Game::GetGamerules();
    lua_newtable(L);
    lua_pushstring(L, rules.mod.data());
    lua_setfield(L, -2, "mod");
    lua_newtable(L);
    lua_pushnumber(L, rules.gravity.x);
    lua_setfield(L, -2, "x");
    lua_pushnumber(L, rules.gravity.y);
    lua_setfield(L, -2, "y");
    lua_pushnumber(L, rules.gravity.z);
    lua_setfield(L, -2, "z");
    lua_setfield(L, -2, "gravity");
    lua_pushnumber(L, rules.numplayers);
    lua_setfield(L, -2, "numplayers");
    lua_pushnumber(L, rules.turnframes);
    lua_setfield(L, -2, "turnframes");
    lua_pushnumber(L, rules.max_contacts);
    lua_setfield(L, -2, "max_contacts");
    lua_pushnumber(L, rules.reaction_time);
    lua_setfield(L, -2, "reaction_time");
    lua_pushnumber(L, rules.engagedistance);
    lua_setfield(L, -2, "engagedistance");
    lua_pushnumber(L, rules.engageheight);
    lua_setfield(L, -2, "engageheight");
    lua_pushnumber(L, rules.friction);
    lua_setfield(L, -2, "friction");
    lua_pushnumber(L, rules.bounce);
    lua_setfield(L, -2, "bounce");
    return 1;
}

static int Game_Reset(lua_State* L)
{
    Game::Reset();
    return 1;
}

static int Game_Quit(lua_State* L)
{
    Game::Stop();
    return 1;
}
static const luaL_Reg api_game[]
{
    {"Reset", Game_Reset},
    {"Quit", Game_Quit},

    {"ImportMod", Game_ImportMod},
    {"NewGame", Game_NewGame},

    {"Step", Game_Step},

    {"EnterMode", Game_EnterMode},

    {"TogglePause", Game_TogglePause},
	{"ToggleGhostCache", Game_ToggleGhostCache},
	{"ToggleReplayCache", Game_ToggleReplayCache},
    {"ToggleTurnFrameGhost", Game_ToggleTurnFrameGhost},
	{"ToggleGhosts", Game_ToggleGhosts},

    {"TogglePlayerPassiveStates", Game_TogglePlayerPassiveStates},
    {"TogglePlayerPassiveStatesAlt", Game_TogglePlayerPassiveStatesAlt},

    {"ToggleSelectedPlayerPassiveStatesAlt", Game_ToggleSelectedPlayerPassiveStatesAlt},
    {"ToggleSelectedPlayerPassiveStates", Game_ToggleSelectedPlayerPassiveStates},

    {"ToggleJointActiveStateAlt", Game_ToggleJointActiveStateAlt},
    {"ToggleJointActiveState", Game_ToggleJointActiveState},
    {"ToggleJointPassiveStateAlt", Game_ToggleJointPassiveStateAlt},
    {"ToggleJointPassiveState", Game_ToggleJointPassiveState},

    {"CycleJointStateAlt", Game_CycleJointStateAlt},
    {"CycleJointState", Game_CycleJointState},
    {"ReverseCycleJointStateAlt", Game_ReverseCycleJointStateAlt},
    {"ReverseCycleJointState", Game_ReverseCycleJointState},

    {"ToggleSelectedJointActiveStateAlt", Game_ToggleSelectedJointActiveStateAlt},
    {"ToggleSelectedJointActiveState", Game_ToggleSelectedJointActiveState},
    {"ToggleSelectedJointPassiveStateAlt", Game_ToggleSelectedJointPassiveStateAlt},
    {"ToggleSelectedJointPassiveState", Game_ToggleSelectedJointPassiveState},

    {"CycleSelectedJointStateAlt", Game_CycleSelectedJointStateAlt},
    {"CycleSelectedJointState", Game_CycleSelectedJointState},
    {"ReverseCycleSelectedJointStateAlt", Game_ReverseCycleSelectedJointStateAlt},
    {"ReverseCycleSelectedJointState", Game_ReverseCycleSelectedJointState},

    {"ToggleBodyState", Game_ToggleBodyState},
    {"ToggleSelectedBodyState", Game_ToggleSelectedBodyState},

    {"ToggleSelectedPlayerBodyStates", Game_ToggleSelectedPlayerBodyStates},

    {"Refreeze", Game_Refreeze},

    {"IsPause", Game_GetPause},
    {"IsFreeze", Game_GetFreeze},

    {"IsMode", Game_IsMode},

    {"IsSelectedPlayerValid", Game_IsSelectedPlayerValid},
    {"IsSelectedJointValid", Game_IsSelectedJointValid},

	{"GhostCacheEnabled", Game_GhostCacheEnabled},
	{"ReplayCacheEnabled", Game_ReplayCacheEnabled},
	{"TurnFrameGhostEnabled", Game_TurnFrameGhostEnabled},
	{"GhostCacheIsReady", Game_GhostCacheIsReady},

    {"SetMode", Game_SetMode},
    {"SetGravity", Game_SetGravity},
    {"SetMaxContacts", Game_SetMaxContacts},
    {"SetFriction", Game_SetFriction},
    {"SetBounce", Game_SetBounce},
    {"SetTurnFrames", Game_SetTurnFrames},
    {"SetReactionTime", Game_SetReactionTime},

    {"GetMod", Game_GetMod},
    {"GetMaxContacts", Game_GetGameFrame},
    {"GetGameFrame", Game_GetGameFrame},
    {"GetReactionTime", Game_GetReactionTime},
    {"GetReactionCount", Game_GetReactionCount},
    {"GetContactCount", Game_GetContactCount},

    {"GetObjectCount", Game_GetObjectCount},
    {"GetPlayerCount", Game_GetPlayerCount},
    {"GetPlayerBodyCount", Game_GetPlayerBodyCount},
    {"GetPlayerJointCount", Game_GetPlayerJointCount},
    {"GetGamerules", Game_GetGamerules},

    {"GetSelectedPlayerID", Game_GetSelectedPlayerID},
    {"GetSelectedJointID", Game_GetSelectedJointID},

    {"GetSelectedJointVelocity", Game_GetSelectedJointVelocity},
    {"GetSelectedJointVelocityAlt", Game_GetSelectedJointVelocityAlt},

    {"UndoSelectedPlayerMove", Game_UndoSelectedPlayerMove},

    {"SetBackgroundColor", Game_SetBackgroundColor},

    {NULL, NULL},
};

static int Replay_Import(lua_State* L)
{
    Replay::Import(lua_tostring(L, -1));
    return 1;
}

static int Replay_Export(lua_State* L)
{
    Replay::Export(lua_tostring(L, -1));
    return 1;
}

static int Replay_GetMod(lua_State* L)
{
    auto mod = Replay::GetMod();
    lua_pushlstring(L, mod.data(), mod.size());
    return 1;
}

static int Replay_GetMaxFrame(lua_State* L)
{
    lua_pushinteger(L, Replay::GetMaxFrame());
    return 1;
}

static const luaL_Reg api_replay[]
{
    {"Import", Replay_Import},
    {"Export", Replay_Export},

    {"GetMod", Replay_GetMod},
    {"GetMaxFrame", Replay_GetMaxFrame},

    {NULL, NULL},
};

int luaopen_api_replay(lua_State* L)
{
    luaL_register(L, "Replay", api_replay);
    return 1;
}

int luaopen_api_game(lua_State* L)
{
    luaL_register(L, "Game", api_game);
    lua_getglobal(L, "Game");
    lua_pushinteger(L, Gamemode::FREE_PLAY);
    lua_setfield(L, -2, "MODE_FREEPLAY");
    lua_pushinteger(L, Gamemode::SELF_PLAY);
    lua_setfield(L, -2, "MODE_SELFPLAY");
    lua_pushinteger(L, Gamemode::REPLAY_EDIT);
    lua_setfield(L, -2, "MODE_REPLAY_EDIT");
    lua_pushinteger(L, Gamemode::REPLAY_PLAY);
    lua_setfield(L, -2, "MODE_REPLAY");
    lua_pop(L, 1);
    return 1;
}

static int Expermental_TriggerSelectedJointActiveStateAlt(lua_State* L)
{
    Game::TriggerSelectedJointActiveStateAlt(lua_tonumber(L, -1));
    return 1;
}

static int Expermental_TriggerSelectedJointActiveState(lua_State* L)
{
    Game::TriggerSelectedJointActiveState(lua_tonumber(L, -1));
    return 1;
}

static int Expermental_ToggleSelectedJointActiveStateAlt(lua_State* L)
{
    Game::ToggleSelectedJointActiveStateAlt(lua_tonumber(L, -1));
    return 1;
}

static int Expermental_ToggleSelectedJointActiveState(lua_State* L)
{
    Game::ToggleSelectedJointActiveState(lua_tonumber(L, -1));
    return 1;
}

static int Expermental_GenMeshPlane(lua_State* L)
{
    int resZ = lua_tointeger(L, -1);
    int resX = lua_tointeger(L, -2);
    float length = lua_tonumber(L, -3);
    float width = lua_tonumber(L, -4);

	int mesh_id = ResourceManager::GenMeshPlane(width, length, resX, resZ);
	lua_pushinteger(L, mesh_id);
    return 1;
}

static int Expermental_LoadModelFromMesh(lua_State* L)
{
    int mesh_id = lua_tointeger(L, -1);
    int model_id = ResourceManager::LoadModelFromMesh(mesh_id);
	lua_pushinteger(L, model_id);
    return 1;
}

static int Expermental_LoadModel(lua_State* L)
{
    auto model_path = lua_tostring(L, -1);
    int model_id = ResourceManager::LoadModel(model_path);
	lua_pushinteger(L, model_id);
    return 1;
}

static int Expermental_LoadTexture(lua_State* L)
{
    auto texture_path = lua_tostring(L, -1);
    int texture_id = ResourceManager::LoadTexture(texture_path);
	lua_pushinteger(L, texture_id);
    return 1;
}

static int Expermental_SetModelTexture(lua_State* L)
{
    int texture_id = lua_tointeger(L, -1);
	int model_id = lua_tointeger(L, -2);
	ResourceManager::SetModelTexture(model_id, texture_id);
    return 1;
}

static int Expermental_DrawModel(lua_State* L)
{
    int model_id = lua_tointeger(L, -1);
	ResourceManager::DrawModel(model_id);
    return 1;
}

static const luaL_Reg api_expermental[]
{
    {"TriggerSelectedJointActiveStateAlt", Expermental_TriggerSelectedJointActiveStateAlt},
    {"TriggerSelectedJointActiveState", Expermental_TriggerSelectedJointActiveState},

    {"ToggleSelectedJointActiveStateAlt", Expermental_ToggleSelectedJointActiveStateAlt},
    {"ToggleSelectedJointActiveState", Expermental_ToggleSelectedJointActiveState},

    {"GenMeshPlane", Expermental_GenMeshPlane},
    {"LoadModelFromMesh", Expermental_LoadModelFromMesh},
	{"LoadModel", Expermental_LoadModel},
    {"LoadTexture", Expermental_LoadTexture},

	{"SetModelTexture", Expermental_SetModelTexture},
	{"DrawModel", Expermental_DrawModel},
	
    {NULL, NULL},
};

int luaopen_api_expermental(lua_State* L)
{
    luaL_register(L, "Expermental", api_expermental);
    lua_getglobal(L, "Expermental");
    lua_pushboolean(L, false);
    lua_setfield(L, -2, "variable_joint_vel");
    return 1;
}

