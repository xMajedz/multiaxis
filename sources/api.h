#pragma once
#include "common.h"
#include "luau.h"
#include "game.h"

#include <fstream>

namespace Api
{
	static lua_State* ML;

	static Gamerules rules;

	static std::vector<Body>   o_vector;
	static std::vector<Joint>   oj_vector;

	static std::vector<Player> p_vector;
	 
	static size_t o_count;
	static size_t oj_count;

	static size_t p_count;

	static Body*   o  = nullptr;
	static Joint*  oj = nullptr;

	static Player* p  = nullptr;

	std::vector<Joint> GetJointObjects();
	size_t GetJointObjectsCount();
	
	static std::map<std::string_view, BodyID> o_map;

	static std::map<std::string_view, BodyID> b_map;

	static std::vector<Body>  b_vector;
	static std::vector<Joint> j_vector;

	static size_t b_count;
	static size_t j_count;
	
	static Body*  b = nullptr;
	static Joint* j = nullptr;
	
	void Init();

	void Boot(std::string filename);

	void UpdateHotKeys();
	void SetHotKey(int key, int ref);

	void Reset();

	void Close();
	
	Gamerules GetRules();

	std::vector<Body> GetObjects();
	std::vector<Player> GetPlayers();

	size_t GetObjectsCount();
	size_t GetPlayersCount();
	
	int UpdateCallback(dReal dt);
	int DrawCallback();
	int Draw3DCallback();
	int NewGameCallback();
	int FreezeCallback();
	int StepCallback();

	int ConsoleCallback(std::string message);

	int FileDroppedCallback(raylib::FilePathList files);

	int loadmod(lua_State* L, std::string modpath);

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
