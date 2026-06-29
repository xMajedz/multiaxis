#pragma once
#include "common.h"
#include "luau.h"
#include "game.h"

#include <fstream>

namespace Api
{
	static lua_State* ML;

	static Gamerules rules;

    static std::vector<EnvPlane> planes_vector;	
	static std::vector<Body> objects_vector;
	static std::vector<Joint> object_joints_vector;
	static std::vector<Player> players_vector;

	static std::vector<Body>  b_vector;
	static std::vector<Joint> j_vector;
	
	static std::map<std::string_view, BodyID> o_map;
	static std::map<std::string_view, BodyID> b_map;

	Gamerules GetRules();

	std::vector<EnvPlane> GetEnvPlanes();
	std::vector<Joint> GetJointObjects();
	
	std::vector<Body> GetObjects();
	std::vector<Player> GetPlayers();

	size_t GetEnvPlanesCount();
	size_t GetObjectsCount();
	size_t GetJointObjectsCount(); 
	size_t GetPlayersCount();
	
	void Init();

	void Boot(std::string filename);

	void UpdateHotKeys();
	void SetHotKey(int key, int ref);

	void Reset();

	void Close();
	
	int UpdateCallback(dReal dt);
	int DrawCallback();
	int Draw3DCallback();
	int NewGameCallback();
	int FreezeCallback();
	int StepCallback();

	int ConsoleCallback(std::string message);

	int FileDroppedCallback(raylib::FilePathList files);

	int loadmodstring(lua_State* L, std::string content);
	int loadmodfile(lua_State* L, std::string modpath);

	int loadscript(lua_State* L, std::string scriptpath);
}

typedef void (*ConsoleCallback_t)(std::string);

namespace Console
{
	static ConsoleCallback_t console_callback = nullptr;

	void SetCallback(ConsoleCallback_t callback);

	void log(std::string message);
};

void luaopenApi(lua_State* L);

int luaopenApiGame(lua_State* L);

int luaopenApiNet(lua_State* L);

int luaopenApiReplay(lua_State* L);

int luaopenApiRaylib(lua_State* L);

int luaopenApiRaygui(lua_State* L);

int luaopenApiRaymath(lua_State* L);

int luaopenApiExpermental(lua_State* L);
